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
        lis     r31, (__CTOR_LIST__ - 4)@h
        ori     r31, r31, (__CTOR_LIST__ - 4)@l
        lis     r30, (__CTOR_END__ - 4)@h
        ori     r30, r30, (__CTOR_END__ - 4)@l

ctor_loop:
        cmpw    r31, r30
        beq     ctor_end

        # Call constructor
        lwzu    r0, 4(r31)
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
        sub     r0, r4, r3
        rlwinm  r0, r0, 0, 6, 29
        oris    r0, r0, 0x4800
        stw     r0, 0(r3)
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
.set ShowNameTagCheck,        NameTag_DisplayInGame+0x28

add_hook CheckScrollingRuleIndex
add_hook Interrupt_AS_Turn
add_hook Interrupt_SpotDodge_OoS
add_hook Interrupt_SquatRv
add_hook Match_Init
add_hook MatchController_UpdateTeamWinners
add_hook MatchController_UpdateWinners
add_hook Menu_CreatePortalDescriptionText
add_hook Menu_CreateRuleDescriptionText
add_hook Menu_SetupMainMenu
add_hook Menu_SetupRuleMenu
add_hook Menu_UpdateRuleValue
add_hook Scene_Match_Exit
add_hook ShowNameTagCheck
add_hook SSS_Init
add_hook Stage_Dreamland_WhispyUpdate
add_hook Stage_FinalDestination_BackgroundUpdate
add_hook Stage_Fountain_Init
add_hook Stage_GetSpawnPoint
add_hook Stage_PokemonStadium_TransformationDecide
add_hook Stage_YoshisStory_ShyGuyUpdate
add_hook TumbleWiggleCheck
add_hook UpdatePadButtons
add_hook UpdatePlayerStick

hooks_end:
