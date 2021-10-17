/*
 * Produces a code blob to be compiled into a GCI
 */

OUTPUT_FORMAT(elf32-powerpc)
OUTPUT_ARCH(powerpc:common);
ENTRY(_start);

PROVIDE(__LOAD_BASE__ = 0x817E0800);

MEMORY
{
    load (rwx) : ORIGIN = __LOAD_BASE__, LENGTH = 0x800
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