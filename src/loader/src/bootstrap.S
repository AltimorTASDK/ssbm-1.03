.section .bootstrap, "ax"

# Return past end of nametag load loop
.set return_address_vs, Menu_LoadNameTags+0x478
#ifdef PAL
.set return_address_css, CSS_LoadNameTags+0x65C
#else
.set return_address_css, CSS_LoadNameTags+0x680
#endif

#if defined(NTSC102)
.set zlib_copy_base, __ZLIB_COPY_BASE_NTSC102__
#elif defined(NTSC101)
.set zlib_copy_base, __ZLIB_COPY_BASE_NTSC101__
#elif defined(NTSC100)
.set zlib_copy_base, __ZLIB_COPY_BASE_NTSC100__
#elif defined(PAL)
.set zlib_copy_base, __ZLIB_COPY_BASE_PAL__
#endif

/*
 * void _start()
 */
 .global _start
 _start:
 _start_vs:
        lis     r0, return_address_vs@h
        ori     r0, r0, return_address_vs@l
        b       1f

 _start_css:
        lis     r0, return_address_css@h
        ori     r0, r0, return_address_css@l

1:
        mtlr    r0

        # Check if a memcard save is in progress
        lis     r3, MemCardErrorCallback@ha
        lwz     r0, MemCardErrorCallback@l(r3)
        cmpwi   r0, 0
        beq     run_loader

        # Load when it's done
        lis     r0, run_loader@h
        ori     r0, r0, run_loader@l
        stw     r0, MemCardErrorCallback@l(r3)
        blr

run_loader:
        mflr    r0
        stw     r0, 0x4(r1)
        stwu    r1, -0x8(r1)

	# Copy loader out of save region
        lis     r3, __LOAD_BASE__@h
        ori     r3, r3, __LOAD_BASE__@l
        lis     r4, __COPY_START__@h
        ori     r4, r4, __COPY_START__@l
        li      r5, __COPY_SIZE__
        bl      memcpy

	# Copy zlib out of save region
        lis     r3, __ZLIB_BASE__@h
        ori     r3, r3, __ZLIB_BASE__@l
        lis     r4, zlib_copy_base@h
        ori     r4, r4, zlib_copy_base@l
        lis     r5, __ZLIB_COPY_SIZE__@h
        ori     r5, r5, __ZLIB_COPY_SIZE__@l
        bl      memcpy

        # Update instruction cache
        lis     r3, __LOAD_BASE__@h
        ori     r3, r3, __LOAD_BASE__@l
        lis     r4, __COPY_SIZE__@h
        ori     r4, r4, __COPY_SIZE__@l
        bl      DCStoreRange

        lis     r3, __LOAD_BASE__@h
        ori     r3, r3, __LOAD_BASE__@l
        lis     r4, __COPY_SIZE__@h
        ori     r4, r4, __COPY_SIZE__@l
        bl      ICInvalidateRange

        lwz     r0, 0xC(r1)
        addi    r1, r1, 0x8
        mtlr    r0

        # Run loader
        b      __LOAD_BASE__
