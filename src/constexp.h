

// rotate operations
constexpr u8 rol_8(u8 val, int shift) { return (val << shift) | (val >> (8-shift)); }
constexpr u8 ror_8(u8 val, int shift) { return (val >> shift) | (val << (8-shift)); }
constexpr u16 rol_16(u16 val, int shift) { return (val << shift) | (val >> (16-shift)); }
constexpr u16 ror_16(u16 val, int shift) { return (val >> shift) | (val << (16-shift)); }	
constexpr u32 rol_32(u32 val, int shift) { return (val << shift) | (val >> (32-shift)); }
constexpr u32 ror_32(u32 val, int shift) { return (val >> shift) | (val << (32-shift)); }	

// character operations
#define _CEXPCHOP_(T) \
REGCALL(1) constexpr static T toUpper(T ch){ return((ch>='a') &&(ch<='z'))?ch-32:ch;} \
REGCALL(1) constexpr static T toLower(T ch){ return((ch>='A') &&(ch<='Z'))?ch+32:ch;}\
constexpr static bool isPathSep(T ch) { return (ch == '\\')||(ch == '/'); }
_CEXPCHOP_(char) _CEXPCHOP_(int)
