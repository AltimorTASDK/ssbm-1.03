/*
 * Produces a code blob to be compiled into a GCI
 */

OUTPUT_FORMAT(elf32-powerpc)
OUTPUT_ARCH(powerpc:common);
ENTRY(_start);

PROVIDE(__LOAD_BASE__ = 0x81781000);

MEMORY
{
    load (rwx) : ORIGIN = __LOAD_BASE__, LENGTH = 0x6B000
}

SECTIONS
{
    /DISCARD/ : { *crtbegin.o(*) *crtend.o(*) }

    .version __VERSION_START__ : {
        */src/version/*(
            .text   .text.*
            .rodata .rodata.*
            .data   .data.*
        )
        */src/compat/*(
            .text   .text.*
            .rodata .rodata.*
            .data   .data.*
        )
    }

    .init   : {      *(.init   .init.*) }
    .text   : {      *(.text   .text.*) }
    .rodata : {      *(.rodata .rodata.*) }
    .ctors  : { KEEP(*(.ctors .ctors.*)) }
    .data   : {      *(.data   .data.*) }

    PROVIDE(__VERSION_START__ = ALIGN(4));

    PROVIDE(__BSS_START__ = __VERSION_START__ + SIZEOF(.version));
    .bss __BSS_START__ : { *(.bss .bss.*) }
    PROVIDE(__BSS_SIZE__ = . - __BSS_START__);

    /DISCARD/ : { *(*) }
}