/*
 * Appends to the Melee DOL
 */

OUTPUT_FORMAT(elf32-powerpc);
OUTPUT_ARCH(powerpc:common);
EXTERN(__start);
ENTRY(__start);

TARGET(binary);
INPUT(build/dol_data.bin);

TARGET(elf32-powerpc);

__LOAD_BASE__  = 0x80003000;
__NEW_BASE__   = 0x817B6000; /* starting VMA of new code */
__MAX_ADDR__   = 0x817F8AC0; /* start of FileStringTable */
__OLD_SIZE__   = __NEW_BASE__ - __LOAD_BASE__;
__NEW_SIZE__   = __MAX_ADDR__ - __NEW_BASE__;

MEMORY
{
    disk (rwx) : ORIGIN = 0,            LENGTH = __OLD_SIZE__
    new  (rwx) : ORIGIN = __NEW_BASE__, LENGTH = __NEW_SIZE__
}

SECTIONS
{
    /DISCARD/ : { *crtbegin.o(*) *crtend.o(*) }

    /* DOL header (from TITANIK's GC docs) */
    .header : {
        /*    0000-001B  Text[0..6] sections File Positions */
        LONG(0x00000100);
        LONG(0x00002520);
        LONG(LOADADDR(.text2));
        LONG(LOADADDR(.hooks));
        LONG(LOADADDR(.ctors));
        LONG(0);
        LONG(0);

        /*    001C-0047  Data[0..10] sections File Positions */
        LONG(0x003B3E20);
        LONG(0x003B3FC0);
        LONG(0x003B4240);
        LONG(0x003B4260);
        LONG(0x003B4280);
        LONG(0x003B6840);
        LONG(0x0042E6C0);
        LONG(0x004313C0);
        LONG(LOADADDR(.data8));
        LONG(0);
        LONG(0);

        /*    0048-0063  Text[0..6] sections Mem Address */
        LONG(0x80003100);
        LONG(0x80005940);
        LONG(ADDR(.text2));
        LONG(ADDR(.hooks));
        LONG(ADDR(.ctors));
        LONG(0);
        LONG(0);

        /*    0064-008F  Data[0..10] sections Mem Address */
        LONG(0x80005520);
        LONG(0x800056C0);
        LONG(0x803B7240);
        LONG(0x803B7260);
        LONG(0x803B7280);
        LONG(0x803B9840);
        LONG(0x804D36A0);
        LONG(0x804D79E0);
        LONG(ADDR(.data8));
        LONG(0);
        LONG(0);

        /*    0090-00AB  Text[0..6] sections Sizes */
        LONG(0x00002420);
        LONG(0x003B1900);
        LONG(SIZEOF(.text2));
        LONG(SIZEOF(.hooks));
        LONG(SIZEOF(.ctors));
        LONG(0);
        LONG(0);

        /*    00AC-00D7  Data[0..10] sections Sizes */
        LONG(0x000001A0);
        LONG(0x00000280);
        LONG(0x00000020);
        LONG(0x00000020);
        LONG(0x000025C0);
        LONG(0x00077E80);
        LONG(0x00002D00);
        LONG(0x00007220);
        LONG(SIZEOF(.data8));
        LONG(0);
        LONG(0);

        /*    00D8  BSS Mem address
         *    00DC  BSS Size */
        LONG(__BSS_START__);
        LONG(__BSS_SIZE__);

        /*    00E0  Entry Point */
        LONG(ABSOLUTE(__start));
    } >disk

    /* original data */
    .old     : ALIGN(32) { build/dol_data.bin } >disk

    /* new sections */
    .text2   : ALIGN(32) {      *(   .text    .text.*)
                                *( .rodata  .rodata.*)  } >new AT>disk
    .hooks   : ALIGN(32) { KEEP(*(  .hooks   .hooks.*)) } >new AT>disk
    .ctors   : ALIGN(32) { KEEP(*(  .ctors   .ctors.*)) } >new AT>disk
    .data8   : ALIGN(32) {      *(   .data    .data.*)  } >new AT>disk
    .bss     : ALIGN(32) {      *(    .bss     .bss.*)  } >new AT>disk

    /* split from output and passed to patch_dol.py */
    .patches : ALIGN(32) { KEEP(*(.patches .patches.*)) } >new AT>disk

    PROVIDE(__BSS_START__ = ADDR(.bss));
    PROVIDE(__BSS_SIZE__  = SIZEOF(.bss));

    /DISCARD/ : { *(*) }
}