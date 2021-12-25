#include "hsd/memory.h"
#include "melee/scene.h"
#include "os/card.h"
#include "os/os.h"
#include "os/thread.h"
#include "util/math.h"
#include "util/mempool.h"
#include "util/gc/wait_object.h"
#include <algorithm>
#include <cstring>
#include <gctypes.h>
#include <ogc/cache.h>
#include <ogc/card.h>

struct buffer_wrapper;

extern "C" void *CardWorkArea;
extern "C" void(*MemCardErrorCallback)(u32);
extern "C" u32 MemCardState;

extern "C" void InitCardBuffers();
extern "C" void MemoryCard_RequestSave();

static mempool pool;

static auto first_save_check = false;

static struct {
	s32 card;
	const char *filename;
	bool read;
	void *buffer;
	u32 size;
	void *src_buffer;
	u32 src_size;

	bool save_pending;

	s32 error;
	card_file file;
	card_stat stats;
	wait_object wait;
} op;

extern "C" void orig_MemoryCard_RequestSave();
extern "C" void hook_MemoryCard_RequestSave()
{
	const auto irq_enable = OSDisableInterrupts();

	// Defer the game's memcard save if we're doing our own
	if (op.wait.is_complete())
		orig_MemoryCard_RequestSave();
	else
		op.save_pending = true;

	OSRestoreInterrupts(irq_enable);
}

extern "C" u32 orig_UpdateMemCardState();
extern "C" u32 hook_UpdateMemCardState()
{
	if (!first_save_check)
		return orig_UpdateMemCardState();

	// Always act as if memcard is present
	MemCardState = 0;
	return 0;
}

extern "C" u32 orig_GetNextSceneMajorCallback();
extern "C" u32 hook_GetNextSceneMajorCallback()
{
	if (!first_save_check) {
		// Load the new save file after loading 1.03
		const auto result = orig_GetNextSceneMajorCallback();
		first_save_check = true;
		return result;
	} else {
		return Scene_Transition;
	}
}

static void card_done()
{
	if (op.buffer != op.src_buffer) {
		if (op.read && op.error >= 0)
			memcpy(op.src_buffer, op.buffer, op.src_size);

		HSD_Free(op.buffer);
	}

	CARD_Unmount(op.card);

	// Wake up the main thread
	op.wait.wake();

	// Check if the game is waiting to use the memcard
	if (op.save_pending) {
		orig_MemoryCard_RequestSave();
		op.save_pending = false;
	}
}

static void card_error(const char *fmt, auto &&...args)
{
	if (op.error != CARD_ERROR_NOFILE)
		OSReport(fmt, args...);

	card_done();
}

static void card_callback(s32 chan, s32 result)
{
	op.error = result;

	if (result >= 0)
		card_done();
	else
		card_error("Card operation failed (%d)\n", result);
}

static void read_callback(s32 chan, s32 result)
{
	if (result < 0) {
		op.error = result;
		return card_error("Card mount failed (%d)\n", result);
	}

	if (op.error = CARD_Open(chan, op.filename, &op.file); op.error < 0)
		return card_error("CARD_Open failed (%d)\n", op.error);

	if (op.error = CARD_GetStatus(chan, op.file.filenum, &op.stats); op.error < 0)
		return card_error("CARD_GetStatus failed (%d)\n", op.error);

	if (op.buffer == nullptr)
		return op.wait.wake();

	const auto size = std::min(op.stats.len, op.size);

	if (op.error = CARD_ReadAsync(&op.file, op.buffer, size, 0, card_callback); op.error < 0)
		return card_error("CARD_ReadAsync failed (%d)\n", op.error);
}

static void write_callback(s32 chan, s32 result)
{
	if (result < 0) {
		op.error = result;
		return card_error("Card mount failed (%d)\n", result);
	}

	static constinit auto write = [](s32 chan, s32 result) {
		if (op.error = CARD_WriteAsync(&op.file, op.buffer, op.size, 0, card_callback);
		    op.error < 0)
			return card_error("CARD_WriteAsync failed (%d)\n", op.error);
	};

	static constinit auto create = [](s32 chan, s32 result) {
		if (op.error = CARD_CreateAsync(op.card, op.filename, op.size, &op.file, write);
		    op.error < 0)
			return card_error("CARD_CreateAsync failed (%d)\n", op.error);
	};

	static constinit auto remake = [](s32 chan, s32 result) {
		if (op.error = CARD_DeleteAsync(op.card, op.filename, create);
		    op.error < 0)
			return card_error("CARD_DeleteAsync failed (%d)\n", op.error);
	};

	if (op.error = CARD_Open(op.card, op.filename, &op.file);
	    op.error < 0 && op.error != CARD_ERROR_NOFILE)
		return card_error("CARD_Open failed (%d)\n", op.error);

	if (op.error == CARD_ERROR_NOFILE)
		return create(op.card, 0);

	if (op.error = CARD_GetStatus(op.card, op.file.filenum, &op.stats); op.error < 0)
		return card_error("CARD_GetStatus failed (%d)\n", op.error);

	// Resize if needed
	if (op.stats.len != op.size)
		remake(op.card, 0);
	else
		write(op.card, 0);
}

static void card_io(s32 card, const char *filename, void *buffer, u32 size, bool read)
{
	// Any previous operations must be complete
	op.wait.sleep();
	op.wait.reset();

	op.card = card;
	op.filename = filename;
	op.read = read;
	op.src_buffer = buffer;
	op.src_size = size;

	// Size must be aligned
	op.size = align_up(size, read ? 0x200 : cardmap[card].sector_size);

	if (buffer != nullptr && op.size != size) {
		// Make resized buffer
		op.buffer = HSD_MemAlloc(op.size);

		if (!read) {
			memcpy(op.buffer, buffer, size);
			memset((u8*)op.buffer + size, 0, op.size - size);
		}
	} else {
		op.buffer = buffer;
	}

	const auto callback = read ? read_callback : write_callback;

	InitCardBuffers();

	if (op.error = CARD_MountAsync(card, CardWorkArea, nullptr, callback); op.error < 0)
		card_error("CARD_MountAsync failed (%d)\n", op.error);
}

void card_read(s32 card, const char *filename, void *out, u32 max_size)
{
	card_io(card, filename, out, max_size, true);
}

void card_write(s32 card, const char *filename, void *in, u32 size)
{
	card_io(card, filename, in, size, false);
}

bool is_card_busy()
{
	return MemCardErrorCallback != nullptr || !op.wait.is_complete();
}

s32 card_sync()
{
	op.wait.sleep();
	return !op.read || op.error < 0 ? op.error : (s32)op.stats.len;
}