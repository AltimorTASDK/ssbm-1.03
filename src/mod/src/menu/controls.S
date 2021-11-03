.section .text

.set Scene_Tournament, 27

.global hook_EnterSpecialMelee
hook_EnterSpecialMelee:
        # Exit to tournament mode (controls menu)
        bl      Scene_GetExitData
        li      r0, Scene_Tournament
        stb     r0, 0(r3)
        bl      Scene_Exit
        
        # Return
        b       VsMenu_Think+0x24C
