.section .text

.global hook_Player_CheckStickDirectionChanged
hook_Player_CheckStickDirectionChanged:
        mflr    r0
        stw     r0, 4(r1)
        stwu    r1, -8(r1)

        # Get player pointer from caller reg
        mr      r3, r31
        bl      update_pad_buffer

        addi    r1, r1, 8
        lwz     r0, 4(r1)
        mtlr    r0

end:
        b       orig_Player_CheckStickDirectionChanged