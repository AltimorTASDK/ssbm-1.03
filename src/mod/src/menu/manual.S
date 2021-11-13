.section .rodata

zero: .float 0

.section .text

.global hook_SetMenuRotation
hook_SetMenuRotation:
        lis     r3, manual_open@ha
        lbz     r0, manual_open@l(r3)
        cmpwi   r0, 0
        beq     1f
        
        # Prevent cstick rotation at the manual
        lis     r3, zero@ha
        lfs     f2, zero@l(r3)
        lfs     f3, zero@l(r3)
1:
        b       orig_SetMenuRotation