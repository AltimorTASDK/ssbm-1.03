#include "rules/saved_config.h"
#include "util/gc/memcard.h"

constexpr char filename[] = "103Config";

saved_config::saved_config()
{
	card_read(CARD_SLOTA, filename, this, sizeof(*this));
	card_sync();
	
	GetGameRules()->widescreen = widescreen;
	GetGameRules()->latency = latency;
}

void saved_config::save()
{
	card_write(CARD_SLOTA, filename, this, sizeof(*this));
}