#include "hsd/archive.h"
#include "hsd/cobj.h"
#include "hsd/gobj.h"
#include "hsd/jobj.h"
#include "hsd/pad.h"
#include "hsd/video.h"
#include "melee/menu.h"
#include "melee/text.h"
#include "util/compression.h"
#include "util/math.h"
#include "util/mempool.h"
#include "util/draw/render.h"
#include "util/draw/texture.h"
#include "util/melee/text_builder.h"
#include <gctypes.h>

#include "resources/screens/manual_p1.tex.h"
#include "resources/screens/manual_p2.tex.h"
#include "resources/screens/manual_p3.tex.h"
#include "resources/screens/manual_border.tex.h"
#include "resources/screens/scrollbar.tex.h"

constexpr auto MAX_SCROLL = 1.7625f;

static texture texture_p1;
static texture texture_p2;
static texture texture_p3;
static texture texture_border;
static texture texture_scrollbar;

static mempool pool;

static float scroll_offset;

bool manual_open;

constexpr auto bottom_text = text_builder::build(
		text_builder::kern(),
		text_builder::left(),
		text_builder::color<170, 170, 170>(),
		text_builder::textbox<179, 179>(),
		text_builder::offset<0, -10>(),
		text_builder::br(),
		text_builder::unk06<0, 0>(),
		text_builder::fit(),
		text_builder::ascii<"Up/Down: Scroll">(),
		text_builder::end_fit(),
		text_builder::end_textbox(),
		text_builder::end_color());

static void manual_input(HSD_GObj *gobj)
{
	const auto buttons = Menu_GetButtons(PORT_ALL);
	
	if (buttons & MenuButton_B) {
		manual_open = false;
		Text_FreeAll();
		GObj_Free(gobj);
		pool.dec_ref();
		return;
	}
	
	auto total_y = 0.f;
	for (auto i = 0; i < 4; i++)
		total_y += HSD_PadCopyStatus[i].stick.y;
	
	total_y = clamp(total_y, -1.f, 1.f);
	
	scroll_offset = clamp(scroll_offset - total_y * .0125f, 0.f, MAX_SCROLL);
}

static void draw_manual(HSD_GObj *gobj, u32 pass)
{
	if (pass != HSD_RP_BOTTOMHALF)
		return;
	
	auto &rs = render_state::get();
	rs.reset_3d();

	constexpr auto scissor_y = 88;
	constexpr auto scissor_h = 288;

	rs.set_scissor(0, scissor_y, 640, scissor_h);
	
	constexpr auto scale = 25.f;
	const auto pos_p1 = vec3(0, 7.5f + scroll_offset * scale, 17);
	const auto pos_p2 = pos_p1 - vec3(0, scale, 0);
	const auto pos_p3 = pos_p2 - vec3(0, scale, 0);

	rs.fill_rect(pos_p1, vec2(1, -1) * scale, texture_p1,
	             uv_coord::zero, uv_coord::one, align::top);

	rs.fill_rect(pos_p2, vec2(1, -1) * scale, texture_p2,
	             uv_coord::zero, uv_coord::one, align::top);

	rs.fill_rect(pos_p3, vec2(1, -1) * scale, texture_p3,
	             uv_coord::zero, uv_coord::one, align::top);
		     
	rs.reset_2d();

	rs.fill_tiled_rect(vec3(320, scissor_y, 17), vec2(515, scissor_h),
	                   color_rgba::hex(0x929196FFu), texture_border, align::top);
	
	constexpr auto scrollbar_size = vec2(11, 6);
	constexpr auto scrollbar_x = 320 + 515.f / 2 - 2;
	constexpr auto scrollbar_y_min = scissor_y + 14.f;
	constexpr auto scrollbar_y_max = scissor_y + scissor_h - scrollbar_size.y - 14.f;
	
	const auto scrollbar_y = lerp(scrollbar_y_min, scrollbar_y_max, scroll_offset / MAX_SCROLL);

	rs.fill_tiled_rect(vec3(scrollbar_x, scrollbar_y, 17), scrollbar_size,
	                   color_rgba::hex(0x929196FFu), texture_scrollbar, align::top_right);
}

extern "C" void orig_Menu_EnterCustomRulesMenu();
extern "C" void hook_Menu_EnterCustomRulesMenu()
{
	manual_open = true;
	
	scroll_offset = 0;
	
	MenuTypePrevious = MenuType;
	MenuType = MenuType_Rules;
	
	auto *gobj = GObj_Create(GOBJ_CLASS_PROC, GOBJ_PLINK_PROC, 0x80);
	GObj_AddProc(gobj, manual_input, 0);
	Menu_SetGObjPrio(gobj);
	GObj_SetupGXLink(gobj, draw_manual, GOBJ_GXLINK_MENU_TOP, 0);

	pool.inc_ref();
	pool.add(init_texture(manual_p1_tex_data, &texture_p1));
	pool.add(init_texture(manual_p2_tex_data, &texture_p2));
	pool.add(init_texture(manual_p3_tex_data, &texture_p3));
	pool.add(init_texture(manual_border_tex_data, &texture_border));
	pool.add(init_texture(scrollbar_tex_data, &texture_scrollbar));
	
	auto *text = Text_Create(0, 1, -9.5f, 8.f, 17.f, 364.68331909f, 76.75543640f);
	text->stretch.x = 0.0521f;
	text->stretch.y = 0.0521f;
	Text_SetFromSIS(text, 0);
	text->data = bottom_text.data();
}