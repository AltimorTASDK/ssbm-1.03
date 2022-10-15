.section .text

.global hook_CheckItemHitShield
hook_CheckItemHitShield:
        mr      r7, r27
        bl      check_item_hit_shield
        # Skip overwritten instruction
        b       orig_CheckItemHitShield+4
