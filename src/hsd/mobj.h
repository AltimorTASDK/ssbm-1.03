#pragma once

struct HSD_AObjDesc;
struct HSD_RenderAnim;
struct HSD_TexAnim;

struct HSD_MatAnim {
	HSD_MatAnim *next;
	HSD_AObjDesc *aobjdesc;
	HSD_TexAnim *texanim;
	HSD_RenderAnim *renderanim;
};

struct HSD_MatAnimJoint {
	HSD_MatAnimJoint *child;
	HSD_MatAnimJoint *next;
	HSD_MatAnim *matanim;
};