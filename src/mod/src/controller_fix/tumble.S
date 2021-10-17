.section .text

.global hook_TumbleWiggleCheck
hook_TumbleWiggleCheck:
        mr      r3, r30 # gobj
        bl      check_tumble_wiggle
        cmpwi   r3, 0
        bne     damagefall_return
        b       Interrupt_AS_DamageFall+0xE0

damagefall_return:
        b       Interrupt_AS_DamageFall+0xFC
