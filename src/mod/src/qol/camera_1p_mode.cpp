#include "melee/camera.h"
#include "melee/player.h"
#include "melee/stage.h"
#include "util/vector.h"

struct CameraBounds {
	vec2 min;
	vec2 max;
	f32 subjects;
	f32 z_pos;
};

static bool is_singleplayer()
{
	auto count = 0;

	for (auto i = 0; i < 6; i++) {
		if (PlayerBlock_GetSlotType(i) != SlotType_None && ++count > 1)
			return false;
	}

	return true;
}

extern "C" void orig_Camera_GetBounds(CameraBounds *out, CameraMovement *movement);
extern "C" void hook_Camera_GetBounds(CameraBounds *out, CameraMovement *movement)
{
	if (MainCamera.mode != CameraMode_Normal || !is_singleplayer())
		return orig_Camera_GetBounds(out, movement);
		
	// Zoomed out 1P mode camera
	constexpr auto mult = .8f;
		
	*out = {
		.min = vec2(Stage_GetCameraLimitLeft(),  Stage_GetCameraLimitBottom()) * mult,
		.max = vec2(Stage_GetCameraLimitRight(), Stage_GetCameraLimitTop())    * mult
	};
}