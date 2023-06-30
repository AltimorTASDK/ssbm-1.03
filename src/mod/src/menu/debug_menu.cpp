#include "hsd/dobj.h"
#include "hsd/gobj.h"
#include "hsd/jobj.h"
#include "melee/debug.h"
#include "melee/text.h"
#include "util/patch_list.h"
#include "util/mempool.h"
#include "util/texture_swap.h"
#include <array>

extern "C" HSD_GObj *LanguageMenuGObj;

extern "C" void Menu_LanguageMenuInput(HSD_GObj *gobj);

constexpr auto anim_frames = std::array { 1.f, 0.f, 0.f, 0.f, 0.f };

struct LanguageMenuData {
	u8 setting;
	u8 old_setting;
	bool ready;
	Text *text;
};

PATCH_LIST(
	// Return to Vs Menu
	// li r3, MenuType_VsMode
	std::pair { Menu_LanguageMenuInput+0x8C, 0x38600002u },
	// li r4, 2
	std::pair { Menu_LanguageMenuInput+0x90, 0x38800002u }
);

extern "C" void orig_Menu_SetupLanguageMenu(u8 state);
extern "C" void hook_Menu_SetupLanguageMenu(u8 state)
{
	orig_Menu_SetupLanguageMenu(state);

	float frame;
	auto *data = LanguageMenuGObj->get<LanguageMenuData>();
	auto *jobj = LanguageMenuGObj->get_hsd_obj<HSD_JObj>()->child;

	// Set the highlighted option based on DbLevel
	switch (DbLevel) {
	case DbLKind_Develop:
		data->setting = 1;
		data->old_setting = 1;
		frame = 0.f;
		break;
	default:
		data->setting = 0;
		data->old_setting = 0;
		frame = 1.f;
		break;
	}

	HSD_JObjReqAnimAll(jobj, frame);
	HSD_JObjStopAnimByTypeAndFlags(jobj, 0xFF, ObjMask_JObj);
	HSD_JObjAnimAll(jobj);
	HSD_JObjReqAnimAll(jobj, 0.f);
	HSD_JObjStopAnimByTypeAndFlags(jobj, 0xFF, ObjMask_MObj);
	HSD_JObjAnimAll(jobj);

	// Hide flags
	HSD_DObjGetByIndex<2>(jobj->u.dobj)->flags |= DOBJ_HIDDEN;
	HSD_DObjGetByIndex<3>(jobj->u.dobj)->flags |= DOBJ_HIDDEN;
	HSD_DObjGetByIndex<6>(jobj->u.dobj)->flags |= DOBJ_HIDDEN;
	HSD_DObjGetByIndex<7>(jobj->u.dobj)->flags |= DOBJ_HIDDEN;
	HSD_DObjGetByIndex<8>(jobj->u.dobj)->flags |= DOBJ_HIDDEN;
}