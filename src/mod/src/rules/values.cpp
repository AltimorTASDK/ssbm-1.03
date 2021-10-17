#include "melee/match.h"
#include "melee/rules.h"
#include "rules/values.h"

struct init_rules {
	init_rules()
	{
		// Set new rule value bounds
		RuleValueBounds[Rule_Mode]           = { 0, Mode_Stock };
		RuleValueBounds[Rule_LedgeGrabLimit] = { 0, ledge_grab_limit_values.size() - 1 };
		RuleValueBounds[Rule_AirTimeLimit]   = { 0, air_time_limit_values.size() - 1 };
		
		// Set default rules
		auto *rules = GetGameRules();
		rules->mode = Mode_Stock;
		rules->time_limit = 0;
		rules->stock_count = 4;
		rules->handicap = 0; // LGL
		rules->damage_ratio = 0; // ATL
		rules->stock_time_limit = 6;
		rules->friendly_fire = true;
	}
} init_rules;

extern "C" void orig_Match_Init(StartMeleeData *data);
extern "C" void hook_Match_Init(StartMeleeData *data)
{
	data->rules.damage_ratio = 1.f;
	data->rules.item_frequency = -1;
	
	for (auto i = 0; i < 6; i++) {
		data->players[i].handicap = 9;
		data->players[i].offense_ratio = 1.f;
		data->players[i].defense_ratio = 1.f;
	}
	
	orig_Match_Init(data);
}