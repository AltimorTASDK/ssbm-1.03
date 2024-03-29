#pragma once

#include <gctypes.h>

enum class controls_type : u8 {
	z_jump    = 0,
	z_angles  = 1,
	no_angles = 2,
	all       = 3,
#ifndef TOURNAMENT
	max
#else
	max = z_jump + 1
#endif
};

enum class sss_type : u8 {
	all  = 0,
	oss  = 1,
	none = 2,
	fdps = 3,
#ifdef FULL_SSS_ROTATOR
	max
#else
	max = oss + 1
#endif
};

enum class cfix : s8 {
	ucf = -1,
	a   =  0,
	b   =  1,
	c   =  2,
	d   =  3,
	max
};

enum class latency_mode : u8 {
	crt = 0,
	lcd = 1,
	low = 2,
	max
};

enum class widescreen_mode : u8 {
	off  = 0,
	on   = 1,
	crop = 2,
	max
};

enum Rule {
	Rule_Mode            = 0,
	Rule_StockCount      = 1,
	Rule_Handicap        = 2,
	Rule_LedgeGrabLimit  = 2,
	Rule_DamageRatio     = 3,
	Rule_AirTimeLimit    = 3,
	Rule_StageSelection  = 4,
	Rule_MenuMusic       = 4,
	Rule_ItemSwitch      = 5,
	Rule_StageMusic      = 5,
	Rule_AdditionalRules = 6,
	Rule_Max             = 7
};

enum ExtraRule {
	ExtraRule_StockMatchTimeLimit = 0,
	// Pause is moved up a row
	ExtraRule_Pause               = 1,
	ExtraRule_Controls            = 2,
	ExtraRule_FriendlyFire        = 2,
	ExtraRule_StageMods           = 3,
	ExtraRule_ControllerFix       = 4,
	ExtraRule_Latency             = 5,
	ExtraRule_Widescreen          = 6,
	ExtraRule_Max                 = 7
};

enum Mode {
	Mode_Time = 0,
	Mode_Stock = 1,
	Mode_Coin = 2,
	Mode_Bonus = 3,
	Mode_Crew = 3
};

struct GameRules {
	bool force_main_menu;
	u8 menu_bgm;
	u8 mode;
	u8 time_limit;
	u8 stock_count;
	union {
		u8 handicap;
		u8 ledge_grab_limit;
	};
	union {
		u8 damage_ratio;
		u8 air_time_limit;
	};
	u8 stage_selection_mode;
	u8 stock_time_limit;
	// Pause is moved up a row
	u8 pause;
	union {
		u8 friendly_fire;
		controls_type controls;
	};
	union {
		u8 score_display;
		sss_type stage_mods;
	};
	union {
		u8 sd_penalty;
		cfix controller_fix;
	};
	u8 pad00D;
	latency_mode latency;
	widescreen_mode widescreen;
};

struct RuleBounds {
	u8 min;
	u8 max;
};

extern "C" {

extern RuleBounds RuleValueBounds[7];
extern RuleBounds ExtraRuleValueBounds[7];

GameRules *GetGameRules();

} // extern "C"