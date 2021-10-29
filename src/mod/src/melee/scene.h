#pragma once

#include <gctypes.h>

struct SceneMinorData;

enum Scene {
	Scene_Menu         = 1,
	Scene_VsMode       = 2,
	Scene_DebugMenu    = 6,
	Scene_Tournament   = 27,
	Scene_Training     = 28,
	Scene_Boot         = 40
};

enum VsScene {
	VsScene_CSS         = 0,
	VsScene_SSS         = 1,
	VsScene_Game        = 2,
	VsScene_SuddenDeath = 3,
	VsScene_Victory     = 4
};

enum PauseBit {
	PauseBit_DevelopPause,
	PauseBit_Pause,
	PauseBit_TrainingMenu
};

using SceneProc = void(SceneMinorData*);

struct SceneMinorData {
	u8 id;
	SceneProc *enter;
	SceneProc *exit;
	u32 pad00C;
	void *enter_data;
	void *exit_data;
};

extern "C" {

extern u8 SceneMajor;
extern u8 SceneMajorPrevious;
extern u8 SceneMinor;
extern u8 SceneMinorPrevious;

bool IsSinglePlayerMode();
void Scene_Exit();
bool Scene_CheckPauseFlag(u32 flag);
void Scene_SetMajorPending(u8 scene);
void *GetSceneExitData();

}