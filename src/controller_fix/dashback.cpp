#include "os/os.h"
#include "melee/action_state.h"
#include "melee/constants.h"
#include "melee/player.h"
#include "util/coords.h"

void check_dashback(HSD_GObj *gobj)
{
	auto *player = gobj->get<Player>();
	
	// Check that Turn wasn't already canceled
	if (player->action_state != AS_Turn)
		return;
	
	// Must be tilt turn f2
	if (player->animation_frame != 2.f)
		return;
		
	if (player->as_data.Turn.is_smash_turn)
		return;
		
	// No Nana
	if (player->is_backup_climber)
		return;
		
	// Must satisfy vanilla xsmash conditions
	// UCF doesn't check direction and it allows a UCF only ICs desync
	if (player->input.stick.x * -player->direction < plco->x_smash_threshold)
		return;

	if (player->input.stick_x_hold_time > plco->x_smash_frames)
		return;

	// Hardware value comparison
	if (!check_ucf_xsmash(player))
		return;
		
	player->direction = -player->direction;
	AS_020_Dash(gobj, false);
	
	// Retroactively write smash turn to Nana inputs
	if (player->character_id != CID_Popo)
		return;
	
	// UCF doesn't zero out Y, making it possible for Nana to achieve impossible input values
	HSD_GObj *nana_gobj = PlayerBlock_GetSubCharGObj(player->slot, 1);
	auto *nana = nana_gobj->get<Player>();
	nana->popo_data_write->stick.x = player->direction > 0.f ? 127 : -128;
	nana->popo_data_write->stick.y = 0;
}

extern "C" void orig_Interrupt_AS_Turn(HSD_GObj *gobj);
extern "C" void hook_Interrupt_AS_Turn(HSD_GObj *gobj)
{
	orig_Interrupt_AS_Turn(gobj);
	check_dashback(gobj);
}