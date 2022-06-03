.section .text

.global hook_SelectExtraRuleModel
hook_SelectExtraRuleModel:
        mr      r3, r27 # index
        bl      select_extra_rule_model
        mr      r28, r3 # model pointer
        # Jump past original model selection
        b       Menu_SetupExtraRulesMenu+0x5A0

.global hook_CheckExtraRuleVisible1
hook_CheckExtraRuleVisible1:
        mr      r3, r26
        bl      is_extra_rule_visible
        cmpwi   r3, 0
        # Jump to check to skip rule index
        b       Menu_UpdateExtraRuleDisplay+0x160

.global hook_CheckExtraRuleVisible2
hook_CheckExtraRuleVisible2:
        mr      r3, r26
        bl      is_extra_rule_visible
        cmpwi   r3, 0
        # Jump to check to skip rule index
        b       Menu_UpdateExtraRuleDisplay+0x4C4

.global hook_CheckExtraRuleVisible3
hook_CheckExtraRuleVisible3:
        mr      r3, r27
        bl      is_extra_rule_visible
        cmpwi   r3, 0
        # Jump to check to skip rule index
        b       Menu_SetupExtraRulesMenu+0x42C
