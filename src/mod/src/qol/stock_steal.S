.section .text

.set Button_A,     0x100
.set Button_Start, 0x1000

.global hook_CheckStockStealInput
hook_CheckStockStealInput:
        # r0 = buttons
        andi.   r0, r0, Button_A | Button_Start
        # Skip overwritten instruction
        b       orig_CheckStockStealInput+4
