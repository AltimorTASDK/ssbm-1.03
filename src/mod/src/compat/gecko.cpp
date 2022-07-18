#if defined(NTSC102) && defined(DOL)

#include "hsd/video.h"
#include "os/serial.h"
#include "latency/polling.h"
#include "melee/rules.h"
#include "rules/saved_config.h" // Must be included to ensure config is already initialized
#include "util/patch_list.h"
#include <gctypes.h>

extern "C" void(*PADSetSamplingCallback(void(*callback)()))();

struct u32_patch_list {
	template<typename ...T, typename ...U>
	u32_patch_list(const std::pair<T, U> &...patches)
	{
		runtime_patch_list { std::pair { (void*)patches.first, patches.second }... };
	}
};

// Disable conflicting gecko codes if found and set the equivalent 1.03 settings

[[gnu::constructor]] static void check_widescreen()
{
	if (*(u32*)0x803BB05C != 0x3EB00000)
		return;

	GetGameRules()->widescreen = true;

	// Optional: Widescreen 16:9 [Dan Salvato, mirrorbender, Achilles1515, UnclePunch]
	u32_patch_list {
		std::pair { 0x803BB05C, 0x3FAAAAAA }, // External/Widescreen/Fix Screen Flash.asm
		std::pair { 0x8036A4A8, 0xC03F0034 }, // External/Widescreen/Overwrite CObj Values.asm
		std::pair { 0x804DDB58, 0x3E000000 }, // External/Widescreen/Adjust Offscreen Scissor/Adjust Bubble Zoom.asm
		std::pair { 0x80086B24, 0x4182000C }, // External/Widescreen/Adjust Offscreen Scissor/Draw High Poly Models.asm
		std::pair { 0x80030C7C, 0xA0010020 }, // External/Widescreen/Adjust Offscreen Scissor/Left Camera Bound.asm
		std::pair { 0x80030C88, 0xA0010022 }, // External/Widescreen/Adjust Offscreen Scissor/Right Camera Bound.asm
		std::pair { 0x804DDB30, 0x3F24D31E }, // External/Widescreen/Adjust Offscreen Scissor/Fix Bubble Positions/Adjust Corner Value 1.asm
		std::pair { 0x804DDB34, 0xBF24D31E }, // External/Widescreen/Adjust Offscreen Scissor/Fix Bubble Positions/Adjust Corner Value 2.asm
		std::pair { 0x804DDB2C, 0xC322B333 }, // External/Widescreen/Adjust Offscreen Scissor/Fix Bubble Positions/Extend Negative Vertical Bound.asm
		std::pair { 0x804DDB28, 0x4322B333 }, // External/Widescreen/Adjust Offscreen Scissor/Fix Bubble Positions/Extend Positive Vertical Bound.asm
		std::pair { 0x804DDB4C, 0x3DCCCCCD }, // External/Widescreen/Adjust Offscreen Scissor/Fix Bubble Positions/Widen Bubble Region.asm
		std::pair { 0x802FCFC4, 0xC002E19C }, // External/Widescreen/Nametag Fixes/Adjust Nametag Background X Scale.asm
		std::pair { 0x804DDB84, 0x3ECCCCCD }, // External/Widescreen/Nametag Fixes/Adjust Nametag Text X Scale.asm
	};
}

[[gnu::constructor]] static void check_pdf()
{
	if (is_faster_melee() || *(u32*)0x80019860 != 0x4BFFFD9D)
		return;

	GetGameRules()->latency = latency_mode::crt;

	// Polling Drift Fix [Dan Salvato]
	u32_patch_list {
		std::pair { 0x80019860, 0x4832A1D1 }, // External/Lag Reduction/Polling Drift/Branch.asm
		std::pair { 0x801A4DA0, 0x901C0000 }, // External/Lag Reduction/Polling Drift/Injection.asm
		std::pair { 0x801A4DB4, 0x4182FFF4 }, // External/Lag Reduction/Polling Drift/Nop.asm
	};
}

[[gnu::constructor]] static void check_pdf_half_vb()
{
	if (is_faster_melee() || *(u32*)0x80158268 != 0xC82280A0)
		return;

	GetGameRules()->latency = latency_mode::lcd;

	// Polling Drift + Half Visual Buffer Fixes [tauKhan]
	u32_patch_list {
		std::pair { 0x801A4D98, 0x481EE0E9 },
		std::pair { 0x801A5050, 0x38600000 },
		std::pair { 0x801A4BEC, 0x38800000 },
		// Keep this, this patches a reference to a constant that gets clobbered
		// std::pair { 0x80158268, 0xC822A6F0 },
		std::pair { 0x8034EB60, 0x386DBD88 },
		std::pair { 0x80397878, 0x801E0000 },
	};

	PADSetSamplingCallback(nullptr);
	SI_EnablePollingInterrupt(true);
	HSD_VISetUserPostRetraceCallback(post_retrace_callback);
}

[[gnu::constructor]] static void check_pdf_vb()
{
	if (is_faster_melee() || *(u32*)0x80019860 != 0x91231F5C)
		return;

	GetGameRules()->latency = latency_mode::low;

	// Polling Drift Fix + Visual Buffer Fixes [tauKhan]
	u32_patch_list {
		std::pair { 0x801A4C24, 0xC042B008 },
		std::pair { 0x8001985C, 0x38E50000 },
		std::pair { 0x80019860, 0x4832A1D1 },
		std::pair { 0x80376200, 0x2C1DFFFF },
		std::pair { 0x801A5018, 0x3B5A0001 },
		// Keep this, this patches a reference to a constant that gets clobbered
		//std::pair { 0x80218D68, 0xC822C150 },
	};

	HSD_VISetUserPostRetraceCallback(post_retrace_callback);
}

#endif