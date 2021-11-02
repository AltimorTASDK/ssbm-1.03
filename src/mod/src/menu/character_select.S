.section .text

.set SlotType_Human, 0

.global hook_CheckIsCPUPuck
hook_CheckIsCPUPuck:
        # Check if set to CPU normally
        cmplwi  r5, SlotType_Human
        bne     end

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

end:
        # Skip overwritten instruction
        b       orig_CheckIsCPUPuck+4
