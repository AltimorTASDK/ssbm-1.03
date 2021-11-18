.section .text

#ifdef PAL
.set Scene_Tournament, 30
#else
.set Scene_Tournament, 27
#endif

.global hook_EnterSpecialMelee
hook_EnterSpecialMelee:
        # Store who pressed A
        bl      store_controls_menu_port

        # Exit to tournament mode (controls menu)
        bl      Scene_GetExitData
        li      r0, Scene_Tournament
        stb     r0, 0(r3)
        bl      Scene_Exit

        # Return
        b       VsMenu_Think+0x24C
