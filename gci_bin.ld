/*
 * Produces a code blob to be compiled into a GCI
 */

OUTPUT_FORMAT(elf32-powerpc)
OUTPUT_ARCH(powerpc:common);
ENTRY(_start);

INCLUDE GALE01.ld

MEMORY
{
    load (rwx) : ORIGIN = 0x817E1000, LENGTH = 0x3700
}

SECTIONS
{
    .init   : { *(.init   .init.*) }
    .text   : { *(.text   .text.*) }
    .rodata : { *(.rodata .rodata.*) }
    .data   : { *(.data   .data.*) }
    PROVIDE(__ctors_base = .)
    .ctors  : { *(.ctors  .ctors.*) }
    PROVIDE(__ctors_end = .)
    .bss    : { *(.bss    .bss.*) }

    /DISCARD/ : { *(*) *crtbegin.o(*) *crtend.o(*) }
}