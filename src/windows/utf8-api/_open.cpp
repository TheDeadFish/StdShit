#include "stdshit.h"

extern "C" {
int _open(const char *name, int oflag, ...) {
	auto fname = getNtPathNameX(name);
	va_list argptr; va_start(argptr, oflag);  
	return _wopen(fname, oflag, va_arg(argptr, char*)); }
}
