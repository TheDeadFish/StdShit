#include "stdshit.h"

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
	
// command line building
bstr& bstr::argcat(cstr str) { return fmtcat(" %$z", str); }
bstr& bstr::argcat(cch* str) { return fmtcat(" %z", str); }
bstr& bstr::argcatf(cstr str) { return fmtcat(" %#$z", str); }
bstr& bstr::argcatf(cch* str) { return fmtcat(" %#z", str); }

/* cstr substring search	
#define CSTR_ISTR(nm, cmp) cstr nm(CSTRG(1), CSTRG(2)) { const char* \
	endPos = str1 + (len1-len2); VARFIX(str2); if(len2 > 0) { while( \
	str1 < endPos) { CMPL(len2, cmp(str1[i], str2[i], NS)); \
	return {str1, len2}; NS: str1++; }} return {0,0}; };
CSTR_ISTR(cstr_str, CMPS); CSTR_ISTR(cstr_istr, CMPI);	
cstr SHITCALL cstr_str(CSTRG(1), cch* str2) { cstr tmp(str2); 
	return cstr_str(str1, len1, CSTRX(tmp)); }
cstr SHITCALL cstr_istr(CSTRG(1), cch* str2) { cstr tmp(str2); 
	return cstr_istr(str1, len1, CSTRX(tmp)); } */
