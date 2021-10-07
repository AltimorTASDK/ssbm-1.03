#pragma once

#include <gctypes.h>

enum Rule {
	Rule_Mode            = 0,
	Rule_StockCount      = 1,
	Rule_Handicap        = 2,
	Rule_LedgeGrabLimit  = 2,
	Rule_DamageRatio     = 3,
	Rule_AirTimeLimit    = 3,
	Rule_StageSelection  = 4,
	Rule_ItemSwitch      = 5,
	Rule_AdditionalRules = 6,
	Rule_Max             = 7
};

struct GameRules {
	u16 pad000;
	u8 mode;
	u8 time_limit;
	u8 stock_count;
	u8 handicap;
	u8 damage_ratio;
	u8 stage_selection_mode;
	u8 stock_time_limit;
	u8 friendly_fire;
	u8 pause;
	u8 score_display;
	u8 sd_penalty;
};

extern "C" {

extern struct {
	u8 min;
	u8 max;
} RuleValueBounds[7];

GameRules *GetGameRules();

}