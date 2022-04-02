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

constexpr auto format_p = text_builder::formatting_options {
	.line_width   = manual_w,
	.indent       = 2,
	.line_spacing = 12,
	.space_width  = 10,
	.scale        = text_scale_p,
	.justify      = true
};

constexpr auto format_h = text_builder::formatting_options {
	.line_width   = manual_w,
	.indent       = 2,
	.line_spacing = 12,
	.space_width  = 10,
	.scale        = text_scale_h,
	.justify      = true
};

constexpr auto format_table_left = text_builder::formatting_options {
	.indent       = 20,
	.line_spacing = 12,
	.space_width  = 10,
	.scale        = text_scale_p,
	.justify      = true
};

constexpr auto format_table_sep = text_builder::formatting_options {
	.indent       = 168,
	.line_spacing = 12,
	.space_width  = 10,
	.scale        = text_scale_p,
	.justify      = true
};

constexpr auto format_table_right = text_builder::formatting_options {
	.indent       = 178,
	.line_spacing = 12,
	.space_width  = 10,
	.scale        = text_scale_p,
	.justify      = true
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

static Text *manual_text;

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
	return text_builder::format_text<format_h, str>();
}

template<string_literal str>
constexpr auto p()
{
	return text_builder::format_text<format_p, str>();
}

constexpr auto big_br()
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

template<float offset, auto break_count = 1>
constexpr auto adjust_y()
{
	constexpr auto amount = (int)((offset / break_count - 32) * 256);
	if constexpr (amount > 0x7FFF || amount < -0x8000) {
		// Requires more line breaks
		return adjust_y<offset, break_count + 1>();
	} else {
		return for_range<break_count>([]<size_t ...I>() {
			return array_cat(text_builder::spacing<0, (s16)amount>(),
			                 (I, text_builder::br())...);
		});
	}
}

template<string_literal left, string_literal ...right>
constexpr auto table_row()
{
	constexpr auto height = sizeof...(right)      * 32 +
	                       (sizeof...(right) - 1) * format_table_right.line_spacing;

	constexpr auto line_height = 32 + format_table_sep.line_spacing;
	constexpr auto sep_offset = -line_height / 4;
	constexpr auto sep_count = sizeof...(right) * 2;
	constexpr auto sep_spacing = line_height / 2;
	constexpr auto sep_height = sep_count * sep_spacing;

	constexpr auto separators = for_range<sep_count>([]<size_t ...I>() {
		return array_cat((I, array_cat(text_builder::format_text<format_table_sep, "|">(),
		                               adjust_y<sep_spacing>()))...);
	});

	constexpr auto left_offset = height / 2 - 16;

	return array_cat(
		adjust_y<4.f>(),
		adjust_y<left_offset>(),
		text_builder::format_text<format_table_left, left>(),
		adjust_y<-left_offset + sep_offset>(),
		separators,
		adjust_y<-sep_height - sep_offset>(),
		array_cat(text_builder::format_text<format_table_right, right>(),
			  text_builder::br())...,
		adjust_y<4.f>());
}

constexpr auto table_padding()
{
	return adjust_y<24.f>();
}

constexpr auto text_test_data = text_builder::build(
	text_builder::kern(),
	h<"Welcome to Melee 1.03!">(),
	big_br(),
	p<"1.03 is created by Hax$ and Altimor. Visit www.b0xx.com for more information.">(),
	big_br(),
	h<"Polling Drift Fix">(),
	hr(),
	p<"1.03 contains the polling drift fix, which fixes a bug that causes Melee's input "
	  "latency to constantly fluctuate.">(),
	big_br(),
	h<"1.03 Controller Fix">(),
	hr(),
	p<"The 1.03 controller fix applies the following fixes:">(),
	text_builder::br(),
	table_padding(),
	table_row<"Dash Back",            "Dash back is increased to a 2-frame window",
	                                  "and tilt intent is applied.">(),
	table_row<"Doraki Walljump",      "Doraki walljump is increased to a 2-frame",
	                                  "window and tilt intent is applied.">(),
	table_row<"Wiggle Out of Tumble", "Wiggle out of tumble is increased to a 2-",
	                                  "frame window.">(),
	table_row<"Smash DI",             "A tilt input on the first frame of hitlag will",
	                                  "not prevent smash DI from occurring on the",
	                                  "second frame of hitlag. Additionally, the",
	                                  "second frame after entering the smash DI",
	                                  "range counts toward the first smash DI",
	                                  "input.">(),
	table_row<"Dash Out of Crouch",   "Dash out of crouch is increased to a 3-frame",
	                                  "window and SquatRv along the rim is",
	                                  "prevented.">(),
	table_row<"Shield Drop",          "Shield drop's range is maximized along the",
	                                  "rim after roll is shut off.">(),
	table_row<"1.0 Cardinal",         "The rims of the control stick and C-stick's",
	                                  "cardinals clamp to 1.0.">(),
	table_row<"Vertical Throws",      "The range for vertical throws is increased to",
	                                  "> 50. This fix does not apply to Ice Climbers.">(),
	table_padding(),
	p<"B0XX is affected only by the increased timing window on dash out of crouch.">(),
	big_br(),
	p<"Credits to tauKhan for designing the dash back fix.">(),
	big_br(),
	h<"Stage Modifications">(),
	hr(),
	p<"1.03 allows you to apply the following stage modifications:">(),
	text_builder::br(),
	table_padding(),
	table_row<"Dreamland",          "Wind is removed.">(),
	table_row<"Final Destination",  "Background visuals are locked to the starry",
	                                "sky.">(),
	table_row<"Fountain of Dreams", "The side platforms start at equal height.">(),
	table_row<u"Pokémon Stadium",   "Transformations are removed.">(),
	table_row<"Yoshi's Story",      "Shy Guys are removed.">(),
	text_builder::br(),
	table_padding(),
	h<"Crew Battle">(),
	hr(),
	p<"Melee's signature exhibition is made better than ever before by 1.03's crew battle "
	  "mode, which features a stock storage system that keeps track of how many stocks are "
	  "remaining at the end of a match.">(),
	big_br(),
	h<"Auto Pause">(),
	hr(),
	p<"1.03's auto pause feature allows you to turn off pause in 4-stock matches specifically "
	  "so that you can freely enter timed matches while still being able to pause.">(),
	big_br(),
	h<"In-Game Shortcuts">(),
	hr(),
	p<"The following shortcuts speed up the process of exiting or restarting a match:">(),
	text_builder::br(),
	table_padding(),
	table_row<"Hold Start",     "L R A Start">(),
	table_row<"Hold Start + B", "L R A Start + Rematch">(),
	text_builder::br(),
	table_padding(),
	h<"Character Select Screen Shortcuts">(),
	hr(),
	p<"The following settings can be toggled at the character select screen by performing the "
	  "corresponding inputs:">(),
	text_builder::br(),
	table_padding(),
	table_row<"Rumble", "D-Pad Up">(),
	table_row<"Z Jump", "X + Z or Y + Z (hold for one second)">(),
	text_builder::br(),
	table_padding(),
	h<"Perfect Angles">(),
	hr(),
	p<"1.03's perfect angles mod modernizes wavedashing by allowing you to airdodge at the "
	  "shallowest angle by pointing horizontally. Your control stick must reach X .8000 or "
	  "greater (the same cut-off as dash and F-smash) for this mod to take effect. "
	  "Additionally, 1.03's perfect angles mod converts exactly half of the cardinal rim  to "
	  "the perfect angle when using a directional up-B.">(),
	big_br(),
	p<"B0XX is affected only by the perfect wavedash aspect of this mod.">(),
	big_br(),
	h<"Latency Toggles">(),
	hr(),
	p<"1.03 contains two custom latency toggles in addition to the default CRT toggle. The LCD "
	  "toggle is intended to counteract the innate latency of an LCD monitor by reducing "
	  "Melee's latency by half a frame. The LOW toggle reduces Melee's latency by one and a "
	  "half frames (the most a Wii can handle) so that you can play Melee at lighting-fast "
	  "speed.">(),
	big_br(),
	h<"Tournament Lock">(),
	hr(),
	p<"To lock all settings that have been chosen in the VS. Mode options menu, remove your "
	  "1.03 memory card and reset your console. This will also prohibit entering the Debug "
	  "Menu and reset Stage Music to the default songs.">(),
	big_br(),
	h<"1.02 Mechanics">(),
	hr(),
	p<"1.03 converts all mechanics that are relevant to tournament play to their NTSC 1.02 "
	  "iterations.">(),
	big_br(),
	h<"Version Compatibility">(),
	hr(),
	p<"1.03 is compatible with NTSC 1.00, 1.01, and 1.02, as well as PAL, 20XX, and "
	  "UnclePunch.">());

extern "C" void Scene_Initialize(SceneMinorData *data);

const auto patches = patch_list {
	// Increase menu text heap size
	// lis r3, 0x20
	std::pair { (char*)Scene_Initialize+0x60, 0x3C600040u },
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
	manual_text->trans.y = manual_y - scroll_offset;
}

static void draw_manual(HSD_GObj *gobj, u32 pass)
{
	GObj_GXProcCamera(gobj, pass);

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
	const auto manual_cobjdesc = HSD_CObjDesc {
		.projection_type = ProjType_Ortho,
		.viewport_right  = 640,
		.viewport_bottom = 480,
		.scissor_left    = scissor_x,
		.scissor_right   = scissor_x + scissor_w,
		.scissor_top     = scissor_y,
		.scissor_bottom  = scissor_y + scissor_h,
		.eye_position    = &canvas_eye,
		.interest        = &canvas_interest,
		.far             = 65535,
		.ortho = {
			.bottom  = -480,
			.right   = 640
		}
	};

	const auto manual_cobjdesc_wide = HSD_CObjDesc {
		.projection_type = ProjType_Ortho,
		.viewport_right  = 640,
		.viewport_bottom = 480,
		.scissor_left    = scissor_x,
		.scissor_right   = scissor_x + scissor_w,
		.scissor_top     = scissor_y,
		.scissor_bottom  = scissor_y + scissor_h,
		.eye_position    = &canvas_eye,
		.interest        = &canvas_interest,
		.far             = 65535,
		.ortho = {
			.bottom  = -480,
			.left    = ortho_left_wide,
			.right   = ortho_right_wide,
		}
	};

	constexpr auto gxlink = 20;
	auto *canvas_gobj = GObj_Create(GOBJ_CLASS_TEXT, GOBJ_PLINK_MENU_CAMERA, 0);
	auto *canvas_cobj = HSD_CObjLoadDesc(is_widescreen() ? &manual_cobjdesc_wide
	                                                     : &manual_cobjdesc);
	GObj_InitKindObj(canvas_gobj, GOBJ_KIND_CAMERA, canvas_cobj);
	GObj_SetupCameraGXLink(canvas_gobj, draw_manual, 19);
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
	manual_text = Text_Create(0, canvas, manual_x, manual_y, 0, 640, 480);
	Text_SetFromSIS(manual_text, 0);
	manual_text->data = text_test_data.data();
}