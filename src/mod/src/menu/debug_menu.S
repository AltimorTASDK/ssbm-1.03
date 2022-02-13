.section .text

.global hook_GetDebugMenuButtons
hook_GetDebugMenuButtons:
        bl      is_20XX
        cmpwi   r3, 0
        bne     1f

        bl      get_debug_menu_buttons
        mr      r4, r3

        b       DebugMenu_Input+0x30

1:
        b       orig_GetDebugMenuButtons
