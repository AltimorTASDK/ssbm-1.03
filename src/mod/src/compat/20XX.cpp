#include "hsd/gobj.h"
#include "hsd/pad.h"
#include "os/thread.h"
#include "melee/scene.h"
#include "util/patch_list.h"

#if defined(NTSC102) && !defined(NOPAL)
extern "C" char SaveFileName[25];
extern "C" char MnSlMapPath[];

extern "C" void Stage_Fountain_Init();
extern "C" void Scene_Match_Exit(SceneMinorData *data, u8 victory_screen, u8 sudden_death);
extern "C" void Scene_RunLoop(void(*think_callback)());
extern "C" void VIRetraceHandler(u16 irq, OSContext *ctx);

extern "C" bool is_20XX()
{
	// XXperSmashBros
	return *(u16*)&SaveFileName == 'XX';
}

[[gnu::constructor]] static void check_20XX()
{
	if (!is_20XX())
		return;

	patch_list {
		// Remove 20XX's results screen hook
		// addi r27, r4, 0
		std::pair { Scene_Match_Exit+0x10,   0x3B640000u },
		// Disable 20XX's lagless FoD patch
		// stwu r1, -8(r1)
		std::pair { Stage_Fountain_Init+0x8, 0x9421FFF8u },
		// Disable 20XX's built in half frame lag reduction
		// bl MCCUpdate
		std::pair { Scene_RunLoop+0x64,      0x481EE0E9u },
		// li r3, 0
		std::pair { Scene_RunLoop+0x31C,     0x38600000u },
		// subi r3, r13, 0x4278
		std::pair { VIRetraceHandler+0x1FC,  0x386DBD88u }
	};
}

extern "C" bool is_20XX_stage_select()
{
	// 20XX loads MnSlMap.1sd, MnSlMap.2sd etc
	return is_20XX() && MnSlMapPath[8] != '1' && MnSlMapPath[8] != 'u';
}

extern "C" void orig_SSS_CursorThink(HSD_GObj *gobj);
extern "C" void hook_SSS_CursorThink(HSD_GObj *gobj)
{
	if (!is_20XX())
		return orig_SSS_CursorThink(gobj);

	// Prevent 20XX from receiving X/Y/Z presses for stage striking
	auto *queue = HSD_PadLibData.queue;
	u16 old_buttons[4];

	for (auto port = 0; port < 4; port++) {
		old_buttons[port] = queue[0].stat[port].buttons;
		queue[0].stat[port].buttons &= (u16)~(Button_X | Button_Y | Button_Z);
	}

	orig_SSS_CursorThink(gobj);

	for (auto port = 0; port < 4; port++)
		queue[0].stat[port].buttons = old_buttons[port];
}
#else
extern "C" bool is_20XX()
{
	return false;
}

extern "C" bool is_20XX_stage_select()
{
	return false;
}
#endif