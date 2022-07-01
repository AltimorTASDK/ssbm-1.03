#pragma once

#include "util/meta.h"
#include <gctypes.h>

struct HSD_CObjDesc;
struct HSD_FogDesc;
struct HSD_JObjDesc;
struct HSD_LObjDesc;
struct HSD_AnimJoint;
struct HSD_MatAnimJoint;
struct HSD_ShapeAnimJoint;

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

struct HSD_RawArchive {
	HSD_ArchiveHeader header;
	u8 data[];
};

struct ArchiveModel {
	HSD_JObjDesc *joint;
	HSD_AnimJoint *animjoint;
	HSD_MatAnimJoint *matanim_joint;
	HSD_ShapeAnimJoint *shapeanim_joint;
};

struct ArchiveModelScene {
	HSD_JObjDesc *joint;
	HSD_AnimJoint **anims;
	HSD_MatAnimJoint **matanims;
	HSD_ShapeAnimJoint **shapeanims;
};

template<typename T = ArchiveModelScene*>
struct ArchiveScene {
	T *models;
	HSD_CObjDesc **cameras;
	HSD_LObjDesc **lights;
	HSD_FogDesc **fogs;
};

extern "C" {

// Loaded archive pointers
extern HSD_Archive *MnSlChr;

HSD_Archive *HSD_ArchiveLoad(const char *path);
void HSD_ArchiveFree(HSD_Archive *archive);
HSD_Archive *HSD_ArchiveLoadAndGetSymbols(const char *path, ...);
void *HSD_ArchiveGetSymbol(const HSD_Archive *archive, const char *symbol);
void HSD_ArchiveGetSymbols(const HSD_Archive *archive, ...);

} // extern "C"

template<typename T>
inline T *HSD_ArchiveGetSymbol(const HSD_Archive *archive, const char *symbol)
{
	return (T*)HSD_ArchiveGetSymbol(archive, symbol);
}

// Helper function to get an entire ArchiveModel at once
template<string_literal name>
inline ArchiveModel HSD_ArchiveGetModel(const HSD_Archive *archive)
{
	ArchiveModel result;
	HSD_ArchiveGetSymbols(
		archive,
		&result.joint,           string_literal(name.value, "_joint"),
		&result.animjoint,       string_literal(name.value, "_animjoint"),
		&result.matanim_joint,   string_literal(name.value, "_matanim_joint"),
		&result.shapeanim_joint, string_literal(name.value, "_shapeanim_joint"));

	return result;
}