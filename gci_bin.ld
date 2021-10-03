/*
 * Produces a code blob to be compiled into a GCI
 */

OUTPUT_FORMAT(binary);
OUTPUT_ARCH(powerpc:common);
EXTERN(__start);
ENTRY(__start);

INCLUDE GALE01.ld

SECTIONS
{
    load_base = 0x817f4000;

    /* new sections */
    init_base = load_base;
    init_base = (init_base + 0x1F) & ~0x1F;
    init_offset = 0;
    .init init_base : AT(init_offset) { obj/src/init.o }

    text_base = init_base + SIZEOF(.init);
    text_base = (text_base + 0x1F) & ~0x1F;
    text_offset = init_offset + SIZEOF(.init);
    .text text_base : AT(text_offset) { *(.load* .text*) }
    
    data_base = text_base + SIZEOF(.text);
    data_base = (data_base + 0x1F) & ~0x1F;
    data_offset = text_offset + SIZEOF(.text);
    .data data_base : AT(data_offset) { *(.rodata* .rdata* .data* .sdata*) }
    
    ctors_base = data_base + SIZEOF(.data);
    ctors_base = (ctors_base + 0x1F) & ~0x1F;
    ctors_end = ctors_base + SIZEOF(.ctors);
    ctors_offset = data_offset + SIZEOF(.data);
    .ctors ctors_base : AT(ctors_offset) { *(.ctors) }

    bss_base = ctors_base + SIZEOF(.ctors);
    bss_base = (bss_base + 0x1F) & ~0x1F;
    bss_offset = ctors_offset + SIZEOF(.ctors);
    .bss bss_base : AT(bss_offset) { *(.bss* .sbss* COMMON) }

    /DISCARD/ : { *(*) }
}