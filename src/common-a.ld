__LOADER_BASE__ = 0x81780000;
__LOADER_SIZE__ = 0xEC00;
__ZLIB_SIZE__   = 0x8000;
__ZLIB_BASE__   = __LOADER_BASE__ + __LOADER_SIZE__;
__MOD_BASE__    = __ZLIB_BASE__ + __ZLIB_SIZE__;
__MAX_ADDR__    = 0x817F8AC0; /* start of vanilla FileStringTable */