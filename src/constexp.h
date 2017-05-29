
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
