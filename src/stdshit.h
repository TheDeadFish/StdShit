#ifndef _STDSHIT_H_
#define _STDSHIT_H_
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>
#include "macro-evil.h"
#include "macro-fest.h"
#include "type_traits.h"
#ifndef no_delagates
 #include "delegates.h"
#endif
#include "x86bits.h"
#include "min_max.h"
#include "algorithm.h"
#include "void.h"
#include "memalloc.h"
#include "copyfill.h"
#include "constexp.h"
#include "math.h"
#include "string.h"
#include "windows\utfconv.h"
#include "windows\windows.h"
#include "compat.h"

// C++11 fest: SCOPE_EXIT
template<class F>
class finally_type { F function;
public: finally_type(F f) : function(f) {}
	__attribute__((always_inline)) ~finally_type() { function(); } };
template<class F> finally_type<F> 
	finally(F f) { return finally_type<F>(f); }
#define SCOPE_EXIT(f) auto MCAT(sExit, __COUNTER__) = \
	finally([&](void) __attribute__((always_inline)) {f;})

// C++11 fest: Ranged base for
template <typename T> struct Range1_type2
{	T value;
	Range1_type2(T in) : value(in) {}
    T operator*() { return value; } 
	bool operator!=(const Range1_type2<T>& that) const
		{ return that.value != value; }
	void operator++() { value++; } };
template <typename T> class Range1_type {
public: Range1_type(T start, T end) :
    begin_(start), end_(end) {}
    Range1_type2<T> begin() { return begin_; }
    Range1_type2<T> end () { return end_; }
	Range1_type2<T> begin_, end_;	};
template <typename T> class Range2_type {
public: Range2_type(T* collection, size_t size) :
    mCollection(collection), mSize(size) {}
    T* begin() { return &mCollection[0]; }
    T* end () { return &mCollection[mSize]; }
private: T* mCollection; size_t mSize; };
template <typename T>
Range1_type<T> Range(T start, T end)
{	return Range1_type<T>(start, end); }
template <typename T>
Range2_type<T> Range(T* array, size_t size)
{	return Range2_type<T>(array, size); }

// C++11 fest: functional helpers
template<class Type, class Next>
bool is_one_of(const Type& needle, const Next& next)
{return needle==next;}
template<class Type, class Next, class ... Rest>
bool is_one_of(const Type& needle, const Next& next, const Rest&... haystack)
{return needle==next || is_one_of(needle, haystack...);}

// endian functions
#define bswap32 __builtin_bswap32
#define bswap16 __builtin_bswap16
#ifndef no_endian
struct big_u32 {
	big_u32(const big_u32& in) {	data = in.data;	}
	big_u32(u32 in) {	data = bswap32(in); }
	operator u32() {	return bswap32(data); }
	template <class screb>
	operator screb() {	return bswap32(data); }
	private: u32 data; };
struct big_u16 {
	big_u16(const big_u16& in) {	data = in.data;	}
	big_u16(u16 in) {	data = bswap16(in); }
	operator u16() {	return bswap16(data); }
	template <class screb>
	operator screb() {	return bswap16(data); }
	private: u16 data; };
#endif

// lods/stos strcpy/stradd
static inline
char* stradd(char* dst, const char* src) {
	char ch; while(lodsb(src, ch), ch) stosb(dst, ch);
	*dst = ch; return dst; }
static inline
wchar_t* stradd(wchar_t* dst, const wchar_t* src) {
	wchar_t ch; while(lodsw(src, ch), ch) stosw(dst, ch);
	*dst = ch; return dst; }
	
// File handling
SHITCALL void xfread(void*, size_t, size_t, FILE*);
SHITCALL void xfwrite(const void*, size_t, size_t, FILE*);
SHITCALL void xchsize(FILE* fp, long size);
SHITCALL int fsize(FILE* fp);
DEF_RETPAIR(loadFile_t, byte*, data, size_t, size);
SHITCALL loadFile_t loadFile(FILE* fp, int extra = 0);
SHITCALL char** loadText(FILE* fp, int& LineCount);
int xvfprintf ( FILE * stream, const char * format, va_list arg );
int xfprintf ( FILE * stream, const char * format, ... );
void xfputs (const char * str, FILE * stream);
SHITCALL void xfclose(FILE* fp); 
SHITCALL void xfclose_ref(FILE*& fp);

// File handling overloads
TMPL(T) void xfread(T* ptr, size_t size, FILE* fp) {	xfread(ptr, sizeof(T), size, fp); }
TMPL(T) void xfwrite(const T* ptr, size_t size, FILE* fp) {	xfwrite(ptr, sizeof(T), size, fp); }
TMPL(T) void xfread(T& ptr, FILE* fp) { xfread(&ptr, 1, fp); }
TMPL(T) void xfwrite(const T& ptr, FILE* fp) { xfwrite(&ptr, 1, fp);	}
template<typename T, int size> void xfread(T(& ptr)[size], FILE* fp) {	xfread(ptr, size, fp); }
template<typename T, int size> void xfwrite(T(& ptr)[size], FILE* fp) {	xfwrite(ptr, size, fp); }

// String overloads
inline FILE* fopen(const wchar_t* fName, const wchar_t* mode)
	{ return _wfopen(fName, mode); }
inline wchar_t* fgets (wchar_t* str, int num, FILE* fp)
	{ return fgetws(str, num, fp); }
inline size_t strlen (const wchar_t * str)	{ return wcslen(str); }
inline size_t strnlen(const wchar_t *s, size_t l) { return wcsnlen(s, l); }
inline wchar_t* strcpy(wchar_t* d, const wchar_t* s) { return wcscpy(d, s); }
inline int strcmp (const wchar_t* str1, const wchar_t* str2) { return wcscmp(str1, str2); }
inline int stricmp (const wchar_t* str1, const wchar_t* str2) { return wcsicmp(str1, str2); }
inline int strncmp (const wchar_t* str1, const wchar_t* str2, size_t num) { return wcsncmp(str1, str2, num); }
inline int strnicmp (const wchar_t* str1, const wchar_t* str2, size_t num) { return wcsnicmp(str1, str2, num); }
inline wchar_t* strdup(const wchar_t* str) { return wcsdup(str); }
inline int vsprintf (wchar_t * s, const wchar_t * format, va_list arg ) {
	return vswprintf(s, format, arg); }

template<typename T, int size>
int strncmp(const T* str1, const T(& str2)[size])
	{ return strncmp(str1, str2, size-1); }
template<typename T, int size>
int strnicmp(const T* str1, const T(& str2)[size])
	{ return strnicmp(str1, str2, size-1); }

#ifndef no_xstrfmt
struct xstrfmt_fmt { enum { FLAG_ABSLEN = 1<<16,
		SPACE_POSI = 1,		FLAG_XCLMTN = 2, 	FLAG_QUOTE = 4,		FLAG_HASH = 8,	 
		FLAG_DOLAR = 16,	FLAG_PRCNT = 32,	FLAG_AMPRSND = 64,	FLAG_APOSTR = 128,
		FLAG_LBRACE = 256,	FLAG_RBRACE = 512, 	UPPER_CASE = 1024, 	FORCE_SIGN = 2048,
		FLAG_COMMA = 4096,	LEFT_JUSTIFY = 8192, FLAG_SLASH = 1<<15, PADD_ZEROS = 1<<16	};
		
	va_list ap; char* dstPosArg; uint flags;
	int width; int precision; int length;
};

#endif

// qsort / bsearch overloads
TMPL2(T, F) void qsort(T* base, size_t num, F compar)
{	 typedef int (*qcomp)(const T&, const T&);
	qsort(base, num, sizeof(*base), (Void)(qcomp)compar); }
TMPL2(T, F) void qsort(T& array, F compar)
{	qsort(std::begin(array), std::end(array)-std::begin(array), compar); }
TMPL2(T, F) T* bsearch(void* key, T* base, size_t num, F compar)
{	 typedef int (*qcomp)(const T&, const T&); return (T*)
	bsearch(key, base, num, sizeof(*base), (Void)(qcomp)compar); }
TMPL2(T, F)	T* bsearch(void* key, T& array, F compar) {	return 
	bsearch(key, std::begin(array), std::end(array)-std::begin(array), compar); }

// xmemdup
SHITCALL2 Void xmemdup8(Void src, int count);
SHITCALL2 Void xmemdup16(Void src, int count);
SHITCALL2 Void xmemdup32(Void src, int count);
TMPL(T)
T* xMemdup(const T* src, int count) {
	if((sizeof(T) % 4) == 0) return xmemdup32(src, count * (sizeof(T)/4));
	ei((sizeof(T) % 2) == 0) return xmemdup16(src, count * (sizeof(T)/2));
	else			  		 return xmemdup8(src, count * sizeof(T)); }
	
// Error handling
extern const char progName[];
void contError(HWND hwnd, const char* fmt, ...);
FATALFUNC void fatalError(const char* fmt, ...);
FATALFUNC void fatalError(HWND hwnd, const char* fmt, ...);
FATALFUNC void errorAlloc(); 
FATALFUNC void errorMaxPath();
FATALFUNC void errorDiskSpace();
FATALFUNC void errorDiskWrite();
FATALFUNC void errorDiskFail();
FATALFUNC void errorBadFile();
TMPL(T) T errorAlloc(T ptr)
	{ if(!ptr) errorAlloc(); return ptr; }
SHITCALL int fopen_ErrChk(void);
	
#include "arrayMem.h"

// sprintf replacement
#define Xstrfmt(...) Cstr(xstrfmt( __VA_ARGS__))
SHITCALL cstr xstrfmt(VaArgFwd<const char*> va);
SHITCALL cstr xstrfmt(const char*, ...);
SHITCALL int strfmt(char* buffer, const char* fmt, ...);
SHITCALL int xstrfmt_len(VaArgFwd<const char*> va);
SHITCALL char* xstrfmt_fill(char* buffer,
	VaArgFwd<const char*> va);

// File handling
SHITCALL FILE* xfopen(const char*, const char*);
SHITCALL char* xfgets(char*, int, FILE*);
SHITCALL loadFile_t loadFile(const char* fileName, int extra = 0);
SHITCALL char** loadText(const char* fileName, int& LineCount);

// String handling
SHITCALL cstr xstrdup(const char*);
SHITCALL cstr xstrdup(const char*, size_t);
SHITCALL char* xstrdupr(char*&, const char*);	
SHITCALL char* xstrdupr(char*&, const char*, size_t);
SHITCALL char* strScmp(const char*, const char*);
SHITCALL char* strSicmp(const char*, const char*);
SHITCALL int strEcmp(const char*, const char*);
SHITCALL int strEicmp(const char*, const char*);
SHITCALL int strNcpy(char*, const char*, int);
SHITCALL int removeCrap(char*);
SHITCALL int strmove(char*, const char*);
SHITCALL char* strstr(const char*, const char*, int maxLen);
SHITCALL char* strstri(const char*, const char*, int maxLen);
SHITCALL int strcmp2(const char* str1, const char* str2);
SHITCALL int stricmp2(const char* str1, const char* str2);

// strings: non-null terminated source 
SHITCALL cstr strcpyn(char*, const char*, int);
SHITCALL bool strcmpn(const char*, const char*, int);
SHITCALL bool stricmpn(const char*, const char*, int);

#endif
