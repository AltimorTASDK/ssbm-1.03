#if defined(NTSC100) || defined(NTSC101)
.section .text

.set HitboxState_Inactive, 0

.set Item.pk_flash.hitbox_updated, 0xDB4

.global hook_CheckToUpdatePKFlashHitbox
hook_CheckToUpdatePKFlashHitbox:
        # Check original condition
        cmpwi   r0, HitboxState_Inactive
        beq     1f

        # Check if hitbox already updated like 1.02
        lwz     r0, Item.pk_flash.hitbox_updated(r31)
        cmpwi   r0, 1
        beq     1f

        # Store hitbox updated
        li      r0, 1
        stw     r0, Item.pk_flash.hitbox_updated(r31)

1:
        # Skip overwritten instruction
        b       orig_CheckToUpdatePKFlashHitbox+4

#endif