.section .text

.set Rule_LedgeGrabLimit, 2

.set Mode_Stock, 1
.set Mode_Crew,  3

.global hook_CheckRuleVisible1
hook_CheckRuleVisible1:
        mr      r3, r29
        bl      is_rule_visible
        mr      r0, r3
        b       Menu_SetupRulesMenu+0x2F0

.global hook_CheckRuleVisible2
hook_CheckRuleVisible2:
        mr      r3, r29
        bl      is_rule_visible
        mr      r0, r3
        b       Menu_SetupRulesMenu+0x34C

.global hook_CheckRuleVisible3
hook_CheckRuleVisible3:
        mr      r3, r27
        bl      is_rule_visible
        mr      r0, r3
        b       Menu_SetupRulesMenu+0x3B0

.global hook_CheckRuleVisible4
hook_CheckRuleVisible4:
        mr      r3, r29
        bl      is_rule_visible
        mr      r0, r3
        b       Menu_RulesMenuInput+0x310

.global hook_CheckRuleVisible5
hook_CheckRuleVisible5:
        mr      r3, r29
        bl      is_rule_visible
        mr      r0, r3
        b       Menu_RulesMenuInput+0x3C4

.global hook_CheckRuleVisible6
hook_CheckRuleVisible6:
        mr      r3, r25
        bl      is_rule_visible
        mr      r0, r3
        b       Menu_UpdateRuleDisplay+0x110

.global hook_CheckRuleVisible7
hook_CheckRuleVisible7:
        mr      r3, r29
        bl      is_rule_visible
        mr      r0, r3
        b       Menu_UpdateRuleDisplay+0x150

.global hook_CheckRuleVisible8
hook_CheckRuleVisible8:
        mr      r3, r25
        bl      is_rule_visible
        mr      r0, r3
        b       Menu_UpdateRuleDisplay+0x418

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
        b       orig_CheckToSetStockCountValue5+4
