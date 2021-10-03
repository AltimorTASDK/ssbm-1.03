/*
 * Define hooks as pairs of words containing the function address and the hook
 * address. We then overwrite the first instruction of the function with a
 * branch to the hook, and then overwrite the hook definition with the original
 * instruction and a branch back to the original function.
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

# Copy original instruction over function address
lwzu    r5, 4(r3)
lwz     r0, 0(r5)
stw     r0, 0(r3)

# Overwrite original instruction with branch to hook
lwzu    r6, 4(r3)
sub     r0, r6, r5
rlwinm  r0, r0, 0, 6, 29
oris    r0, r0, 0x4800
stw     r0, 0(r5)

# Overwrite hook address with branch to original
addi    r0, r5, 4
sub     r0, r0, r3
rlwinm  r0, r0, 0, 6, 29
oris    r0, r0, 0x4800
stw     r0, 0(r3)

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

add_hook Interrupt_SpotDodge_OoS

hooks_end:
