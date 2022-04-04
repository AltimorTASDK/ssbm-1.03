#ifdef NOPAL
.section .zalloc, "ax"

.global zcalloc
zcalloc:
        mullw   r3, r4, r5
        b       HSD_MemAlloc

.global zcfree
zcfree:
        mr      r3, r4
        b       HSD_Free
#endif
