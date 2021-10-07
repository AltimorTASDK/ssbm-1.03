.section .text

.set Rule_LedgeGrabLimit, 2

.global hook_CheckScrollingRuleIndex
hook_CheckScrollingRuleIndex:
        # Don't interfere with custom handicap/ledge grab limit display
        cmplwi  r25, Rule_LedgeGrabLimit
        bne     end
        li      r0, 0

end:
        b       orig_CheckScrollingRuleIndex
