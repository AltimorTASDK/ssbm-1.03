#include <gctypes.h>

struct HSD_ArchiveHeader {
	u32 file_size;
	u32 data_size;
	u32 nb_reloc;
	u32 nb_public;
	u32 nb_extern;
	u8 version[4];
	u32 pad[2];
};

struct HSD_ArchiveRelocationInfo {
	u32 offset;
};

struct HSD_ArchivePublicInfo {
	u32 offset;
	u32 symbol;
};

struct HSD_ArchiveExternInfo {
	u32 offset;
	u32 symbol;
};

struct HSD_Archive {
	HSD_ArchiveHeader header;
	u8 *data;
	HSD_ArchiveRelocationInfo *reloc_info;
	HSD_ArchivePublicInfo *public_info;
	HSD_ArchiveExternInfo *extern_info;
	char *symbols;
	HSD_Archive *next;
	char *name;
	u32 flags;
	void *top_ptr;
};

// Loaded archive pointers
extern "C" HSD_Archive *MnSlChr;
