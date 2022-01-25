.section .text

.set OSD_UCF, 28

.global hook_InitRandomStageToggle
hook_InitRandomStageToggle:
        # Check if UCF toggle index
        cmpwi   r26, OSD_UCF
        bne     1f
        # Check if UnclePunch OSD
        lis     r4, IsEnteringMenu@ha
        lwz     r4, IsEnteringMenu@l(r4)
        cmpwi   r4, 2
        blt     1f
        # Disable
        li      r3, 0
        b       orig_InitRandomStageToggle+4
1:
        b       orig_InitRandomStageToggle