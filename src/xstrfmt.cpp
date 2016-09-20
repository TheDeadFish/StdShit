#ifndef no_xstrfmt

struct xstrfmt_fmt_ : xstrfmt_fmt
{
	char getFillCh() {
		char fillCh = ' '; 
		if(flags&PADD_ZEROS)
			movb2(fillCh, '0');
		return fillCh; }
		
	size_t ext_mode() { size_t (__thiscall *funcPtr)
		(void* ctx) = va_arg(ap, Void);	return funcPtr(this); }

	size_t str_mode(void);
	size_t dec_mode(bool sign);
	size_t hex_mode(); size_t cmd_mode();
	size_t sep_mode(); 
	
	static REGCALL(1) void slash(void);
	
	DEF_RETPAIR(core_t, size_t, extraLen, char*, str);
	core_t core(char* str);
	
	
};

// tristi quod ad hunc
#define APPEND_SLASH asm("call xstrfmt_slash" : "+D"(dstPos))
asm(".section .text$_ZN12xstrfmt_fmt_8str_modeEv,\"x\";"
	"xstrfmt_slash: movb -1(%edi), %al;"
	"cmp $92, %al; jz 1f; cmp $47, %al; jz 1f;"
	"movb $92, %al; stosb; 1: ret");

size_t xstrfmt_fmt_::str_mode(void)
{
	char* dstPos = dstPosArg;	
	char* str = (char*)va_arg(ap, char*);
	movfx(S, str); movfx(b, flags);
	if(!str) str = (char*)"";
	
	// calculate lenth
	size_t strLen; 
	if(flags & FLAG_DOLAR) {
		strLen = va_arg(ap, size_t);
	} else { strLen = precision;
		if(!dstPos || width) strLen
			= strnlen(str, strLen);
	}
	
	// dir seperator / return length
	if((flags & FLAG_XCLMTN) && !(strLen && *str))
		{ strLen = 1; str = (char*)"."; }
	if(!dstPos)	return max(strLen, width)
		+ !!(flags & FLAG_SLASH)
		+ !!(flags & FLAG_COMMA);
	if(flags & FLAG_COMMA) APPEND_SLASH;
		
	// output string padding
	int len = width - strLen;
	if(len > 0) { //ARGFIX(flags);
		char fillCh = getFillCh();
		do { stosx(dstPos, fillCh);
		} while(--len > 0);}
		
	// output string data
	VARFIX(flags); if(flags & FLAG_DOLAR)
		memcpy_ref(dstPos, str, strLen);
	else {		
	char* endPos = str+strLen; asm goto ( 
		"jmp %l1" :: "r"(endPos):: LOOP_START);
	do { {char ch; lodsx(str, ch);
		if(ch == '\0') break;
		stosx(dstPos, ch); }
LOOP_START:;
	} while(str != endPos);
	}
	
	if(flags & FLAG_SLASH)
		APPEND_SLASH;
	return (size_t)dstPos;
}

size_t xstrfmt_fmt_::dec_mode(bool sign)
{
	char* dstPos = dstPosArg;
	size_t data = va_arg(ap, size_t);

	// calculate length
	char signCh = 0;
	if(sign != 0) {	if(int(data) < 0) {
			signCh = '-'; data = -data; }
		ei(flags & SPACE_POSI) signCh = ' ';
		ei(flags & FORCE_SIGN) signCh = '+';
	} int strLen = 1;
	for(; strLen < ARRAYSIZE(powersOf10); strLen++) {
	  if(powersOf10[strLen] > data) break; }
	strLen = max(strLen+bool(signCh), width);
	if(dstPos == NULL) return strLen;
	
	// output string
	char* endPos = dstPos + strLen;
	char* curPos = endPos;
	do { *(--curPos) = '0'+data%10;
	} while(data /= 10);
	if(curPos > dstPos) {
		char fillCh;
		if(flags & PADD_ZEROS) {
			if(signCh) stosx(dstPos, signCh);
			fillCh = '0';
		} else {
			if(signCh) *(--curPos) = signCh;
			fillCh = ' '; }
		while(curPos-- > dstPos)
			*curPos = fillCh; 
	} return (size_t)endPos;
}

size_t xstrfmt_fmt_::hex_mode(void)
{
	INT64 data = (length >= 2) ?
		va_arg(ap, INT64) : va_arg(ap, size_t);

	// calculate lenth
	int maxBit = (DWORD(data>>32)) ?
	32+(__builtin_clz(data>>32)^31) : (__builtin_clz(data|1)^31);
	size_t strLen = max((maxBit+4)/4, width);
	if(dstPosArg == NULL) return strLen;

	// output string
	char* endPos = dstPosArg + strLen;
	char* curPos = endPos;
	const byte* hexTab = (flags & UPPER_CASE) ?
		tableOfHex[1] : tableOfHex[0];
	do { *(--curPos) = hexTab[data&15];
	} while(data >>= 4);
	char fillCh = getFillCh();
	while(curPos-- > dstPosArg)
		*curPos = fillCh;
	return (size_t)endPos;
}

size_t xstrfmt_fmt_::cmd_mode(void)
{
	char cmdFlag = 0;
	asm("sar $3, %k1; rolb $2, %b0;" 
		: "=Q"(cmdFlag) : "0"(flags));
	char* src = va_arg(ap, char*);
	size_t maxLen = (cmdFlag & ESC_FIXED) ?
		va_arg(ap, size_t) : precision;
	if(dstPosArg == NULL) return cmd_escape_len(src, maxLen, cmdFlag);
	else return (size_t)cmd_escape(dstPosArg, src, maxLen, cmdFlag);
}

size_t xstrfmt_fmt_::sep_mode(void)
{
	if(dstPosArg == NULL) return 1;
	else { WRI(dstPosArg, '\\');
		return (size_t)dstPosArg;
	}
}

xstrfmt_fmt_::core_t xstrfmt_fmt_::core(char* str)
{
	// flag stage
	flags = 0; char ch; char ch2;
	while(lodsx(str, ch), ch2 = ch-' ', 
	(ch2 < 17)&&(ch != '*')&&(ch != '.'))
		flags |= 1 << ch2;
	
	// width/precision stage
	int* dst = &width;
GET_INT_NEXT: { int result;
		if(ch == '*') {	lodsx(str, ch);
			result = va_arg(ap, int); }
		else { result = 0; byte tmp;
			while((tmp = ch-'0') < 10) { result *= 10;
				result += tmp; lodsx(str, ch); }
		} *dst = movfx(D, result);
	if(dst == &width) {
		dst = &precision; *dst = 0x7FFFFFFF;
		if(ch == '.') { lodsx(str, ch); 
			goto GET_INT_NEXT; }
	}}

	// length stage
	length = 0; LENGTH_NEXT:
	if(ch == 'h') { length--; lodsx(str, ch); goto LENGTH_NEXT; }
	if(ch == 'l') { length++; lodsx(str, ch); goto LENGTH_NEXT; }
	if((ch >= 'A')&&(ch <= 'Z')) { asm("add $32, %0": 
		"=r"(ch) : "r"(ch)); flags |= UPPER_CASE; }
	size_t extraLen;
	
	switch(ch) {
	if(0) { case 'j': flags |= FLAG_XCLMTN; }
	if(0) { case 'k': flags |= FLAG_COMMA; }
	if(0) { case 'v': flags |= FLAG_DOLAR; }
	case 's': extraLen = str_mode(); break;
	
	case 'x': extraLen = hex_mode(); break;
	case 'd': extraLen = dec_mode(false); break;
	case 'u': extraLen = dec_mode(true); break;
	case 'q': extraLen = ext_mode(); break;
	case 'z': extraLen = cmd_mode(); break;
	case ':': extraLen = sep_mode(); break;
	
	
	default: UNREACH;
	}
	
	
	
	

	return core_t(extraLen, str);
}

SHITCALL
int xstrfmt_len(VaArgFwd<const char*> va)
{
	xstrfmt_fmt_ ctx; ctx.ap = va.start();
	ctx.dstPosArg = 0;
	
	int extraLen = 1; char ch;
	DEF_ESI(char* curPos) = (char*)*va.pfmt;
	while(lodsx(curPos, ch), ch) {
		if(ch != '%') { 
	ESCAPE_PERCENT:	extraLen++; }
		ei(*curPos == '%') { curPos++;
			goto ESCAPE_PERCENT; }
		else {
			auto result = ctx.core(curPos);
			curPos = result.str;
			extraLen += result.extraLen; }
	} return extraLen;
}

SHITCALL
char* xstrfmt_fill(char* buffer,
	VaArgFwd<const char*> va)
{
	xstrfmt_fmt_ ctx; ctx.ap = va.start();
	
	DEF_ESI(char* curPos) = (char*)*va.pfmt;
	DEF_EDI(char* dstPos) = buffer;
	while(1) {
		char ch; lodsx(curPos, ch);
		if(ch != '%') { ESCAPE_PERCENT:
			stosx(dstPos, ch);
			if(ch == '\0') return dstPos; }
		ei(*curPos == '%') { curPos++;
			goto ESCAPE_PERCENT; }
		else {
			ctx.dstPosArg = dstPos;
			auto result = ctx.core(curPos);
			curPos = result.str;
			dstPos = (char*)result.extraLen;
		}
	}
	return dstPos;
}

SHITCALL
cstr xstrfmt(VaArgFwd<const char*> va)
{
	va_list ap = va.start();
	char* buffer = xMalloc(xstrfmt_len(va));
	char* endPos = xstrfmt_fill(buffer, va);
	return {buffer, (endPos-1)-buffer};
}

SHITCALL NEVER_INLINE
cstr xstrfmt(const char* fmt, ...)
{
	VA_ARG_FWD(fmt); return xstrfmt(va);
}

SHITCALL NEVER_INLINE int strfmt(
	char* buffer, const char* fmt, ...)
{
	VA_ARG_FWD(fmt);
	char* endPos = xstrfmt_fill(buffer, va);
	return (endPos-1)-buffer;
}

void xvector_::fmtcat(const char* fmt, ...)
{
	VA_ARG_FWD(fmt);
	int strLen = xstrfmt_len(va)*sizeof(char);
	char* buffer = xnxalloc_(strLen); 
	dataSize -= sizeof(char);
	xstrfmt_fill(buffer, va); 
}
#endif
