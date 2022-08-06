/*
 * Produces a code blob to be compiled into a GCI
 */

INCLUDE ../common-b.ld

OUTPUT_FORMAT(elf32-powerpc)
OUTPUT_ARCH(powerpc:common);
ENTRY(_start);

__LOAD_BASE__ = __LOADER_BASE__;

MEMORY
{
    bootstrap      : ORIGIN = __BOOTSTRAP_BASE__, LENGTH = __LOADER_SIZE__
    load           : ORIGIN = __LOAD_BASE__,      LENGTH = __LOADER_SIZE__
}

SECTIONS
{
    /DISCARD/ : { *crtbegin.o(*) *crtend.o(*) }

    .bootstrap : { *(.bootstrap) } >bootstrap

    .loader : { KEEP(*(.loader .loader.*)) } >load AT>bootstrap
    .text   : {      *(.text   .text.*)    } >load AT>bootstrap
    .rodata : {      *(.rodata .rodata.*)  } >load AT>bootstrap
    .data   : {      *(.data   .data.*)    } >load AT>bootstrap
    .bss    : {      *(.bss    .bss.*)     } >load AT>bootstrap
    PROVIDE(__BSS_START__  = ADDR(.bss));
    PROVIDE(__BSS_SIZE__   = SIZEOF(.bss));
    PROVIDE(__COPY_START__ = LOADADDR(.loader));
    PROVIDE(__COPY_SIZE__  = __BSS_START__ - __LOAD_BASE__);
    PROVIDE(__ENTRY__      = ADDR(.loader));

    /DISCARD/ : { *(*) }
}