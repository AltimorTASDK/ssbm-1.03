#include "hsd/archive.h"
#include "hsd/cobj.h"
#include "hsd/gobj.h"
#include "hsd/jobj.h"
#include "hsd/mobj.h"
#include "hsd/pad.h"
#include "hsd/tobj.h"
#include "hsd/video.h"
#include "melee/menu.h"
#include "melee/scene.h"
#include "melee/text.h"
#include "rules/values.h"
#include "qol/widescreen.h"
#include "util/texture_swap.h"
#include "util/math.h"
#include "util/mempool.h"
#include "util/patch_list.h"
#include "util/draw/render.h"
#include "util/draw/texture.h"
#include "util/melee/pad.h"
#include "util/melee/text_builder.h"
#include <cmath>
#include <gctypes.h>

#if 0
#ifdef NOPAL
#include "resources/manual/a/manual_p1.tex.h"
#include "resources/manual/a/manual_p2.tex.h"
#else
#include "resources/manual/b/manual_p1.tex.h"
#include "resources/manual/b/manual_p2.tex.h"
#endif
#endif
#include "resources/manual/manual_border.tex.h"
#include "resources/manual/scrollbar.tex.h"

constexpr auto scissor_w = 516;
constexpr auto scissor_h = 288;
constexpr auto scissor_x = 320 - scissor_w / 2;
constexpr auto scissor_y =  88;

constexpr auto manual_margin_x = 25;
constexpr auto manual_margin_y = 13;
constexpr auto manual_x = scissor_x + manual_margin_x;
constexpr auto manual_y = scissor_y + manual_margin_y;
constexpr auto manual_w = scissor_w - manual_margin_x * 2;

constexpr auto text_scale_p = .36250f;
constexpr auto text_scale_h = .46875f;

constexpr auto text_options_p = text_builder::formatting_options {
	.line_width   = manual_w,
	.indent       = 2,
	.line_spacing = 12,
	.space_width  = 10,
	.scale        = text_scale_p,
	.justify      = true
};

constexpr auto text_options_h = text_builder::formatting_options {
	.line_width   = manual_w,
	.indent       = 2,
	.line_spacing = 12,
	.space_width  = 10,
	.scale        = text_scale_h,
	.justify      = true
};

constexpr auto text_options_table_left = text_builder::formatting_options {
	.line_width   = manual_w,
	.indent       = 27,
	.line_spacing = 12,
	.space_width  = 10,
	.scale        = text_scale_p,
	.justify      = false
};

constexpr auto text_options_table_right = text_builder::formatting_options {
	.line_width   = manual_w,
	.indent       = 140,
	.line_spacing = 12,
	.space_width  = 10,
	.scale        = text_scale_p,
	.justify      = false
};

#if 0
static texture texture_p1;
static texture texture_p2;
#endif
static texture texture_border;
static texture texture_scrollbar;

static mempool pool;

static float scroll_offset;
static int scroll_frames;

bool manual_open;

constexpr auto bottom_text = text_builder::build(
	text_builder::kern(),
	text_builder::center(),
	text_builder::color<170, 170, 170>(),
	text_builder::scale<179, 179>(),
	text_builder::offset<0, -10>(),
	text_builder::br(),
	text_builder::type_speed<0, 0>(),
	text_builder::fit(),
	text_builder::text<"Up/Down: Scroll">(),
	text_builder::end_fit(),
	text_builder::reset_scale(),
	text_builder::end_color());

template<string_literal str>
constexpr auto h()
{
	return text_builder::format_text<text_options_h, str>();
}

template<string_literal str>
constexpr auto p()
{
	return text_builder::format_text<text_options_p, str>();
}

constexpr auto br()
{
	return array_cat(text_builder::scale<256, 256>(),
	                 text_builder::spacing<0, 12 * 256>(),
	                 text_builder::br());
}

constexpr auto hr()
{
	constexpr auto scale_x = 32;
	constexpr auto scale_y = 128;
	constexpr auto spacing = -4;
	constexpr auto width = (float)(text_builder::char_width<'-'>() + spacing) * scale_x / 256;
	constexpr auto count = (size_t)(manual_w / width);

	return for_range<count>([]<size_t ...I>() {
		return array_cat(
			text_builder::scale<scale_x, scale_y>(),
			text_builder::spacing<spacing * 256, -10 * 256>(),
			text_builder::br(),
			(I, text_builder::character<'-'>())...,
			text_builder::br());
	});
}

template<string_literal left, string_literal right>
constexpr auto table_row()
{
	return array_cat(
		text_builder::format_text<text_options_table_left, left>(),
		text_builder::spacing<0, -32 * 256>(),
		text_builder::br(),
		text_builder::format_text<text_options_table_right, right>());
}

constexpr auto text_test_data = text_builder::build(
	text_builder::kern(),
	h<"Welcome to Melee 1.03 (Version B4)!">(),
	br(),
	p<"1.03 is created by Hax$ and Altimor. Visit www.b0xx.com for more information.">(),
	br(),
	h<"Polling Drift Fix">(),
	hr(),
	p<"1.03 contains the polling drift fix, which fixes a bug that causes Melee's input "
	  "latency to constantly fluctuate.">(),
	br(),
	h<"1.03 Controller Fix">(),
	hr(),
	p<"The 1.03 controller fix applies the following fixes:">(),
	br(),
	table_row<"Dash Back",
	          "Dash back is increased to a 2-frame window and tilt intent is applied.">());

extern "C" void Scene_Initialize(SceneMinorData *data);

const auto patches = patch_list {
	// Increase menu text heap size
	// lis r3, 1
	std::pair { (char*)Scene_Initialize+0x60, 0x3C600001u },
};

static float max_scroll_offset()
{
#if 0
	const auto total_height = texture_p1.height() + texture_p2.height();
#else
	const auto total_height = 986 + 672;
#endif
	return (float)std::max(total_height - (scissor_h - manual_margin_y * 2), 0);
}

static void manual_input(HSD_GObj *gobj)
{
	const auto buttons = Menu_GetButtons(PORT_ALL);

	if (buttons & MenuButton_B) {
		manual_open = false;
		IsEnteringMenu = false;
		Menu_PlaySFX(MenuSFX_Back);
		Text_FreeAll();
		pool.dec_ref();

		// Frees gobj
		return Menu_MainMenuTransition(MenuType_VsMode, 3, MenuState_ExitTo);
	}

	auto total_y = 0;

	for (auto port = 0; port < 4; port++) {
		const auto y = get_input<0>(port).stick.y;
		// Check deadzone
		if (std::abs(y) > DEADZONE)
			total_y += y;
	}

	if (std::abs(total_y) >= STICK_MAX)
		scroll_frames++;
	else
		scroll_frames = 0;

	const auto ramp_up = inv_lerp((float)scroll_frames, 40.f, 100.f);
	const auto speed = 7.f * (1.f + ramp_up);
	const auto delta = clamp((float)total_y / STICK_MAX, -1.f, 1.f) * speed;
	scroll_offset = clamp(scroll_offset - delta, 0.f, max_scroll_offset());
}

static void draw_manual(HSD_GObj *gobj, u32 pass)
{
	if (pass != HSD_RP_BOTTOMHALF)
		return;

	auto &rs = render_state::get();
	rs.reset_2d();
	rs.set_scissor(scissor_x, scissor_y, scissor_w, scissor_h);

	constexpr auto manual_z = 17;

#if 0
	const auto pos_p1 = vec3(320, manual_y - scroll_offset, manual_z);
	const auto pos_p2 = pos_p1 + vec3(0, texture_p1.height(), 0);

	rs.fill_rect(pos_p1, vec2(texture_p1.size()), color_rgba::white, texture_p1,
	             uv_coord::zero, uv_coord::one, align::top);

	rs.fill_rect(pos_p2, vec2(texture_p2.size()), color_rgba::white, texture_p2,
	             uv_coord::zero, uv_coord::one, align::top);
#endif

	constexpr auto border_color = color_rgba::hex(0x929196FFu);

	rs.fill_tiled_rect(vec3(scissor_x, scissor_y, manual_z), vec2(scissor_w, scissor_h),
	                   border_color, texture_border, align::top_left);

	constexpr auto scrollbar_margin      = vec2(2, 14);
	constexpr auto scrollbar_size        = vec2(11, 6);
	constexpr auto scrollbar_area_right  = scissor_x + scissor_w - scrollbar_margin.x;
	constexpr auto scrollbar_area_top    = scissor_y             + scrollbar_margin.y;
	constexpr auto scrollbar_area_bottom = scissor_y + scissor_h - scrollbar_margin.y;

	const auto scrollbar_y = lerp(scrollbar_area_top,
	                              scrollbar_area_bottom - scrollbar_size.y,
	                              scroll_offset / max_scroll_offset());

	rs.fill_tiled_rect(vec3(scrollbar_area_right, scrollbar_y, manual_z), scrollbar_size,
	                   border_color, texture_scrollbar, align::top_right);
}

static int create_text_canvas()
{
	constexpr auto gxlink = 20;
	auto *canvas_gobj = GObj_Create(GOBJ_CLASS_TEXT, GOBJ_PLINK_MENU_CAMERA, 0);
	auto *canvas_cobj = HSD_CObjLoadDesc(is_widescreen() ? &canvas_cobjdesc_wide
	                                                     : &canvas_cobjdesc);
	GObj_InitKindObj(canvas_gobj, GOBJ_KIND_CAMERA, canvas_cobj);
	GObj_SetupCameraGXLink(canvas_gobj, GObj_GXProcCamera, 19);
	canvas_gobj->gxlink_prios = 1 << gxlink;

	return Text_CreateCanvas(0, canvas_gobj, GOBJ_CLASS_TEXT, GOBJ_PLINK_MENU_CAMERA, 0,
	                         gxlink, 0, 19);
}

extern "C" void hook_Menu_EnterCustomRulesMenu()
{
	manual_open = true;

	scroll_offset = 0;
	scroll_frames = 0;

	MenuTypePrevious = MenuType;
	MenuType = MenuType_Rules;

	auto *gobj = GObj_Create(GOBJ_CLASS_PROC, GOBJ_PLINK_PROC, 0x80);
	GObj_AddProc(gobj, manual_input, 0);
	Menu_SetGObjPrio(gobj);
	GObj_SetupGXLink(gobj, draw_manual, GOBJ_GXLINK_MENU_TOP, 0x80);

	pool.inc_ref();

#if 0
	pool.add(init_texture(manual_p1_tex_data, &texture_p1));
	pool.add(init_texture(manual_p2_tex_data, &texture_p2));
#endif
	pool.add(init_texture(manual_border_tex_data, &texture_border));
	pool.add(init_texture(scrollbar_tex_data, &texture_scrollbar));

	// Bottom text
	auto *text = Text_Create(0, 1, -9.5f, 8.f, 17.f, 364.68331909f, 76.75543640f);
	text->stretch.x = 0.0521f;
	text->stretch.y = 0.0521f;
	Text_SetFromSIS(text, 0);
	text->data = bottom_text.data();

	const auto canvas = create_text_canvas();

	// Test
	auto *text_test = Text_Create(0, canvas, manual_x, manual_y, 0, 640, 480);
	Text_SetFromSIS(text_test, 0);
	text_test->data = text_test_data.data();
}