#ifdef PAL
.section .text

.set Player.held_player, 0x1A5C

.global hook_ThrowSyncSideB
hook_ThrowSyncSideB:
        # Apply freeze glitch
        li      r0, 0
        stw     r0, Player.held_player(r30)
        b       orig_ThrowSyncSideB

#endif