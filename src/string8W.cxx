#include "stdshit.h"
#define NWIDE NWIDE2

#define MWBW MIF(NWIDE, "w", "b")
#define MWSZ MIF(NWIDE, "2", "1")
#define NWRDTX(n, t) NWNM(DEF_RDTEXT)(NWNM(n),t)

ASM_FUNC("_cstr_len" NWTX, "push %ecx; movl 8(%esp), %eax;"
	"push %eax; test %eax, %eax; jz 1f; call _" MIF(NWIDE, "wcs", "str") 
	"len; 1: movl %eax, %edx; pop %eax; pop %ecx; ret $4");
NCSTR cstr_dup(NCSTR str) { NCHR* buff = xMalloc(str.slen+1);
	*(NCHR*)memcpyX(buff, str.data, str.slen) = '\0'; 
	return { buff, str.slen }; }
	
template<> NCSTR cstr_alc<NCHR>(int len) { 
	return NCSTR(xMalloc(len+1), len); }

// string comparison
struct POFS { Void o; POFS() {} POFS(Void p2, Void p1) :
	o(p2-p1) { } TMPL(T) T* operator()(T* p) { return Void(p,o); }};
#define SYX(p1, p2, ln) auto* end = p1+ln; POFS pdx(p2,p1);

#define CSTR_CMP(nm, tl) \
int nm(CSTRG(1), CSTRG(2)) { \
	SYX(str1, str2, len1) int diff = len1-len2; \
	for(; !diff && (str1<end); diff = uns(tl(*str1))\
	-uns(tl(*pdx(str1))), str1++); return diff; } \
int nm(CSTRG(1), NCCH* str2) { SYX(str1, str2, len1) \
	for(; str1 < end; str1++) IFRET(uns(tl(*str1))\
	-uns(tl(*pdx(str1)))); return -uns(*pdx(str1)); }
CSTR_CMP(cstr_cmp,) CSTR_CMP(cstr_icmp, toLower)

// String Handling
#define STRCMP2(nm, cmp) SHITCALL int nm(NCCH* str1, NCCH* str2) {\
	for(NCCH* curPos = str2;; curPos++) { NCHR ch1; lodsx(str1, ch1);\
	NCHR ch2 = *curPos; if(cmp) return curPos-str2; if(!ch2) return -1; }}
STRCMP2(strcmp2, ch1 != ch2); STRCMP2(stricmp2, !cmpi(ch1, ch2));
#define STRSCMP(nm, cmp) SHITCALL NCHR* nm(NCCH* str1, NCCH* str2) \
	{ if(str1) while(1) { NCHR ch2; lodsx(str2, ch2); if( ch2 == 0 ) \
	return (NCHR*)str1; cmp(ch2, *str1++, NS); } NS: return NULL; }
STRSCMP(strScmp, CMPS); STRSCMP(strSicmp, CMPI);

// cstr substring search	
#define CSTR_ISTR(nm, cmp) NCSTR nm(CSTRG(1), CSTRG(2)) { \
	if(len2 > 0) { NCCH* endPos = str1 + (len1-len2); VARFIX(str2); \
	while(str1 <= endPos) { CMPL(len2, cmp(str1[i], str2[i], NS)); \
	return {str1, len2}; NS: str1++; }} return {0,0}; };
CSTR_ISTR(cstr_str, CMPS); CSTR_ISTR(cstr_istr, CMPI);	
NCSTR SHITCALL cstr_str(CSTRG(1), NCCH* str2) { NCSTR tmp(str2); 
	return cstr_str(str1, len1, CSTRX(tmp)); }
NCSTR SHITCALL cstr_istr(CSTRG(1), NCCH* str2) { NCSTR tmp(str2); 
	return cstr_istr(str1, len1, CSTRX(tmp)); }

// tokenization/splitting
SHITCALL NCSTR cstr_split(NCSTR& str, NCHR ch) {
	NCHR* begin = str; int slen = str.slen; REGFIX(b, ch);
	NCSTR str2; int idx = 0; for(;slen > 0; idx++) { slen--; 
	if(begin[idx] == ch) goto SPLIT; } str2 = {0,0};
	if(0) { SPLIT: VARFIX(idx); str2 = {begin+idx+1, 
	slen};}; str = str2; return {begin,idx}; }

NCSTR SHITCALL cstr_chr(CSTRG(1), NCHR ch) {
	xRngPtr<NCHR> p(str1, len1); while(p.chk()) {
		if(p.f() == ch) return {p.data, p.end_};
		INCP(p.data); } return {0,0}; }
NCSTR SHITCALL cstr_rchr(CSTRG(1), NCHR ch) {
	NCHR* end = str1+len1; xRngPtr<NCHR> p(str1, end); 
	while(p.chk()) { if(p.ld() == ch) return {
		p.end_, end}; } return {0,0}; }		
NCSTR SHITCALL cstr_chr2(CSTRG(1), NCHR ch) {		
	NCSTR tmp = cstr_chr(str1, len1, ch);
	if(tmp) tmp = {str1, tmp.data}; return tmp; }
NCSTR SHITCALL cstr_rchr2(CSTRG(1), NCHR ch) {		
	NCSTR tmp = cstr_rchr(str1, len1, ch);
	if(tmp) tmp = {str1, tmp.data}; return tmp; }
	
// filename handling
ASM_FUNC("_isRelPath0"NWTX, "test %eax, %eax; jz 1f;"
	"cmp"MWBW" $0, (%eax); jz 1f; jmp 4f;" GLOB_LAB("_isRelPath" NWTX) 
	"cmp $1, %edx; jbe 2f; 4:cmp"MWBW" $58, "MWSZ"(%eax); jnz 3f; 0: "
	"movb $0, %cl; ret; 2: test %edx, %edx; jz 1f; 3: cmp"MWBW" $92, (%eax);"
	"jz 0b; cmp"MWBW" $47, (%eax); jz 0b; 1: movb $1, %cl; ret;");
NCSTR getPath(NCSTR str) { int end = str.slen; REGFIX(d, end);
	while(--end >= 0) { if(str[end] == '\\') goto L1;
	ARGFIX(*str); if(str[end] == '/') goto L1; } VARFIX(end);
	if((str.slen >= 2)&&(str[1] == ':')) end += 2;
	L1: nothing(); return {str.data, end+1}; }
NCSTR getName(NCSTR str) { NCSTR tmp = getPath(str);
	return { tmp.end(), str.slen-tmp.slen }; }

// Path Handling
NWRDTX(str_pathCat, "%$j%$k")
SHITCALL NCSTR pathCat(NCSTR name1, NCSTR name2) { 
	if(!isRelPath(name2)) return name2.xdup();
	return xstrfmt(NWNM(str_pathCat), name1, name2); }
SHITCALL NCSTR replName(NCSTR name1, NCSTR name2) { 
	return pathCat(getPath(name1), name2); }
SHITCALL NCSTR pathCatF(NCSTR name1, NCSTR name2) { 
	SCOPE_EXIT(free(name1)); return pathCat(name1, name2); }
SHITCALL NCSTR replNameF(NCSTR name1, NCSTR name2) { 
	return pathCatF(getPath(name1), name2); }
SHITCALL NCSTR fullNameRepl(NCSTR base, NCSTR name) {
	return getFullPathF(replName(base, name)); }
SHITCALL NCSTR fullNameCat(NCSTR base, NCSTR name) {
	return getFullPathF(pathCat(base, name)); }
NCSTR getName2(NCSTR str) { 
	auto p = (str = getName(str)).ptr();
	while(p.chk() && p.f() == '.') p.fi();
	while(p.chk()) { if(p.ld() == '.') {
	str.sete(p); break; }} return str; }
NCSTR getExt(NCSTR str) { NCSTR tmp = getName2(str);
	return {tmp.end(), str.slen-tmp.slen}; }

// extension Handling
NWRDTX(str_extCat, "%v%v\0%v.%v")
SHITCALL NCSTR extCat(NCSTR name1, NCSTR name2) { 
	NCCH* fmt = NWNM(str_extCat);
	if(name2.get(0) != '.') fmt += 5;
	return xstrfmt(fmt, name1, name2); }
NCSTR getName3(NCSTR str) { return {
	str.data, getName2(str).end()}; }
SHITCALL NCSTR replExt(NCSTR name1, NCSTR name2) { 
	return extCat(getName3(name1), name2); }
CSTRTH1_(getName3) CSTRTH2_(replExt)

CSTRTH1_(getPath) CSTRTH1_(getName) 
CSTRTH1_(getName2) CSTRTH1_(getExt)
CSTRTH2_(pathCat) CSTRTH2_(replName)
CSTRTH2_(pathCatF) CSTRTH2_(replNameF)
CSTRTH2_(fullNameRepl)
CSTRTH2_(fullNameCat)



SHITCALL NCSTR xstrdup(NCCH* str)
{
	if(str == NULL) return {0,0};
	int len = strlen(str);
	NCHR* ret = xMalloc(len+1);
	return {strcpy(ret, str), len};
}

SHITCALL NCSTR xstrdup(NCCH* str, size_t maxLen)
{
	if(str == NULL) return NULL;
	int strLen = strnlen(str, maxLen);
	NCHR* buffer = xMalloc(strLen+1);
	return strcpyn(buffer, str, strLen);
}

SHITCALL
NCHR* xstrdupr(NCHR*& str1, NCCH* str2) {
	return free_repl(str1, xstrdup(str2)); }
SHITCALL
NCHR* xstrdupr(NCHR*& str1,NCCH* str2, size_t sz) {
	return free_repl(str1, xstrdup(str2, sz)); }

SHITCALL
int strEcmp(NCCH* str1, NCCH* str2)
{
	int diff = strlen(str1)-strlen(str2);
	if(diff < 0)
		return 1;
	return strcmp(str1+diff, str2);
}

SHITCALL
int strEicmp(NCCH* str1, NCCH* str2)
{
	int diff = strlen(str1)-strlen(str2);
	if(diff < 0) return 1;
	return stricmp(str1+diff, str2);
}

SHITCALL
int strNcpy(NCHR* dst, NCCH* src, int num)
{
	for(int i = 0; i < num; i++)
	  if(!(dst[i] = src[i]))
		return i;
	if(num >= 0)
		dst[num] = '\0';
	return num;
}

SHITCALL NCSTR strcpyn(
	NCHR* dst, NCCH* src, int len)
{
	memcpyX(dst, src, len);
	dst[len] = '\0'; return {dst, len};
}

SHITCALL
bool strcmpn(NCCH* str1, NCCH* str2, int len)
{
	if(strlen(str1) != len)
		return false;
	return !strncmp(str1, str2, len);
}

SHITCALL
bool stricmpn(NCCH* str1, NCCH* str2, int len)
{
	if(strlen(str1) != len)
		return false;
	return !strnicmp(str1, str2, len);
}

SHITCALL
int removeCrap(NCHR* str)
{
	int len = strlen(str);
	while(len--)
		if(uns(str[len]) > ' ')
			break;
	str[len+1] = '\0';
	return len+1;
}

SHITCALL 
int strmove(NCHR* dst, NCCH* src)
{
	int len = strlen(src)+1;
	memmove(dst, src, len*sizeof(*dst));
	return len;
}

#undef NWIDE
