/*
 * Define hooks as pairs of words containing the function address and the hook
 * address. We then overwrite the first instruction of the function with a
 * branch to the hook, and then overwrite the hook definition with the original
 * instruction and a branch back to the original function.
 */
.section .init

/*
 * void _start()
 */
 .global _start
 _start:
        mflr    r0
        stw     r0, 4(r1)
        stwu    r1, -16(r1)
        stw     r31, 12(r1)
        stw     r30, 8(r1)

        # Apply hooks
        lis     r31, (hooks_start - 4)@h
        ori     r31, r31, (hooks_start - 4)@l
        lis     r30, (hooks_end - 4)@h
        ori     r30, r30, (hooks_end - 4)@l

hook_loop:
        cmpw    r31, r30
        beq     hook_end

        # Copy original instruction over function address
        lwzu    r3, 4(r31)
        lwz     r0, 0(r3)
        stw     r0, 0(r31)

        # Overwrite original instruction with branch to hook
        lwzu    r4, 4(r31)
        bl      write_branch

        # Overwrite hook address with branch to original
        addi    r4, r3, 4
        mr      r3, r31
        bl      write_branch

        b       hook_loop
hook_end:

        # Zero out the bussy
        lis     r3, __BSS_START__@h
        ori     r3, r3, __BSS_START__@l
        li      r4, 0
        li      r5, __BSS_SIZE__
        bl      memset

        # Run C++ constructors
        lis     r31, __CTOR_END__@h
        ori     r31, r31, __CTOR_END__@l
        lis     r30, __CTOR_LIST__@h
        ori     r30, r30, __CTOR_LIST__@l

ctor_loop:
        cmpw    r31, r30
        beq     ctor_end

        # Call constructor
        lwzu    r0, -4(r31)
        mtctr   r0
        bctrl
        b       ctor_loop
ctor_end:

        lwz     r0, 20(r1)
        lwz     r31, 12(r1)
        lwz     r30, 8(r1)
        addi    r1, r1, 16
        mtlr    r0
        blr

/*
 * void *write_branch(void *address, void *target)
 * Returns address
 */
write_branch:
        mflr    r0
        stw     r0, 4(r1)
        stwu    r1, -12(r1)
        stw     r31, 8(r1)

        mr      r31, r3

        sub     r0, r4, r3
        rlwinm  r0, r0, 0, 6, 29
        oris    r0, r0, 0x4800
        stw     r0, 0(r3)

        # Clear cache for overwritten instruction
        li      r4, 4
        bl      ICInvalidateRange

        mr      r3, r31

        lwz     r0, 16(r1)
        lwz     r31, 8(r1)
        addi    r1, r1, 12
        mtlr    r0
        blr

hooks_start:
.macro add_hook name
        .global orig_\name
        orig_\name:
        .int \name
        .int hook_\name
.endm

.set TumbleWiggleCheck,       Interrupt_AS_DamageFall+0x9C
.set UpdatePadButtons,        HSD_PadRenewMasterStatus+0x400
.set UpdatePlayerStick,       PlayerThink_Input+0x1D0
.set CheckScrollingRuleIndex, Menu_UpdateRuleDisplay+0x418
.set ShowNameTagCheck,        NameTag_Think+0x28
.set CheckStockStealInput,    Match_CheckStockSteal+0xF8
.set GetDebugMenuButtons,     DebugMenu_Input+0x1C
.set EnterSpecialMelee,       VsMenu_Think+0xBC
.set TapJumpCheck_Jump,       Interrupt_Jump+0x40
.set TapJumpCheck_JumpAerial, Interrupt_JumpAerial+0x44
.set TapJumpCheck_JumpInput,  Player_CheckJumpInput+0x14
.set TapJumpCheck_JumpOoRun,  Interrupt_Jump_OoRun+0x44
.set TapJumpCheck_JumpOoS,    Interrupt_Jump_OoS+0x48
.set CheckUTiltInput,         Interrupt_UTilt+0x24
.set CheckFTiltInput,         Interrupt_FTilt+0x24
.set CheckDTiltInput,         Interrupt_DTilt+0x24
.set CheckJabInput,           Interrupt_Jab+0x20
.set CheckJab2Input,          Interrupt_Jab2+0x40
.set CheckJab3Input,          Interrupt_Jab3+0x3C
.set CheckRapidJabInput,      Interrupt_RapidJab+0x18
.set CheckFTiltAngle,         AS_053_056_FTilt+0x34

add_hook Bubble_GetPosition
add_hook CheckScrollingRuleIndex
add_hook CheckStockStealInput
add_hook CheckUTiltInput
add_hook CheckFTiltInput
add_hook CheckDTiltInput
add_hook CheckJabInput
add_hook CheckJab2Input
add_hook CheckJab3Input
add_hook CheckRapidJabInput
add_hook CheckFTiltAngle
add_hook CmSubject_WorldToScreen
add_hook CObjLoad
add_hook DebugMenu_Exit
add_hook DevelopText_Draw
add_hook EnterSpecialMelee
add_hook GetDebugMenuButtons
add_hook HSD_ResetScene
add_hook HUD_GetStockIconPosition
add_hook Interrupt_AS_Turn
add_hook Interrupt_SpotDodge
add_hook Interrupt_SpotDodge_OoS
add_hook Interrupt_SquatRv
add_hook Interrupt_TurnOrDash
add_hook Match_HandlePause
add_hook MatchController_UpdateTeamWinners
add_hook MatchController_UpdateWinners
add_hook Menu_CreateExtraRulesMenu
add_hook Menu_CreateRandomStageMenu
add_hook Menu_CreateRuleDescriptionText
add_hook Menu_ExitToRulesMenu
add_hook Menu_ExtraRulesMenuInput
add_hook Menu_GetBGM
add_hook Menu_ItemMenuInput
add_hook Menu_MenuPanelThink
add_hook Menu_RandomStageMenuInput
add_hook Menu_RandomStageMenuScroll
add_hook Menu_RulesMenuInput
add_hook Menu_SetupExtraRulesMenu
add_hook Menu_SetupItemToggles
add_hook Menu_SetupRulesMenu
add_hook Menu_SetupRandomStageMenu
add_hook Menu_SetupRandomStageToggles
add_hook Menu_UpdateExtraRuleDescriptionText
add_hook Menu_UpdateItemDisplay
add_hook Menu_UpdateRandomStageDisplay
add_hook Menu_UpdateRuleDisplay
add_hook Menu_UpdateRuleValue
add_hook NameTag_SetupForPlayer
add_hook Player_CheckCStickUSmashInput
add_hook Player_CheckCStickFSmashInput
add_hook Player_CheckCStickDSmashInput
add_hook Player_GetAirdodgeAngle
add_hook Scene_Match_Exit
add_hook ShowNameTagCheck
add_hook SSS_Init
add_hook SSS_Think
add_hook Stage_Dreamland_WhispyUpdate
add_hook Stage_FinalDestination_BackgroundUpdate
add_hook Stage_Fountain_CreateReflection
add_hook Stage_Fountain_CreateStars
add_hook Stage_Fountain_SetupModel
add_hook Stage_Fountain_UpdateReflection
add_hook Stage_GetBGM
add_hook Stage_IsValidRandomChoice
add_hook Stage_PokemonStadium_TransformationDecide
add_hook Stage_Setup
add_hook Stage_YoshisStory_ShyGuyUpdate
add_hook TapJumpCheck_Jump
add_hook TapJumpCheck_JumpAerial
add_hook TapJumpCheck_JumpInput
add_hook TapJumpCheck_JumpOoRun
add_hook TapJumpCheck_JumpOoS
add_hook Text_UpdateSubtextSize
add_hook TrainingMenu_Create
add_hook TrainingMenu_Think
add_hook TumbleWiggleCheck
add_hook UpdatePadButtons
add_hook UpdatePlayerStick
# PDF+Latency
.set SceneLoopStart,   Scene_RunLoop+0x68
.set FirstEngineFrame, Scene_RunLoop+0xA8
add_hook SceneLoopStart
add_hook FirstEngineFrame
add_hook HSD_VICopyXFBASync
add_hook UpdatePadFetchRate
add_hook SISetSamplingRate
#ifdef POLL_DEBUG
add_hook PadFetchCallback
add_hook HSD_PerfSetStartTime
add_hook HSD_VIGetXFBDrawEnable
add_hook HSD_VIPreRetraceCB
add_hook Scene_RunLoop
#endif
# Nana hardware input buffer
add_hook Player_Nana_RecordPopoData
# Memcard
add_hook MemoryCard_RequestSave
add_hook MemoryCard_DoLoadData
add_hook UpdateMemCardState
add_hook GetNextSceneMajorCallback
# 1P camera
#if 0
add_hook Camera_GetBounds
#endif
add_hook Camera_SetNormal
# Neutral spawns
add_hook Stage_GetSpawnPoint
add_hook PlayerBlock_GetRespawnPoint
add_hook Match_SetSpawnFacingDirections
# Crew mode
.set CheckToSetStockCount,             Menu_RulesMenuThink+0x2AC
.set CheckToUseStockCountText,         Menu_SetupRulesMenu+0x410
.set CheckToUseStockCountDescription,  Menu_CreateRuleDescriptionText+0x48
.set CheckToUpdateStockCountText,      Menu_UpdateRuleDisplay+0x230
.set CheckToUpdateStockCountTextAgain, Menu_UpdateRuleDisplay+0x32C
.set CheckToCycleToStockCountText,     Menu_UpdateRuleDisplay+0x504
.set CheckToUpdateStockCountValue,     Menu_UpdateRuleValue+0x6C
.set CheckToSetStockCountValue1,       Menu_RulesMenuInput+0x328
.set CheckToSetStockCountValue2,       Menu_RulesMenuInput+0x3DC
.set CheckToSetStockCountValue3,       Menu_RulesMenuInput+0x43C
.set CheckToSetStockCountValue4,       Menu_RulesMenuInput+0x510
.set CheckToSetStockCountValue5,       Menu_RulesMenuInput+0x5FC
add_hook CheckToSetStockCount
add_hook CheckToUseStockCountText
add_hook CheckToUseStockCountDescription
add_hook CheckToUpdateStockCountText
add_hook CheckToUpdateStockCountTextAgain
add_hook CheckToCycleToStockCountText
add_hook CheckToUpdateStockCountValue
add_hook CheckToSetStockCountValue1
add_hook CheckToSetStockCountValue2
add_hook CheckToSetStockCountValue3
add_hook CheckToSetStockCountValue4
add_hook CheckToSetStockCountValue5
add_hook Menu_UpdateStockCountOrTimerText
# Match rules
add_hook VsMode_InitDataFromRules
add_hook Match_Init
# CSS
.set CreateCSSNameText,          CSS_Setup+0x1180
.set CheckCSSPlayerCount,        CSS_ReadyThink+0x124
.set CheckCSSMatchStartCooldown, CSS_ReadyThink+0x310
.set CheckCSSModeSwap,           CSS_PlayerThink+0x150C
#ifdef PAL
.set CheckIsCPUPuck,             CSS_PlayerThink+0x1D00
#else
.set CheckIsCPUPuck,             CSS_PlayerThink+0x1CF4
#endif
add_hook CreateCSSNameText
add_hook CheckCSSPlayerCount
add_hook CheckCSSMatchStartCooldown
add_hook CheckCSSModeSwap
add_hook CheckIsCPUPuck
add_hook CSS_ChooseTopString
add_hook CSS_Init
add_hook CSS_PlayerThink
add_hook CSS_Setup
add_hook CSS_UpdatePortrait
# Settings lock
add_hook InitializeGlobalData
add_hook InitializeSaveData
# Manual
.set SetMenuRotation, Menu_UpdateCStickRotation+0xF8
add_hook SetMenuRotation
add_hook Menu_EnterCustomRulesMenu
# Main menu
add_hook MainMenu_Enter
add_hook MainMenu_Init
add_hook Menu_SetupMainMenu
add_hook Menu_UpdateMainMenuPreview
add_hook Menu_CreatePortalDescriptionText
# Unlocks
add_hook IsFeatureUnlocked
add_hook IsTrophyUnlocked
add_hook GetUnlockedEventMatchCount
add_hook ShouldShowUnlockMessage
#if defined(NTSC100) || defined(NTSC101)
# v1.02 hitlag
.set RoundDealtDamage, Player_MeleeDamage+0x36C
.set RoundReceivedDamage, Player_MeleeDamage+0x454
add_hook RoundDealtDamage
add_hook RoundReceivedDamage
# Flame cancel
add_hook Collision_AS_Bowser_FlameStartAir
#endif
# File patching
#if defined(NTSC100) || defined(PAL)
.set ParsePreloadedArchive, Archive_GetPreloaded+0x238
add_hook ParsePreloadedArchive
#endif
# DK loses charge when hit out of up B
#if defined(PAL)
add_hook Player_DK_NeutralBHitCallback
add_hook Player_DK_UpBHitCallback
#endif

hooks_end:
