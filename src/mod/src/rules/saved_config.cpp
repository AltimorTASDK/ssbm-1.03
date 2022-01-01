#include "rules/saved_config.h"
#include "rules/values.h"
#include "util/gc/memcard.h"

constexpr char filename[] = "103Config";

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
	config_values<config_version::current> read = { 0 };
	card_read(CARD_SLOTA, filename, &read, sizeof(read));

	if (card_sync() < 0)
		return;

	using v = config_version;

	// Copy values for the highest matching version
	if (read.version >= v::a3)
		(config_values<v::a3>&)*this = (config_values<v::a3>&)read;
	else if (read.version >= v::a2)
		(config_values<v::a2>&)*this = (config_values<v::a2>&)read;
	else
		(config_values<v::a1>&)*this = (config_values<v::a1>&)read;

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
		card_write(CARD_SLOTA, filename, this, sizeof(*this));
}

extern "C" void orig_MemoryCard_CheckToSaveData();
extern "C" void hook_MemoryCard_CheckToSaveData()
{
	if (save_pending && !is_card_busy()) {
		save_pending = false;
		card_write(CARD_SLOTA, filename, &config, sizeof(config));
	}

	orig_MemoryCard_CheckToSaveData();
}