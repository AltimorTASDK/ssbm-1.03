#include "os/os.h"

#define DEFINE_THROW(x) \
	__attribute__((noreturn)) void __throw_##x() { PANIC("__throw_" #x); }

#define DEFINE_THROW_WITH_ARGS(x, ...) \
	__attribute__((noreturn)) void __throw_##x(__VA_ARGS__) { PANIC("__throw_" #x); }

namespace std {

DEFINE_THROW(bad_exception)
DEFINE_THROW(bad_alloc)
DEFINE_THROW(bad_array_new_length)
DEFINE_THROW(bad_cast)
DEFINE_THROW(bad_typeid)
DEFINE_THROW(bad_function_call)
DEFINE_THROW_WITH_ARGS(logic_error, const char *__s)
DEFINE_THROW_WITH_ARGS(domain_error, const char *__s)
DEFINE_THROW_WITH_ARGS(invalid_argument, const char *__s)
DEFINE_THROW_WITH_ARGS(length_error, const char *__s)
DEFINE_THROW_WITH_ARGS(out_of_range, const char *__s)
DEFINE_THROW_WITH_ARGS(out_of_range_fmt, const char *__fmt, ...)
DEFINE_THROW_WITH_ARGS(runtime_error, const char *__s)
DEFINE_THROW_WITH_ARGS(range_error, const char *__s)
DEFINE_THROW_WITH_ARGS(overflow_error, const char *__s)
DEFINE_THROW_WITH_ARGS(underflow_error, const char *__s)

}

extern "C" {

void *__cxa_allocate_exception(size_t)
{
	PANIC("__cxa_allocate_exception");
}

void __cxa_free_exception(void*)
{
	PANIC("__cxa_allocate_exception");
}

void __cxa_throw(void*, void*, void(*)(void*))
{
	PANIC("__cxa_throw");
}

void __cxa_rethrow()
{
	PANIC("__cxa_rethrow");
}

void __cxa_begin_catch(void*)
{
}

void __cxa_end_catch()
{
}

void __cxa_tm_cleanup(void*, void*, unsigned int)
{
	PANIC("__cxa_tm_cleanup");
}

void _Unwind_DeleteException(struct _Unwind_Exception*)
{
	PANIC("_Unwind_DeleteException");
}

void _Unwind_Resume(struct _Unwind_Exception*)
{
	PANIC("_Unwind_Resume");
}

}