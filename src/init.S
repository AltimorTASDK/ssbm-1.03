/*
 * Define hooks as pairs of words containing the original function address
 * and the hook address. We then apply these and overwrite the original address
 * with the contents of the overwritten instruction.
 */
.section .init

mflr    r0
stw     r0, 4(r1)
stwu    r1, -16(r1)
stw     r31, 12(r1)
stw     r30, 8(r1)

# Apply hooks
lis     r3, (hooks_start - 4)@h
ori     r3, r3, (hooks_start - 4)@l
lis     r4, (hooks_end - 4)@h
ori     r4, r4, (hooks_end - 4)@l

hook_loop:
cmpw    r3, r4
beq     hook_end

# Copy instruction bytes over original address
lwzu    r5, 4(r3)
lwz     r6, 0(r5)
stw     r6, 0(r3)

# Overwrite original instruction with branch to hook
lwzu    r6, 4(r3)
sub     r6, r6, r5
rlwinm  r6, r6, 0, 6, 29
oris    r6, r6, 0x4800
stw     r6, 0(r5)

b       hook_loop

hook_end:

# Run C++ constructors
lis     r31, (ctors_base - 4)@h
ori     r31, r31, (ctors_base - 4)@l
lis     r30, (ctors_end - 4)@h
ori     r30, r30, (ctors_end - 4)@l

ctor_loop:
cmpw    r31, r30
beq     ctor_end

# Call constructor
lwzu    r0, 4(r31)
mtctr   r0
bctrl
b       ctor_loop

ctor_end:

lwz     r0, 20(r1)
lwz     r31, 12(r1)
lwz     r30, 8(r1)
addi    r1, r1, 16
mtlr    r0
blr

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
