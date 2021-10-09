/*
 * Produces a code blob to be compiled into a GCI
 */

OUTPUT_FORMAT(elf32-powerpc)
OUTPUT_ARCH(powerpc:common);
ENTRY(_start);

INCLUDE GALE01.ld

MEMORY
{
    load (rwx) : ORIGIN = 0x817E1000, LENGTH = 0x7000
}

SECTIONS
{
    /DISCARD/ : { *crtbegin.o(*) *crtend.o(*) }

    .init   : { *(.init   .init.*) }
    .text   : { *(.text   .text.*) }
    .rodata : { *(.rodata .rodata.*) }
    .ctors  : { KEEP(*(.ctors .ctors.*)) }
    .data   : { *(.data   .data.*) }
    PROVIDE(__BSS_START__ = .);
    .bss    : { *(.bss    .bss.*) }
    .sbss   : { *(.sbss   .sbss.*) }
    PROVIDE(__BSS_SIZE__ = . - __BSS_START__);

    /DISCARD/ : { *(*) }
}