#include "melee/player.h"

// Called from tumble.S
extern "C" bool check_tumble_wiggle(Player *player)
{
	return false;
}