#include "dolphin/os.h"

#if defined(DOL)
#define VERSIONSTR "ISO"
#elif defined(NTSC102)
#define VERSIONSTR "NTSC 1.02"
#elif defined(NTSC101)
#define VERSIONSTR "NTSC 1.01"
#elif defined(NTSC100)
#define VERSIONSTR "NTSC 1.00"
#elif defined(PAL)
#define VERSIONSTR "PAL"
#endif

[[gnu::constructor]] static void show_build_date()
{
	OSReport(MODNAME " (" VERSIONSTR ") built " __DATE__ " " __TIME__ "\n");
}