
// value comparion
template<class Type, class Next> constexpr
bool is_one_of(const Type& needle, const Next& next) 
{return needle==next;}
template<class Type, class Next, class ... Rest> constexpr
bool is_one_of(const Type& needle, const Next& next, const Rest&... haystack)
{return needle==next || is_one_of(needle, haystack...);}

// range comparison
template <class T, class U, class V> constexpr bool inRng(
	T t, U u, V v) { return (t >= u)&&(t <= v); }
template <class T, class U, class V> constexpr bool inRng1(
	T t, U u, V v) { return (t >= u)&&(t < v); }

// rotate operations
constexpr u8 rol_8(u8 val, int shift) { return (val << shift) | (val >> (8-shift)); }
constexpr u8 ror_8(u8 val, int shift) { return (val >> shift) | (val << (8-shift)); }
constexpr u16 rol_16(u16 val, int shift) { return (val << shift) | (val >> (16-shift)); }
constexpr u16 ror_16(u16 val, int shift) { return (val >> shift) | (val << (16-shift)); }	
constexpr u32 rol_32(u32 val, int shift) { return (val << shift) | (val >> (32-shift)); }
constexpr u32 ror_32(u32 val, int shift) { return (val >> shift) | (val << (32-shift)); }	

// character operations
#define isPathSep(...) is_one_of(__VA_ARGS__, '\\', '/')
#define isPathSep2(...) is_one_of(__VA_ARGS__, '\\', '/', ':')
#define isDigit(x) inRng(x, '0', '9')



#define _CEXPCHOP_(T) \
REGCALL(1) constexpr static T toUpper(T ch){ return((ch>='a') &&(ch<='z'))?ch-32:ch;} \
REGCALL(1) constexpr static T toLower(T ch){ return((ch>='A') &&(ch<='Z'))?ch+32:ch;}
_CEXPCHOP_(char) _CEXPCHOP_(int)
REGCALL(1) constexpr static bool isAlpha(int ch) { 
	return unsigned((ch & ~0x20) - 'A') < 26; }	
	
// overflow helpers
TMPL2(T, U) bool ovfAddChk(T& dst, U src, size_t ofs) { 
	size_t tmp; bool ret = __builtin_add_overflow((size_t)src,
	ofs, &tmp); dst = (T)tmp; return ret; }
TMPL2(T, U) bool ovfAddChk(T& dst, U src, size_t ofs, size_t sz) {
	return __builtin_mul_overflow(ofs, sz, &ofs) || 
		ovfAddChk(dst, src, ofs); }		
TMPL3(T, U, V) bool ovfAddChk2(V&& lim, T& dst, U src, size_t ofs) { 
	IFRET(ovfAddChk(dst, src, ofs)); nothing(); return (dst > (T)lim); }
TMPL3(T, U, V) bool ovfAddChk2(V&& lim, T& dst, U src, size_t ofs, size_t sz) {
	IFRET(ovfAddChk(dst, src, ofs, sz)); nothing(); return (dst > (T)lim); }
	
// pointer overflow
TMPL3(T=void, V, U) 
bool ptrAddChk(V*& dst, U* src, size_t len = 1) { 
	TMPL_ALT(X,T,U); return ovfAddChk(dst, src, len, sizeof(X)); }
TMPL2(T=void, U) bool ptrAddChk(U*& p, size_t len = 1) {
	return ptrAddChk<T>(p, p, len); }
		
	
template <class T, class U, class V>
ALWAYS_INLINE bool ovfchk(T a, U b, V c) { typeof(a) tmp;
	if(__builtin_constant_p(c)&&(c==1)) return b>=a;
	if(__builtin_add_overflow(b, c, &tmp)) return true;
	nothing(); return (tmp > a); }
template <class T, class U, class V>
ALWAYS_INLINE bool ovfchkU(T a, U b, V c) {
	return ovfchk(uns(a), uns(b), uns(c)); }
	
	
// overflow checked base+offset*scale 
template <class T, class U, class V, class W = int>
bool ovfAddBos(T& out, U b, V o, W s = 1) { typeof(out) S;
	return (__builtin_mul_overflow(o, s, &S))
	|| __builtin_add_overflow(b, S, &out); }
template <class T, class U, class V, class W = int>
bool ovfAddBosU(T& out, U b, V o, W s = 1) { return 
	ovfAddBos(unsR(out), uns(b), uns(o), uns(s)); }
template <class T, class U, class V = int>
T* ovfAddPosU(T* p, U o, V s = 1) { 
	if(ovfAddBosU(RT(&p), RT(&p), o, s)) 
	return 0; return notNull(p); }

// overflow checked string
DEF_RETPAIR(ovfStrChk_t, char*, src, void*, pos);
REGCALL(2) ovfStrChk_t ovfStrChk_(void* limit, void* pos);
TMPL(T) char* ovfStrChk(T*& pos, void* limit) { auto x 
	= ovfStrChk_(limit, pos); pos = (T*)x.pos; return x; }
