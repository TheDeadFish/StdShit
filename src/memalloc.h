// memmory allocation functions and helpers
// implments basic allocators and container
// allocators

// frame information helper
#ifdef __GCC_HAVE_DWARF2_CFI_ASM
 asm(".macro cfia_ n; .cfi_adjust_cfa_offset \\n; .endm");
#else 
 asm(".macro cfia_ n; .endm");
#endif

// register preserving allocators
#ifndef _WIN64
#define free free_
#define pcpx(ph, fn) "cfia_ 4; push "#ph"; call "#fn"; cfia_ -4;"
ALWAYS_INLINE void free_(void* mem) { asm( pcpx(%0, _sfree)
	:: "g"(mem) : "memory"); }
ALWAYS_INLINE void free_ref(Void& ptr) { asm(pcpx(%0, _sfreer)
	:: "g"(&ptr) : "memory"); }
ALWAYS_INLINE Void malloc_(size_t sz) { Void r; asm(
	"call _smalloc;" : "=a"(r) : "a"(sz)); return r; }
ALWAYS_INLINE Void realloc_(void* ptr, size_t sz) { Void r; 
	asm(pcpx(%1, _srealloc) : "=a"(r) : "g"(ptr), "d"(sz)); return r; }
ALWAYS_INLINE Void xmalloc(size_t sz) { Void r; asm(pcpx(%1, _xmalloc)
	 : "=a"(r) : "g"(sz)); return r; }
#else
void free_ref(Void& ptr); Void xmalloc(size_t sz);
ALWAYS_INLINE Void malloc_(size_t sz){ return malloc(sz); }
ALWAYS_INLINE Void realloc_(void* ptr, size_t sz){ return realloc(ptr, sz); }
#endif

static uint snapUpSize(uint val) {	return 2 << (__builtin_clz(val-1)^31); }

// resource freeing functions
/*SHITCALL void free_ref(Void& ptr);*/ SHITCALL uint snapNext(uint val);
SHITCALL void freeLst(Void ptr, int count); 
SHITCALL void freeLst_ref(Void& ptr, int count);
#define fclose fclose_
SHITCALL int fclose_ ( FILE * stream );
SHITCALL int fclose_ref( FILE*& stream );
REGCALL(1) void free_ref(void* ptr, void* repl);

// Memory allocation wrappers
SHITCALL Void calloc (size_t size);
TMPL(T) static inline Void realloc2(T& ptr, size_t size) { Void tmp =
	realloc(ptr, size); if(tmp) ptr = tmp; return tmp; }
#define malloc malloc_
#define realloc realloc_

// Memory allocation functions
/*SHITCALL2 Void xmalloc(size_t size);*/ REGCALL(2) Void xrealloc(Void& ptr, size_t size); 
SHITCALL2 Void xcalloc(size_t size); SHITCALL2 Void xrecalloc(Void& ptr, size_t size);
SHITCALL2 Void xnxalloc(Void& ptr, size_t& count, size_t size);
SHITCALL2 Void nxalloc(Void& ptr, size_t& count, size_t size);
__thiscall Void xnxalloc2(void* p, size_t size);
__thiscall Void xnxcalloc2(void* p, size_t size);

// Memory allocation templates
TMPL(T) void free_ref(T*& p) {	free_ref(*(Void*)&p); }
TMPL(T) void freeLst_ref(T& p, int c) {	freeLst_ref(*(Void*)&p, c); }
#define _Realloc(n1, n2) TMPL(T) Void n1(T*& p, size_t sz) { return n1(*(Void*)&p, \
	sz); } TMPL(T) Void n2(T*& p, size_t sz) { return n1(p, sz*sizeof(T)); }
_Realloc(xrealloc, xRealloc) _Realloc(xrecalloc, xRecalloc)
#define _Nxalloc(n1, n2) TMPL2(T,U) Void n1(T*& p, U& ct, size_t sz) { return n1(*(Void*)&p, \
	*(size_t*)&ct, sz); } TMPL2(T,U) T& n2(T*& p, U& ct){ return *(T*)n1(p, *(size_t*)&ct, sizeof(T)); }
_Nxalloc(xnxalloc, xNextAlloc) _Nxalloc(nxalloc, NextAlloc) 
#define xMalloc_(name, func) struct name { name(size_t sz) : sz(sz) {} \
	operator void*() { return func(sz); } TMPL(T) \
	ALWAYS_INLINE operator T*() { return (T*)func(sizeof(T)*sz); } private: size_t sz; };
xMalloc_(xMalloc, xmalloc); xMalloc_(xCalloc, xcalloc);
xMalloc_(Malloc, malloc); xMalloc_(Calloc, calloc);

// xmemdup
SHITCALL2 Void xmemdup8(Void src, int count);
SHITCALL2 Void xmemdup16(Void src, int count);
SHITCALL2 Void xmemdup32(Void src, int count);
TMPL(T)
T* xMemdup(const T* src, int count) {
	if((sizeof(T) % 4) == 0) return xmemdup32(src, count * (sizeof(T)/4));
	ei((sizeof(T) % 2) == 0) return xmemdup16(src, count * (sizeof(T)/2));
	else			  		 return xmemdup8(src, count * sizeof(T)); }
