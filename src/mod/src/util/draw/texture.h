#pragma once

#include "util/meta.h"
#include "util/vector.h"
#include "util/draw/render.h"
#include <ogc/gx.h>

class texture {
	mutable GXTexObj tex_obj;

public:
	texture() = default;

	texture(void *data, u16 width, u16 height, u8 format,
	        u8 wrap_s = GX_CLAMP, u8 wrap_t = GX_CLAMP, u8 mipmap = GX_FALSE)
	{
		init(data, width, height, format, wrap_s, wrap_t, mipmap);
	}

	void init(void *data, u16 width, u16 height, u8 format,
	          u8 wrap_s = GX_CLAMP, u8 wrap_t = GX_CLAMP, u8 mipmap = GX_FALSE)
	{
		GX_InitTexObj(&tex_obj, data, width, height, format, wrap_s, wrap_t, mipmap);
	}

	void apply() const
	{
		render_state::get().load_tex_obj(&tex_obj);
	}
	
	u16 width() const
	{
		return GX_GetTexObjWidth(&tex_obj);
	}

	u16 height() const
	{
		return GX_GetTexObjHeight(&tex_obj);
	}

	vec2i size() const
	{
		return vec2i(width(), height());
	}
};