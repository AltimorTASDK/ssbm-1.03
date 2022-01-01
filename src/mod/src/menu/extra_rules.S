.section .text

.set ExtraRule_Latency, 5

.global hook_SelectExtraRuleModel
hook_SelectExtraRuleModel:
        mr      r3, r27 # index
        bl      select_extra_rule_model
        mr      r28, r3 # model pointer

        # Jump past original model selection
        b       Menu_SetupExtraRulesMenu+0x5A0

.global hook_CheckExtraRuleVisible1
hook_CheckExtraRuleVisible1:
        bl      is_faster_melee
        cmpwi   r3, 0
        beq     1f
        cmpwi   r26, ExtraRule_Latency
        # Jump to check to skip rule index
        b       Menu_UpdateExtraRuleDisplay+0x160
1:
        # Jump past check to skip rule index
        b       Menu_UpdateExtraRuleDisplay+0x164

.global hook_CheckExtraRuleVisible2
hook_CheckExtraRuleVisible2:
        bl      is_faster_melee
        cmpwi   r3, 0
        beq     1f
        cmpwi   r26, ExtraRule_Latency
        # Jump to check to skip rule index
        b       Menu_UpdateExtraRuleDisplay+0x4C4
1:
        # Jump past check to skip rule index
        b       Menu_UpdateExtraRuleDisplay+0x4C8

.global hook_CheckExtraRuleVisible3
hook_CheckExtraRuleVisible3:
        bl      is_faster_melee
        cmpwi   r3, 0
        beq     1f
        cmpwi   r27, ExtraRule_Latency
        # Jump to check to skip rule index
        b       Menu_SetupExtraRulesMenu+0x42C
1:
        # Jump past check to skip rule index
        b       Menu_SetupExtraRulesMenu+0x430