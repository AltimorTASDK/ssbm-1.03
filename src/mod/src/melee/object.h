#pragma once

#include "hsd/gobj.h"
#include "hsd/jobj.h"
#include "util/vector.h"

constexpr auto NO_LINE = -1;

using PlatformPassCallback = bool(void *param, int line_id);

// GObj proc slinks
enum SLink {
	SLink_Hitlag          = 0,
	SLink_Animation       = 1,
	SLink_CPU             = 2,
	SLink_Input           = 3,
	SLink_Physics         = 4,
	SLink_ItemCollision   = 5,
	SLink_PlayerCollision = 6,
	SLink_IK              = 7,
	SLink_ForcePosition   = 8,
	SLink_GFX             = 9,
	SLink_GrabCollision   = 12,
	SLink_AttackCollision = 13,
	SLink_Damage          = 14,
	SLink_JointPhysics    = 16,
	SLink_Camera          = 18,
	SLink_NameTag         = 22
};

enum Collide {
	Collide_LeftWallHug  = 0x20,
	Collide_RightWallHug = 0x800
};

enum ECBFlag {
	ECBFlag_FreezeBottom = 0x10
};

struct ECB {
	vec2 top, bottom, left, right;
};

struct CollisionContact {
	s32 line;
	u32 attributes;
	vec3 normal;
};

struct Physics {
	HSD_GObj *gobj;
	vec3 position;
	vec3 trace_start_position;
	vec3 start_position;
	vec3 last_position;
	u8 push_flags;
	u16 direction;
	s32 last_stage_animation_frame;
	s32 platform_pass_line;
	s32 right_ledge_line;
	s32 left_ledge_line;
	s32 exclude_joint;
	s32 only_check_joint;
	f32 weight;
	f32 ledge_grab_width;
	f32 ledge_grab_y_offset;
	f32 ledge_grab_height;
	f32 stick_x;
	ECB unsqueezed_ecb;
	ECB desired_ecb;
	ECB ecb;
	ECB trace_start_ecb;
	ECB last_ecb;
	s32 use_ecb_bones;
	HSD_JObj *root_bone;
	HSD_JObj *ecb_bones[6];
	f32 ecb_side_y_offset;
	f32 min_asymmetrical_ecb_height;
	f32 min_asymmetrical_ecb_width;
	u32 ecb_flags;
	u32 collide;
	u32 last_collide;
	char pad013C[0x140 - 0x13C];
	vec3 contact_point;
	CollisionContact floor;
	CollisionContact left_wall;
	CollisionContact right_wall;
	CollisionContact ceiling;
};

struct Hitbox {
	u32 state;
	u32 active;
	u32 damage;
	f32 staled_damage;
	vec3 offset;
	f32 radius;
	u32 angle;
	u32 kbg;
	u32 fkv;
	u32 bkb;
	u32 element;
	u32 shield_damage;
	char pad0038[0x3C - 0x38];
	u32 sfx;
	u8 flags1;
	u8 flags2;
	struct {
		u8 flags3_80 : 1;
		u8 flags3_40 : 1;
		u8 flags3_20 : 1;
		u8 flags3_10 : 1;
		u8 is_projectile : 1;
		u8 flags3_04 : 1;
		u8 flags3_02 : 1;
		u8 flags3_01 : 1;
	};
	u8 flags4;
	char pad0044[0x4C - 0x44];
	vec3 position;
	vec3 last_position;
	vec3 last_contact;
	f32 last_contact_depth;
	struct {
		HSD_GObj *gobj;
		u32 flags;
	} hit_objects[12];
	char pad00D4[0x138 - 0xD4];
};

struct Hurtbox {
	u32 body_state;
	vec3 offset1;
	vec3 offset2;
	f32 radius;
	HSD_JObj *bone;
	u8 flags;
	char align0025[0x28 - 0x25];
	vec3 position1;
	vec3 position2;
	u32 bone_id;
	u32 flinch_type;
	u32 grabbable;
};

extern "C" {

u32 Physics_IsOnPlatform(const Physics *phys);

bool Physics_Collision_Grounded(Physics *phys);
bool Physics_Collision_Air_StayAirborne(Physics *phys);

bool Collision_GetLineSpeed(s32 line, const vec3 &position, vec3 *speed);

} // extern "C"