#include "melee/match.h"
#include "melee/player.h"
#include "melee/scene.h"
#include "melee/text.h"
#include "util/melee/match.h"

constexpr auto winner_color = color_rgba::hex(0xFFD700FF);

extern "C" void set_css_name_color(int port, Text *text)
{
	// Don't display in singleplayer modes
	if (IsSinglePlayerMode())
		return;
	
	if (won_last_match(port))
		Text_SetSubtextColor(text, 0, winner_color);
}