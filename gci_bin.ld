/*
 * Produces a code blob to be compiled into a GCI
 */

OUTPUT_FORMAT(binary);
OUTPUT_ARCH(powerpc:common);
EXTERN(entry);
ENTRY(entry);

INCLUDE GALE01.ld

SECTIONS
{
    load_base = 0x817F1000;

    init_offset = 0;
    init_base = load_base;
    .init init_base : AT(init_offset) { *(.init*) }

    text_offset = init_offset + SIZEOF(.init);
    text_offset = (text_offset + 3) & ~3;
    text_base = text_offset + load_base;
    .text text_base : AT(text_offset) { *(.load* .text*) }
    
    data_offset = text_offset + SIZEOF(.text);
    data_offset = (data_offset + 3) & ~3;
    data_base = data_offset + load_base;
    .data data_base : AT(data_offset) { *(.rodata* .rdata* .data* .sdata*) }
    
    ctors_offset = data_offset + SIZEOF(.data);
    ctors_offset = (ctors_offset + 3) & ~3;
    ctors_base = ctors_offset + load_base;
    ctors_end = ctors_base + SIZEOF(.ctors);
    .ctors ctors_base : AT(ctors_offset) { *(.ctors) }

    bss_offset = ctors_offset + SIZEOF(.ctors);
    bss_offset = (bss_offset + 3) & ~3;
    bss_base = bss_offset + load_base;
    .bss bss_base : AT(bss_offset) { *(.bss* .sbss* COMMON) }

    /DISCARD/ : { *(*) }
}