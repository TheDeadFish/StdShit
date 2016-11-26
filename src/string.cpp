
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
