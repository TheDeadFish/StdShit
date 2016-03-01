

// rotate operations
constexpr u8 rol_8(u8 val, int shift) { return (val << shift) | (val >> (8-shift)); }
constexpr u8 ror_8(u8 val, int shift) { return (val >> shift) | (val << (8-shift)); }
constexpr u16 rol_16(u16 val, int shift) { return (val << shift) | (val >> (16-shift)); }
constexpr u16 ror_16(u16 val, int shift) { return (val >> shift) | (val << (16-shift)); }	
constexpr u32 rol_32(u32 val, int shift) { return (val << shift) | (val >> (32-shift)); }
constexpr u32 ror_32(u32 val, int shift) { return (val >> shift) | (val << (32-shift)); }	

// character operations
REGCALL(1) constexpr static int toUpper(int ch){ return((ch>='a') &&(ch<='z'))?ch-32:ch;}
REGCALL(1) constexpr static int toLower(int ch){ return((ch>='A') &&(ch<='Z'))?ch+32:ch;}
constexpr static bool isPathSep(int ch) { return (ch == '\\')||(ch == '/'); }
