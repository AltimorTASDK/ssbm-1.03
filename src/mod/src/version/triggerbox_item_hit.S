#ifdef PAL
.section .text

.global hook_OnTriggerboxHitItem
hook_OnTriggerboxHitItem:
        # Disable item hitbox
        li      r18, 1
        b       orig_OnTriggerboxHitItem

#endif