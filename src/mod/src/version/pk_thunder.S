#if defined(NTSC100) || defined(NTSC101)
.section .text

.global hook_OnPKThunderOwnerChange
hook_OnPKThunderOwnerChange:
        # Destroy the projectile on owner change (including changing to null
        # from Ness getting hit) like on 1.02
        mr      r3, r30
        bl      Item_PKThunder_Destroy
        li      r3, 1
        b       Item_PKThunder_Update+0x10C

#endif