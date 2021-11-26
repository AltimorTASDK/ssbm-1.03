#pragma once

#include <gctypes.h>

enum BGM {
	BGM_Undefined            = -1,

	BGM_BalloonFight         = 2,
	BGM_BigBlue              = 3,
	BGM_Brinstar             = 97,
	BGM_BrinstarDepths       = 51,
	BGM_Corneria             = 6,
	BGM_DrMario              = 7,
	BGM_Earthbound           = 31,
	BGM_FireEmblem           = 1,
	BGM_FlatZone             = 30,
	BGM_GreatBay             = 34,
	BGM_GreenGreens          = 35,
	BGM_IcicleMountain       = 40,
	BGM_JungleJapes          = 50,
	BGM_KongoJungle          = 33,
	BGM_KongoJungleN64       = 57,
	BGM_MachRider            = 55,

	BGM_MetalBattle          = 80,
	BGM_Mother               = 60,
	BGM_Mother2              = 61,
	BGM_MushroomKingdom      = 41,
	BGM_MushroomKingdomII    = 43,
	BGM_MuteCity             = 56,
	BGM_PokeFloats           = 65,
	BGM_PrincessPeachsCastle = 4,
	BGM_RainbowCruise        = 66,
	BGM_SariasTheme          = 74,
	BGM_SuperMarioBros3      = 77,
	BGM_Temple               = 75,
	BGM_Venom                = 84,
	BGM_YoshisIsland         = 95,
	BGM_YoshisIslandN64      = 59,

	// Legal stage themes
	BGM_Battlefield          = 81,
	BGM_BattleTheme          = 63,
	BGM_DreamLandN64         = 58,
	BGM_FinalDestination     = 78,
	BGM_FountainOfDreams     = 49,
	BGM_MultiManMelee1       = 38,
	BGM_MultiManMelee2       = 39,
	BGM_PokemonStadium       = 64,
	BGM_YoshisStory          = 96,

	// Additional menu only music
	BGM_AllStarIntro         = 0,
	BGM_Targets              = 83,
	BGM_Trophy               = 53
};

extern "C" {

extern char CurrentBGMPath[64];

s32 PlayBGM(s32 bgm);
s32 Menu_GetBGM();

}