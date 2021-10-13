#include "melee/constants.h"

// Don't display port tags (P1, P2 etc) when idle
extern "C" void orig_LoadPlCo();
extern "C" void hook_LoadPlCo()
{
	orig_LoadPlCo();
	// Disable and fully remove port tags with a patch isntead
	// plco->idle_timer = 0;
}