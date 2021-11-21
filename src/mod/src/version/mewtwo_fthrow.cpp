#ifdef NTSC100

#include "hsd/gobj.h"
#include "util/vector.h"

extern "C" void Item_SetUngrabbable(HSD_GObj *gobj);

extern "C" HSD_GObj *orig_Player_Mewtwo_CreateFThrowBall(
	HSD_GObj *owner, const vec3 &position, f32 rotation, f32 direction, u32 id, u32 flags);
extern "C" HSD_GObj *hook_Player_Mewtwo_CreateFThrowBall(
	HSD_GObj *owner, const vec3 &position, f32 rotation, f32 direction, u32 id, u32 flags)
{
	auto *gobj = orig_Player_Mewtwo_CreateFThrowBall(owner, position, rotation, direction, id,
	                                                 flags);

	// Prevent catching Mewtwo's fthrow projectiles like 1.01+
	if (gobj != nullptr)
		Item_SetUngrabbable(gobj);

	return gobj;
}

#endif