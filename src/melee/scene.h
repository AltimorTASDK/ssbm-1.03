#pragma once

#include <gctypes.h>

enum Scene {
	Scene_Menu         = 1,
	Scene_VsMode       = 2,
	Scene_Tournament   = 0x1B,
	Scene_Training     = 0x1C
};

extern "C" {

extern u8 SceneMajor;
extern u8 SceneMinor;

bool IsSinglePlayerMode();

}