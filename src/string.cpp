#include "stdshit.h"

ASM_FUNC("_cstr_len", "push %ecx; movl 8(%esp), %eax;"
	"push %eax; test %eax, %eax; jz 1f; call _strlen;"
	"1: movl %eax, %edx; pop %eax; pop %ecx; ret $4");

cstr cstr_dup(cstr str)
{
	char* buff = xmalloc(str.slen+1);
	buff[str.slen] = '\0'; return { (char*)memcpy(
	buff, str.data, str.slen), str.slen};
}

#define BSTR_ALLOC() auto* This = this; if(mlen<= len) {\
	GET_RETPAIR(This, len, alloc(len)); } char* data = \
	This->data; int slen = This->slen; data[len] = 0;
	
bstr::bstr(cch* str) : bstr(cstr(str)) {}
bstr::bstr(cstr that) : cstr(that.xdup()) { mlen = slen; }

bstr::alloc_t bstr::alloc(int len)
{
	if(mlen == 0) data = NULL;
	mlen = ALIGN(max(mlen + (mlen>>1),
		len+1), (8/sizeof(char))-1);
	xRealloc(data, mlen); 
	return {this, len};
}

void bstr::push_back(char c)
{
	*xnalloc(1) = c;
}

// string comparison
#define CSTR_CMP(nm, tl) \
int nm(CSTRG(1), CSTRG(2)) { int diff = len1-len2; \
	for(int i = 0; !diff && (i<len1); diff = \
	tl(str1[i])-tl(str2[i]), i++); return diff; } \
int nm(CSTRG(1), cch* str2) { \
	for(u8 ch : CSTRS(1)) { if(int diff = tl(ch)-tl(RDI \
	(str2))) return diff; } return -u8(*str2); }
CSTR_CMP(cstr_cmp,) CSTR_CMP(cstr_icmp, toLower)

char* bstr::xresize(int len) { BSTR_ALLOC();
	This->slen = len; return data; }
char* bstr::xnalloc(int len) { len += slen; BSTR_ALLOC();
	This->slen = len; return data+slen; }
char* bstr::xreserve(int len) {
	BSTR_ALLOC(); return data; }
char* bstr::xralloc(int len) { len += slen;
	BSTR_ALLOC(); return data+slen; }	
	
// basic copy/concat
bstr& bstr::strcpy(const char* str) { ::strcpy(xresize(
	strlen(str)), str); return *this; }
bstr& bstr::strcpy(cstr str) { memcpyX(xresize(
	str.slen), str.data, str.slen); return *this; }
bstr& bstr::strcat(const char* str) { ::strcpy(xnalloc(
	strlen(str)), str); return *this; }
bstr& bstr::strcat(cstr str) { memcpyX(xnalloc(
	str.slen), str.data, str.slen); return *this; }
bstr& bstr::fmtcpy(const char* fmt, ...) {
	VA_ARG_FWD(fmt);  setend(xstrfmt_fill(xreserve(
		xstrfmt_len(va)-1), va)); return *this; }
bstr& bstr::fmtcat(const char* fmt, ...) {
	VA_ARG_FWD(fmt); setend(xstrfmt_fill(xralloc(
		xstrfmt_len(va)-1), va)); return *this; }
	
// null-termination
cstr bstr::nullTerm(void) { int len = slen; 
	BSTR_ALLOC(); return {This->data, len}; }
cstr bstr::calcLen(void) { cstr ret = cstr_len(data);
	return {ret.data, slen = ret.slen}; }
cstr bstr::updateLen(void) { slen +=
	cstr_len(end()).slen; return *this; }
	
// path handling
bstr& bstr::pathcat(cch* str) { return pathcat(cstr(str)); }
bstr& bstr::pathend(cch* str) { return pathend(cstr(str)); }
bstr& bstr::pathcat(cstr str) { int extra = sepReq();
	char* buff = xnalloc(str.slen+extra); if(extra)
	WRI(buff, '\\'); ::strcpy(buff, str.data); return *this; }
bstr& bstr::pathend(cstr str) { int len = slen; bstr&
	ret = pathcat(str); ret.slen = len; return ret; }
	
// tokenization/splitting
SHITCALL cstr cstr_split(cstr& str, char ch) {
	char* begin = str; int slen = str.slen; REGFIX(b, ch);
	cstr str2; int idx = 0; for(;slen > 0; idx++) { slen--; 
	if(begin[idx] == ch) goto SPLIT; } str2 = {0,0};
	if(0) { SPLIT: VARFIX(idx); str2 = {begin+idx+1, 
	slen};}; str = str2; return {begin,idx}; }

cstr SHITCALL cstr_chr(CSTRG(1), char ch) {
	xRngPtr<char> p(str1, len1); while(p.chk()) {
		if(p.f() == ch) return {p.data, p.end_};
		INCP(p.data); } return {0,0}; }
cstr SHITCALL cstr_rchr(CSTRG(1), char ch) {
	char* end = str1+len1; xRngPtr<char> p(str1, end); 
	while(p.chk()) { if(p.ld() == ch) return {
		p.end_, end}; } return {0,0}; }

// cstr substring search	
#define CSTR_ISTR(nm, cmp) cstr nm(CSTRG(1), CSTRG(2)) { const char* \
	endPos = str1 + (len1-len2); VARFIX(str2); if(len2 > 0) { while( \
	str1 < endPos) { CMPL(len2, cmp(str1[i], str2[i], NS)); \
	return {str1, len2}; NS: str1++; }} return {0,0}; };
CSTR_ISTR(cstr_str, CMPS); CSTR_ISTR(cstr_istr, CMPI);	
cstr SHITCALL cstr_str(CSTRG(1), cch* str2) { cstr tmp(str2); 
	return cstr_str(str1, len1, CSTRX(tmp)); }
cstr SHITCALL cstr_istr(CSTRG(1), cch* str2) { cstr tmp(str2); 
	return cstr_istr(str1, len1, CSTRX(tmp)); }
