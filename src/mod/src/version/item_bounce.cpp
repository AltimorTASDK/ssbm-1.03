#ifdef NTSC100

#include "melee/item.h"
#include "util/vector.h"
#include <cmath>

extern "C" void normalize_item_bounce_velocity(const Item *item, vec2 *velocity)
{
	// Normalize after low magnitude check like 1.02 rather than before
	if (velocity->length() < .01f)
		*velocity = vec2(item->velocity.x, -item->velocity.y).normalized();
	else
		*velocity = velocity->normalized();
}

#endif