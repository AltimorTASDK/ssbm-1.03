#include "melee/menu.h"

// Check for the flag UnclePunch uses to indicate the OSD menu
inline bool is_unclepunch_osd()
{
	return IsEnteringMenu == 2;
}