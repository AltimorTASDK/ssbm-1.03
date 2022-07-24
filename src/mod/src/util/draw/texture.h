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

	texture(void *data, u16 width, u16 height, u8 format,
	        u8 minfilt, u8 magfilt, f32 minlod, f32 maxlod, f32 lodbias,
	        u8 biasclamp, u8 edgelod, u8 maxaniso,
	        u8 wrap_s = GX_CLAMP, u8 wrap_t = GX_CLAMP, u8 mipmap = GX_FALSE)
	{
		init(data, width, height, format, wrap_s, wrap_t, mipmap);
		init_lod(minfilt, magfilt, minlod, maxlod, lodbias, biasclamp, edgelod, maxaniso);
	}

	void init(void *data, u16 width, u16 height, u8 format,
	          u8 wrap_s = GX_CLAMP, u8 wrap_t = GX_CLAMP, u8 mipmap = GX_FALSE)
	{
		GX_InitTexObj(&tex_obj, data, width, height, format, wrap_s, wrap_t, mipmap);
	}

	void init_lod(u8 minfilt, u8 magfilt, f32 minlod, f32 maxlod, f32 lodbias,
	              u8 biasclamp, u8 edgelod, u8 maxaniso)
	{
		GX_InitTexObjLOD(&tex_obj, minfilt, magfilt, minlod, maxlod, lodbias,
		                 biasclamp, edgelod, maxaniso);
	}

	void apply() const
	{
		render_state::get().load_tex_obj(&tex_obj);
	}

	void invalidate() const
	{
		render_state::get().invalidate_tex_obj(&tex_obj);
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

	float ratio() const
	{
		return (float)width() / (float)height();
	}

	float inv_ratio() const
	{
		return (float)height() / (float)width();
	}
};