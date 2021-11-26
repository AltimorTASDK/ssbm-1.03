#ifdef NTSC100
.section .text

.global hook_NormalizeItemBounceVelocity
hook_NormalizeItemBounceVelocity:
        # Pass item pointer and bounce velocity
        mr      r3, r31
        addi    r4, r1, 0x30
        bl      normalize_item_bounce_velocity

        # Skip original math
        b       Item_ElasticCollision+0x3A4

#endif