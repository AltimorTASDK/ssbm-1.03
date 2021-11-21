#if defined(NTSC100) || defined(NTSC101)
.section .rodata

.balign 4
zero: .float 0

.section .text

round_up_damage:
        # Check if damage is rounded to 0
        cmpwi   r0, 0
        bne     1f

        # Check that damage isn't exactly 0
        lis     r3, zero@ha
        lfs     f0, zero@l(r3)
        fcmpo   cr0, f31, f0
        beq     1f

        # Round up to 1 instead
        li      r0, 1

1:
        blr

.global hook_RoundDealtDamage
hook_RoundDealtDamage:
        bl      round_up_damage
        b       orig_RoundDealtDamage

.global hook_RoundReceivedDamage
hook_RoundReceivedDamage:
        bl      round_up_damage
        b       orig_RoundReceivedDamage

#endif