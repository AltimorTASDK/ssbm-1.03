#pragma once

#include <gctypes.h>

struct SceneMinorData;

enum Scene {
	Scene_Menu         = 1,
	Scene_VsMode       = 2,
	Scene_Tournament   = 0x1B,
	Scene_Training     = 0x1C
};

enum VsScene {
	VsScene_CSS         = 0,
	VsScene_SSS         = 1,
	VsScene_Game        = 2,
	VsScene_SuddenDeath = 3,
	VsScene_Victory     = 4
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
extern u8 SceneMinor;

bool IsSinglePlayerMode();
void Scene_Exit();

}