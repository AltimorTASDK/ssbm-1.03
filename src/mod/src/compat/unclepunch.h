enum OSD {
	OSD_UCF = 28
};

#ifndef STEALTH
extern "C" bool is_unclepunch();
extern "C" bool is_unclepunch_osd();
#else
consteval bool is_unclepunch()     { return false; }
consteval bool is_unclepunch_osd() { return false; }
#endif