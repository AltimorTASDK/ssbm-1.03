/*
 * Define hooks as pairs of words containing the original function address
 * and the hook address. We then apply these and overwrite the original address
 * with the contents of the overwritten instruction.
 */
.section .init

lis     r3, hooks_start@h
ori     r3, r3, hooks_start@l

hooks_start:

.macro add_hook name
        .global orig_\name
        orig_\name:
        .int \name
        .int hook_\name
.endm

#add_hook HSD_ResetScene
#add_hook StartMelee
#add_hook DevelopText_DrawAll

hooks_end:
