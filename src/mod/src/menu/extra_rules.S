
.section .text

.global hook_SelectExtraRuleModel
hook_SelectExtraRuleModel:
        mr      r3, r27 # index
        bl      select_extra_rule_model
        mr      r28, r3 # model pointer

        # Jump past original model selection
        b       Menu_SetupExtraRulesMenu+0x5A0