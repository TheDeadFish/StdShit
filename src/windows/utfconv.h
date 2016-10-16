
// wide string struct
struct cstrW { WCHAR* data; int slen; operator WCHAR*(){return data;}};
#define AUTFDRVCLS_(nm,bs,fr)  struct nm : bs { nm(const bs& t) : bs(t){} \
	nm(nm&& t) = default; nm(const nm& t) = delete; ~nm() { fr; } };
AUTFDRVCLS_(CstrW, cstrW, free(this->data));	

// UTF8/UTF16 conversion
#define UTF816A(rt,fn,ty) rt __stdcall fn(const ty* mbstr_); \
	rt __stdcall fn(const ty* mbstr_, int len);
UTF816A(int, utf816_size, char); UTF816A(int, utf816_size, wchar_t);
UTF816A(cstrW, utf816_dup, char); UTF816A(cstr, utf816_dup, wchar_t);
#define UTF816B(t1, t2) t1* __stdcall utf816_cpy(t1* wstr_, t2* mbstr_); \
	t1* __stdcall utf816_cpy(t1* wstr_, t2* mbstr_, int len);
UTF816B(wchar_t, const char); UTF816B(char, const wchar_t);

#define UNICODE_MAX 0x10FFFF
static inline int utf8_len1(int ch) { asm(
	"call _UTF8_LEN1;" : "+a"(ch)); return ch; }
static inline int utf8_len2(int ch) { asm(
	"call _UTF8_LEN2;" : "+a"(ch)); return ch; }
static inline char* utf8_put1(char* dst, int ch) { asm(
	"call _UTF8_PUT1;" : "+a"(ch), "+D"(dst) :: "edx"); return dst; }
static inline char* utf8_put2(char* dst, int ch) { asm(
	"call _UTF8_PUT2;" : "+a"(ch), "+D"(dst) :: "edx"); return dst; }
