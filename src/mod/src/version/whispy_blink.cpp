#ifdef NTSC100

#include "melee/player.h"
#include "util/patch_list.h"

extern "C" void Player_UpdatePhysicsJointAnimFrameAndRate(Player *player);
extern "C" void Player_SetBoneAnimFrameAndRate(Player *player, u32 bone,
                                               HSD_FigaTree *figatree, f32 frame, f32 rate);

PATCH_LIST(
	// Patch out Player_SetAnimationFrameAndRate calls
	// nop
	std::pair { Player_UpdatePhysicsJointAnimFrameAndRate+0xB0, 0x60000000u },
	std::pair { Player_UpdatePhysicsJointAnimFrameAndRate+0xFC, 0x60000000u }
);

extern "C" void orig_Player_UpdatePhysicsJointAnimFrameAndRate(Player *player);
extern "C" void hook_Player_UpdatePhysicsJointAnimFrameAndRate(Player *player)
{
	orig_Player_UpdatePhysicsJointAnimFrameAndRate(player);

	if (player->override_phys_joints == 0 && player->update_phys_joints == 0)
		return;

	if (player->figatree == nullptr)
		return;

	// Update physics joints individually like 1.01+
	for (auto i = 0u; i < player->physics_joint_count; i++) {
		const auto bone = player->char_dat->joint_data->phys_joints[i].bone;
		const auto frame = player->animation_frame;
		const auto rate = player->animation_speed;
		Player_SetBoneAnimFrameAndRate(player, bone, player->figatree, frame, rate);
	}
}

#endif