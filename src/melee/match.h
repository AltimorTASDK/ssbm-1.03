#pragma once

#include <gctypes.h>

struct PlayerMatchStats {
	char pad000[0x05];
	u8 is_big_loser;
	char pad006[0xA8 - 0x06];
};

struct ActiveMatchInfo {
	char pad000[0x58];
	PlayerMatchStats players[6];
};

struct PlayerInitData {
	char pad000[0x04];
	u8 port;
	s8 spawnpoint;
	u8 spawn_direction;
	char pad007;
	u8 handicap;
	char pad009[0x18 - 0x09];
	f32 offense_ratio;
	f32 defense_ratio;
	char pad009[0x24 - 0x20];
};

struct StartMeleeRules {
	char pad000[0x0B];
	s8 item_frequency;
	char pad00C[0x30 - 0x0C];
	f32 damage_ratio;
	char pad034[0x60 - 0x34];
};

struct StartMeleeData {
	StartMeleeRules rules;
	PlayerInitData players[6];
};

extern "C" {
	
extern ActiveMatchInfo *LastMatchInfo;

bool MatchInfo_IsTeams();

}