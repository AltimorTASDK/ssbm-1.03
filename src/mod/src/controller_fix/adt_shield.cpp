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

static bool is_in_adt_shield(const Player *player)
{
	// Intentionally doesn't detect Yoshi GuardReflect.
	// His ADT shield is normally bugged and useless, so he doesn't get ADT PS.
	return get_ucf_type() != ucf_type::ucf && player->action_state == AS_GuardReflect &&
	       player->custom_as_data<ps_data>()->adt && player->reflect_active;
}

static bool update_adt_shield(Player *player)
{
	if (!is_in_adt_shield(player))
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
	const auto *as_data = player->custom_as_data<ps_data>();
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
	Player_InitShield(gobj);

	auto *player = gobj->get<Player>();

	if (player->character_id == CID_Yoshi) {
		// Fix the shield scale and invincibility after they get owned by the AS change
		Player_UpdateShieldScale(player);
		Player_SetHurtboxBodyState(gobj, BodyState_Invincible);
		return;
	}

	// Initialize ADT shield data
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

extern "C" bool Hitbox_CheckShieldBubbleCollision(Hitbox *hitbox, float hitbox_scale,
                                                  ShieldBubble *shield, float shield_scale,
                                                  matrix3x4 *z_scale_matrix, float z_pos,
                                                  bool force_hit);

extern "C" bool check_item_hit_shield(Hitbox *hitbox, float hitbox_scale,
                                      ShieldBubble *shield, float shield_scale,
                                      matrix3x4 *z_scale_matrix, float z_pos,
                                      bool force_hit, const Player *player)
{
	// Don't collide with the shield bubble if the player wants to ADT PS
	if (is_in_adt_shield(player))
		return false;

	return Hitbox_CheckShieldBubbleCollision(hitbox, hitbox_scale, shield, shield_scale,
	                                         z_scale_matrix, z_pos, force_hit);
}