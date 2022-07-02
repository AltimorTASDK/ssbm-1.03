#pragma once

#include "hsd/figatree.h"
#include "hsd/gobj.h"
#include "hsd/jobj.h"
#include "melee/ftcmd.h"
#include "melee/object.h"
#include "util/vector.h"
#include <gctypes.h>

constexpr auto NANA_BUFFER = 30;

enum CID {
	CID_Mario,
	CID_Fox,
	CID_Falcon,
	CID_DK,
	CID_Kirby,
	CID_Bowser,
	CID_Link,
	CID_Sheik,
	CID_Ness,
	CID_Peach,
	CID_Popo,
	CID_Nana,
	CID_Pikachu,
	CID_Samus,
	CID_Yoshi,
	CID_Puff,
	CID_Mewtwo,
	CID_Luigi,
	CID_Marth,
	CID_Zelda,
	CID_YLink,
	CID_DrMario,
	CID_Falco,
	CID_Pichu,
	CID_GameNWatch,
	CID_Ganondorf,
	CID_Roy,
	CID_MasterHand,
	CID_CrazyHand,
	CID_MaleWire,
	CID_FemaleWire,
	CID_GigaBowser,
	CID_Sandbag,
	CID_Max
};

enum ActionStat {
	ActionStat_LedgeGrabs   = 0x70,
	ActionStat_Taunts       = 0x71
};

enum SlotType {
	SlotType_Human = 0,
	SlotType_CPU   = 1,
	SlotType_Demo  = 2,
	SlotType_None  = 3,
	SlotType_Boss  = 4
};

struct PlayerInput {
    vec2 stick;
    vec2 last_stick;
    vec2 frozen_stick;
    vec2 cstick;
    vec2 last_cstick;
    vec2 frozen_cstick;
    f32 analog_lr;
    f32 last_analog_lr;
    f32 frozen_analog_lr;
    u32 held_buttons;
    u32 last_held_buttons;
    u32 frozen_buttons;
    u32 instant_buttons;
    u32 released_buttons;
    u8 stick_x_hold_time;
    u8 stick_y_hold_time;
    u8 analog_lr_hold_time;
    u8 true_stick_x_hold_time;
    u8 true_stick_y_hold_time;
    u8 true_analog_lr_hold_time;
    u8 stick_x_neutral_time;
    u8 stick_y_neutral_time;
    u8 analog_lr_neutral_time;
    u8 stick_x_direction_hold_time;
    u8 stick_y_direction_hold_time;
    u8 analog_lr_direction_hold_time;
    u8 last_a_press;
    u8 last_b_press;
    u8 last_xy_press;
    u8 last_analog_lr_press;
    u8 last_digital_lr_press;
    u8 last_dpad_down_press;
    u8 last_dpad_up_press;
    u8 a_press_interval;
    u8 digital_lr_press_interval;
    u8 last_jump_input;
    u8 last_up_b_input;
    u8 last_down_b_input;
    u8 last_side_b_input;
    u8 last_neutral_b_input;
    u8 jump_input_interval;
    u8 up_b_input_interval;
};

struct PopoData {
	u32 buttons;
	u8 analog_l;
	u8 analog_r;
	vec2c stick;
	vec2c cstick;
	vec3 position;
	f32 direction;
};

struct PhysicsJointData {
	u32 bone;
	char pad004[0x18 - 0x04];
};

struct SubactionInfo {
	char *name;
	u32 anim_offset;
	u32 anim_size;
	u8 *script;
	u32 anim_flags;
	HSD_RawArchive *anim_data;
};

struct JointData {
	u32 phys_joint_count;
	PhysicsJointData *phys_joints;
	char pad008[0x18 - 0x08];
};

struct CharacterDat {
	char pad000[0x08];
	u8 *archive_base;
	char pad00C[0x2C - 0x0C];
	JointData *joint_data;
	char pad030[0x60 - 0x30];
};

struct CharacterStats {
	char pad000[0x38];
	f32 jumpsquat;
	char pad03C[0x58 - 0x3C];
	s32 jumps;
	char pad05C[0x84 - 0x5C];
	f32 tilt_turn_frames;
	char pad088[0x184 - 0x88];
};

struct ItemGrabBox {
	f32 offset_x;
	f32 offset_y;
	f32 width;
	f32 height;
};

struct JostleBox {
	f32 size;
	f32 size_front;
};

struct MultiJumpStats {
	s32 turn_frames;
	f32 turn_threshold;
	f32 x_impulse;
	f32 accel_mult;
	f32 speed_mult;
	f32 y_impulse[5];
	s32 state_count;
	s32 start_state;
	// Used when Kirby has a transformation with a helmet
	s32 start_state_helmet;
};

struct ExtraStats {
	ItemGrabBox item_grab_box[3];
	JostleBox jostle;
	void *dk_stats;
	MultiJumpStats *multijump_stats;
	char pad040[0x5C - 0x40];
};

struct Player {
	static constexpr auto MAX_HITBOXES = 4;
	static constexpr auto MAX_HURTBOXES = 15;

	HSD_GObj *gobj;
	u32 character_id;
	u32 spawn_count;
	u8 slot;
	s32 action_state;
	s32 subaction;
	u32 max_common_as_index;
	struct ActionStateInfo *common_as_table;
	struct ActionStateInfo *character_as_table;
	SubactionInfo *subaction_table;
	struct AnimationInfo *animation_info_table;
	f32 direction;
	f32 model_direction;
	f32 initial_scale;
	f32 scale;
	f32 z_scale;
	char pad0040[0x74 - 0x40];
	vec3 move_vel_delta;
	vec3 move_vel;
	vec3 knockback_vel;
	vec3 pushback_vel;
	vec3 move_vel_lerp;
	vec3 position;
	vec3 last_position;
	vec3 delta_vel;
	vec3 kb_vel_accumulator;
	u32 airborne;
	f32 ground_vel_delta;
	f32 ground_accel;
	f32 ground_vel;
	f32 ground_knockback_vel;
	f32 ground_pushback_vel;
	f32 jostle_delta_x;
	f32 jostle_delta_z;
	char pad0100[0x10C - 0x100];
	CharacterDat *char_dat;
	CharacterStats char_stats;
	ExtraStats extra_stats;
	char pad02F0[0x3E0 - 0x2F0];
	u32 physics_joint_count;
	FtCmdState ftcmd_state;
	char pad0408[0x4B8 - 0x408];
	f32 overlay_r;
	f32 overlay_g;
	f32 overlay_b;
	f32 overlay_a;
	f32 overlay_flash_rate_r;
	f32 overlay_flash_rate_g;
	f32 overlay_flash_rate_b;
	f32 overlay_flash_rate_a;
	char pad04D8[0x504 - 0x4D8];
	u8 overlay_flags;
	char pad0505[0x58C - 0x505];
	u32 subaction_count;
	HSD_FigaTree *figatree;
	struct {
		u8 anim_flags_80 : 1;
		u8 anim_flags_40 : 1;
		u8 anim_flags_20 : 1;
		u8 update_phys_joints : 1;
		u8 override_phys_joints : 1;
		u8 anim_flags_04 : 1;
		u8 anim_flags_02 : 1;
		u8 anim_flags_01 : 1;
	};
	char pad0595[0x614 - 0x595];
	u8 override_color_r;
	u8 override_color_g;
	u8 override_color_b;
	u8 override_color_a;
	u8 port;
	u8 costume;
	u8 subcolor;
	u8 team;
	char pad061C[0x620 - 0x61C];
	PlayerInput input;
	char pad068C[0x6F0 - 0x68C];
	Physics phys;
	s32 ecb_timer;
	void *camera_data;
	f32 animation_frame;
	f32 subaction_speed;
	f32 animation_speed;
	char pad08A0[0x8A4 - 0x8A0];
	f32 anim_lerp_duration;
	f32 anim_lerp_progress;
	HSD_JObj *lerp_target_jobj;
	char pad08B0[0x914 - 0x8B0];
	Hitbox hitboxes[MAX_HITBOXES];
	char pad0DF4[0x119E - 0xDF4];
	u8 hurtbox_count;
	char pad119F[0x11A0 - 0x119F];
	Hurtbox hurtboxes[MAX_HURTBOXES];
	char pad1614[0x18A8 - 0x1614];
	f32 last_received_knockback;
	s32 frames_since_last_hit;
	f32 kb_resistance_innate;
	f32 kb_resistance_subaction;
	char pad18B0[0x18F8 - 0x18B8];
	u8 hit_grounded;
	char align18FA[0x18FA - 0x18F9];
	s16 damage_shake_frames;
	char pad18FC[0x1900 - 0x18FC];
	vec2 hit_ground_normal;
	char pad1908[0x1968 - 0x1908];
	u8 jumps_used;
	u8 walljump_count;
	char pad196A[0x1974 - 0x196A];
	HSD_GObj *held_item;
	char pad1978[0x1988 - 0x1978];
	u32 body_state_subaction;
	u32 body_state_timed;
	s32 intangible_frames;
	s32 invincible_frames;
	f32 shield_size;
	f32 lightshield_amount;
	s32 shield_damage_received;
	char pad19A4[0x1A68 - 0x19A4];
	u16 grab_type_mask;
	u16 grab_invuln_mask;
	char pad1A6C[0x1B80 - 0x1A6C];
	u8 cpu_flags1;
	u8 cpu_flags2;
	struct {
		u8 cpu_flags3_80 : 1;
		u8 cpu_flags3_40 : 1;
		u8 cpu_flags3_20 : 1;
		u8 cpu_flags3_10 : 1;
		u8 cpu_flags3_8 : 1;
		u8 cpu_flags3_4 : 1;
		u8 cpu_flags3_2 : 1;
		u8 is_nana_synced : 1;
	};
	u8 cpu_flags4;
	PopoData popo_data_buffer[30];
	PopoData *popo_data_write;
	PopoData *popo_data_read;
	char pad1ED4[0x210C - 0x1ED4];
	u8 walljump_eligible_frames;
	f32 walljump_direction;
	char pad2114[0x2218 - 0x2114];
	struct {
		u8 iasa : 1;
		u8 flags1_40 : 1;
		u8 flags1_20 : 1;
		u8 flags1_10 : 1;
		u8 flags1_08 : 1;
		u8 flags1_04 : 1;
		u8 flags1_02 : 1;
		u8 flags1_01 : 1;
	};
	u8 flags2;
	struct {
#ifdef PAL
		u8 flags3_80 : 1;
		u8 flags3_40 : 1;
		u8 flags3_20 : 1;
		u8 in_hitlag : 1;
		u8 flags3_08 : 1;
		u8 flags3_04 : 1;
		u8 flags3_02 : 1;
		u8 flags3_01 : 1;
#else
		u8 flags3_80 : 1;
		u8 flags3_40 : 1;
		u8 in_hitlag : 1;
		u8 flags3_10 : 1;
		u8 flags3_08 : 1;
		u8 flags3_04 : 1;
		u8 flags3_02 : 1;
		u8 flags3_01 : 1;
#endif
	};
	u8 flags4;
	u8 flags5;
	u8 flags6;
	struct {
#ifdef PAL
		u8 flags7_80 : 1;
		u8 is_invisible : 1;
		u8 flags7_20 : 1;
		u8 flags7_10 : 1;
		u8 flags7_08 : 1;
		u8 flags7_04 : 1;
		u8 flags7_02 : 1;
		u8 flags7_01 : 1;
#else
		u8 is_invisible : 1;
		u8 flags7_40 : 1;
		u8 flags7_20 : 1;
		u8 flags7_10 : 1;
		u8 flags7_08 : 1;
		u8 flags7_04 : 1;
		u8 flags7_02 : 1;
		u8 flags7_01 : 1;
#endif
	};
	struct {
#ifdef PAL
		u8 flags8_80 : 1;
		u8 flags8_40 : 1;
		u8 flags8_20 : 1;
		u8 flags8_10 : 1;
		u8 flags8_08 : 1;
		u8 no_update : 1;
		u8 is_backup_climber : 1;
		u8 flags8_01 : 1;
#else
		u8 flags8_80 : 1;
		u8 flags8_40 : 1;
		u8 flags8_20 : 1;
		u8 no_update : 1;
		u8 is_backup_climber : 1;
		u8 flags8_04 : 1;
		u8 flags8_02 : 1;
		u8 flags8_01 : 1;
#endif
	};
	u8 flags9;
	u8 flags10;
	struct {
#ifdef PAL
		u8 flags11_80 : 1;
		u8 flags11_40 : 1;
		u8 multijump : 1;
		u8 flags11_10 : 1;
		u8 flags11_08 : 1;
		u8 flags11_04 : 1;
		u8 flags11_02 : 1;
		u8 flags11_01 : 1;
#else
		u8 flags11_80 : 1;
		u8 multijump : 1;
		u8 flags11_20 : 1;
		u8 flags11_10 : 1;
		u8 flags11_08 : 1;
		u8 flags11_04 : 1;
		u8 flags11_02 : 1;
		u8 flags11_01 : 1;
#endif
	};
	u8 flags12;
#ifdef PAL
	u8 flags13;
	struct {
		u8 can_walljump : 1;
		u8 flags14_40 : 1;
		u8 flags14_20 : 1;
		u8 flags14_10 : 1;
		u8 flags14_08 : 1;
		u8 flags14_04 : 1;
		u8 flags14_02 : 1;
		u8 flags14_01 : 1;
	};
#else
	struct {
		u8 flags13_80 : 1;
		u8 flags13_40 : 1;
		u8 flags13_20 : 1;
		u8 flags13_10 : 1;
		u8 flags13_08 : 1;
		u8 flags13_04 : 1;
		u8 flags13_02 : 1;
		u8 can_walljump : 1;
	};
	u8 flags14;
#endif
	u8 flags15;
	u8 flags16;
	u8 flags17;
	u8 flags18;
	u8 flags19;
	char pad222B[0x222C - 0x222B];
	union {
		char raw[0xD0];
		struct {
			char pad000[0x14];
			u8 extender_state;
		} samus;
	} char_data;
	char pad22FC[0x2340 - 0x22FC];
	union {
		char raw[0xAC];
		struct {
			u32 is_smash_turn;
			f32 turn_direction;
			f32 dash_direction;
			u32 pad00C;
			f32 tilt_turn_timer;
			u32 pad014;
			u32 can_dash;
			u32 buffered_buttons;
		} Turn;
		struct {
			s32 respawn_timer;
		} Dead;
	} as_data;

	// Place new data at end of AS data space
	template<typename T>
	T *custom_as_data()
	{
		return (T*)&as_data.raw[sizeof(as_data.raw) - sizeof(T)];
	}

	template<typename T>
	const T *custom_as_data() const
	{
		return (T*)&as_data.raw[sizeof(as_data.raw) - sizeof(T)];
	}
};

struct PlayerBlockStats {
	char pad000[0xCB0];
	f32 total_sdi_distance_x;
	f32 total_sdi_distance_y;
	char padCB8[0xDA8 - 0xCB8];
};

extern "C" {

HSD_GObj *PlayerBlock_GetSubCharGObj(s32 slot, s32 subchar);
HSD_GObj *PlayerBlock_GetGObj(s32 slot);
s32 PlayerBlock_GetCharacterID(s32 slot, s32 subchar);
u8 PlayerBlock_GetPort(s32 slot);
u8 PlayerBlock_GetTeam(s32 slot);
u32 PlayerBlock_GetSlotType(s32 slot);
bool PlayerBlock_ShouldDisplayPortTag(s32 slot);
void PlayerBlock_AddTotalSDIDistance(s32 slot, f32 x, f32 y);

SubactionInfo *Player_GetSubactionInfo(const Player *player, s32 subaction);

PlayerBlockStats *PlayerBlock_GetStats(s32 slot);
s32 PlayerBlockStats_GetActionStat(const PlayerBlockStats *stats, u32 index);

void PlayerThink_Input(HSD_GObj *gobj);

bool Player_IsCPU(const Player *player);

bool Player_GetWindPush(const HSD_GObj *gobj, vec3 *push);

void Player_DoThrow(HSD_GObj *gobj, u32 state);

bool Player_CollisionAir(HSD_GObj *gobj);
void Player_AirToGroundTransition(Player *player);

void Player_ASChange(HSD_GObj *gobj, u32 new_state, u32 flags, HSD_GObj *parent,
                     f32 start_frame, f32 frame_rate, f32 lerp_override);

bool GetPortRumbleFlag(u32 port);
void SetPortRumbleFlag(u32 port, bool flag);

} // extern "C"

// Returns Popo's port if given a Nana pointer
inline u8 Player_GetPort(const Player *player)
{
	if (!player->is_backup_climber)
		return player->port;

	const auto *gobj = PlayerBlock_GetSubCharGObj(player->slot, 0);
	return gobj != nullptr ? gobj->get<Player>()->port : player->port;
}

// Like Player_IsCPU, but returns false for a synced human Nana
inline bool Player_IsCPUControlled(const Player *player)
{
	if (PlayerBlock_GetSlotType(player->slot) != SlotType_Human)
		return true;

	return player->is_backup_climber && !player->is_nana_synced;
}