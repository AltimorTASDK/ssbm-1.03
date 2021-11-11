#pragma once

#include <gctypes.h>

enum MatchResult {
	MatchResult_Timeout       = 1,
	MatchResult_Elimination   = 2,
	MatchResult_LRAStart      = 7,
	MatchResult_Retry         = 8
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
	s32 last_update_time;
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
	u8 character_kind;
	u8 slot_type;
	u8 stocks;
	u8 color;
	u8 port;
	s8 spawnpoint;
	u8 spawn_direction;
	u8 subcolor;
	u8 handicap;
	u8 team;
	u8 nametag;
	char pad00B[0x18 - 0x0B];
	f32 offense_ratio;
	f32 defense_ratio;
	char pad020[0x24 - 0x20];
};

struct StartMeleeRules {
	struct {
		u8 flags1_80 : 1;
		u8 flags1_40 : 1;
		u8 is_stock_match : 1;
		u8 max_players : 3;
		u8 timer_enabled : 1;
		u8 count_up : 1;
	};
	struct {
		u8 flags2_80 : 1;
		u8 flags2_40 : 1;
		u8 flags2_20 : 1;
		u8 flags2_10 : 1;
		u8 flags2_08 : 1;
		u8 flags2_04 : 1;
		u8 flags2_02 : 1;
		u8 friendly_fire : 1;
	};
	struct {
		u8 flags3_80 : 1;
		u8 flags3_40 : 1;
		u8 flags3_20 : 1;
		u8 flags3_10 : 1;
		u8 pause_disabled : 1;
		u8 flags3_04 : 1;
		u8 flags3_02 : 1;
		u8 flags3_01 : 1;
	};
	struct {
		u8 score_display : 1;
		u8 flags4_40 : 1;
		u8 flags4_20 : 1;
		u8 flags4_10 : 1;
		u8 flags4_08 : 1;
		u8 flags4_04 : 1;
		u8 flags4_02 : 1;
		u8 flags4_01 : 1;
	};
	char pad004[0x08 - 0x04];
	u8 is_teams;
	char pad009[0x0B - 0x09];
	s8 item_frequency;
	s8 sd_penalty;
	s32 time_limit;
	char pad014[0x30 - 0x14];
	f32 damage_ratio;
	f32 game_speed;
	char pad038[0x60 - 0x38];
};

struct StartMeleeData {
	StartMeleeRules rules;
	PlayerInitData players[6];
};

struct VsModeData {
	char pad000[0x08];
	StartMeleeData data;
};

struct MatchExitData {
	char pad000[0x0C];
	MatchController match;
};

struct MatchInfo {
	char pad000;
	u8 pauser;
	u8 pause_timer;
	char pad003[0x24C - 0x003];
	MatchController match;
	char pad24C4[0x24C8 - 0x24C4];
	StartMeleeRules rules;
};

extern "C" {
	
extern MatchExitData LastMatchData;

MatchInfo *GetMatchInfo();

bool MatchInfo_IsTeams();
bool MatchInfo_ShouldDisplayPortTag(u32 slot);

void Match_NoContestOrRetry(s32 unused, u32 result);

}