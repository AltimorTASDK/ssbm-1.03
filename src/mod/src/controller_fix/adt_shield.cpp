#include "melee/action_state.h"
#include "melee/constants.h"
#include "melee/player.h"
#include "melee/characters/yoshi.h"
#include "rules/values.h"

// Custom AS data
struct ps_data {
	// Whether GuardReflect was entered by analog to digital transition
	bool adt;
	float adt_scale;
};

static bool is_in_guardreflect(const Player  *player)
{
	if (player->character_id == CID_Yoshi)
		return player->action_state == AS_Yoshi_GuardReflect;
	else
		return player->action_state == AS_GuardReflect;
}

static bool update_adt_shield(Player *player)
{
	if (get_ucf_type() == ucf_type::ucf || !is_in_guardreflect(player))
		return false;

	const auto *as_data = player->custom_as_data<ps_data>();

	if (!as_data->adt || !player->reflect_active)
		return false;

	const auto analog_lr = player->input.analog_lr;
	const auto deadzone = plco->lr_deadzone;

	// No need to update if no new input
	if (analog_lr < deadzone)
		return true;

	player->lightshield_amount = (analog_lr - deadzone) / (1.f - deadzone);

	// Update the shield scale so the player gets the correct physical shield size
	Player_UpdateShieldScale(player);

	// Adjust the PS bubble to maintain its size
	const auto ratio = as_data->adt_scale / player->reflect_bubble.jobj->scale.x;
	player->reflect_bubble.radius = plco->ps_bubble_radius * ratio;

	return true;
}

extern "C" void orig_AS_182_GuardReflect_YoshiCheck(HSD_GObj *gobj);
extern "C" void hook_AS_182_GuardReflect_YoshiCheck(HSD_GObj *gobj)
{
	orig_AS_182_GuardReflect_YoshiCheck(gobj);
	gobj->get<Player>()->custom_as_data<ps_data>()->adt = false;
}

extern "C" void orig_AS_182_GuardReflect_ADT_YoshiCheck(HSD_GObj *gobj);
extern "C" void hook_AS_182_GuardReflect_ADT_YoshiCheck(HSD_GObj *gobj)
{
	orig_AS_182_GuardReflect_ADT_YoshiCheck(gobj);

	if (get_ucf_type() == ucf_type::ucf)
		return;

	// Enable physical shield
	Player_EnterShield(gobj);

	// Initialize ADT shield data
	auto *player = gobj->get<Player>();
	auto *as_data = player->custom_as_data<ps_data>();
	as_data->adt = true;
	as_data->adt_scale = player->reflect_bubble.jobj->scale.x;

	update_adt_shield(player);
}

extern "C" bool orig_Player_UpdateShieldHealth(HSD_GObj *gobj);
extern "C" bool hook_Player_UpdateShieldHealth(HSD_GObj *gobj)
{
	// Return false to not update shield if in ADT frames
	return !update_adt_shield(gobj->get<Player>()) && orig_Player_UpdateShieldHealth(gobj);
}