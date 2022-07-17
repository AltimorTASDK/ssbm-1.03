#include "rules/saved_config.h"
#include "rules/values.h"
#include "util/enum_math.h"
#include "util/gc/memcard.h"

static constexpr char filename[] = "103Config";

static bool save_pending = false;

saved_config::saved_config()
{
	load();

	auto *rules = GetGameRules();

	rules->widescreen       = widescreen;
	rules->latency          = latency;
	rules->ledge_grab_limit = ledge_grab_limit;
	rules->air_time_limit   = air_time_limit;
	rules->stock_time_limit = stock_time_limit;
	rules->controller_fix   = controller_fix;
	rules->stage_mods       = stage_mods;
	rules->controls         = controls;
}

void saved_config::load()
{
	card_read_buffer<config_values<config_version::current>> read;
	card_read(CARD_SLOTA, filename, &read, sizeof(read));

	if (card_sync() < 0)
		return;

	using v = config_version;

	// Copy values for the highest matching version
	if (read->version >= v::v3)
		(config_values<v::v3>&)*this = (config_values<v::v3>&)*read;
	else if (read->version >= v::v2)
		(config_values<v::v2>&)*this = (config_values<v::v2>&)*read;
	else
		(config_values<v::v1>&)*this = (config_values<v::v1>&)*read;

	// Adjust for new enum member
	if (read->version < v::v4 && controls >= controls_type::no_angles)
		controls++;

#ifndef FULL_SSS_ROTATOR
	if (stage_mods == sss_type::none)
		stage_mods = sss_type::oss;
	else if (stage_mods == sss_type::fdps)
		stage_mods = sss_type::all;
#endif

	// Update to current version
	version = v::current;
}

void saved_config::save()
{
	if (get_settings_lock())
		return;

	const auto *rules = GetGameRules();

	widescreen       = rules->widescreen;
	latency          = rules->latency;
	ledge_grab_limit = rules->ledge_grab_limit;
	air_time_limit   = rules->air_time_limit;
	stock_time_limit = rules->stock_time_limit;
	controller_fix   = rules->controller_fix;
	stage_mods       = rules->stage_mods;
	controls         = rules->controls;

	if (is_card_busy())
		save_pending = true;
	else
		card_write(CARD_SLOTA, filename, &config_buffer, sizeof(config_buffer));
}

extern "C" void orig_MemoryCard_CheckToSaveData();
extern "C" void hook_MemoryCard_CheckToSaveData()
{
	if (save_pending && !is_card_busy()) {
		save_pending = false;
		card_write(CARD_SLOTA, filename, &config_buffer, sizeof(config_buffer));
	}

	orig_MemoryCard_CheckToSaveData();
}