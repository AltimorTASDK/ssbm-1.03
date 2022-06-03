#pragma once

#include <gctypes.h>

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

enum class controls_type : u8 {
	z_jump   = 0,
	z_angles = 1,
	all      = 2,
	max
};

enum class ucf_type : u8 {
	hax = 0,
	ucf = 1,
	max
};

enum class latency_mode : u8 {
	crt = 0,
	lcd = 1,
	low = 2,
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
	ExtraRule_StageMods           = 2,
	ExtraRule_FriendlyFire        = 2,
	ExtraRule_Controls            = 3,
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
	u16 pad000;
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
		sss_type stage_mods;
	};
	union {
		u8 score_display;
		controls_type controls;
	};
	union {
		u8 sd_penalty;
		ucf_type controller_fix;
	};
	latency_mode latency;
	u8 widescreen;
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