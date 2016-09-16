
cstr cstr_len(const char* str) {
	int len = 0; if(str != NULL) {
	asm("push %2; call _strlen;"
	"movl %%eax, %%edx; pop %%eax" : "=a"(str),
	"=d"(len) : "g"(str) : "ecx"); } return {str, len};
}

SHITCALL cstr getPath(cstr str)
{
	int i = str.slen;
	while(i--) { if(isPathSep(
		str[i])) return str.left(i+1); }
	return cstr(str, 0);
}

SHITCALL cstr getName(cstr str)
{
	auto tmp = getPath(str);
	return tmp.endRel(str.slen);
}

#define BSTR_ALLOC() auto* This = this; if(mlen<= len) {\
	GET_RETPAIR(This, len, alloc(len)); } char* data = \
	This->data; int slen = This->slen; data[len] = 0;

bstr::alloc_t bstr::alloc(int len)
{
	if(mlen == 0) data = NULL;
	mlen = ALIGN(max(mlen + (mlen>>1),
		len+1), (8/sizeof(char))-1);
	xRealloc(data, mlen); 
	return {this, len};
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
	VA_ARG_FWD(fmt);
	char* buff = xresize(xstrfmt_len(va)-1);
	xstrfmt_fill(buff, va); return *this; }
bstr& bstr::fmtcat(const char* fmt, ...) {
	VA_ARG_FWD(fmt);
	char* buff = xnalloc(xstrfmt_len(va)-1);
	xstrfmt_fill(buff, va); return *this; }
	
// null-termination
cstr bstr::nullTerm(void) { int len = slen; 
	BSTR_ALLOC(); return {This->data, len}; }
cstr bstr::calcLen(void) { cstr ret = cstr_len(data);
	return {ret.data, slen = ret.slen}; }
cstr bstr::updateLen(void) { cstr ret = cstr_len(data);
	return {ret.data, slen += ret.slen}; }

/*
bstr::strcat(char* str)
{
	int len = strlen(str);
	strcpy(xNalloc, str);
}
*/
