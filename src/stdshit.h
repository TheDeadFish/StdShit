#ifndef _STDSHIT_H_
#define _STDSHIT_H_
#include <tchar.h>
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>
#include <memory>
#include "compat.h"
#include "macro-evil.h"
#include "macro-fest.h"
#include "type_traits.h"
#ifndef no_delagates
 #include "delegates.h"
#endif
#include "x86bits.h"
#include "x64bits.h"
#include "min_max.h"
#include "algorithm.h"
#include "void.h"
#include "memalloc.h"
#include "copyfill.h"
#include "constexp.h"
#include "math.h"
#include "arrayMem.h"
#include "string.h"
#include "windows\utfconv.h"
#include "windows\windows.h"
#include "xvector.h"
#include "xqsort.h"

SHITCALL Void memmem(const void *b1, 
	const void *b2, size_t len1, size_t len2);

// C++11 fest: SCOPE_EXIT
template<class F>
class finally_type { F function;
public: finally_type(F f) : function(f) {}
	__attribute__((always_inline)) ~finally_type() { function(); } };
template<class F> finally_type<F> 
	finally(F f) { return finally_type<F>(f); }
#define SCOPE_EXIT(f) auto MCAT(sExit, __COUNTER__) = \
	finally([&](void) __attribute__((always_inline)) {f;})
#define SCOPE_REF(T,R,M) T R=M; SCOPE_EXIT(M=R)

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
wchar_t* stradd(wchar_t* dst, cchw* src) {
	wchar_t ch; while(lodsw(src, ch), ch) stosw(dst, ch);
	*dst = ch; return dst; }
	
// File handling
SHITCALL void xfread(void*, size_t, size_t, FILE*);
SHITCALL void xfwrite(const void*, size_t, size_t, FILE*);
SHITCALL void xchsize(FILE* fp, long size);
SHITCALL int fsize(FILE* fp);
SHITCALL xarray<byte> loadFile(FILE* fp, int extra = 0);
SHITCALL xarray<char> loadText(FILE* fp);
SHITCALL char** loadText(FILE* fp, int& LineCount);
SHITCALL int saveFile(cch* fName, void* data, size_t size);
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
inline FILE* fopen(cchw* fName, cchw* mode)
	{ return _wfopen(fName, mode); }
inline wchar_t* fgets (wchar_t* str, int num, FILE* fp)
	{ return fgetws(str, num, fp); }
inline size_t strlen (cchw * str)	{ return wcslen(str); }
inline size_t strnlen(cchw *s, size_t l) { return wcsnlen(s, l); }
inline wchar_t* strcpy(wchar_t* d, cchw* s) { return wcscpy(d, s); }
inline int strcmp (cchw* str1, cchw* str2) { return wcscmp(str1, str2); }
inline int stricmp (cchw* str1, cchw* str2) { return wcsicmp(str1, str2); }
inline int strncmp (cchw* str1, cchw* str2, size_t num) { return wcsncmp(str1, str2, num); }
inline int strnicmp (cchw* str1, cchw* str2, size_t num) { return wcsnicmp(str1, str2, num); }
inline wchar_t* strdup(cchw* str) { return wcsdup(str); }
inline int vsprintf (wchar_t * s, cchw * format, va_list arg ) {
	return vswprintf(s, format, arg); }
inline int system(cchw* s) { return _wsystem(s); }

template<typename T, int size>
int strncmp(const T* str1, const T(& str2)[size])
	{ return strncmp(str1, str2, size-1); }
template<typename T, int size>
int strnicmp(const T* str1, const T(& str2)[size])
	{ return strnicmp(str1, str2, size-1); }

// qsort / bsearch overloads
TMPL2(T, F) void qsort(T* base, size_t num, F compar)
{	 typedef int (*qcomp)(const T&, const T&);
	qsort(base, num, sizeof(*base), (Void)(qcomp)compar); }
TMPL2(T, F) void qsort(T& array, F compar)
{	qsort(std::begin(array), std::end(array)-std::begin(array), compar); }
TMPL2(T, F) T* bsearch(void* key, T* base, size_t num, F compar)
{	 typedef int (*qcomp)(const T&, const T&); return (T*)
	bsearch(key, base, num, sizeof(*base), (Void)(qcomp)compar); }
TMPL2(T, F)	auto* bsearch(void* key, T& array, F compar) {	return 
	bsearch(key, std::begin(array), std::end(array)-std::begin(array), compar); }
	
// binrary search2
xRngPtr<byte> bsearch2 (const void*, const void*, size_t, size_t, compar_t);
TMPL2(T, F) xRngPtr<T> bsearch2(const void* key, T* base, size_t num, F compar)
{ return bit_cast<xRngPtr<T>>(bsearch2(key, base, num, sizeof(*base), (Void)compar)); }
TMPL2(T, F) auto bsearch2(const void* key, T& array, F compar) { return
	bsearch2(key, std::begin(array), std::end(array)-std::begin(array), compar); }

// lower_bound
byte* bsearch_lower (const void*, const void*, size_t, size_t, compar_t);
TMPL2(T, F) T* bsearch_lower(const void* key, T* base, size_t num, F compar)
{ return (T*)(bsearch_lower(key, base, num, sizeof(*base), (Void)compar)); }
TMPL2(T, F) auto bsearch_lower(const void* key, T& array, F compar) { return
	bsearch_lower(key, std::begin(array), std::end(array)-std::begin(array), compar); }

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

// File handling
SHITCALL FILE* xfopen(const char*, const char*);
SHITCALL char* xfgets(char*, int, FILE*);
SHITCALL xarray<byte> loadFile(const char* fileName, int extra = 0);
SHITCALL xarray<char> loadText(cch* fName);
SHITCALL char** loadText(const char* fileName, int& LineCount);

extern const char mem_zp4[];

// like unique_ptr, only this is actually usable
TMPL(T) struct xMem
{ 
	// data access
	operator T*() { return data; }
	void init(T* p) { data = p; }
	void reset(T* p = 0) { free(data); init(p); }
	T* release(T* p = 0) { return ::release(data, p); }

	// ctor/dtor/assignment
	constexpr xMem() : data(0) {}
	ALWAYS_INLINE ~xMem() { free(data); } 
	constexpr xMem(T* p) : data(p) {}
	xMem& operator=(T* p) { reset(p); return *this; }
	xMem(xMem&& u) : data(u.release()) {}
	xMem& operator=(xMem&& u) { reset(u.release()); return *this; }

	T* data;
};

#endif
