.section .text

.set HSD_GObj.data, 0x2C

.set Player.instant_buttons,  0x668
.set Player.released_buttons, 0x66C

.global hook_CheckUTiltInput
hook_CheckUTiltInput:
        bl      check_cstick_utilt
        cmpwi   r3, 0
        beq     1f
        b       Interrupt_UTilt+0x5C
1:
        # Restore r3 and r0
        mr      r3, r31
        lwz     r0, Player.instant_buttons(r3)
        b       orig_CheckUTiltInput

.global hook_CheckFTiltInput
hook_CheckFTiltInput:
        bl      check_cstick_ftilt
        cmpwi   r3, 0
        beq     1f
        b       Interrupt_FTilt+0x88
1:
        # Restore r3 and r0
        mr      r3, r31
        lwz     r0, Player.instant_buttons(r3)
        b       orig_CheckFTiltInput

.global hook_CheckDTiltInput
hook_CheckDTiltInput:
        bl      check_cstick_dtilt
        cmpwi   r3, 0
        beq     1f
        b       Interrupt_DTilt+0x60
1:
        # Restore r3 and r0
        mr      r3, r31
        lwz     r0, Player.instant_buttons(r3)
        b       orig_CheckDTiltInput

.global hook_CheckDTiltInputIASA
hook_CheckDTiltInputIASA:
        bl      check_cstick_dtilt
        cmpwi   r3, 0
        beq     1f
        b       Interrupt_AS_DTilt+0x124
1:
        # Restore r0
        lwz     r0, Player.instant_buttons(r30)
        b       orig_CheckDTiltInputIASA

.global hook_CheckFTiltAngle
hook_CheckFTiltAngle:
        mr      r3, r31
        bl      check_cstick_ftilt
        cmpwi   r3, 0
        beq     1f
        # Overwrite with cstick angle
        mr      r3, r31
        bl      Player_GetCStickAngleAbsX
1:
        b       orig_CheckFTiltAngle

.global hook_CheckJabInput
hook_CheckJabInput:
        mr      r3, r31
        bl      check_cstick_jab
        cmpwi   r3, 0
        # Skip original comparison
        bne     1f
        # Restore r0
        lwz     r0, Player.instant_buttons(r31)
        b       orig_CheckJabInput
1:
        b       orig_CheckJabInput+4

.global hook_CheckJab2Input
hook_CheckJab2Input:
        bl      check_cstick_jab
        cmpwi   r3, 0
        # Restore r3
        lwz     r3, HSD_GObj.data(r29)
        # Skip original comparison
        bne     1f
        # Restore r0
        lwz     r0, Player.instant_buttons(r3)
        b       orig_CheckJab2Input
1:
        b       orig_CheckJab2Input+4

.global hook_CheckJab3Input
hook_CheckJab3Input:
        bl      check_cstick_jab
        cmpwi   r3, 0
        # Restore r3
        lwz     r3, HSD_GObj.data(r30)
        # Skip original comparison
        bne     1f
        # Restore r0
        lwz     r0, Player.instant_buttons(r3)
        b       orig_CheckJab3Input
1:
        b       orig_CheckJab3Input+4

.global hook_CheckRapidJabInput
hook_CheckRapidJabInput:
        stwu    r1, -0x14(r1)
        stw     r3, 0x8(r1)
        stw     r5, 0xC(r1)
        stw     r6, 0x10(r1)
        mr      r3, r6
        bl      check_cstick_rapid_jab
        cmpwi   r3, 0
        # Restore r3/r5/r6
        lwz     r6, 0x10(r1)
        lwz     r5, 0xC(r1)
        lwz     r3, 0x8(r1)
        addi    r1, r1, 0x14
        # Skip original comparison
        bne     1f
        # Restore r0
        lwz     r0, Player.instant_buttons(r6)
        b       orig_CheckRapidJabInput
1:
        b       orig_CheckRapidJabInput+4
