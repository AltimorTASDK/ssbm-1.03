// These go in .version.text because they're currently only used in the 1.02 build
extern "C" [[gnu::section(".version.text")]] int __cxa_guard_acquire(void *gv)
{
	return 1;
}

extern "C" [[gnu::section(".version.text")]] int __cxa_guard_release(void *gv)
{
	return 1;
}