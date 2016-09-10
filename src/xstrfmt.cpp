#ifndef no_xstrfmt
#undef xstrfmt_fmt_
#define xstrfmt_fmt_ MCAT(xstrfmt_fmt_,NCHAR)

struct xstrfmt_fmt_ : xstrfmt_fmt<NCHAR>
{
	NCHAR getFillCh() {
		char fillCh = ' '; 
		if(flags&PADD_ZEROS)
			movb2(fillCh, '0');
		return fillCh; }
		
	size_t ext_mode() { size_t (__thiscall *funcPtr)
		(void* ctx) = va_arg(ap, Void);	return funcPtr(this); }

	size_t str_mode(bool fixed);
	size_t dec_mode(bool sign);
	size_t hex_mode(); size_t cmd_mode();
	DEF_RETPAIR(core_t, size_t, extraLen, NCHAR*, str);
	core_t core(NCHAR* str);
};

size_t xstrfmt_fmt_::str_mode(bool fixed)
{
	NCHAR* dstPos = dstPosArg;
	size_t data = va_arg(ap, size_t);
	if(fixed) precision = va_arg(ap, size_t);

#ifndef no_utf816
	if(flags & UPPER_CASE) {
  #ifdef NWIDE
		if(data == 0) data = (size_t)"(null)";
		if(dstPos == NULL) return utf8To16_size((char*)data)-1;
		dstPos = utf8To16_cpy(dstPos, (char*)data);
		nothing(); return (size_t)dstPos;
  #else
		if(data == 0) data = (size_t)L"(null)";
 		if(dstPos == NULL) return utf16To8_size((wchar_t*)data)-1;
		dstPos = utf16To8_cpy(dstPos, (wchar_t*)data);
		nothing(); return (size_t)dstPos;
  #endif
	}
#endif

	// calculate lenth
	movfx(S, data);
	NCHAR* str = (NCHAR*)data;
	if(str == NULL) str = (NCHAR*)_N("(null)");
	size_t strLen = precision;
	if(!fixed && (!dstPos || width))
		strLen = strnlen(str, strLen);
	if(dstPos == 0)
		return max(strLen, width);
		
	// output string padding
	int len = width - strLen;
	if(len > 0) { ARGFIX(flags);
		NCHAR fillCh = getFillCh();
		do { stosx(dstPos, fillCh);
		} while(--len > 0);}
		
	// output string data
	if(fixed == true) {
		memcpy_ref(dstPos, str, strLen);
		return (size_t)dstPos; }
	NCHAR* endPos = str+strLen; asm goto ( 
		"jmp %l1" :: "r"(endPos):: LOOP_START);
	do { {NCHAR ch; lodsx(str, ch);
		if(ch == '\0') break;
		stosx(dstPos, ch); }
LOOP_START:;
	} while(str != endPos);
	return (size_t)dstPos;
}

size_t xstrfmt_fmt_::dec_mode(bool sign)
{
	NCHAR* dstPos = dstPosArg;
	size_t data = va_arg(ap, size_t);

	// calculate length
	NCHAR signCh = 0;
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
	NCHAR* endPos = dstPos + strLen;
	NCHAR* curPos = endPos;
	do { *(--curPos) = '0'+data%10;
	} while(data /= 10);
	if(curPos > dstPos) {
		NCHAR fillCh;
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
	NCHAR* endPos = dstPosArg + strLen;
	NCHAR* curPos = endPos;
	const byte* hexTab = (flags & UPPER_CASE) ?
		tableOfHex[1] : tableOfHex[0];
	do { *(--curPos) = hexTab[data&15];
	} while(data >>= 4);
	NCHAR fillCh = getFillCh();
	while(curPos-- > dstPosArg)
		*curPos = fillCh;
	return (size_t)endPos;
}

size_t xstrfmt_fmt_::cmd_mode(void)
{
	char cmdFlag = 0;
	asm("sar $3, %k1; rolb $2, %b0;" 
		: "=Q"(cmdFlag) : "0"(flags));
	NCHAR* src = va_arg(ap, NCHAR*);
	size_t maxLen = (cmdFlag & ESC_FIXED) ?
		va_arg(ap, size_t) : precision;
	if(dstPosArg == NULL) return cmd_escape_len(src, maxLen, cmdFlag);
	else return (size_t)cmd_escape(dstPosArg, src, maxLen, cmdFlag);
}

xstrfmt_fmt_::core_t xstrfmt_fmt_::core(NCHAR* str)
{
	// flag stage
	flags = 0; NCHAR ch; NCHAR ch2;
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
	if(ch < 'a') { asm("add $32, %0": "=r"(ch) : "r"(ch)); flags |= UPPER_CASE; }
	size_t extraLen;
	
	// tristi quod ad hunc
	switch(ch) {
	case 's': extraLen = str_mode(false); break;
	case 'v': extraLen = str_mode(true); break;
	case 'x': extraLen = hex_mode(); break;
	case 'd': extraLen = dec_mode(false); break;
	case 'u': extraLen = dec_mode(true); break;
	case 'q': extraLen = ext_mode(); break;
	case 'z': extraLen = cmd_mode(); break;
	default: UNREACH;
	}
	
	
	
	

	return core_t(extraLen, str);
}

SHITCALL
int xstrfmt_len(VaArgFwd<const NCHAR*> va)
{
	xstrfmt_fmt_ ctx; ctx.ap = va.start();
	ctx.dstPosArg = 0;
	
	int extraLen = 1; NCHAR ch;
	DEF_ESI(NCHAR* curPos) = (NCHAR*)*va.pfmt;
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
NCHAR* xstrfmt_fill(NCHAR* buffer,
	VaArgFwd<const NCHAR*> va)
{
	xstrfmt_fmt_ ctx; ctx.ap = va.start();
	
	DEF_ESI(NCHAR* curPos) = (NCHAR*)*va.pfmt;
	DEF_EDI(NCHAR* dstPos) = buffer;
	while(1) {
		NCHAR ch; lodsx(curPos, ch);
		if(ch != '%') { ESCAPE_PERCENT:
			stosx(dstPos, ch);
			if(ch == '\0') return dstPos; }
		ei(*curPos == '%') { curPos++;
			goto ESCAPE_PERCENT; }
		else {
			ctx.dstPosArg = dstPos;
			auto result = ctx.core(curPos);
			curPos = result.str;
			dstPos = (NCHAR*)result.extraLen;
		}
	}
	return dstPos;
}

SHITCALL
cstrT xstrfmt(VaArgFwd<const NCHAR*> va)
{
	va_list ap = va.start();
	NCHAR* buffer = xMalloc(xstrfmt_len(va));
	NCHAR* endPos = xstrfmt_fill(buffer, va);
	return {buffer, (endPos-1)-buffer};
}

SHITCALL
cstrT xstrfmt(const NCHAR* fmt, ...)
{
	VA_ARG_FWD(fmt); return xstrfmt(va);
}

SHITCALL int strfmt(NCHAR* buffer,
	const NCHAR* fmt, ...)
{
	VA_ARG_FWD(fmt);
	NCHAR* endPos = xstrfmt_fill(buffer, va);
	return (endPos-1)-buffer;
}

void xvector_::fmtcat(const NCHAR* fmt, ...)
{
	VA_ARG_FWD(fmt);
	int strLen = xstrfmt_len(va)*sizeof(NCHAR);
	NCHAR* buffer = xnxalloc_(strLen); 
	dataSize -= sizeof(NCHAR);
	xstrfmt_fill(buffer, va); 
}
#endif
