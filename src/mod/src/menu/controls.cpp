#include "hsd/archive.h"
#include "hsd/cobj.h"
#include "hsd/fog.h"
#include "hsd/gobj.h"
#include "hsd/jobj.h"
#include "hsd/lobj.h"
#include "hsd/archive.h"
#include "melee/menu.h"
#include "melee/music.h"

enum class menu_option {
	z_jump,
	perfect_wavedash,
	c_up,
	c_horizontal,
	c_down,
	tap_jump,

	ok,
	reset,
	
	toggle_count = tap_jump + 1
};

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

static struct {
	int selected;
	bool scrolled_left;
	bool scrolled_right;
	
	void reset()
	{
		selected = 0;
		scrolled_left = false;
		scrolled_right = false;
	}
} menu_state;

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

static HSD_GObj *create_model(ArchiveModelScene *model, GObjProcCallback callback,
                              bool hidden = false, bool animated = true,
                              u32 anim_index = 0, f32 anim_frame = 0.f,
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

static float calc_toggle_y_position(int index)
{
	return 11.5f - (float)index * 3.5f;
}

static void set_y_position(HSD_GObj *gobj, float y)
{
	auto *jobj = gobj->get_hsd_obj<HSD_JObj>();
	jobj->position.y = y;
	HSD_JObjSetMtxDirty(jobj);
}

static void update_arrows(HSD_GObj *gobj)
{
	const auto index = gobj->get_primitive<int>();
	auto *jobj = gobj->get_hsd_obj<HSD_JObj>();
	auto *left_arrow = jobj->child;
	auto *right_arrow = left_arrow->next;

	if (index == menu_state.selected) {
		HSD_JObjReqAnimAll(left_arrow,  menu_state.scrolled_left  ? 1.f : 0.f);
		HSD_JObjReqAnimAll(right_arrow, menu_state.scrolled_right ? 1.f : 0.f);
		menu_state.scrolled_left = false;
		menu_state.scrolled_right = false;
	} else {
		HSD_JObjReqAnimAll(left_arrow, 0.f);
		HSD_JObjReqAnimAll(right_arrow, 0.f);
	}
	
	HSD_JObjAnimAll(jobj);
}

static void update_option(HSD_GObj *gobj)
{
	const auto index = gobj->get_primitive<int>();
	auto *jobj = gobj->get_hsd_obj<HSD_JObj>();
	
	if (index == menu_state.selected) {
		HSD_JObjLoopAnim(jobj, { 10, 20, 10 });
	} else {
		HSD_JObjReqAnimAll(jobj, 0.f);
		HSD_JObjAnimAll(jobj);
	}
}

static void update_selection(HSD_GObj *gobj)
{
	auto *jobj = gobj->get_hsd_obj<HSD_JObj>();
	HSD_JObjLoopAnim(jobj, { 0, 100, 0 });

	if (menu_state.selected >= (int)menu_option::toggle_count) {
		HSD_JObjSetFlagsAll(jobj, HIDDEN);
		return;
	}

	HSD_JObjClearFlagsAll(jobj, HIDDEN);
	set_y_position(gobj, calc_toggle_y_position(menu_state.selected));
}

static void create_toggles()
{
	for (auto index = 0; index < (int)menu_option::toggle_count; index++) {
		const auto position = calc_toggle_y_position(index);

		auto *arrows = create_model(scene->models->arrows, update_arrows);
		arrows->set_primitive(index);
		set_y_position(arrows, position);

		auto *option = create_model(scene->models->option, update_option);
		option->set_primitive(index);
		set_y_position(option, position);
	}
}

static void update_background(HSD_GObj *gobj)
{
	HSD_JObjLoopAnim(gobj->get_hsd_obj<HSD_JObj>(), { 0, 801, 0 });
}

static void update_panel(HSD_GObj *gobj)
{
	HSD_JObjLoopAnim(gobj->get_hsd_obj<HSD_JObj>(), { 0, 40, 10 });
}

static void create_menu()
{
	create_camera();
	create_lights();
	create_fog();
	create_model(scene->models->background, update_background);
	create_model(scene->models->panel, update_panel);
	create_model(scene->models->selection, update_selection);
	create_toggles();
	
	menu_state.reset();
}

static void Controls_Think()
{
	Menu_GetButtons(PORT_ALL);
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