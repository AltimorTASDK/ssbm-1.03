/*
 * Define hooks as pairs of words containing the function address and the hook
 * address. We then overwrite the first instruction of the function with a
 * branch to the hook, and then overwrite the hook definition with the original
 * instruction and a branch back to the original function.
 */
.section .init

/*
 * void entry()
 */
 .global entry
 entry:
        mflr    r0
        stw     r0, 4(r1)
        stwu    r1, -16(r1)
        stw     r31, 12(r1)
        stw     r30, 8(r1)

        # Apply hooks
        lis     r31, (hooks_start - 4)@h
        ori     r31, r31, (hooks_start - 4)@l
        lis     r30, (hooks_end - 4)@h
        ori     r30, r30, (hooks_end - 4)@l

hook_loop:
        cmpw    r31, r30
        beq     hook_end

        # Copy original instruction over function address
        lwzu    r3, 4(r31)
        lwz     r0, 0(r3)
        stw     r0, 0(r31)

        # Overwrite original instruction with branch to hook
        lwzu    r4, 4(r31)
        bl      write_branch

        # Overwrite hook address with branch to original
        addi    r4, r3, 4
        mr      r3, r31
        bl      write_branch

        b       hook_loop
hook_end:

/*
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
*/

        lwz     r0, 20(r1)
        lwz     r31, 12(r1)
        lwz     r30, 8(r1)
        addi    r1, r1, 16
        mtlr    r0
        blr

/*
 * void *write_branch(void *address, void *target)
 * Returns address
 */
write_branch:
        sub     r0, r4, r3
        rlwinm  r0, r0, 0, 6, 29
        oris    r0, r0, 0x4800
        stw     r0, 0(r3)
        blr

hooks_start:
.macro add_hook name
        .global orig_\name
        orig_\name:
        .int \name
        .int hook_\name
.endm

add_hook HSD_PadClamp
add_hook Interrupt_AS_Turn
add_hook Interrupt_SpotDodge_OoS
add_hook Interrupt_SquatRv

hooks_end:
