.section .text

.global hook_GetDebugMenuButtons
hook_GetDebugMenuButtons:
        bl      get_debug_menu_buttons
        mr      r4, r3

        b       DebugMenu_Input+0x30
