#pragma once

#include <gctypes.h>

struct HSD_GObj;

using GObjRenderCallback = void(HSD_GObj *gobj, u32 pass);
using GObjProcCallback = void(HSD_GObj *gobj);

enum GObjKind {
	GOBJ_KIND_JOBJ = 3
};

struct HSD_GObjProc {
	HSD_GObjProc *child;
	HSD_GObjProc *next;
	HSD_GObjProc *prev;
	u8 s_link;
	s8 flags;
	HSD_GObj *gobj;
	GObjProcCallback *callback;
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

extern "C" {

extern HSD_GObj **plinkhigh_gobjs;

HSD_GObj *GObj_Create(u8 obj_class, u8 p_link, u8 prio);
void GObj_InitKindObj(HSD_GObj *gobj, s8 obj_kind, void *obj_ptr);
void GObj_SetupGXLink(HSD_GObj *gobj, GObjRenderCallback *render_cb, u8 gx_link, u8 priority);
void GObj_CreateProcWithCallback(HSD_GObj *gobj, GObjProcCallback *cb, u8 s_prio);
void GObj_ProcRemove(HSD_GObjProc *proc);

void GObj_ProcAnimate(HSD_GObj *gobj);
void GObj_GXProcDisplay(HSD_GObj *gobj, u32 pass);

}