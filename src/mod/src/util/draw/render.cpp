#include "hsd/cobj.h"
#include "os/gx.h"
#include "os/os.h"
#include "qol/widescreen.h"
#include "rules/values.h"
#include "util/matrix.h"
#include "util/meta.h"
#include "util/vector.h"
#include "util/draw/render.h"
#include "util/draw/texture.h"
#include <ogc/gx.h>
#include <concepts>
#include <vector>

render_state render_state::instance;

static void write_vector(const vector_type auto &vector)
{
	std::apply([](auto ...values) { gx_fifo->write(values...); }, vector.elems());
}

void vertex_pos_clr::write() const
{
	write_vector(position);
	write_vector(color);
}

void vertex_pos_uv::write() const
{
	write_vector(position);
	write_vector(uv);
}

void vertex_pos_clr_uv::write() const
{
	write_vector(position);
	write_vector(color);
	write_vector(uv);
}

void vertex_pos_clr::set_format()
{
	GX_ClearVtxDesc();
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
}

void vertex_pos_clr_uv::set_format()
{
	GX_ClearVtxDesc();
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);
}

void vertex_pos_uv::set_format()
{
	GX_ClearVtxDesc();
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);
}

vec3 alignment_offset(const vec2 &size, align alignment)
{
	switch (alignment) {
	case align::top_left:
		return vec3::zero;
	case align::top:
		return -vec3(size.x / 2, 0, 0);
	case align::top_right:
		return -vec3(size.x, 0, 0);
	case align::left:
		return -vec3(0, size.y / 2, 0);
	case align::center:
		return -vec3(size.x / 2, size.y / 2, 0);
	case align::right:
		return -vec3(size.x, size.y / 2, 0);
	case align::bottom_left:
		return -vec3(0, size.y, 0);
	case align::bottom:
		return -vec3(size.x / 2, size.y, 0);
	case align::bottom_right:
		return -vec3(size.x, size.y, 0);
	}

	PANIC("Invalid align constant");
}

void render_state::reset()
{
	current_vertex_fmt = -1;
	current_tex_obj = nullptr;

	GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP);

	GX_SetNumTevStages(1);
	GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);

	GX_SetNumChans(1);
	GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHTNULL, GX_DF_NONE,
	               GX_AF_NONE);

	GX_SetNumTexGens(1);
	GX_SetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE,
	                   GX_DTTIDENTITY);

	GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP);

	GX_SetCullMode(GX_NONE);
}

void render_state::reset_2d()
{
	reset();

	constexpr auto wide_l    = ortho_left_wide;
	constexpr auto wide_r    = ortho_right_wide;
	constexpr auto proj      = ortho_projection(0, 480,      0,    640, -1000, 1000);
	constexpr auto proj_wide = ortho_projection(0, 480, wide_l, wide_r, -1000, 1000);

	GX_SetCurrentMtx(0);
	GX_LoadProjectionMtx(is_widescreen() ? proj_wide : proj, GX_ORTHOGRAPHIC);
	GX_LoadPosMtxImm(matrix3x4::identity, GX_PNMTX0);
	set_scissor(0, 0, 640, 480);
}

void render_state::reset_3d()
{
	reset();

	Mtx view_matrix;
	const auto *cobj = HSD_CObjGetCurrent();
	HSD_CObjGetViewingMtx(cobj, view_matrix);
	GX_LoadPosMtxImm(view_matrix, GX_PNMTX0);
	set_scissor(0, 0, 640, 480);
}

void render_state::set_scissor(u32 x, u32 y, u32 w, u32 h)
{
	GX_SetScissor(x, y, w, h);
	current_scissor = { x, y, w, h };
}

void render_state::push_scissor()
{
	scissor_stack.push(current_scissor);
}

void render_state::pop_scissor()
{
	const auto &scissor = scissor_stack.top();
	set_scissor(scissor[0], scissor[1], scissor[2], scissor[3]);
	scissor_stack.pop();
}

void render_state::restrict_scissor(u32 x, u32 y, u32 w, u32 h)
{
	// Get scissor corners
	const auto old_min = vec2i(current_scissor[0], current_scissor[1]);
	const auto old_max = vec2i(current_scissor[2], current_scissor[3]) + old_min;
	const auto new_min = vec2i(x, y);
	const auto new_max = vec2i(w, h) + new_min;

	const auto min = vec2i::max(old_min, new_min);
	const auto max = vec2i::min(old_max, new_max);
	const auto size = max - min;
	set_scissor(min.x, min.y, size.x, size.y);
}

void render_state::fill_rect(const vec3 &origin, const vec2 &size, const color_rgba &color,
                             align alignment)
{
	const auto aligned = origin + alignment_offset(size, alignment);

	draw_quads<vertex_pos_clr>({
		{ aligned,                           color },
		{ aligned + vec3(size.x, 0,      0), color },
		{ aligned + vec3(size.x, size.y, 0), color },
		{ aligned + vec3(0,      size.y, 0), color }
	});
}

void render_state::fill_rect(const vec3 &origin, const vec2 &size, const texture &tex,
                             const uv_coord &uv1, const uv_coord &uv2, align alignment)
{
	const auto aligned = origin + alignment_offset(size, alignment);

	tex.apply();

	draw_quads<vertex_pos_uv>({
		{ aligned,                           uv1                    },
		{ aligned + vec3(size.x, 0,      0), uv_coord(uv2.u, uv1.v) },
		{ aligned + vec3(size.x, size.y, 0), uv2                    },
		{ aligned + vec3(0,      size.y, 0), uv_coord(uv1.u, uv2.v) }
	});
}

void render_state::fill_rect(const vec3 &origin, const vec2 &size, const color_rgba &color,
                             const texture &tex, const uv_coord &uv1, const uv_coord &uv2,
                             align alignment)
{
	const auto aligned = origin + alignment_offset(size, alignment);

	tex.apply();

	draw_quads<vertex_pos_clr_uv>({
		{ aligned,                           color, uv1                    },
		{ aligned + vec3(size.x, 0,      0), color, uv_coord(uv2.u, uv1.v) },
		{ aligned + vec3(size.x, size.y, 0), color, uv2                    },
		{ aligned + vec3(0,      size.y, 0), color, uv_coord(uv1.u, uv2.v) }
	});
}

static void iterate_tiled_rect_corners(auto &&callable, const vec2 &size, const texture &tex)
{
	const auto tile_size = vec2(
		std::min(size.x, (float)tex.width()) / 3,
		std::min(size.y, (float)tex.height()) / 3);

	vec2 offset_table[] = {
		vec2::zero,
		vec2(tile_size),
		vec2(size - tile_size),
		size
	};

	uv_coord uv_table[] = {
		uv_coord::zero,
		uv_coord(tile_size / vec2(tex.size())),
		uv_coord::one - uv_coord(tile_size / vec2(tex.size())),
		uv_coord::one
	};

	for (auto i = 0; i < 3; i++) {
		for (auto j = 0; j < 3; j++) {
			const auto offset1 = vec2(offset_table[i].x, offset_table[j].y);
			const auto offset2 = vec2(offset_table[i + 1].x, offset_table[j + 1].y);
			const auto uv1 = uv_coord(uv_table[i].u, uv_table[j].v);
			const auto uv2 = uv_coord(uv_table[i + 1].u, uv_table[j + 1].v);
			callable(offset1, offset2, uv1, uv2);
		}
	}
}

void render_state::fill_tiled_rect(const vec3 &origin, const vec2 &size, const texture &tex,
                                   align alignment)
{
	const auto aligned = origin + alignment_offset(size, alignment);

	tex.apply();

	iterate_tiled_rect_corners([&](auto offset1, auto offset2, auto uv1, auto uv2) {
		fill_rect(aligned + vec3(offset1), offset2 - offset1, tex, uv1, uv2);
	}, size, tex);
}

void render_state::fill_tiled_rect(const vec3 &origin, const vec2 &size, const color_rgba &color,
		                   const texture &tex, align alignment)
{
	const auto aligned = origin + alignment_offset(size, alignment);

	tex.apply();

	iterate_tiled_rect_corners([&](auto offset1, auto offset2, auto uv1, auto uv2) {
		fill_rect(aligned + vec3(offset1), offset2 - offset1, color, tex, uv1, uv2);
	}, size, tex);
}