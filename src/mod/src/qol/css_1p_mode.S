.section .text

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
        bne     orig_CheckCSSPlayerCount+4

        # Reset ready frames value so animation restarts
        li      r0, 0
        stb     r0, CSSReadyFrames@l(r3)

        # Skip overwritten instruction
        b       orig_CheckCSSPlayerCount+4