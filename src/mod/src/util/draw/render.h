#pragma once

#include "dolphin/gx.h"
#include "util/vector.h"
#include <ogc/gx.h>
#include <stack>

class texture;

enum class align {
	top_left,
	top,
	top_right,
	left,
	center,
	right,
	bottom_left,
	bottom,
	bottom_right
};

struct vertex {
	void write() const;
};

struct vertex_pos_clr : vertex {
	static constexpr auto index = 0;

	vec3 position;
	color_rgba color;

	vertex_pos_clr(vec3 position, color_rgba color) : position(position), color(color) {}

	static void set_format();
	void write() const;
};

struct vertex_pos_uv : vertex {
	static constexpr auto index = 1;

	vec3 position;
	uv_coord uv;

	vertex_pos_uv(vec3 position, uv_coord uv) : position(position), uv(uv) {}

	static void set_format();
	void write() const;
};

struct vertex_pos_clr_uv : vertex {
	static constexpr auto index = 2;

	vec3 position;
	color_rgba color;
	uv_coord uv;

	vertex_pos_clr_uv(vec3 position, color_rgba color, uv_coord uv) :
		position(position), color(color), uv(uv) {}

	static void set_format();
	void write() const;
};

template<typename T>
concept VertexFormat = std::is_base_of_v<vertex, T>;

vec3 alignment_offset(const vec2 &size, align alignment);

class render_state {
	static constexpr auto resolution = vec2(640, 480);

	static render_state instance;

	int current_vertex_fmt = -1;
	GXTexObj *current_tex_obj = nullptr;
	std::array<u32, 4> current_scissor;
	std::stack<std::array<u32, 4>> scissor_stack;

public:
	static render_state &get()
	{
		return instance;
	}

	void reset();
	void reset_2d(bool check_widescreen = true);
	void reset_3d();

	void set_scissor(u32 x, u32 y, u32 w, u32 h);
	void push_scissor();
	void pop_scissor();

	void set_scissor(vec2i origin, vec2i size)
	{
		set_scissor(origin.x, origin.y, size.x, size.y);
	}

	// Like set_scissor, but restricts the new scissor to the bounds of the
	// old scissor
	void restrict_scissor(u32 x, u32 y, u32 w, u32 h);

	void restrict_scissor(vec2i origin, vec2i size)
	{
		restrict_scissor(origin.x, origin.y, size.x, size.y);
	}

	void load_tex_obj(GXTexObj *obj)
	{
		if (obj == current_tex_obj)
			return;

		current_tex_obj = obj;
		GX_LoadTexObj(obj, GX_TEXMAP0);
	}

	void invalidate_tex_obj(GXTexObj *obj)
	{
		if (obj == current_tex_obj)
			current_tex_obj = nullptr;
	}

	template<VertexFormat T>
	void draw_primitive(u8 prim_type, const std::vector<T> &vertices)
	{
		// Ensure correct vertex desc is set
		if (T::index != current_vertex_fmt) {
			current_vertex_fmt = T::index;
			T::set_format();
		}

		GX_Begin(prim_type, GX_VTXFMT0, (u16)vertices.size());
		for (const auto &vertex : vertices)
			vertex.write();
	}

	template<VertexFormat T>
	void draw_quads(const std::vector<T> &vertices)
	{
		draw_primitive(GX_QUADS, vertices);
	}

	void fill_rect(const vec3 &origin, const vec2 &size, const color_rgba &color,
		align alignment = align::top_left);

	void fill_rect(const vec3 &origin, const vec2 &size, const color_rgba &color,
		const texture &tex, const uv_coord &uv1, const uv_coord &uv2,
		align alignment = align::top_left);

	void fill_rect(const vec3 &origin, const vec2 &size, const texture &tex,
	        const uv_coord &uv1, const uv_coord &uv2, align alignment = align::top_left);

	void fill_tiled_rect(const vec3 &origin, const vec2 &size, const texture &tex,
	                     align alignment = align::top_left);

	void fill_tiled_rect(const vec3 &origin, const vec2 &size, const color_rgba &color,
		             const texture &tex, align alignment = align::top_left);
};

inline render_state render_state::instance;