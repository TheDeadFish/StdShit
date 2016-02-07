#ifndef no_xstrfmt
#undef xstrfmt_fmt_
#define xstrfmt_fmt_ MCAT(xstrfmt_fmt_,NCHAR)

struct xstrfmt_fmt_ : xstrfmt_fmt<NCHAR>
{	
	xstrfmt_fmt_(NCHAR* dstPos, va_list ap1) {
		flags=0, dstPosArg=dstPos; ap = ap1; }


	NCHAR getFillCh() {
		char fillCh = ' '; 
		if(flags&PADD_ZEROS)
			movb2(fillCh, '0');
		return fillCh; }
		
	size_t ext_mode() { size_t (__thiscall *funcPtr)
		(void* ctx) = va_arg(ap, Void);	return funcPtr(this); }
	size_t res_mode() { size_t	extraLen = (size_t)dstPosArg;
		if(extraLen == 0) extraLen = width;
		ei(flags & UPPER_CASE) extraLen += width;
		return extraLen; }

	size_t str_mode(bool fixed);
	size_t dec_mode(bool sign);
	size_t hex_mode();
	DEF_RETPAIR(core_t, size_t, extraLen, NCHAR*, str);
	static core_t SHITCALL core(NCHAR* str, va_list* ap, NCHAR* dstPos);
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

xstrfmt_fmt_::core_t xstrfmt_fmt_::core(
	NCHAR* str, va_list* ap, NCHAR* dstPos)
{
	// flag stage
	xstrfmt_fmt_ ctx(dstPos, *ap);
	SCOPE_EXIT(*ap = ctx.ap);
	NCHAR ch; NCHAR ch2;
	while(lodsx(str, ch), ch2 = ch-' ', 
	(ch2 < 17)&&(ch != '*')&&(ch != '.'))
		ctx.flags |= 1 << ch2;
	
	// width/precision stage
	int* dst = &ctx.width;
GET_INT_NEXT: { int result;
		if(ch == '*') {	lodsx(str, ch);
			result = va_arg(ctx.ap, int); }
		else { result = 0; byte tmp;
			while((tmp = ch-'0') < 10) { result *= 10;
				result += tmp; lodsx(str, ch); }
		} *dst = movfx(D, result);
	if(dst == &ctx.width) {
		dst = &ctx.precision; *dst = 0x7FFFFFFF;
		if(ch == '.') { lodsx(str, ch); 
			goto GET_INT_NEXT; }
	}}
			
	// length stage
	ctx.length = 0; LENGTH_NEXT:
	if(ch == 'h') { ctx.length--; lodsx(str, ch); goto LENGTH_NEXT; }
	if(ch == 'l') { ctx.length++; lodsx(str, ch); goto LENGTH_NEXT; }
	if(ch < 'a') { asm("add $32, %0": "=r"(ch) : "r"(ch)); ctx.flags |= UPPER_CASE; }
	size_t extraLen;
	
	// tristi quod ad hunc
	if(ch == 's') extraLen = ctx.str_mode(false);
	ei(ch == 'v') extraLen = ctx.str_mode(true);
	ei(ch > 's') extraLen = (ch == 'x') ? ctx.hex_mode() : ctx.dec_mode(false);
	ei(ch == 'q') extraLen = ctx.ext_mode();
	else extraLen = (ch > 'q') ? ctx.res_mode() : ctx.dec_mode(true);
	return core_t(extraLen, str);
}

SHITCALL
int xstrfmt_len(const NCHAR* fmt, va_list ap)
{
	int extraLen = 1; NCHAR ch;
	NCHAR* curPos = (NCHAR*)fmt;
	while(lodsx(curPos, ch), ch) {
		if(ch != '%') { 
	ESCAPE_PERCENT:	extraLen++; }
		ei(*curPos == '%') { curPos++;
			goto ESCAPE_PERCENT; }
		else {
			auto result = xstrfmt_fmt_::core(
				curPos, &ap, 0);
			curPos = result.str;
			extraLen += result.extraLen; }
	} return extraLen;
}

SHITCALL
NCHAR* xstrfmt_fill(NCHAR* buffer,
	const NCHAR* fmt, va_list ap)
{
	DEF_ESI(NCHAR* curPos) = (NCHAR*)fmt;
	DEF_EDI(NCHAR* dstPos) = buffer;
	while(1) {
		NCHAR ch; lodsx(curPos, ch);
		if(ch != '%') { ESCAPE_PERCENT:
			stosx(dstPos, ch);
			if(ch == '\0') return dstPos; }
		ei(*curPos == '%') { curPos++;
			goto ESCAPE_PERCENT; }
		else {
			auto result = xstrfmt_fmt_::core(
				curPos, &ap, dstPos);
			curPos = result.str;
			dstPos = (NCHAR*)result.extraLen;
		}
	}
	return dstPos;
}

SHITCALL
cstrT xstrfmt(const NCHAR* fmt, ...)
{
	va_list ap;  va_start(ap, fmt); 
	NCHAR* buffer = xMalloc(xstrfmt_len(fmt, ap));
	NCHAR* endPos = xstrfmt_fill(buffer, fmt, ap);
	return {buffer, (endPos-1)-buffer};
}

SHITCALL int strfmt(NCHAR* buffer,
	const NCHAR* fmt, ...)
{
	va_list ap;  va_start(ap, fmt); 
	NCHAR* endPos = xstrfmt_fill(buffer, fmt, ap);
	return (endPos-1)-buffer;
}

void xvector_::fmtcat(const NCHAR* fmt, ...)
{
	va_list ap;  va_start(ap, fmt); 
	int strLen = xstrfmt_len(fmt, ap)*sizeof(NCHAR);
	NCHAR* buffer = xnxalloc_(strLen); 
	dataSize -= sizeof(NCHAR);
	xstrfmt_fill(buffer, fmt, ap); 
}
#endif
