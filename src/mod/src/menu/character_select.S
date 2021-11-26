.section .text

.set SlotType_Human, 0

.set Mode_Crew, 3

.set MenuSFX_Error, 3

.set GameRules.mode, 0x02

.global hook_CheckIsCPUPuck
hook_CheckIsCPUPuck:
        # Check if set to CPU normally
        cmplwi  r5, SlotType_Human
        bne     1f

        # Back up registers used in loop
        mfctr   r0
        stwu    r1, -0x18(r1)
        stw     r3, 0x8(r1)
        stw     r4, 0xC(r1)
        stw     r6, 0x10(r1)
        stw     r0, 0x14(r1)

        mr      r3, r6 # port
        bl      check_is_cpu_puck

        cmplwi  r3, 0

        lwz     r0, 0x14(r1)
        lwz     r6, 0x10(r1)
        lwz     r4, 0xC(r1)
        lwz     r3, 0x8(r1)
        addi    r1, r1, 0x18
        mtctr   r0

1:
        # Skip overwritten instruction
        b       orig_CheckIsCPUPuck+4

.global hook_CheckCSSPlayerCount
hook_CheckCSSPlayerCount:
        stwu    r1, -0x10(r1)
        stw     r8, 0xC(r1)
        stw     r7, 0x8(r1)

        cmpwi   r4, 1
        bgt     ready_2p
        bne     not_ready

        li      r3, 1
        bl      check_can_start_match
        cmpwi   r3, 0
        beq     not_ready

        lis     r3, CSSReadyFrames@ha

        # Store special value for ready frames
        li      r0, 0xFF
        stb     r0, CSSReadyFrames@l(r3)

        # Hide banner without clearing ready state
        mr      r3, r29
        lwz     r7, 0x8(r1)
        lwz     r8, 0xC(r1)
        addi    r1, r1, 0x10
        b       CSS_ReadyThink+0x290

not_ready:
        lwz     r7, 0x8(r1)
        lwz     r8, 0xC(r1)
        addi    r1, r1, 0x10
        b       CSS_ReadyThink+0x27C

ready_2p:
        li      r3, 1
        bl      check_can_start_match
        cmpwi   r3, 0
        beq     not_ready

        # Check for special 1p ready frames value
        lis     r3, CSSReadyFrames@ha
        lbz     r0, CSSReadyFrames@l(r3)
        cmpwi   r0, 0xFF
        bne     1f

        # Reset ready frames value so animation restarts
        li      r0, 0
        stb     r0, CSSReadyFrames@l(r3)

1:
        lwz     r7, 0x8(r1)
        lwz     r8, 0xC(r1)
        addi    r1, r1, 0x10

        # Skip overwritten instruction
        b       orig_CheckCSSPlayerCount+4

.global hook_CheckCSSMatchStartCooldown
hook_CheckCSSMatchStartCooldown:
        # Bail out if ready to start match
        cmplwi  r0, 0
        beq     2f

        lis     r3, CSSReadyFrames@ha
        lbz     r0, CSSReadyFrames@l(r3)
        cmplwi  r0, 0
        bne     1f

        # Check if crew
        bl      GetGameRules
        lbz     r0, GameRules.mode(r3)
        cmpwi   r0, Mode_Crew
        bne     1f

        # Back up r7
        mr      r28, r7

        # Don't play error sound if non-crew-related failure
        li      r3, 0
        bl      check_can_start_match
        cmpwi   r3, 0
        mr      r7, r28
        beq     1f

        # Allow condition to succeed if not ready in crews so "denied" SFX plays
        crset   eq
        b       2f

1:
        # Cooldown not ready
        crclr   eq
2:
        # Skip overwritten instruction
        b       orig_CheckCSSMatchStartCooldown+4

.global hook_CheckCSSModeSwap
hook_CheckCSSModeSwap:
        # Bail if not pressing A
        beq     1f

        # Check if crew
        bl      GetGameRules
        lbz     r0, GameRules.mode(r3)
        cmpwi   r0, Mode_Crew
        bne     1f

        li      r3, MenuSFX_Error
        bl      Menu_PlaySFX

        # Set cr0.eq to prevent changing to teams
        creqv   eq, eq, eq

1:
        # Restore r3
        li      r3, 1
        b       orig_CheckCSSModeSwap
