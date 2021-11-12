.section .text

.set SlotType_Human, 0

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
        lis     r3, CSSReadyFrames@ha

        cmpwi   r4, 1
        bgt     ready_2p
        bne     not_ready
        
        # Store special value for ready frames
        li      r0, 0xFF
        stb     r0, CSSReadyFrames@l(r3)
        
        # Hide banner without clearing ready state
        mr      r3, r29
        b       CSS_ReadyThink+0x290
        
not_ready:
        b       CSS_ReadyThink+0x27C
        
ready_2p:
        # Check for special 1p ready frames value
        lbz     r0, CSSReadyFrames@l(r3)
        cmpwi   r0, 0xFF
        bne     1f

        # Reset ready frames value so animation restarts
        li      r0, 0
        stb     r0, CSSReadyFrames@l(r3)

1:
        # Skip overwritten instruction
        b       orig_CheckCSSPlayerCount+4
