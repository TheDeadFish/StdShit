
cstrT cstr_len(const NCHAR* str) {
	int len = 0; if(str != NULL) {
#if NWIDE == 0
	asm("push %2; call _strlen;"
#else
	asm("push %2; call _wcslen;"
#endif
	"movl %%eax, %%edx; pop %%eax" : "=a"(str),
	"=d"(len) : "g"(str) : "ecx"); } return {str, len};
}

SHITCALL cstrT getPath(cstrT str)
{
	int i = str.slen;
	while(i--) { if(isPathSep(
		str[i])) return str.left(i+1); }
	return cstrT(str, 0);
}

SHITCALL cstrT getName(cstrT str)
{
	auto tmp = getPath(str);
	return tmp.endRel(str.slen);
}

#define BSTR_ALLOC() auto* This = this; if(mlen<= len) {\
	GET_RETPAIR(This, len, alloc(len)); } NCHAR* data = \
	This->data; int slen = This->slen; data[len] = 0;

template <>
bstrT::alloc_t bstrT::alloc(int len)
{
	if(mlen == 0) data = NULL;
	mlen = ALIGN(max(mlen + (mlen>>1),
		len+1), (8/sizeof(NCHAR))-1);
	xRealloc(data, mlen); 
	return {this, len};
}




template <>
NCHAR* bstrT::xresize(int len) { BSTR_ALLOC();
	This->slen = len; return data; }
template <>
NCHAR* bstrT::xnalloc(int len) { len += slen; BSTR_ALLOC();
	This->slen = len; return data+slen; }
template <>
NCHAR* bstrT::xreserve(int len) {
	BSTR_ALLOC(); return data; }
template <>
NCHAR* bstrT::xralloc(int len) { len += slen;
	BSTR_ALLOC(); return data+slen; }
	
	
// basic copy/concat
template <>
bstrT& bstrT::strcpy(const NCHAR* str) { ::strcpy(xresize(
	strlen(str)), str); return *this; }
template <>
bstrT& bstrT::strcpy(cstrT str) { memcpyX(xresize(
	str.slen), str.data, str.slen); return *this; }
template <>
bstrT& bstrT::strcat(const NCHAR* str) { ::strcpy(xnalloc(
	strlen(str)), str); return *this; }
template <>
bstrT& bstrT::strcat(cstrT str) { memcpyX(xnalloc(
	str.slen), str.data, str.slen); return *this; }
template <>
bstrT& bstrT::fmtcpy(const NCHAR* fmt, ...) {
	VA_ARG_FWD(fmt);
	NCHAR* buff = xresize(xstrfmt_len(va)-1);
	xstrfmt_fill(buff, va); return *this; }
template <>
bstrT& bstrT::fmtcat(const NCHAR* fmt, ...) {
	VA_ARG_FWD(fmt);
	NCHAR* buff = xnalloc(xstrfmt_len(va)-1);
	xstrfmt_fill(buff, va); return *this; }
	
// null-termination
template <>
cstrT bstrT::nullTerm(void) { int len = slen; 
	BSTR_ALLOC(); return {This->data, len}; }
template <>
cstrT bstrT::calcLen(void) { cstrT ret = cstr_len(data);
	return {ret.data, slen = ret.slen}; }
template <>
cstrT bstrT::updateLen(void) { cstrT ret = cstr_len(data);
	return {ret.data, slen += ret.slen}; }

/*
bstrT::strcat(NCHAR* str)
{
	int len = strlen(str);
	strcpy(xNalloc, str);
}
*/
