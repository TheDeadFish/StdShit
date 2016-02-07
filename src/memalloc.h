// memmory allocation functions and helpers
// implments basic allocators and container
// allocators

static uint snapUpSize(uint val) {	return 2 << (__builtin_clz(val-1)^31); }

// resource freeing functions
SHITCALL void free_ref(Void& ptr); SHITCALL uint snapNext(uint val);
SHITCALL void freeLst(Void ptr, int count); 
SHITCALL void freeLst_ref(Void& ptr, int count);
#define fclose fclose_
SHITCALL int fclose_ ( FILE * stream );
SHITCALL void fclose_ref( FILE*& stream );

// Memory allocation wrappers
SHITCALL Void calloc (size_t size);
TMPL(T) static inline Void realloc2(T& ptr, size_t size) { Void tmp =
	realloc(ptr, size); if(tmp) ptr = tmp; return tmp; }
static inline Void malloc_(size_t size) { return malloc(size); }
static inline Void realloc_(void* ptr, size_t size) { return realloc(ptr, size); }
#define malloc malloc_
#define realloc realloc_

// Memory allocation functions
SHITCALL2 Void xmalloc(size_t size); SHITCALL2 Void xrealloc(Void& ptr, size_t size);
SHITCALL2 Void xcalloc(size_t size); SHITCALL2 Void xrecalloc(Void& ptr, size_t size);
SHITCALL2 Void xnxalloc(Void& ptr, size_t& count, size_t size);
SHITCALL2 Void nxalloc(Void& ptr, size_t& count, size_t size);

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
	operator T*() { return (T*)func(sizeof(T)*sz); } private: size_t sz; };
xMalloc_(xMalloc, xmalloc); xMalloc_(xCalloc, xcalloc);
xMalloc_(Malloc, malloc); xMalloc_(Calloc, calloc);
