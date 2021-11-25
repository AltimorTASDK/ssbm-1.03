#include "hsd/gobj.h"
#include "rules/values.h"

extern "C" void orig_Stage_PokemonStadium_TransformationDecide(HSD_GObj *gobj);
extern "C" void hook_Stage_PokemonStadium_TransformationDecide(HSD_GObj *gobj)
{
	// Do nothing if not original
	if (!is_stage_frozen(Stage_PS))
		orig_Stage_PokemonStadium_TransformationDecide(gobj);
}