#include "hsd/pad.h"
#include "melee/player.h"
#include "melee/scene.h"
#include "util/melee/pad.h"
#include <cstddef>
#include <gctypes.h>

// Manually maintain a buffer of hardware values
static PADStatus nana_hw_buffer[6][NANA_BUFFER];

// Remember whether the last frame was develop paused
static bool develop_pause[6];

static ptrdiff_t get_nana_read_index(const Player *nana)
{
	return nana->popo_data_read - nana->popo_data_buffer;
}

static ptrdiff_t get_nana_write_index(const Player *nana)
{
	return nana->popo_data_write - nana->popo_data_buffer;
}

namespace detail {

const PADStatus &get_input_impl(int port, int offset)
{
	// index is guaranteed to be in range [0, 8], avoid modulo
	auto index = HSD_PadLibData.qread + offset;
	if (index >= PAD_QNUM)
		index -= PAD_QNUM;

	return HSD_PadLibData.queue[index].stat[port];
}

const PADStatus &get_nana_input_impl(const Player *nana, int offset)
{
	// index is guaranteed to be in range [0, 58], avoid modulo
	auto index = get_nana_read_index(nana) + offset;
	if (index >= NANA_BUFFER)
		index -= NANA_BUFFER;

	return nana_hw_buffer[nana->slot][index];
}

} // namespace detail

extern "C" void orig_Player_Nana_RecordPopoData(Player *popo, Player *nana);
extern "C" void hook_Player_Nana_RecordPopoData(Player *popo, Player *nana)
{
	orig_Player_Nana_RecordPopoData(popo, nana);

	const auto index = get_nana_write_index(nana);
	nana_hw_buffer[nana->slot][index] = get_input<0>(popo->port);

	// Prevent Nana from grabbing when exiting develop pause
	if (develop_pause[nana->slot])
		nana->popo_data_write->buttons &= ~Button_Z;

	develop_pause[nana->slot] = Scene_CheckPauseFlag(PauseBit_DevelopPause);
}

extern "C" void update_pad_buffer(Player *player)
{
	auto *buffer = &detail::pad_buffer[player->slot];
	buffer->index = (buffer->index + 1) & detail::PAD_BUFFER_MASK;
	buffer->entries[buffer->index] = get_input<0>(player->port);
}