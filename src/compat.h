#ifndef _DF_COMPAT_H_
#define _DF_COMPAT_H_

#ifndef __forceinline
 #define __forceinline inline __attribute__((__always_inline__))
#endif
#ifndef ARRAYSIZE 
 #define ARRAYSIZE(a) (sizeof(a)/sizeof((a)[0]))
#endif

// strnlen
#if __MINGW32__ && !__MINGW64_VERSION_MAJOR 
#define _OLDMINGW_
SHITCALL size_t strnlen(const char* s, size_t l);
SHITCALL size_t wcsnlen(const wchar_t* s, size_t l);
#endif

/* msvc/msvcrt compatibility
Void memmem(const void *b1, const void *b2,
	size_t len1, size_t len2);
#define __forceinline inline
#define _vsnwprintf_s _vsnprintf_s
#define swprintf_s sprintf_s
#define _vsntprintf_s _vsnprintf_s
#define _stprintf_s sprintf_s
int _vsnprintf_s(char *buffer, size_t sizeOfBuffer,
	const char *format, va_list ap);
int sprintf_s(char *buffer, size_t sizeOfBuffer,
	const char *format, ... );
int _vsnprintf_s(wchar_t *buffer, size_t sizeOfBuffer,
	const wchar_t *format, va_list ap);
int sprintf_s(wchar_t *buffer, size_t sizeOfBuffer,
	const wchar_t *format, ... );
*/

#endif
