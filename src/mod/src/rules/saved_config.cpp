#include "rules/saved_config.h"
#include "rules/values.h"
#include "util/gc/memcard.h"

constexpr char filename[] = "103Config";

static bool save_pending = false;

saved_config::saved_config()
{
	load();

	GetGameRules()->widescreen       = widescreen;
	GetGameRules()->latency          = latency;
	GetGameRules()->ledge_grab_limit = ledge_grab_limit;
	GetGameRules()->air_time_limit   = air_time_limit;
	GetGameRules()->stock_time_limit = stock_time_limit;
	GetGameRules()->controller_fix   = controller_fix;
	GetGameRules()->stage_mods       = stage_mods;
}

void saved_config::load()
{
	config_values<config_version::current> read = { 0 };
	card_read(CARD_SLOTA, filename, &read, sizeof(read));

	if (card_sync() < 0)
		return;

	using v = config_version;

	// Copy values for the highest matching version
	if (read.version >= v::a2)
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

	widescreen       = GetGameRules()->widescreen;
	latency          = GetGameRules()->latency;
	ledge_grab_limit = GetGameRules()->ledge_grab_limit;
	air_time_limit   = GetGameRules()->air_time_limit;
	stock_time_limit = GetGameRules()->stock_time_limit;
	controller_fix   = GetGameRules()->controller_fix;
	stage_mods       = GetGameRules()->stage_mods;

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