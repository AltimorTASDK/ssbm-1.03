#include "hsd/gobj.h"
#include "melee/camera.h"
#include "melee/player.h"
#include "melee/scene.h"
#include "melee/stage.h"
#include "util/math.h"
#include "util/vector.h"

struct CameraBounds {
	vec2 min;
	vec2 max;
	f32 subjects;
	f32 z_pos;
};

static bool is_1p_mode()
{
	if (SceneMajor != Scene_VsMode)
		return false;

	auto count = 0;

	for (auto i = 0; i < 6; i++) {
		if (PlayerBlock_GetSlotType(i) != SlotType_None && ++count > 1)
			return false;
	}

	return true;
}

extern "C" void orig_Camera_SetNormal();
extern "C" void hook_Camera_SetNormal()
{
	if (is_1p_mode())
		MainCamera.mode = CameraMode_Fixed;
	else
		MainCamera.mode = CameraMode_Normal;
}

#ifdef USE_CUSTOM_1P_CAMERA
extern "C" void orig_Camera_GetBounds(CameraBounds *out, CameraMovement *movement);
extern "C" void hook_Camera_GetBounds(CameraBounds *out, CameraMovement *movement)
{
	orig_Camera_GetBounds(out, movement);

	if (MainCamera.mode != CameraMode_Normal || !is_1p_mode())
		return;

	// Always keep center stage in view
	const auto limits_min = vec2(Stage_GetCameraLimitLeft(),  Stage_GetCameraLimitBottom());
	const auto limits_max = vec2(Stage_GetCameraLimitRight(), Stage_GetCameraLimitTop());
	const auto center = (limits_min + limits_max) / 2;

	constexpr auto limit_scale = .75f;
	const auto scaled_min = lerp(center, limits_min, limit_scale);
	const auto scaled_max = lerp(center, limits_max, limit_scale);
	const auto new_min = vec2::min(out->min, scaled_min);
	const auto new_max = vec2::max(out->max, scaled_max);

	constexpr auto lerp_strength = .5f;
	out->min = lerp(out->min, new_min, lerp_strength);
	out->max = lerp(out->max, new_max, lerp_strength);
}
#endif

extern "C" bool orig_Player_CheckForDeath(HSD_GObj *gobj);
extern "C" bool hook_Player_CheckForDeath(HSD_GObj *gobj)
{
	if (!orig_Player_CheckForDeath(gobj))
		return false;

	// Force instant respawn in 1P mode
	if (is_1p_mode())
		gobj->get<Player>()->as_data.Dead.respawn_timer = 1;

	return true;
}

extern "C" void orig_Player_StopAllSFX(Player *player);
extern "C" void hook_Player_StopAllSFX(Player *player)
{
	// Prevent instant respawns from cutting off death SFX
	if (!is_1p_mode())
		Player_StopAllSFX(player);
}