#pragma once

#include "hsd/gobj.h"
#include "hsd/jobj.h"
#include "util/vector.h"

constexpr auto NO_LINE = -1;

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
	char pad0034[0x84 - 0x34];
	ECB desired_ecb;
	ECB ecb;
	ECB trace_start_ecb;
	ECB last_ecb;
	s32 use_ecb_bones;
	HSD_JObj *root_bone;
	HSD_JObj *ecb_bones[6];
	char pad0124[0x130 - 0x124];
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
	u8 flags3;
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

bool Physics_IsOnPlatform(const Physics *phys);

bool Physics_Collision_Grounded(Physics *phys);
bool Physics_Collision_Air_StayAirborne(Physics *phys);

bool Collision_GetLineSpeed(s32 line, const vec3 &position, vec3 *speed);

} // extern "C"