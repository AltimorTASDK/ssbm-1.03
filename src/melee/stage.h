#pragma once

union StageParams {
	struct {
		float left_plat_height;
		float pad004;
		float right_plat_height;
	} fod;
};

extern "C" {

StageParams *Stage_GetParameters();

}