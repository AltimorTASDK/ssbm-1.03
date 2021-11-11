#pragma once

#include "melee/match.h"
#include "melee/rules.h"
#include <gctypes.h>

inline int crew_stocks[4] = { -1, -1, -1, -1 };
inline int last_stock_count = -1;

bool won_last_match(const MatchController &match, u32 port);
void update_crew_stocks(const MatchController &match);

inline int get_crew_stocks(u32 port)
{
	if (port < 4 && crew_stocks[port] != -1 && GetGameRules()->stock_count == last_stock_count)
		return crew_stocks[port];
	else
		return GetGameRules()->stock_count;
}

inline void reset_crew_stocks()
{
	for (auto i = 0; i < 4; i++)
		crew_stocks[i] = -1;
}