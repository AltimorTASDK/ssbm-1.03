#include <gctypes.h>

extern "C" u32 hook_ShouldShowSpecialMessage(u32 index)
{
	return 0;
}

extern "C" u32 hook_ShouldShowTrophyMessage(u32 index)
{
	return 0;
}