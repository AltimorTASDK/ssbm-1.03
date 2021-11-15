#pragma once

#include <gctypes.h>

int get_player_count();
bool is_1v1();

// Sorts port indices
// Used for neutral spawns and centered stock icons
// Returns -1 if not 1v1/2v2
int get_sorted_port_index(u32 port);