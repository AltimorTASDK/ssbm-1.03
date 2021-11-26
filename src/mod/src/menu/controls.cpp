#include "controls/config.h"
#include "hsd/archive.h"
#include "hsd/cobj.h"
#include "hsd/fog.h"
#include "hsd/gobj.h"
#include "hsd/jobj.h"
#include "hsd/lobj.h"
#include "hsd/pad.h"
#include "hsd/wobj.h"
#include "melee/menu.h"
#include "melee/music.h"
#include "melee/scene.h"
#include "melee/text.h"
#include "qol/widescreen.h"
#include "rules/values.h"
#include "util/enum_math.h"
#include "util/math.h"
#include "util/melee/text_builder.h"
#include <type_traits>

enum class option {
	min,

	z_jump = min,

	max_locked,

	perfect_wavedash = max_locked,
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

constexpr auto max_toggle_count = std::to_underlying(option::max_toggle);

template<string_literal str, u16 scale_x, u16 scale_y>
static consteval auto make_text()
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
static consteval auto make_label_text()
{
	return make_text<str, scale_x, 163>();
}

template<string_literal str, u16 scale_x = 153>
static consteval auto make_value_text()
{
	return make_text<str, scale_x, 153>();
}

template<string_literal str, u16 scale_x = 225>
static consteval auto make_description_text()
{
	return text_builder::build(
		text_builder::unk05<10>(),
		text_builder::textbox<scale_x, 202>(),
		text_builder::center(),
		text_builder::kern(),
		text_builder::fit(),
		text_builder::ascii<str>(),
		text_builder::end_fit(),
		text_builder::end_textbox());
}

constexpr auto label_text_data = multi_array {
	make_label_text<"Z Jump">(),
	make_label_text<"Perfect Wavedash", 134>(),
	make_label_text<"C-Stick Up">(),
	make_label_text<"C-Stick Horizontal", 124>(),
	make_label_text<"C-Stick Down">(),
	make_label_text<"Tap Jump">()
};

constexpr auto description_text_data = multi_array {
	make_description_text<"Jump with the Z button.">(),
	make_description_text<"Wavedash perfectly.">(),
	make_description_text<"Customize the C-stick.">(),
	make_description_text<"Customize the C-stick.">(),
	make_description_text<"Customize the C-stick.">(),
	make_description_text<"Toggle tap jump.">(),
	make_description_text<"Is this correct?">(),
	make_description_text<"Is this correct?">()
};

constexpr auto value_text_data = multi_array {
	multi_array {
		make_value_text<"Off">(),
		make_value_text<"Swap X/Z">(),
		make_value_text<"Swap Y/Z">()
	},
	multi_array {
		make_value_text<"Off">(),
		make_value_text<"On">()
	},
	multi_array {
		make_value_text<"Smash">(),
		make_value_text<"Tilt">()
	},
	multi_array {
		make_value_text<"Smash">(),
		make_value_text<"Tilt">()
	},
	multi_array {
		make_value_text<"Smash">(),
		make_value_text<"Tilt">()
	},
	multi_array {
		make_value_text<"On">(),
		make_value_text<"Off">()
	}
};

constexpr auto value_counts = for_range<value_text_data.size()>([]<size_t ...I> {
	return std::array { (int)value_text_data.size(I)... };
});

static HSD_Archive *GmTou1p;
static HSD_Archive *MnExtAll;
static scene_type *scene;
static int canvas;

static struct {
	u8 port;
	option max_toggle;
	option selected;
	int left_arrow_timer;
	int right_arrow_timer;
	int footer_timer;
	Text *description_text;

	struct {
		int value;
		Text *label_text;
		Text *value_text;
		int left_arrow_timer;
		int right_arrow_timer;
	} toggles[max_toggle_count];

	void init()
	{
		max_toggle = !get_settings_lock() ? option::max_toggle : option::max_locked;

		selected = option::min;
		footer_timer = 0;

		for (auto index = 0; index < max_toggle; index++) {
			toggles[index].left_arrow_timer = 0;
			toggles[index].right_arrow_timer = 0;
		}

		load_config();
	}

	auto *get_toggle(option index)
	{
		return &toggles[std::to_underlying(index)];
	}

	void update_value_text(int index)
	{
		toggles[index].value_text->data = value_text_data[index][toggles[index].value];
	}

	void update_description()
	{
		description_text->data = description_text_data[std::to_underlying(selected)];
	}

	void set_selected(option value)
	{
		selected = value;
		update_description();
	}

	void exit()
	{
		Scene_SetMajorPending(Scene_Menu);
		Scene_Exit();
	}

	void redo()
	{
		for (auto index = 0; index < max_toggle; index++) {
			toggles[index].value = 0;
			update_value_text(index);
		}

		set_selected(option::min);
	}

	void scroll_up()
	{
		if (selected == option::min)
			set_selected(option::ok);
		else if (selected >= max_toggle)
			set_selected(max_toggle - 1);
		else
			set_selected(selected - 1);

		Menu_PlaySFX(MenuSFX_Scroll);
	}

	void scroll_down()
	{
		if (selected >= max_toggle)
			set_selected(option::min);
		else if (selected == max_toggle - 1)
			set_selected(option::ok);
		else
			set_selected(selected + 1);

		Menu_PlaySFX(MenuSFX_Scroll);
	}

	void pressed_start()
	{
		Menu_PlaySFX(MenuSFX_Scroll);
		set_selected(option::ok);
	}

	void pressed_a()
	{
		if (selected == option::redo) {
			Menu_PlaySFX(MenuSFX_Back);
			redo();
		} else if (selected == option::ok) {
			Menu_PlaySFX(MenuSFX_Activate);
			save_config();
			exit();
		}
	}

	void scroll_left()
	{
		Menu_PlaySFX(MenuSFX_Scroll);

		if (selected >= option::max_toggle) {
			selected = clamp(selected - 1, option::max_toggle,
			                               option::max - 1);
			return;
		}

		const auto index = std::to_underlying(selected);
		toggles[index].value = decrement_mod(toggles[index].value, value_counts[index]);
		toggles[index].left_arrow_timer = 5;
		update_value_text(index);
	}

	void scroll_right()
	{
		Menu_PlaySFX(MenuSFX_Scroll);

		if (selected >= option::max_toggle) {
			selected = clamp(selected + 1, option::max_toggle,
			                               option::max - 1);
			return;
		}

		const auto index = std::to_underlying(selected);
		toggles[index].value = increment_mod(toggles[index].value, value_counts[index]);
		toggles[index].right_arrow_timer = 5;
		update_value_text(index);
	}

	void load_config()
	{
		auto *config = &controller_configs[port];

		get_toggle(option::z_jump)->value =
			config->z_jump_bit == __builtin_ctz(Button_X) ? 1 :
			config->z_jump_bit == __builtin_ctz(Button_Y) ? 2 : 0;

		if (get_settings_lock())
			return;

		get_toggle(option::perfect_wavedash)->value =
			config->perfect_wavedash ? 1 : 0;

		get_toggle(option::c_up)->value =
			config->c_up         == cstick_type::tilt ? 1 : 0;

		get_toggle(option::c_horizontal)->value =
			config->c_horizontal == cstick_type::tilt ? 1 : 0;

		get_toggle(option::c_down)->value =
			config->c_down       == cstick_type::tilt ? 1 : 0;

		get_toggle(option::tap_jump)->value =
			config->tap_jump ? 0 : 1;
	}

	void save_config()
	{
		auto *config = &controller_configs[port];

		config->z_jump_bit = (u8)std::array {
			0,
			__builtin_ctz(Button_X),
			__builtin_ctz(Button_Y)
		}[get_toggle(option::z_jump)->value];

		if (get_settings_lock())
			return;

		config->perfect_wavedash = std::array {
			false,
			true
		}[get_toggle(option::perfect_wavedash)->value];

		config->c_up = std::array {
			cstick_type::smash,
			cstick_type::tilt
		}[get_toggle(option::c_up)->value];

		config->c_horizontal = std::array {
			cstick_type::smash,
			cstick_type::tilt
		}[get_toggle(option::c_horizontal)->value];

		config->c_down = std::array {
			cstick_type::smash,
			cstick_type::tilt
		}[get_toggle(option::c_down)->value];

		config->tap_jump = std::array {
			true,
			false
		}[get_toggle(option::tap_jump)->value];
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
		GObj_AddProc(gobj, callback, 0);

	if (animated) {
		HSD_JObjAddSceneAnimByIndex(jobj, model, anim_index);
		HSD_JObjReqAnimAll(jobj, anim_frame);
		HSD_JObjAnimAll(jobj);
	}

	if (hidden)
		HSD_JObjSetFlagsAll(jobj, HIDDEN);

	return gobj;
}

static float calc_toggle_y_position(option index)
{
	return 11.5f - (!get_settings_lock() ? (float)index : 2.5f) * 3.5f;
}

static void set_y_position(HSD_GObj *gobj, float y)
{
	auto *jobj = gobj->get_hsd_obj<HSD_JObj>();
	jobj->position.y = y;
	HSD_JObjSetMtxDirty(jobj);
}

static void update_arrows(HSD_GObj *gobj)
{
	const auto index = gobj->get_primitive<option>();
	auto *toggle = &menu_state.toggles[std::to_underlying(index)];
	auto *jobj = gobj->get_hsd_obj<HSD_JObj>();
	auto *left_arrow = jobj->child;
	auto *right_arrow = left_arrow->next;

	if (toggle->left_arrow_timer == 0) {
		HSD_JObjReqAnimAll(left_arrow, 0.f);
	} else {
		HSD_JObjReqAnimAll(left_arrow, 1.f);
		toggle->left_arrow_timer--;
	}

	if (toggle->right_arrow_timer == 0) {
		HSD_JObjReqAnimAll(right_arrow, 0.f);
	} else {
		HSD_JObjReqAnimAll(right_arrow, 1.f);
		toggle->right_arrow_timer--;
	}

	HSD_JObjAnimAll(jobj);
}

static void update_option(HSD_GObj *gobj)
{
	const auto index = gobj->get_primitive<option>();
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

	if (menu_state.selected >= option::max_toggle) {
		HSD_JObjSetFlagsAll(jobj, HIDDEN);
		return;
	}

	HSD_JObjClearFlagsAll(jobj, HIDDEN);
	set_y_position(gobj, calc_toggle_y_position(menu_state.selected));
}

static void create_toggles()
{
	for (auto index = option::min; index < menu_state.max_toggle; index++) {
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

	const auto flash_frame = (float)menu_state.footer_timer;
	HSD_JObjReqAnimAll(ok,   menu_state.selected == option::ok   ? flash_frame : 0);
	HSD_JObjReqAnimAll(redo, menu_state.selected == option::redo ? flash_frame : 0);
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

static void create_text_canvas()
{
	auto *canvas_gobj = GObj_Create(GOBJ_CLASS_TEXT, GOBJ_PLINK_MENU_CAMERA, 0);
	auto *canvas_cobj = HSD_CObjLoadDesc(is_widescreen() ? &canvas_cobjdesc_wide
	                                                     : &canvas_cobjdesc);
	GObj_InitKindObj(canvas_gobj, GOBJ_KIND_CAMERA, canvas_cobj);
	GObj_SetupCameraGXLink(canvas_gobj, GObj_GXProcCamera, 19);
	canvas_gobj->gxlink_prios = 1 << GOBJ_GXLINK_MENU_FG;

	canvas = Text_CreateCanvas(0, canvas_gobj, GOBJ_CLASS_TEXT, GOBJ_PLINK_MENU_CAMERA, 0,
	                           GOBJ_GXLINK_MENU_FG, 0, 19);
}

static void create_text()
{
	menu_state.description_text = Text_Create(0, canvas, 124.5f, 45, 0, 391, 30);
	Text_SetFromSIS(menu_state.description_text, 0);
	menu_state.update_description();

	for (auto index = 0; index < menu_state.max_toggle; index++) {
		const auto y_pos = 113.f + (!get_settings_lock() ? (float)index : 2.5f) * 35.7f;
		auto *toggle = &menu_state.toggles[index];

		toggle->label_text = Text_Create(0, canvas, 123, y_pos, 0, 360, 40);
		Text_SetFromSIS(toggle->label_text, 0);
		toggle->label_text->data = label_text_data[index];

		toggle->value_text = Text_Create(0, canvas, 343, y_pos, 0, 360, 40);
		Text_SetFromSIS(toggle->value_text, 0);
		menu_state.update_value_text(index);
	}
}

static void update_background(HSD_GObj *gobj)
{
	HSD_JObjLoopAnim(gobj->get_hsd_obj<HSD_JObj>(), { 0, 800, 0 });
}

static void update_panel(HSD_GObj *gobj)
{
	HSD_JObjLoopAnim(gobj->get_hsd_obj<HSD_JObj>(), { 0, 40, 10 });
}

static void create_menu()
{
	menu_state.init();

	create_camera();
	create_lights();
	create_fog();
	create_model(scene->models->background, update_background);
	create_model(scene->models->panel, update_panel);
	create_model(scene->models->selection, update_selection);
	create_toggles();
	create_footer();
	create_text_canvas();
	create_text();
}

static void Controls_Think()
{
	const auto buttons = Menu_GetButtons(menu_state.port);

	// Exit menu on B press or unplug
	if (HSD_PadCopyStatus[menu_state.port].err != 0 || (buttons & MenuButton_B)) {
		Menu_PlaySFX(MenuSFX_Back);
		menu_state.exit();
		return;
	}

	// Use start on toggles to jump to OK
	if (menu_state.selected < option::max_toggle && (buttons & MenuButton_Start)) {
		menu_state.pressed_start();
		return;
	}

	if (buttons & MenuButton_Up)
		menu_state.scroll_up();

	if (buttons & MenuButton_Down)
		menu_state.scroll_down();

	if (buttons & MenuButton_Left)
		menu_state.scroll_left();

	if (buttons & MenuButton_Right)
		menu_state.scroll_right();

	if (buttons & MenuButton_Confirm)
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

void store_controls_menu_port()
{
	// Remember who entered the Controls menu
	for (u8 i = 0; i < 4; i++) {
		if (Menu_GetButtons(i) & MenuButton_Confirm) {
			menu_state.port = i;
			break;
		}
	}
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