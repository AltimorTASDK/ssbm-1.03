#include "hsd/archive.h"
#include "hsd/cobj.h"
#include "hsd/fog.h"
#include "hsd/gobj.h"
#include "hsd/jobj.h"
#include "hsd/lobj.h"
#include "hsd/archive.h"
#include "melee/menu.h"
#include "melee/music.h"

struct TournamentModels {
	ArchiveModelScene *option;
	ArchiveModelScene *selection;
	ArchiveModelScene *footer;
	ArchiveModelScene *arrows;
	ArchiveModelScene *panel;
	ArchiveModelScene *background;
};

using scene_type = ArchiveScene<TournamentModels>;

static HSD_Archive *GmTou1p;
static HSD_Archive *MnExtAll;
static scene_type *scene;

static void create_camera()
{
	auto *gobj = GObj_Create(GOBJ_CLASS_CAMERA, GOBJ_PLINK_MENU_CAMERA, 0);
	auto *cobj = HSD_CObjLoadDesc(scene->cameras[0]);
	GObj_InitKindObj(gobj, GOBJ_KIND_JOBJ, cobj);
	GObj_SetupCameraGXLink(gobj, GObj_GXProcCamera, 0);

	// Render mask
	gobj->gxlink_prios =
		(1 << GOBJ_GXLINK_FOG) |
		(1 << GOBJ_GXLINK_LIGHT) |
		(1 << GOBJ_GXLINK_MENU_BG);
}

static void create_lights()
{
	auto *gobj = GObj_Create(GOBJ_CLASS_LIGHT, GOBJ_PLINK_MENU_SCENE, 0);
	auto *lobj = HSD_LObjLoadDescList(scene->lights);
	GObj_InitKindObj(gobj, GOBJ_KIND_LIGHT, lobj);
	GObj_SetupGXLink(gobj, GObj_GXProcLight, GOBJ_GXLINK_LIGHT, 0);
}

static void create_fog()
{
	auto *gobj = GObj_Create(GOBJ_CLASS_UI, GOBJ_PLINK_MENU_SCENE, 0);
	auto *fog = HSD_FogLoadDesc(scene->fogs[0]);
	GObj_InitKindObj(gobj, GOBJ_KIND_FOG, fog);
	GObj_SetupGXLink(gobj, GObj_GXProcFog, GOBJ_GXLINK_FOG, 0);
}

static HSD_GObj *create_model(ArchiveModelScene *model,
                              bool hidden, bool animated, u32 anim_index, f32 anim_frame,
                              GObjProcCallback callback,
                              u8 plink = GOBJ_PLINK_MENU_SCENE, u8 gx_link = GOBJ_GXLINK_MENU_BG)
{
	auto *gobj = GObj_Create(GOBJ_CLASS_UI, plink, 0);
	auto *jobj = HSD_JObjLoadDesc(model->joint);
	GObj_InitKindObj(gobj, GOBJ_KIND_JOBJ, jobj);
	GObj_SetupGXLink(gobj, GObj_GXProcJoint, gx_link, 0);
	
	if (callback != nullptr)
		GObj_CreateProcWithCallback(gobj, callback, 0);
		
	if (animated) {
		HSD_JObjAddSceneAnimByIndex(jobj, model, anim_index);
		HSD_JObjReqAnimAll(jobj, anim_frame);
		HSD_JObjAnimAll(jobj);
	}
		
	if (hidden)
		HSD_JObjSetFlagsAll(jobj, HIDDEN);
		
	return gobj;
}

static void update_background(HSD_GObj *gobj)
{
	HSD_JObjLoopAnim(gobj->get_hsd_obj<HSD_JObj>(), { 0, 800, JOBJ_LOOP_NONE });
}

static void update_panel(HSD_GObj *gobj)
{
	HSD_JObjLoopAnim(gobj->get_hsd_obj<HSD_JObj>(), { 0, 39, JOBJ_LOOP_NONE });
}

static void create_menu()
{
	create_camera();
	create_lights();
	create_fog();
	create_model(scene->models->background, false, true, 0, 0.f, update_background);
	create_model(scene->models->panel,      false, true, 0, 0.f, update_panel);
}

static void Controls_Think()
{
}

static void Controls_Init(void *enter_data)
{
	GmTou1p = HSD_ArchiveLoad("GmTou1p");
	MnExtAll = HSD_ArchiveLoad("MnExtAll");
	scene = HSD_ArchiveGetSymbol<scene_type>(GmTou1p, "ScGamTour_scene_data");
	create_menu();
	
	PlayBGM(Menu_GetBGM());
}

static void Controls_Free(void *exit_data)
{
	HSD_ArchiveFree(GmTou1p);
	HSD_ArchiveFree(MnExtAll);
}

struct set_menu_callbacks {
	set_menu_callbacks()
	{
		// Replace tournament mode
		auto *callbacks = Menu_GetCallbacks(MenuID_Tournament);
		callbacks->think = Controls_Think;
		callbacks->enter = Controls_Init;
		callbacks->exit = Controls_Free;
	}
} set_menu_callbacks;