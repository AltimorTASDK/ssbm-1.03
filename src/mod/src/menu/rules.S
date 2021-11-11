.section .text

.set Rule_LedgeGrabLimit, 2

.set Mode_Stock, 1
.set Mode_Crew,  2

.global hook_CheckScrollingRuleIndex
hook_CheckScrollingRuleIndex:
        # Don't interfere with custom handicap/ledge grab limit display
        cmplwi  r25, Rule_LedgeGrabLimit
        bne     1f
        li      r0, 0
1:
        b       orig_CheckScrollingRuleIndex
        
check_mode:
        # Check for both stock and crew
        cmplwi  cr0, r0, Mode_Stock
        cmplwi  cr1, r0, Mode_Crew
        cror    4*cr0+eq, 4*cr0+eq, 4*cr1+eq
        blr

.global hook_CheckToSetStockCount
hook_CheckToSetStockCount:
        bl      check_mode
        # Skip overwritten instruction
        b       orig_CheckToSetStockCount+4

.global hook_CheckToUseStockCountText
hook_CheckToUseStockCountText:
        bl      check_mode
        # Skip overwritten instruction
        b       orig_CheckToUseStockCountText+4

.global hook_CheckToUseStockCountDescription
hook_CheckToUseStockCountDescription:
        bl      check_mode
        # Skip overwritten instruction
        b       orig_CheckToUseStockCountDescription+4

.global hook_CheckToUpdateStockCountText
hook_CheckToUpdateStockCountText:
        bl      check_mode
        # Skip overwritten instruction
        b       orig_CheckToUpdateStockCountText+4

.global hook_CheckToUpdateStockCountTextAgain
hook_CheckToUpdateStockCountTextAgain:
        bl      check_mode
        # Skip overwritten instruction
        b       orig_CheckToUpdateStockCountTextAgain+4

.global hook_CheckToCycleToStockCountText
hook_CheckToCycleToStockCountText:
        bl      check_mode
        # Skip overwritten instruction
        b       orig_CheckToCycleToStockCountText+4

.global hook_CheckToUpdateStockCountValue
hook_CheckToUpdateStockCountValue:
        bl      check_mode
        # Skip overwritten instruction
        b       orig_CheckToUpdateStockCountValue+4

.global hook_CheckToSetStockCountValue1
hook_CheckToSetStockCountValue1:
        bl      check_mode
        # Skip overwritten instruction
        b       orig_CheckToSetStockCountValue1+4

.global hook_CheckToSetStockCountValue2
hook_CheckToSetStockCountValue2:
        bl      check_mode
        # Skip overwritten instruction
        b       orig_CheckToSetStockCountValue2+4

.global hook_CheckToSetStockCountValue3
hook_CheckToSetStockCountValue3:
        bl      check_mode
        # Skip overwritten instruction
        b       orig_CheckToSetStockCountValue3+4

.global hook_CheckToSetStockCountValue4
hook_CheckToSetStockCountValue4:
        bl      check_mode
        # Skip overwritten instruction
        b       orig_CheckToSetStockCountValue4+4

.global hook_CheckToSetStockCountValue5
hook_CheckToSetStockCountValue5:
        bl      check_mode
        # Skip overwritten instruction
        b       orig_CheckToSetStockCountValue5+5
