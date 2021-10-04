.section .text

.extern apply_cardinal_fix

.global hook_UpdatePlayerStick
hook_UpdatePlayerStick:
        mr      r3, r30 # gobj
        bl      apply_cardinal_fix
        b       orig_UpdatePlayerStick
