#if defined(NTSC100) || defined(PAL)
.section .text

.global hook_ParsePreloadedArchive
hook_ParsePreloadedArchive:
        mr      r3, r30
        bl      patch_preloaded_archive
        b       orig_ParsePreloadedArchive

#endif