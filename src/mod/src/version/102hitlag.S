#if defined(NTSC100) || defined(NTSC101)
.section .rodata

zero: .float 0

.section .text

.global hook_RoundReceivedDamage
hook_RoundReceivedDamage:
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
        b       orig_RoundReceivedDamage

#endif