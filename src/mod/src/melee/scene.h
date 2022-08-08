#pragma once

#include <gctypes.h>

struct SceneMinorData;

enum Scene {
	Scene_Menu           = 1,
	Scene_VsMode         = 2,
	Scene_Classic        = 3,
	Scene_Adventure      = 4,
	Scene_AllStar        = 5,
	Scene_DebugMenu      = 6,
	Scene_Slippi         = 8, // Unused debug CSS in vailla
	Scene_TargetTest     = 15,
#ifdef PAL
	Scene_AttractMode    = 27,
	Scene_Tournament     = 30,
	Scene_Controls       = 30,
	Scene_Training       = 31,
	Scene_Boot           = 43,
	Scene_ReloadData     = 44,
	Scene_None           = 48,
#else
	Scene_AttractMode    = 24,
	Scene_Tournament     = 27,
	Scene_Controls       = 27,
	Scene_Training       = 28,
	Scene_HomeRunContest = 32,
	Scene_10ManMelee     = 33,
	Scene_100ManMelee    = 34,
	Scene_3MinMelee      = 35,
	Scene_15MinMelee     = 36,
	Scene_EndlessMelee   = 37,
	Scene_CruelMelee     = 38,
	Scene_Boot           = 40,
	Scene_ReloadData     = 41,
	Scene_EventMatch     = 43,
	Scene_None           = 45,
#endif
};

enum VsScene {
	VsScene_CSS         = 0,
	VsScene_SSS         = 1,
	VsScene_Game        = 2,
	VsScene_SuddenDeath = 3,
	VsScene_Victory     = 4
};

enum PauseBit {
	PauseBit_DevelopPause = 0,
	PauseBit_Pause        = 1,
	PauseBit_TrainingMenu = 2
};

using SceneProc = void(SceneMinorData*);

struct SceneDataPointers {
	u8 menu_id;
	void *enter_data;
	void *exit_data;
};

struct SceneMinorData {
	u8 id;
	SceneProc *enter;
	SceneProc *exit;
	SceneDataPointers data;
};

extern "C" {

extern u8 SceneMajor;
extern u8 SceneMajorPrevious;
extern u8 SceneMinor;
extern u8 SceneMinorPrevious;
extern u32 SceneFrameCount;

bool IsSinglePlayerMode();
void Scene_Exit();
bool Scene_CheckPauseFlag(u32 flag);
void Scene_SetMajorPending(u8 scene);
void *Scene_GetExitData();

} // extern "C"