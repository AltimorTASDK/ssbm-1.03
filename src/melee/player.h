#pragma once

#include "hsd/jobj.h"
#include "hsd/gobj.h"
#include "melee/object.h"
#include "util/vector.h"
#include <gctypes.h>

enum CID {
	CID_Mario,
	CID_Fox,
	CID_Falcon,
	CID_Dong,
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
	CID_Jigglypuff,
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
	CID_Boi,
	CID_Gorl,
	CID_GigaBowser,
	CID_Sandbag,
	CID_Max
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
    u8 stick_x_hold_time_2;
    u8 stick_y_hold_time_2;
    u8 analog_lr_hold_time_2;
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
	float direction;
};

struct Player {
	static constexpr auto MAX_HITBOXES = 4;
	static constexpr auto MAX_HURTBOXES = 15;

	HSD_GObj *gobj;
	u32 character_id;
	u32 spawn_count;
	u8 slot;
	u32 action_state;
	u32 subaction;
	char pad0018[0x2C - 0x18];
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
	char pad0100[0x3E4 - 0x100];
	SubactionState subaction_state;
	char pad0400[0x4B8 - 0x400];
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
	char pad0505[0x614 - 0x505];
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
	char pad1908[0x1988 - 0x1908];
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
	u8 cpu_flags3;
	u8 cpu_flags4;
	PopoData popo_data_buffer[30];
	PopoData *popo_data_write;
	PopoData *popo_data_read;
	char pad1ED4[0x2218 - 0x1ED4];
	u8 flags1;
	u8 flags2;
	u8 flags3;
	u8 flags4;
	u8 flags5;
	u8 flags6;
	struct {
		u8 is_invisible : 1;
	};
	struct {
		u8 flags8_80 : 1;
		u8 flags8_40 : 1;
		u8 flags8_20 : 1;
		u8 no_update : 1;
		u8 is_backup_climber : 1;
	};
	u8 flags9;
	u8 flags10;
	u8 flags11;
	u8 flags12;
	u8 flags13;
	u8 flags14;
	u8 flags15;
	u8 flags16;
	u8 flags17;
	u8 flags18;
	u8 flags19;
	char pad2226[0x222C - 0x222B];
	char char_specific_data[0xD0];
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
	} as_data;
};

extern "C" {
	
HSD_GObj *PlayerBlock_GetSubCharGObj(s32 slot, s32 subchar);
HSD_GObj *PlayerBlock_GetGObj(s32 slot);

void PlayerThink_Input(HSD_GObj *gobj);
	
}