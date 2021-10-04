#pragma once

#include "hsd/gobj.h"

enum ActionState {
	AS_DeadDown,
	AS_DeadLeft,
	AS_DeadRight,
	AS_DeadUp,
	AS_DeadUpStar,
	AS_DeadUpStarIce,
	AS_DeadUpFall,
	AS_DeadUpFallHitCamera,
	AS_DeadUpFallHitCameraFlat,
	AS_DeadUpFallIce,
	AS_DeadUpFallHitCameraIce,
	AS_Sleep,
	AS_Rebirth,
	AS_RebirthWait,
	AS_Wait,
	AS_WalkSlow,
	AS_WalkMiddle,
	AS_WalkFast,
	AS_Turn,
	AS_TurnRun,
	AS_Dash,
	AS_Run,
	AS_RunDirect,
	AS_RunBrake,
	AS_KneeBend,
	AS_JumpF,
	AS_JumpB,
	AS_JumpAerialF,
	AS_JumpAerialB,
	AS_Fall,
	AS_FallF,
	AS_FallB,
	AS_FallAerial,
	AS_FallAerialF,
	AS_FallAerialB,
	AS_FallSpecial,
	AS_FallSpecialF,
	AS_FallSpecialB,
	AS_DamageFall,
	AS_Squat,
	AS_SquatWait,
	AS_SquatRv,
	AS_Landing,
	AS_LandingFallSpecial,
	AS_Attack11,
	AS_Attack12,
	AS_Attack13,
	AS_Attack100Start,
	AS_Attack100Loop,
	AS_Attack100End,
	AS_AttackDash,
	AS_AttackS3Hi,
	AS_AttackS3HiS,
	AS_AttackS3S,
	AS_AttackS3LwS,
	AS_AttackS3Lw,
	AS_AttackHi3,
	AS_AttackLw3,
	AS_AttackS4Hi,
	AS_AttackS4HiS,
	AS_AttackS4S,
	AS_AttackS4LwS,
	AS_AttackS4Lw,
	AS_AttackHi4,
	AS_AttackLw4,
	AS_AttackAirN,
	AS_AttackAirF,
	AS_AttackAirB,
	AS_AttackAirHi,
	AS_AttackAirLw,
	AS_LandingAirN,
	AS_LandingAirF,
	AS_LandingAirB,
	AS_LandingAirHi,
	AS_LandingAirLw,
	AS_DamageHi1,
	AS_DamageHi2,
	AS_DamageHi3,
	AS_DamageN1,
	AS_DamageN2,
	AS_DamageN3,
	AS_DamageLw1,
	AS_DamageLw2,
	AS_DamageLw3,
	AS_DamageAir1,
	AS_DamageAir2,
	AS_DamageAir3,
	AS_DamageFlyHi,
	AS_DamageFlyN,
	AS_DamageFlyLw,
	AS_DamageFlyTop,
	AS_DamageFlyRoll,
	AS_LightGet,
	AS_HeavyGet,
	AS_LightThrowF,
	AS_LightThrowB,
	AS_LightThrowHi,
	AS_LightThrowLw,
	AS_LightThrowDash,
	AS_LightThrowDrop,
	AS_LightThrowAirF,
	AS_LightThrowAirB,
	AS_LightThrowAirHi,
	AS_LightThrowAirLw,
	AS_HeavyThrowF,
	AS_HeavyThrowB,
	AS_HeavyThrowHi,
	AS_HeavyThrowLw,
	AS_LightThrowF4,
	AS_LightThrowB4,
	AS_LightThrowHi4,
	AS_LightThrowLw4,
	AS_LightThrowAirF4,
	AS_LightThrowAirB4,
	AS_LightThrowAirHi4,
	AS_LightThrowAirLw4,
	AS_HeavyThrowF4,
	AS_HeavyThrowB4,
	AS_HeavyThrowHi4,
	AS_HeavyThrowLw4,
	AS_SwordSwing1,
	AS_SwordSwing3,
	AS_SwordSwing4,
	AS_SwordSwingDash,
	AS_BatSwing1,
	AS_BatSwing3,
	AS_BatSwing4,
	AS_BatSwingDash,
	AS_ParasolSwing1,
	AS_ParasolSwing3,
	AS_ParasolSwing4,
	AS_ParasolSwingDash,
	AS_HarisenSwing1,
	AS_HarisenSwing3,
	AS_HarisenSwing4,
	AS_HarisenSwingDash,
	AS_StarRodSwing1,
	AS_StarRodSwing3,
	AS_StarRodSwing4,
	AS_StarRodSwingDash,
	AS_LipStickSwing1,
	AS_LipStickSwing3,
	AS_LipStickSwing4,
	AS_LipStickSwingDash,
	AS_ItemParasolOpen,
	AS_ItemParasolFall,
	AS_ItemParasolFallSpecial,
	AS_ItemParasolDamageFall,
	AS_LGunShoot,
	AS_LGunShootAir,
	AS_LGunShootEmpty,
	AS_LGunShootAirEmpty,
	AS_FireFlowerShoot,
	AS_FireFlowerShootAir,
	AS_ItemScrew,
	AS_ItemScrewAir,
	AS_DamageScrew,
	AS_DamageScrewAir,
	AS_ItemScopeStart,
	AS_ItemScopeRapid,
	AS_ItemScopeFire,
	AS_ItemScopeEnd,
	AS_ItemScopeAirStart,
	AS_ItemScopeAirRapid,
	AS_ItemScopeAirFire,
	AS_ItemScopeAirEnd,
	AS_ItemScopeStartEmpty,
	AS_ItemScopeRapidEmpty,
	AS_ItemScopeFireEmpty,
	AS_ItemScopeEndEmpty,
	AS_ItemScopeAirStartEmpty,
	AS_ItemScopeAirRapidEmpty,
	AS_ItemScopeAirFireEmpty,
	AS_ItemScopeAirEndEmpty,
	AS_LiftWait,
	AS_LiftWalk1,
	AS_LiftWalk2,
	AS_LiftTurn,
	AS_GuardOn,
	AS_Guard,
	AS_GuardOff,
	AS_GuardSetOff,
	AS_GuardReflect,
	AS_DownBoundU,
	AS_DownWaitU,
	AS_DownDamageU,
	AS_DownStandU,
	AS_DownAttackU,
	AS_DownFowardU,
	AS_DownBackU,
	AS_DownSpotU,
	AS_DownBoundD,
	AS_DownWaitD,
	AS_DownDamageD,
	AS_DownStandD,
	AS_DownAttackD,
	AS_DownFowardD,
	AS_DownBackD,
	AS_DownSpotD,
	AS_Passive,
	AS_PassiveStandF,
	AS_PassiveStandB,
	AS_PassiveWall,
	AS_PassiveWallJump,
	AS_PassiveCeil,
	AS_ShieldBreakFly,
	AS_ShieldBreakFall,
	AS_ShieldBreakDownU,
	AS_ShieldBreakDownD,
	AS_ShieldBreakStandU,
	AS_ShieldBreakStandD,
	AS_FuraFura,
	AS_Catch,
	AS_CatchPull,
	AS_CatchDash,
	AS_CatchDashPull,
	AS_CatchWait,
	AS_CatchAttack,
	AS_CatchCut,
	AS_ThrowF,
	AS_ThrowB,
	AS_ThrowHi,
	AS_ThrowLw,
	AS_CapturePulledHi,
	AS_CaptureWaitHi,
	AS_CaptureDamageHi,
	AS_CapturePulledLw,
	AS_CaptureWaitLw,
	AS_CaptureDamageLw,
	AS_CaptureCut,
	AS_CaptureJump,
	AS_CaptureNeck,
	AS_CaptureFoot,
	AS_EscapeF,
	AS_EscapeB,
	AS_Escape,
	AS_EscapeAir,
	AS_ReboundStop,
	AS_Rebound,
	AS_ThrownF,
	AS_ThrownB,
	AS_ThrownHi,
	AS_ThrownLw,
	AS_ThrownLwWomen,
	AS_Pass,
	AS_Ottotto,
	AS_OttottoWait,
	AS_FlyReflectWall,
	AS_FlyReflectCeil,
	AS_StopWall,
	AS_StopCeil,
	AS_MissFoot,
	AS_CliffCatch,
	AS_CliffWait,
	AS_CliffClimbSlow,
	AS_CliffClimbQuick,
	AS_CliffAttackSlow,
	AS_CliffAttackQuick,
	AS_CliffEscapeSlow,
	AS_CliffEscapeQuick,
	AS_CliffJumpSlow1,
	AS_CliffJumpSlow2,
	AS_CliffJumpQuick1,
	AS_CliffJumpQuick2,
	AS_AppealR,
	AS_AppealL,
	AS_ShoulderedWait,
	AS_ShoulderedWalkSlow,
	AS_ShoulderedWalkMiddle,
	AS_ShoulderedWalkFast,
	AS_ShoulderedTurn,
	AS_ThrownFF,
	AS_ThrownFB,
	AS_ThrownFHi,
	AS_ThrownFLw,
	AS_CaptureCaptain,
	AS_CaptureYoshi,
	AS_YoshiEgg,
	AS_CaptureKoopa,
	AS_CaptureDamageKoopa,
	AS_CaptureWaitKoopa,
	AS_ThrownKoopaF,
	AS_ThrownKoopaB,
	AS_CaptureKoopaAir,
	AS_CaptureDamageKoopaAir,
	AS_CaptureWaitKoopaAir,
	AS_ThrownKoopaAirF,
	AS_ThrownKoopaAirB,
	AS_CaptureKirby,
	AS_CaptureWaitKirby,
	AS_ThrownKirbyStar,
	AS_ThrownCopyStar,
	AS_ThrownKirby,
	AS_BarrelWait,
	AS_Bury,
	AS_BuryWait,
	AS_BuryJump,
	AS_DamageSong,
	AS_DamageSongWait,
	AS_DamageSongRv,
	AS_DamageBind,
	AS_CaptureMewtwo,
	AS_CaptureMewtwoAir,
	AS_ThrownMewtwo,
	AS_ThrownMewtwoAir,
	AS_WarpStarJump,
	AS_WarpStarFall,
	AS_HammerWait,
	AS_HammerWalk,
	AS_HammerTurn,
	AS_HammerKneeBend,
	AS_HammerFall,
	AS_HammerJump,
	AS_HammerLanding,
	AS_KinokoGiantStart,
	AS_KinokoGiantStartAir,
	AS_KinokoGiantEnd,
	AS_KinokoGiantEndAir,
	AS_KinokoSmallStart,
	AS_KinokoSmallStartAir,
	AS_KinokoSmallEnd,
	AS_KinokoSmallEndAir,
	AS_Entry,
	AS_EntryStart,
	AS_EntryEnd,
	AS_DamageIce,
	AS_DamageIceJump,
	AS_CaptureMasterhand,
	AS_CapturedamageMasterhand,
	AS_CapturewaitMasterhand,
	AS_ThrownMasterhand,
	AS_CaptureKirbyYoshi,
	AS_KirbyYoshiEgg,
	AS_CaptureLeadead,
	AS_CaptureLikelike,
	AS_DownReflect,
	AS_CaptureCrazyhand,
	AS_CapturedamageCrazyhand,
	AS_CapturewaitCrazyhand,
	AS_ThrownCrazyhand,
	AS_BarrelCannonWait,
	AS_NAMED_MAX
};

extern "C" {

void AS_020_Dash(HSD_GObj *gobj, int forward);
void AS_029_Fall(HSD_GObj *gobj);
void AS_041_SquatRv(HSD_GObj *gobj);

}