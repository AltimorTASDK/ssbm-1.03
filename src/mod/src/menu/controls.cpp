#include "hsd/archive.h"
#include "hsd/cobj.h"
#include "hsd/fog.h"
#include "hsd/gobj.h"
#include "hsd/jobj.h"
#include "hsd/lobj.h"
#include "hsd/archive.h"
#include "melee/menu.h"
#include "melee/music.h"
#include "melee/scene.h"
#include "melee/text.h"
#include "util/math.h"
#include "util/melee/text_builder.h"

enum class menu_option {
	z_jump,
	perfect_wavedash,
	c_up,
	c_horizontal,
	c_down,
	tap_jump,

	max_toggle,

	ok = max_toggle,
	redo,
	
	max
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
static int canvas;

static struct {
	int selected;
	int left_arrow_timer;
	int right_arrow_timer;
	int footer_timer;
	
	Text *label_text[(int)menu_option::max_toggle];
	Text *value_text[(int)menu_option::max_toggle];
	
	void reset()
	{
		selected = 0;
		left_arrow_timer = 0;
		right_arrow_timer = 0;
		footer_timer = 0;
	}
	
	void exit()
	{
		Menu_PlaySFX(MenuSFX_Exit);
		Scene_SetMajorPending(Scene_Menu);
		Scene_Exit();
	}

	void scroll_up()
	{
		if (selected == 0)
			selected = (int)menu_option::ok;
		else if (selected >= (int)menu_option::max_toggle)
			selected = (int)menu_option::max_toggle - 1;
		else
			selected--;
	}
	
	void scroll_down()
	{
		if (selected >= (int)menu_option::max_toggle)
			selected = 0;
		else
			selected++;
	}

	void pressed_a()
	{
		if (selected == (int)menu_option::redo)
			Menu_PlaySFX(MenuSFX_Activated);
		else if (selected == (int)menu_option::ok) {
			Menu_PlaySFX(MenuSFX_Activated);
			exit();
		}
	}

	void pressed_left()
	{
		if (selected >= (int)menu_option::max_toggle) {
			selected = clamp(selected - 1, (int)menu_option::max_toggle,
			                               (int)menu_option::max - 1);
		} else if (selected != (int)menu_option::ok) {
			left_arrow_timer = 5;
			Menu_PlaySFX(MenuSFX_Activated);
		}
	}

	void pressed_right()
	{
		if (selected == (int)menu_option::ok) {
			selected = clamp(selected + 1, (int)menu_option::max_toggle,
			                               (int)menu_option::max - 1);
		} else if (selected != (int)menu_option::redo) {
			right_arrow_timer = 5;
			Menu_PlaySFX(MenuSFX_Activated);
		}
	}
} menu_state;

template<string_literal str, u16 scale_x, u16 scale_y>
static constexpr auto make_text()
{
	return text_builder::build(
		text_builder::kern(),
		text_builder::textbox<scale_x, scale_y>(),
		text_builder::fit(),
		text_builder::ascii<str>(),
		text_builder::end_fit(),
		text_builder::end_textbox());
}

template<string_literal str, u16 scale_x = 153>
static constexpr auto make_label_text()
{
	return make_text<str, scale_x, 163>();
}

template<string_literal str, u16 scale_x = 153>
static constexpr auto make_value_text()
{
	return make_text<str, scale_x, 153>();
}

const auto label_text = std::make_tuple(
	make_label_text<"Z Jump">(),
	make_label_text<"Perfect Wavedash", 134>(),
	make_label_text<"C-Stick Up">(),
	make_label_text<"C-Stick Horizontal", 124>(),
	make_label_text<"C-Stick Down">(),
	make_label_text<"Tap Jump">());

constexpr auto label_text_data = for_range<sizeof_tuple<decltype(label_text)>>([]<size_t ...I>() {
		return std::array { (std::get<I>(label_text).data())... };
	});

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
	
	if (index != menu_state.selected) {
		HSD_JObjReqAnimAll(jobj, 0.f);
		HSD_JObjAnimAll(jobj);
		return;
	}

	if (menu_state.left_arrow_timer == 0) {
		HSD_JObjReqAnimAll(left_arrow, 0.f);
	} else {
		HSD_JObjReqAnimAll(left_arrow, 1.f);
		menu_state.left_arrow_timer--;
	}

	if (menu_state.right_arrow_timer == 0) {
		HSD_JObjReqAnimAll(right_arrow, 0.f);
	} else {
		HSD_JObjReqAnimAll(right_arrow, 1.f);
		menu_state.right_arrow_timer--;
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

	if (menu_state.selected >= (int)menu_option::max_toggle) {
		HSD_JObjSetFlagsAll(jobj, HIDDEN);
		return;
	}

	HSD_JObjClearFlagsAll(jobj, HIDDEN);
	set_y_position(gobj, calc_toggle_y_position(menu_state.selected));
}

static void create_toggles()
{
	for (auto index = 0; index < (int)menu_option::max_toggle; index++) {
		const auto position = calc_toggle_y_position(index);

		auto *arrows = create_model(scene->models->arrows, update_arrows);
		arrows->set_primitive(index);
		set_y_position(arrows, position);

		auto *option = create_model(scene->models->option, update_option);
		option->set_primitive(index);
		set_y_position(option, position);
	}
}

static void update_footer(HSD_GObj *gobj)
{
	auto *jobj = gobj->get_hsd_obj<HSD_JObj>();
	auto [redo, ok] = HSD_JObjGetFromTreeTuple<1, 5>(jobj);
	
	float flash_frame = (float)menu_state.footer_timer;
	HSD_JObjReqAnimAll(ok,   menu_state.selected == (int)menu_option::ok   ? flash_frame : 0);
	HSD_JObjReqAnimAll(redo, menu_state.selected == (int)menu_option::redo ? flash_frame : 0);
	HSD_JObjAnimAll(ok);
	HSD_JObjAnimAll(redo);
	
	menu_state.footer_timer = (menu_state.footer_timer + 1) % 20;
}

static void create_footer()
{
	auto *footer = create_model(scene->models->footer, update_footer);
	auto *jobj = footer->get_hsd_obj<HSD_JObj>();
	auto [redo, ok, options] = HSD_JObjGetFromTreeTuple<1, 5, 7>(jobj);
	
	// Remove Options
	HSD_JObjSetFlagsAll(options, HIDDEN);
	
	// Display centered with spacing matching vanilla
	ok->position.x   = -4.5f;
	redo->position.x =  4.5f;
	HSD_JObjSetMtxDirty(ok);
	HSD_JObjSetMtxDirty(redo);
}

static void create_text()
{
	canvas = Text_CreateCanvas(0, nullptr, GOBJ_CLASS_TEXT, GOBJ_PLINK_MENU_CAMERA, 0,
	                           GOBJ_GXLINK_MENU_FG, 0, 19);
				   
	for (auto index = 0; index < (int)menu_option::max_toggle; index++) {
		const auto y_pos = 113.f + (float)index * 35.7f;
		menu_state.label_text[index] = Text_Create(0, canvas, 123, y_pos, 0, 360, 40);
		menu_state.value_text[index] = Text_Create(0, canvas, 343, y_pos, 0, 360, 40);
		Text_SetFromSIS(menu_state.label_text[index], 0);
		Text_SetFromSIS(menu_state.value_text[index], 0);
		menu_state.label_text[index]->data = label_text_data[index];
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
	create_footer();
	create_text();
	
	menu_state.reset();
}

static void Controls_Think()
{
	const auto buttons = Menu_GetButtons(PORT_ALL);

	if (buttons & MenuButton_B) {
		menu_state.exit();
		return;
	}

	if (buttons & MenuButton_Up)
		menu_state.scroll_up();
	
	if (buttons & MenuButton_Down)
		menu_state.scroll_down();

	if (buttons & MenuButton_Left)
		menu_state.pressed_left();

	if (buttons & MenuButton_Right)
		menu_state.pressed_right();

	if (buttons & MenuButton_A)
		menu_state.pressed_a();
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
	Text_FreeAll();
}

struct set_menu_callbacks {
	set_menu_callbacks()
	{
		// Replace tournament mode
		auto *callbacks = Menu_GetCallbacks(MenuID_Controls);
		callbacks->think = Controls_Think;
		callbacks->enter = Controls_Init;
		callbacks->exit = Controls_Free;
	}
} set_menu_callbacks;