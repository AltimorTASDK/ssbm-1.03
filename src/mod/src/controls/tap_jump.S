.section .text

suppress_tap_jump:
        mflr    r0
        stw     r0, 0x4(r1)
        stwu    r1, -0x14(r1)
        stw     r4, 0x8(r1)
        stw     r5, 0xC(r1)
        mfcr    r0
        stw     r0, 0x10(r1)

        bl      is_tap_jump_on

        lwz     r0, 0x10(r1)
        mtcr    r0
        lwz     r5, 0xC(r1)
        lwz     r4, 0x8(r1)
        lwz     r0, 0x18(r1)
        addi    r1, r1, 0x14
        mtlr    r0

        # Clear bits from stick_y >= threshold if tap jump is off
        cmpwi   cr1, r3, 0
        crandc  4*cr0+eq, 4*cr0+eq, 4*cr1+eq
        crandc  4*cr0+gt, 4*cr0+gt, 4*cr1+eq

        blr

.global hook_TapJumpCheck_Jump
hook_TapJumpCheck_Jump:
        mr      r3, r4
        bl      suppress_tap_jump
        b       orig_TapJumpCheck_Jump

.global hook_TapJumpCheck_JumpAerial
hook_TapJumpCheck_JumpAerial:
        mr      r3, r30
        bl      suppress_tap_jump
        b       orig_TapJumpCheck_JumpAerial

.global hook_TapJumpCheck_JumpInput
hook_TapJumpCheck_JumpInput:
        mflr    r0
        stw     r0, 0x4(r1)
        stwu    r1, -0x8(r1)
        mr      r3, r4
        bl      suppress_tap_jump
        lwz     r0, 0xC(r1)
        addi    r1, r1, 0x8
        mtlr    r0
        b       orig_TapJumpCheck_JumpInput

.global hook_TapJumpCheck_JumpOoRun
hook_TapJumpCheck_JumpOoRun:
        mr      r3, r31
        bl      suppress_tap_jump
        b       orig_TapJumpCheck_JumpOoRun

.global hook_TapJumpCheck_JumpOoS
hook_TapJumpCheck_JumpOoS:
        mr      r3, r4
        bl      suppress_tap_jump
        b       orig_TapJumpCheck_JumpOoS

.global hook_TapJumpCheck_MultiJump
hook_TapJumpCheck_MultiJump:
        stwu    r1, -0xC(r1)
        stw     r3, 0x8(r1)
        mr      r3, r28
        bl      suppress_tap_jump
        lwz     r3, 0x8(r1)
        addi    r1, r1, 0xC
        b       orig_TapJumpCheck_MultiJump

.global hook_TapJumpCheck_FirstMultiJump
hook_TapJumpCheck_FirstMultiJump:
        mflr    r0
        stw     r0, 0x4(r1)
        stwu    r1, -0x10(r1)
        stw     r3, 0x8(r1)
        stw     r6, 0xC(r1)
        bl      suppress_tap_jump
        lwz     r6, 0xC(r1)
        lwz     r3, 0x8(r1)
        lwz     r0, 0x14(r1)
        addi    r1, r1, 0x10
        mtlr    r0
        b       orig_TapJumpCheck_FirstMultiJump