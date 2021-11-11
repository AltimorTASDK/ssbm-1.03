#include "melee/match.h"
#include "melee/player.h"
#include "melee/rules.h"
#include "melee/scene.h"
#include "rules/values.h"
#include "util/melee/match.h"

struct init_rules {
	init_rules()
	{
		// Set new rule value bounds
		RuleValueBounds[Rule_Mode]           = { Mode_Time, Mode_Crew };
		RuleValueBounds[Rule_LedgeGrabLimit] = { 0, ledge_grab_limit_values.size() - 1 };
		RuleValueBounds[Rule_AirTimeLimit]   = { 0, air_time_limit_values.size() - 1 };

		ExtraRuleValueBounds[ExtraRule_Latency]    = { 0, (u8)latency_mode::low };
		ExtraRuleValueBounds[ExtraRule_Widescreen] = { 0, 1 };
		
		// Set default rules
		auto *rules = GetGameRules();
		rules->mode = Mode_Stock;
		rules->time_limit = 0;
		rules->stock_count = 4;
		rules->ledge_grab_limit = 0; // LGL
		rules->air_time_limit = 0; // ATL
		rules->stage_selection_mode = 0;
		rules->stock_time_limit = 6;
		rules->pause = false;
		rules->controller_fix = ucf_type::hax;
		rules->latency = latency_mode::crt;
		rules->widescreen = false;
	}
} init_rules;

static bool is_singleplayer(const StartMeleeData *data)
{
	auto count = 0;
	for (auto i = 0; i < 6; i++) {
		if (data->players[i].slot_type != SlotType_None && ++count > 1)
			return false;
	}
	return true;
}

static bool is_pause_disabled(const StartMeleeData *data)
{
	return !GetGameRules()->pause && data->rules.is_stock_match && data->players[0].stocks == 4;
}

extern "C" void orig_VsMode_InitDataFromRules(VsModeData *vs_data);
extern "C" void hook_VsMode_InitDataFromRules(VsModeData *vs_data)
{
	auto *rules = GetGameRules();

	if (rules->mode == Mode_Crew) {
		// Use stock rules for crew
		rules->mode = Mode_Stock;
		orig_VsMode_InitDataFromRules(vs_data);
		rules->mode = Mode_Crew;
	} else {
		orig_VsMode_InitDataFromRules(vs_data);
	}
}

extern "C" void orig_Match_Init(StartMeleeData *data);
extern "C" void hook_Match_Init(StartMeleeData *data)
{
	if (SceneMajor == Scene_AttractMode) {
		// Don't alter attract demo match settings
		orig_Match_Init(data);
		return;
	}
	
	if (is_singleplayer(data)) {
		// Force infinite time match + pause
		data->rules.is_stock_match = false;
		data->rules.timer_enabled = false;
		data->rules.pause_disabled = false;
	} else {
		data->rules.pause_disabled = is_pause_disabled(data);
	}

	data->rules.damage_ratio = 1.f;
	data->rules.item_frequency = -1;
	data->rules.friendly_fire = true;
	data->rules.score_display = false;
	data->rules.sd_penalty = -2;
	
	for (auto i = 0; i < 6; i++) {
		data->players[i].handicap = 9;
		data->players[i].offense_ratio = 1.f;
		data->players[i].defense_ratio = 1.f;
		
		// Carry over winner stock counts for crew
		if (GetGameRules()->mode == Mode_Crew)
			data->players[i].stocks = (u8)get_crew_stocks(i);
	}
	
	orig_Match_Init(data);
}