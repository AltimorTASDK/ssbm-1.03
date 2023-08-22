#include "hsd/archive.h"
#include "hsd/cobj.h"
#include "hsd/gobj.h"
#include "hsd/jobj.h"
#include "hsd/mobj.h"
#include "hsd/pad.h"
#include "hsd/tobj.h"
#include "hsd/video.h"
#include "melee/menu.h"
#include "melee/text.h"
#include "util/texture_swap.h"
#include "util/math.h"
#include "util/mempool.h"
#include "util/draw/render.h"
#include "util/draw/texture.h"
#include "util/melee/pad.h"
#include "util/melee/text_builder.h"
#include <cmath>
#include <gctypes.h>

#ifdef TOURNAMENT
#ifdef DOL
#include "resources/manual/te/dol/manual_p1.tex.h"
#include "resources/manual/te/dol/manual_p2.tex.h"
#else
#include "resources/manual/te/manual_p1.tex.h"
#include "resources/manual/te/manual_p2.tex.h"
#endif
#else
#ifdef DOL
#include "resources/manual/le/dol/manual_p1.tex.h"
#include "resources/manual/le/dol/manual_p2.tex.h"
#else
#include "resources/manual/le/manual_p1.tex.h"
#include "resources/manual/le/manual_p2.tex.h"
#endif
#endif
#include "resources/manual/manual_border.tex.h"
#include "resources/manual/scrollbar.tex.h"

constexpr auto scissor_w = 516;
constexpr auto scissor_h = 307;
constexpr auto scissor_x = 320 - scissor_w / 2;
constexpr auto scissor_y =  88;

constexpr auto manual_margin = 13;

static texture texture_p1;
static texture texture_p2;
static texture texture_border;
static texture texture_scrollbar;

static mempool pool;

static float scroll_offset;
static int scroll_frames;

static Text *text;

bool manual_open;

constexpr auto bottom_text = text_builder::build(
	text_builder::kern(),
	text_builder::center(),
	text_builder::color<170, 170, 170>(),
	text_builder::scale<179, 179>(),
	text_builder::offset<0, -1>(),
	text_builder::br(),
	text_builder::type_speed<0, 0>(),
	text_builder::fit(),
	text_builder::text<"Up/Down: Scroll">(),
	text_builder::end_fit(),
	text_builder::reset_scale(),
	text_builder::end_color());

static float max_scroll_offset()
{
	const auto total_height = texture_p1.height() + texture_p2.height();
	return (float)std::max(total_height - (scissor_h - manual_margin * 2), 0);
}

static void manual_input(HSD_GObj *gobj)
{
	const auto buttons = Menu_GetButtons(PORT_ALL);

	if (buttons & MenuButton_B) {
		manual_open = false;
		IsEnteringMenu = false;
		Menu_PlaySFX(MenuSFX_Back);
		Text_Free(text);
		pool.dec_ref();

		// Frees gobj
		return Menu_MainMenuTransition(MenuType_VsMode, VsMenu_Manual, MenuState_ExitTo);
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

	const auto pos_p1 = vec3(320, scissor_y + manual_margin - scroll_offset, manual_z);
	const auto pos_p2 = pos_p1 + vec3(0, texture_p1.height(), 0);

	rs.fill_rect(pos_p1, vec2(texture_p1.size()), texture_p1, align::top);
	rs.fill_rect(pos_p2, vec2(texture_p2.size()), texture_p2, align::top);

	constexpr auto border_color = color_rgba::hex(0x929196FFu);

	rs.fill_tiled_rect({scissor_x, scissor_y, manual_z}, {scissor_w, scissor_h},
	                   border_color, texture_border, align::top_left);

	constexpr auto scrollbar_margin      = vec2(2, 14);
	constexpr auto scrollbar_size        = vec2(11, 6);
	constexpr auto scrollbar_area_right  = scissor_x + scissor_w - scrollbar_margin.x;
	constexpr auto scrollbar_area_top    = scissor_y             + scrollbar_margin.y;
	constexpr auto scrollbar_area_bottom = scissor_y + scissor_h - scrollbar_margin.y;

	const auto scrollbar_y = lerp(scrollbar_area_top,
	                              scrollbar_area_bottom - scrollbar_size.y,
	                              scroll_offset / max_scroll_offset());

	rs.fill_tiled_rect({scrollbar_area_right, scrollbar_y, manual_z}, scrollbar_size,
	                   border_color, texture_scrollbar, align::top_right);
}

extern "C" void hook_Menu_EnterCustomRulesMenu()
{
	manual_open = true;

	scroll_offset = 0;
	scroll_frames = 0;

	MenuTypePrevious = MenuType;
	MenuType = MenuType_Manual;

	auto *gobj = GObj_Create(GOBJ_CLASS_PROC, GOBJ_PLINK_PROC, 0x80);
	GObj_AddProc(gobj, manual_input, 0);
	Menu_SetGObjPrio(gobj);
	GObj_SetupGXLink(gobj, draw_manual, GOBJ_GXLINK_MENU_TOP, 0x80);

	pool.inc_ref();

	pool.add(init_texture(manual_p1_tex_data, &texture_p1));
	pool.add(init_texture(manual_p2_tex_data, &texture_p2));
	pool.add(init_texture(manual_border_tex_data, &texture_border));
	pool.add(init_texture(scrollbar_tex_data, &texture_scrollbar));

	// Bottom text
	text = Text_Create(0, 1, -9.5f, 8.f, 17.f, 364.68331909f, 76.75543640f);
	text->stretch.x = 0.0521f;
	text->stretch.y = 0.0521f;
	Text_SetFromSIS(text, 0);
	text->data = bottom_text.data();
}