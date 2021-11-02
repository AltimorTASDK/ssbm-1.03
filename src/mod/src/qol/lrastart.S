
.section .text

.global hook_GetDebugPauseButtons
hook_GetDebugPauseButtons:
        bl      get_debug_pause_buttons
        mr      r0, r3

        # Skip overwritten instruction
        b       orig_GetDebugPauseButtons+4
