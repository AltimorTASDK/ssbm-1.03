#include "melee/rules.h"
#include "rules/values.h"

// Set default LGL and ATL values
struct init_rules {
	init_rules()
	{
		auto *rules = GetGameRules();
		rules->handicap = 0;
		rules->damage_ratio = 0;
	
		// Set rule value bounds
		RuleValueBounds[Rule_LedgeGrabLimit] = { 0, ledge_grab_limit_values.size() - 1 };
		RuleValueBounds[Rule_AirTimeLimit]   = { 0, air_time_limit_values.size() - 1 };
	}
} init_rules;