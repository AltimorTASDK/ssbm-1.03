#pragma once

#include <gctypes.h>

enum MatchResult {
	MatchResult_Timeout       = 1,
	MatchResult_Elimination   = 2,
	MatchResult_LRAStart      = 7,
	// Custom
	MatchResult_RuleViolation = 9
};

struct MatchPlayerData {
	u8 slot_type;
	char pad001[0x05 - 0x01];
	u8 is_big_loser;
	u8 is_small_loser;
	u8 team;
	u8 stocks;
	char pad009[0x0C - 0x09];
	u16 percent;
	char pad00E[0x2C - 0x0E];
	s32 score;
	u32 subscore;
	char pad034[0x6C - 0x34];
	s32 ground_time;
	s32 air_time;
	char pad074[0x8C - 0x74];
	s32 ledge_grabs;
	char pad090[0xA8 - 0x90];
};

struct MatchTeamData {
	int score;
	int subscore;
	u8 is_big_loser;
	char pad009;
	u8 exists;
	char pad00B;
};

struct MatchController {
	s32 timer;
	u8 result;
	char pad005;
	u8 is_teams;
	char pad007;
	s32 frame_count;
	char pad009;
	u8 winner_count;
	u8 team_winner_count;
	char pad00F;
	u8 winners[6];
	u8 team_winners[5];
	MatchTeamData teams[5];
	MatchPlayerData players[6];
	char pad448[0x2278 - 0x448];
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
	char pad020[0x24 - 0x20];
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
	
MatchController *GetLastMatchController();

bool MatchInfo_IsTeams();

}