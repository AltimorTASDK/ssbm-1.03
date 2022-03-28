/*
 * Produces a code blob to be compiled into a GCI
 */

OUTPUT_FORMAT(elf32-powerpc)
OUTPUT_ARCH(powerpc:common);
ENTRY(_start);

PROVIDE(__LOAD_BASE__ = 0x81781000);
__MAX_SIZE__  = 0x817EC000 - __LOAD_BASE__;

MEMORY
{
    load (rwx) : ORIGIN = __LOAD_BASE__, LENGTH = __MAX_SIZE__
}

SECTIONS
{
    /DISCARD/ : { *crtbegin.o(*) *crtend.o(*) }

    .init   : { EXCLUDE_FILE(*/src/version/* */src/compat/*) *(  .init   .init.*) }
    .text   : { EXCLUDE_FILE(*/src/version/* */src/compat/*) *(  .text   .text.*) }
    .rodata : { EXCLUDE_FILE(*/src/version/* */src/compat/*) *(.rodata .rodata.*) }
    .data   : { EXCLUDE_FILE(*/src/version/* */src/compat/*) *(  .data   .data.*)
                . = ALIGN(4);
                KEEP(*(.hooks)) }
    .ctors  : { KEEP(*(.ctors .ctors.*)) }

    .version.text : {
        */src/version/*(          .text           .text.*)
        */src/compat/* (          .text           .text.*)
        *              (  .version.text   .version.text.*)
    }
    .version.rodata : {
        */src/version/*(        .rodata         .rodata.*)
        */src/compat/* (        .rodata         .rodata.*)
        *              (.version.rodata .version.rodata.*)
    }
    .version.data : {
        */src/version/*(          .data           .data.*)
        */src/compat/* (          .data           .data.*)
        *              (  .version.data   .version.data.*)
    }

    .bss : { *(.bss .bss.*) }

    PROVIDE(__BSS_START__ = ADDR(.bss));
    PROVIDE(__BSS_SIZE__  = SIZEOF(.bss));

    /DISCARD/ : { *(*) }
}