#include "melee/rules.h"
#include "rules/values.h"

// Set default rules
struct init_rules {
	init_rules()
	{
		// Set rule value bounds
		RuleValueBounds[Rule_LedgeGrabLimit] = { 0, ledge_grab_limit_values.size() - 1 };
		RuleValueBounds[Rule_AirTimeLimit]   = { 0, air_time_limit_values.size() - 1 };
		
		auto *rules = GetGameRules();
		rules->mode = Mode_Stock;
		rules->stock_count = 4;
		rules->handicap = 0; // LGL
		rules->damage_ratio = 0; // ATL
		rules->stock_time_limit = 6;
		rules->friendly_fire = true;
	}
} init_rules;