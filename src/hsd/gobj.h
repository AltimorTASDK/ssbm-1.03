#pragma once

#include <gctypes.h>

struct HSD_GObj;

using GObjRenderCallback = void(HSD_GObj *gobj, u32 pass);

struct HSD_GObjProc {
	HSD_GObjProc *child;
	HSD_GObjProc *next;
	HSD_GObjProc *prev;
	u8 s_link;
	s8 flags;
	HSD_GObj *gobj;
	void (*callback)(HSD_GObj*);
};

struct HSD_GObj {
	u16 classifier;
	s8 p_link;
	s8 gx_link;
	u8 p_priority;
	u8 render_priority;
	s8 obj_kind;
	s8 user_data_kind;
	HSD_GObj *next;
	HSD_GObj *prev;
	HSD_GObj *next_gx;
	HSD_GObj *prev_gx;
	HSD_GObjProc *proc;
	GObjRenderCallback *render_cb;
	u64 gxlink_prios;
	void *hsd_obj;
	void *data;
	void (*user_data_remove_func)(void *data);
	void *pad0034;

	template<typename T>
	T *get()
	{
		return static_cast<T*>(data);
	}
	
	template<typename T>
	const T *get() const
	{
		return static_cast<const T*>(data);
	}
};