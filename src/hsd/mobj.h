#pragma once

struct HSD_AObjDesc;
struct HSD_RenderAnim;
struct HSD_TexAnim;

struct HSD_ChanAnim {
	HSD_ChanAnim *next;
	HSD_AObjDesc *aobjdesc;
};

struct HSD_TevRegAnim {
	HSD_TevRegAnim *next;
	HSD_AObjDesc *aobjdesc;
};

struct HSD_RenderAnim {
	HSD_ChanAnim *chananim;
	HSD_TevRegAnim *reganim;
};

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