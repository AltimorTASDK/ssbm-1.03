/*
 * Produces a code blob to be compiled into a GCI
 */

OUTPUT_FORMAT(elf32-powerpc)
OUTPUT_ARCH(powerpc:common);
ENTRY(_start);

PROVIDE(__BOOTSTRAP_BASE__ = 0x8045D930);
PROVIDE(__LOAD_BASE__      = 0x817AF000);

MEMORY
{
    bootstrap : ORIGIN = __BOOTSTRAP_BASE__, LENGTH = 0x800
    load      : ORIGIN = __LOAD_BASE__,      LENGTH = 0x800
}

SECTIONS
{
    /DISCARD/ : { *crtbegin.o(*) *crtend.o(*) }

    .bootstrap : { *(.bootstrap) } >bootstrap

    PROVIDE(__COPY_START__ = .);
    .loader : { KEEP(*(.loader .loader.*)) } >load AT>bootstrap
    .text   : {      *(.text   .text.*)    } >load AT>bootstrap
    .rodata : {      *(.rodata .rodata.*)  } >load AT>bootstrap
    .ctors  : { KEEP(*(.ctors  .ctors.*))  } >load AT>bootstrap
    .data   : {      *(.data   .data.*)    } >load AT>bootstrap
    PROVIDE(__BSS_START__ = .);
    .bss    : {      *(.bss    .bss.*)     } >load AT>bootstrap
    PROVIDE(__BSS_SIZE__ = . - __BSS_START__);
    PROVIDE(__COPY_SIZE__ = . - __LOAD_BASE__);

    /DISCARD/ : { *(*) }
}