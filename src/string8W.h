

ALWAYS_INLINE NCSTR cstr_len(NCCH* si) { NCSTR so; asm("push %1; call "
	"_cstr_len" MIF(NWIDE, "W", "") : "=A"(so): "g"(si)); return so; }
REGCALL(2) NCSTR cstr_dup(NCSTR str);

// cstr: string comparison
int SHITCALL cstr_cmp(CSTRG(1), CSTRG(2));
int SHITCALL cstr_icmp(CSTRG(1), CSTRG(2));
int SHITCALL cstr_cmp(CSTRG(1), NCCH* str2);
int SHITCALL cstr_icmp(CSTRG(1), NCCH* str2);
NCSTR SHITCALL cstr_str(CSTRG(1), CSTRG(2));
NCSTR SHITCALL cstr_istr(CSTRG(1), CSTRG(2));
NCSTR SHITCALL cstr_str(CSTRG(1), NCCH* str2);
NCSTR SHITCALL cstr_istr(CSTRG(1), NCCH* str2);

// cstr: tokenization/splitting
SHITCALL NCSTR cstr_split(cstr& str, NCHR ch);
CSTRFN4_(cstr_chr) CSTRFN4_(cstr_rchr)
CSTRFN4_(cstr_chr2) CSTRFN4_(cstr_rchr2)

// String handling
SHITCALL NCSTR xstrdup(NCCH*);
SHITCALL NCSTR xstrdup(NCCH*, size_t);
SHITCALL NCHR* xstrdupr(NCHR*&, NCCH*);	
SHITCALL NCHR* xstrdupr(NCHR*&, NCCH*, size_t);
SHITCALL NCHR* strScmp(NCCH*, NCCH*);
SHITCALL NCHR* strSicmp(NCCH*, NCCH*);
SHITCALL int strEcmp(NCCH*, NCCH*);
SHITCALL int strEicmp(NCCH*, NCCH*);
SHITCALL int strNcpy(NCHR*, NCCH*, int);
SHITCALL int removeCrap(NCHR*);
SHITCALL int strmove(NCHR*, NCCH*);
SHITCALL NCHR* strstr(NCCH*, NCCH*, int maxLen);
SHITCALL NCHR* strstri(NCCH*, NCCH*, int maxLen);
SHITCALL int strcmp2(NCCH* str1, NCCH* str2);
SHITCALL int stricmp2(NCCH* str1, NCCH* str2);

// strings: non-null terminated source 
SHITCALL NCSTR strcpyn(NCHR*, NCCH*, int);
SHITCALL bool strcmpn(NCCH*, NCCH*, int);
SHITCALL bool stricmpn(NCCH*, NCCH*, int);

// Path Handling
CSTRFN1_(getPath) CSTRFN1_(getName) 
CSTRFN1_(getName2) CSTRFN1_(getExt)
CSTRFN2_(replName) CSTRFN2_(pathCat)
CSTRFN2_(fullNameRepl) CSTRFN2_(fullNameCat)
static inline NCSTR getPath0(NCSTR str) {
	return getPath(str).nterm(); }
	
// path test functions
static inline bool isRelPath(NCCH* str) { bool ret; asm(
	"call _isRelPath0"NWTX : "=c"(ret) : "a"(str)); return ret; }
static inline bool isRelPath(NCSTR str) { bool ret; asm(
	"call _isRelPath"NWTX : "=c"(ret) : "A"(str)); return ret; }

// sprintf replacement
//#define Xstrfmt(...) Cstr(xstrfmt( __VA_ARGS__))
SHITCALL NCSTR xstrfmt(void* cbCtx, xstrfmt_fmtN<NCHR>::
	cbfn_t cbfn, VaArgFwd<NCCH*> va);
SHITCALL NCSTR xstrfmt(void* cbCtx, xstrfmt_fmtN<NCHR>::
	cbfn_t cbfn, NCCH*, ...);
SHITCALL NCSTR xstrfmt(VaArgFwd<NCCH*> va);
SHITCALL NCSTR xstrfmt(NCCH*, ...);
SHITCALL int strfmt(NCHR* buffer, NCCH* fmt, ...);
SHITCALL int xstrfmt_len(VaArgFwd<NCCH*> va);
SHITCALL NCHR* xstrfmt_fill(NCHR* buffer,
	VaArgFwd<NCCH*> va);

#undef NWIDE
