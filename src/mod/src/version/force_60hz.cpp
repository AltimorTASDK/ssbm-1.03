#ifdef PAL

extern "C" int Is60Hz;
extern "C" void ApplyVideoSettings();

[[gnu::constructor]] static void force_60hz()
{
	if (!Is60Hz) {
		Is60Hz = true;
		ApplyVideoSettings();
	}
}

#endif