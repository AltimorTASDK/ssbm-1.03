.section .text

.global hook_GetMenuLightColorIndex
hook_GetMenuLightColorIndex:
        # Pass menu type selected index as args
        mr      r3, r4
        mr      r4, r0
        bl      Menu_GetLightColorIndex
        mr      r30, r3
        # Skip the menu type switch
        b       Menu_UpdateLightColor+0x8C