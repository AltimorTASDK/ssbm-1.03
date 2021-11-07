#include "hsd/aobj.h"
#include "hsd/cobj.h"
#include "hsd/dobj.h"
#include "hsd/gobj.h"
#include "hsd/jobj.h"
#include "hsd/mobj.h"
#include "hsd/tobj.h"
#include "melee/camera.h"
#include "melee/match.h"
#include "melee/scene.h"
#include "melee/stage.h"
#include "menu/stage_select.h"
#include "rules/values.h"
#include "util/math.h"
#include "util/matrix.h"
#include <cstring>
#include <ogc/gx.h>

struct FountainReflectionData {
	matrix3x4 texture_matrix;
	HSD_ImageDesc *image;
};

static bool should_use_simple_fod()
{
#if 0
	// Force simple FoD in low latency mode
	if (get_latency() == latency_mode::low)
		return true;
#endif
	
	return MatchInfo_IsTeams() && SceneMajor != Scene_Training;
}

extern "C" void orig_Stage_Fountain_UpdateReflection(HSD_GObj *gobj);
extern "C" void hook_Stage_Fountain_UpdateReflection(HSD_GObj *gobj)
{
	// Disable reflection in teams
	if (!should_use_simple_fod()) {
		orig_Stage_Fountain_UpdateReflection(gobj);
		return;
	}

#if 0
	// Needs to invert eye pos/interest Y value like original code

	// Update texture matrix
	auto *cobj = MainCamera.gobj->get_hsd_obj<HSD_CObj>();
	const auto inv_fov_tan = 1.f / std::tan(deg_to_rad(cobj->perspective.fov / 2.f));
	const auto aspect = cobj->perspective.aspect;
	const auto x_scale = inv_fov_tan * aspect * .49f;
	const auto y_scale = inv_fov_tan * -.49f;
	const auto z_scale = -.5f;

	const auto proj_matrix = matrix3x3 {
		x_scale, 0.f,     z_scale,
		0.f,     y_scale, z_scale,
		0.f,     0.f,     -1.f
	};

	matrix3x4 view_matrix;
	HSD_CObjGetViewingMtx(cobj, view_matrix.as_multidimensional());

	auto *data = gobj->get<FountainReflectionData>();
	data->texture_matrix = proj_matrix * view_matrix;
#endif
}

extern "C" HSD_GObj *orig_Stage_Fountain_CreateReflection();
extern "C" HSD_GObj *hook_Stage_Fountain_CreateReflection()
{
	auto *gobj = orig_Stage_Fountain_CreateReflection();

	if (!should_use_simple_fod())
		return gobj;

	// Initialize the reflection buffer to white
	const auto *image = gobj->get<FountainReflectionData>()->image;
	const auto size = GX_GetTexBufferSize(image->width, image->height, image->format, 0, 0);
	memset(image->img_ptr, 0xFF, size);
	
	return gobj;
}

extern "C" void orig_Stage_Fountain_CreateStars();
extern "C" void hook_Stage_Fountain_CreateStars()
{
	// Disable stars in teams
	if (!should_use_simple_fod())
		orig_Stage_Fountain_CreateStars();
}

extern "C" void orig_Stage_Fountain_SetupModel(HSD_GObj *gobj);
extern "C" void hook_Stage_Fountain_SetupModel(HSD_GObj *gobj)
{
	if (!use_og_stage_select) {
		// Use the average of the default platform heights (20 and 28)
		auto *params = Stage_GetParameters();
		params->fod.left_plat_height = 24.f;
		params->fod.right_plat_height = 24.f;
	}

	orig_Stage_Fountain_SetupModel(gobj);

	if (!should_use_simple_fod())
		return;
		
	// Enable shadows on water with reflections disabled
	auto *jobj = gobj->get_hsd_obj<HSD_JObj>();
	auto *water_jobj = HSD_JObjGetFromTreeByIndex(jobj->child, 9);
	water_jobj->u.dobj->mobj->rendermode |= RENDER_SHADOW;
}

extern "C" void orig_Stage_Setup(StageIDPair *id);
extern "C" void hook_Stage_Setup(StageIDPair *id)
{
	if (id->grkind == Stage_FoD && should_use_simple_fod()) {
		// Remove FoD particles in teams
		Stage.map_ptcl = nullptr;
		Stage.map_texg = nullptr;
	}
	
	orig_Stage_Setup(id);
}