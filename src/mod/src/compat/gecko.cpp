#if defined(NTSC102) && (defined(DOL) || !defined(NOPAL))

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

// Wait to apply settings because gecko code checks happen before C++ constructors
static bool use_lcd;

#ifdef DOL

static bool use_wide;
static bool use_crop;
static bool use_crt;
static bool use_low;

// Disable conflicting gecko codes if found and set the equivalent 1.03 settings

static void check_widescreen()
{
	if (*(u32*)0x803BB05C != 0x3EB00000)
		return;

	use_wide = true;

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

	if (*(u32*)0x80302784 == 0x93EDB77C)
		return;

	use_crop = true;

	// Optional: 16:9 -> 73:60 [Dan Salvato, mirrorbender, Achilles1515, UnclePunch, Fizzi]
	u32_patch_list {
		std::pair { 0x80302784, 0x93EDB77Cu }, // External/WidescreenShutters/Add Shutters.asm
	};
}

static void check_pdf()
{
	if (is_faster_melee() || *(u32*)0x80019860 != 0x4BFFFD9D)
		return;

	use_crt = true;

	// Polling Drift Fix [Dan Salvato]
	u32_patch_list {
		std::pair { 0x80019860, 0x4832A1D1 }, // External/Lag Reduction/Polling Drift/Branch.asm
		std::pair { 0x801A4DA0, 0x901C0000 }, // External/Lag Reduction/Polling Drift/Injection.asm
		std::pair { 0x801A4DB4, 0x4182FFF4 }, // External/Lag Reduction/Polling Drift/Nop.asm
	};
}

#endif // DOL

static void check_pdf_half_vb()
{
	if (is_faster_melee() || *(u32*)0x801A4D98 == 0x481EE0E9)
		return;

	use_lcd = true;

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
}

#ifdef DOL

static void check_pdf_vb()
{
	if (is_faster_melee() || *(u32*)0x80019860 != 0x91231F5C)
		return;

	use_low = true;

	// Polling Drift Fix + Visual Buffer Fixes [tauKhan]
	u32_patch_list {
		std::pair { 0x801A4C24, 0xC042B008 },
		std::pair { 0x8001985C, 0x38E50000 },
		std::pair { 0x80019860, 0x4832A1D1 },
		std::pair { 0x80376200, 0x2C1DFFFF },
		std::pair { 0x801A5018, 0x3B5A0001 },
		// Keep this, this patches a reference to a constant that gets clobbered
		// std::pair { 0x80218D68, 0xC822C150 },
	};
}

extern "C" void orig_HSD_InitComponent();
extern "C" void hook_HSD_InitComponent()
{
	orig_HSD_InitComponent();

	check_widescreen();
	check_pdf();
	check_pdf_half_vb();
	check_pdf_vb();

	// Don't run the codehandler every frame on console
	u32_patch_list {
		std::pair { 0x8034BAFC, 0x4E800020 }
	};
}

[[gnu::constructor]] static void apply_settings()
{
	auto *rules = GetGameRules();

	if (use_crop)
		rules->widescreen = widescreen_mode::crop;
	else if (use_wide)
		rules->widescreen = widescreen_mode::on;

	if (use_low)
		rules->latency = latency_mode::low;
	else if (use_lcd)
		rules->latency = latency_mode::lcd;
	else if (use_crt)
		rules->latency = latency_mode::crt;
}

#else // DOL

[[gnu::constructor]] static void check_20XX_pdf_half_vb()
{
	check_pdf_half_vb();

	if (use_lcd) {
		GetGameRules()->latency = latency_mode::lcd;
		init_latency();
	}
}

#endif // DOL

#endif // defined(NTSC102) && (defined(DOL) || !defined(NOPAL))