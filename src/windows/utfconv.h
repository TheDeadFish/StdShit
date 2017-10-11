
// wide string struct
struct cstrW { WCHAR* data; int slen; operator WCHAR*(){return data;}};
typedef xstr_<WCHAR, cstrW> wxstr;
SHITCALL cstrW xstrdup(const WCHAR*);

//#define AUTFDRVCLS_(nm,bs,fr)  struct nm : bs { nm(const bs& t) : bs(t){} \
//	nm(nm&& t) = default; nm(const nm& t) = delete; ~nm() { fr; } };
//AUTFDRVCLS_(CstrW, cstrW, free(this->data));	


// UTF8/UTF16 conversion

#define UTF816L(rt, ty) DEF_RETPAIR(rt, int, \
	size, const ty*, endp); UTF816A(rt, utf816_size, ty)
#define UTF816A(rt,fn,ty) rt __stdcall fn(const ty* mbstr_); \
	rt __stdcall fn(const ty* mbstr_, int len);
UTF816L(utf816_size_tW, char); UTF816L(utf816_size_t8, wchar_t);
UTF816A(cstrW, utf816_dup, char); UTF816A(cstr, utf816_dup, wchar_t);
#define UTF816B(t1, t2) t1* __stdcall utf816_cpy(t1* wstr_, t2* mbstr_); \
	t1* __stdcall utf816_cpy(t1* wstr_, t2* mbstr_, int len);
UTF816B(char, const wchar_t);
#define UTF816CP_(nm,dn,sn) WCHAR* __stdcall MCAT(utf816_cpy,nm)( \
	WCHAR* dst, MIF(dn, (int dstMax,),) cch* src MIF(sn, (,int len),))
UTF816CP_(,0,0); UTF816CP_(,0,1); UTF816CP_(,1,0); UTF816CP_(,1,1);

//non-null terminated
int __stdcall utf816_size2(cch*, int);
WCHAR* __stdcall utf816_cpy2_(WCHAR*, cch*, int);
WCHAR* __stdcall utf816_cpy2(WCHAR*, cch*, int);
cstrW __stdcall utf816_dup2(cch*, int);
cstrW __stdcall utf816_strLst_dup(cch* str);

#define UNICODE_MAX 0x10FFFF

// utf8 inline assembly macros
#define UTF8_GET2(ch,s,e) ({ void* t; asm("call _UTF8_GET2" \
	: "+a"(ch), "+S"(s), "=d"(t) : "d"(e)); });
#define UTF816_LEN8B(ch,s,e,l) if(s8(ch)<0) { \
	UTF8_GET2(ch,s,e); if(ch>>16) ADDP(l,1); }
#define UTF816_CPY8B(ch,d,s,e) ({ void* t; asm("call _UTF8_GET2; "\
	"call _UTF16_PUT1" : "+a"(ch), "+S"(s), "+D"(d), "=d"(t) : "d"(e)); })

/*
static inline int utf8_len1(int ch) { asm(
	"call _UTF8_LEN1;" : "+a"(ch)); return ch; }
static inline int utf8_len2(int ch) { asm(
	"call _UTF8_LEN2;" : "+a"(ch)); return ch; }
static inline char* utf8_put1(char* dst, int ch) { asm(
	"call _UTF8_PUT1;" : "+a"(ch), "+D"(dst) :: "edx"); return dst; }
static inline char* utf8_put2(char* dst, int ch) { asm(
	"call _UTF8_PUT2;" : "+a"(ch), "+D"(dst) :: "edx"); return dst; }
*/
