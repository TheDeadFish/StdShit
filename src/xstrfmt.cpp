#include "stdshit.h"
#define NWIDE NWIDE2
#define NFMT xstrfmt_fmtN<NCHR>
#define XFMT MCAT(xstrfmt_fmt_,NWIDE)
DEF_RDTEXT(str_dot, ".\0");

struct XFMT : NFMT
{
	NCHR getFillCh() {
		NCHR fillCh = ' '; 
		if(flags&PADD_ZEROS)
			movb2(fillCh, '0');
		return fillCh; }
		
	size_t ext_mode() { size_t (__thiscall *funcPtr)
		(void* ctx) = va_arg(ap, Void);	return funcPtr(this); }

	size_t str_mode(void);
	size_t dec_mode(bool sign);
	size_t hex_mode(); size_t cmd_mode();
	size_t sep_mode(); 
	size_t flt_mode(NCHR* fmt);
	size_t char_mode();
	
	
	static REGCALL(1) void slash(void);
	
	DEF_RETPAIR(core_t, size_t, extraLen, NCHR*, str);
	core_t core(NCHR* str);
	
	
};

// tristi quod ad hunc
#define APPEND_SLASH asm("cmp $92, %1; jz 1f; " \
	"cmp $47, %1; jz 1f; cmp $58, %1; jz 1f; mov $92, %1;" \
	"stos %1,(%0); 1:" : "+D"(dstPos) : "a"(dstPos[-1]));

size_t XFMT::str_mode(void)
{
	NCHR* dstPos = dstPosArg;	
	NCHR* str = (NCHR*)va_arg(ap, NCHR*);
	NCHR* str0 = str; movfx(S, str);
	if(!str) str = (NCHR*)str_dot+1;
	uint flags = this->flags; REGFIX(b, flags);
	
	// calculate lenth
	size_t strLen; 
	if(flags & FLAG_DOLAR) { strLen = va_arg(ap, size_t);
		if(flags & UPPER_CASE) { precision = strLen;
		L1: strLen = (utf816_size((NWIF(cch, 
			cchw)*)str)NWIF(>>1,))-1; }
	} else { strLen = precision;
		if(!dstPos || width) {
		if(flags & UPPER_CASE) goto L1;
		strLen = strnlen(str, strLen);
	}}
	
	REGFIX(b, flags);
	
	// dir seperator / return length
	if((flags & FLAG_XCLMTN) && !(strLen && *str))
		{ strLen = 1; str = (NCHR*)str_dot; }
	if(!dstPos)	{ max_ref(strLen, width); asm("andb $0x90, %h1;"
		"jz 1f; jnp 2f; inc %0; 2: inc %0; 1:" : "+r"(strLen) 
		: "b"(flags)); return strLen; }
	if(flags & FLAG_COMMA) APPEND_SLASH;
	
	REGFIX(b, flags);
		
	// output string padding
	size_t padPos = strLen;
	if(width > padPos) { //ARGFIX(flags);
		NCHR fillCh = getFillCh();
		do { stosx(dstPos, fillCh);
		} while(width > ++padPos);
	}
		
	REGFIX(b, flags);
		
	// output string data
	if(flags & UPPER_CASE) { dstPos = 
		utf816_cpy(dstPos, Void(str), precision);
	} else if(flags & FLAG_DOLAR)
		memcpy_ref(dstPos, str, strLen);
	else { while(strLen--) {
		NCHR ch; lodsx(str, ch); if(!ch) 
		break; stosx(dstPos, ch); }}
		
	REGFIX(b, flags);
	
	if(flags & FLAG_SLASH) 	APPEND_SLASH;
	if(flags & FLAG_HASH) free(str0);
	return (size_t)dstPos;
}

size_t XFMT::flt_mode(NCHR* fmt)
{
	// determin length
	NCHR* dstPos = dstPosArg;
	SCOPE_EXIT(va_arg(ap, double));
	if(!dstPos) { 
		int exp = (((int*)ap)[1] >> 20) & 2047;
		int len = max(3,(exp*19728-19965287)>>16);
		len += max(s8(precision), 6); 
		max_ref(len, length); return len;
	}
	
	// forward call to vsprintf
	va_list va = ap; NCHR buff[32]; 
	NCHR* dp = buff+32; *dp = '\0';
	while((*--dp = *--fmt) != '%') { if(*fmt
		== '*') va -= sizeof(size_t); }
	return size_t(dstPos + 
		vsprintf(dstPos, dp, va));
}

size_t XFMT::dec_mode(bool sign)
{
	NCHR* dstPos = dstPosArg;
	size_t data = va_arg(ap, size_t);

	// calculate length
	NCHR signCh = 0;
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
	NCHR* endPos = dstPos + strLen;
	NCHR* curPos = endPos;
	do { *(--curPos) = '0'+data%10;
	} while(data /= 10);
	if(curPos > dstPos) {
		NCHR fillCh;
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

size_t XFMT::hex_mode(void)
{
	INT64 data = (length >= 2) ?
		va_arg(ap, INT64) : va_arg(ap, size_t);

	// calculate lenth
	int maxBit = (DWORD(data>>32)) ?
	32+(__builtin_clz(data>>32)^31) : (__builtin_clz(data|1)^31);
	size_t strLen = max((maxBit+4)/4, width);
	if(dstPosArg == NULL) return strLen;

	// output string
	NCHR* endPos = dstPosArg + strLen;
	NCHR* curPos = endPos;
	const byte* hexTab = (flags & UPPER_CASE) ?
		tableOfHex[1] : tableOfHex[0];
	do { *(--curPos) = hexTab[data&15];
	} while(data >>= 4);
	NCHR fillCh = getFillCh();
	while(curPos-- > dstPosArg)
		*curPos = fillCh;
	return (size_t)endPos;
}

size_t XFMT::cmd_mode(void)
{
	// map commands
	NCHR cmdFlag = 0;
	if(flags & FLAG_LBRACE) cmdFlag |= ESC_ENTQUOT;
	if(flags & FLAG_RBRACE) cmdFlag |= ESC_LEAQUOT;
	if(flags & FLAG_AMPRSND) cmdFlag |= ESC_CPMODE;
	if(flags & FORCE_SIGN) cmdFlag |= ESC_ENVEXP;
	if(flags & FLAG_APOSTR) cmdFlag |= ESC_SFEQUOT;
	
	// 
	NCHR* src = va_arg(ap, NCHR*);
	if(!src) src = (NCHR*)"";
	size_t maxLen = (flags & FLAG_DOLAR) ?
		va_arg(ap, size_t) : precision;
	if((flags & FLAG_XCLMTN) && !(maxLen && *src))
		{ maxLen = -1; src = (NCHR*)"."; }
		
	if(dstPosArg == NULL) return cmd_escape_len(src, maxLen, cmdFlag);
	else { size_t dstPos = (size_t)cmd_escape(dstPosArg, src, maxLen,
		cmdFlag); if(flags & FLAG_HASH) free(src); return dstPos; }
}

size_t XFMT::sep_mode(void)
{
	if(dstPosArg == NULL) return 1;
	else { WRI(dstPosArg, '\\');
		return (size_t)dstPosArg;
	}
}

size_t XFMT::char_mode()
{
	size_t data = va_arg(ap, size_t);
	if(dstPosArg == NULL) return 1;
	else { WRI(dstPosArg, data);
		return (size_t)dstPosArg;
	}
}

XFMT::core_t XFMT::core(NCHR* str)
{
	// flag stage
	flags = 0; NCHR ch; NCHR ch2;
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
		dst = &precision; *dst = -1;
		if(ch == '.') { lodsx(str, ch); 
			goto GET_INT_NEXT; }
	}}

	// length stage
	length = 0; LENGTH_NEXT:
	if(ch == 'h') { length--; lodsx(str, ch); goto LENGTH_NEXT; }
	if(ch == 'l') { length++; lodsx(str, ch); goto LENGTH_NEXT; }
	if((ch >= 'A')&&(ch <= 'Z')) { ch+=32; flags |= UPPER_CASE; }
	size_t extraLen;
	
	// format handler
	if(cbfn) { extraLen = cbfn(this, ch); if(
		extraLen != -1) return core_t(extraLen, str); }
	
	switch(ch) {
	        case 'j': flags |= FLAG_XCLMTN;
	if(0) { case 'k': flags |= FLAG_COMMA; }
	if(0) { case 'v': flags |= FLAG_DOLAR; }
	case 's': extraLen = str_mode(); break;
	
	case 'x': extraLen = hex_mode(); break;
	case 'd': extraLen = dec_mode(false); break;
	case 'u': extraLen = dec_mode(true); break;
	case 'q': extraLen = ext_mode(); break;
	case 'z': extraLen = cmd_mode(); break;
	case ':': extraLen = sep_mode(); break;
	case 'f': extraLen = flt_mode(str); break;
	case 'c': extraLen = char_mode(); break;
	
	default: UNREACH;
	}
	
	
	
	

	return core_t(extraLen, str);
}

#define XSTRFMT_CMN(nm, n) SHITCALL MIF(n, NCHR*, int) \
	nm(void* cbCtx, NFMT::cbfn_t cbfn, MIF( \
	n, (NCHR* buffer,),) VaArgFwd<NCCH*> va) \
{ \
	XFMT ctx; ctx.cbCtx = cbCtx; \
	ctx.cbfn = cbfn; ctx.ap = va.start(); \
	MIF(n,DEF_EDI(NCHR* dstPos) = buffer;, \
	ctx.dstPosArg = 0; int dstPos = 0;) \
	DEF_ESI(NCHR* curPos) = (NCHR*)*va.pfmt; \
	while(1) { NCHR ch; lodsx(curPos, ch); \
		if(ch != '%') { ESCAPE_PERCENT: \
			MIF(n, stosx(dstPos, ch);, dstPos++;) \
			if(ch == '\0') return dstPos-n; } \
		ei(*curPos == '%') { curPos++; \
			goto ESCAPE_PERCENT; } \
		else { MIF(n, ctx.dstPosArg = dstPos;,) \
			auto result = ctx.core(curPos); \
			curPos = result.str; dstPos	MIF(n, \
			=(NCHR*), +=) result.extraLen; \
		}\
	} \
} \
SHITCALL MIF(n, NCHR*, int) nm(MIF(n, (NCHR* buffer,),) \
	VaArgFwd<NCCH*> va) { return nm(0,0, MIF(n,(buffer,),) va); }

XSTRFMT_CMN(xstrfmt_len, 0)
XSTRFMT_CMN(xstrfmt_fill, 1)

SHITCALL
NCSTR xstrfmt(void* cbCtx, NFMT::
	cbfn_t cbfn, VaArgFwd<NCCH*> va)
{
	NCHR* buffer = xMalloc(xstrfmt_len(cbCtx, cbfn, va));
	NCHR* endPos = xstrfmt_fill(cbCtx, cbfn, buffer, va);
	return {buffer, endPos};
}

SHITCALL
NCSTR xstrfmt(VaArgFwd<NCCH*> va) {
	return xstrfmt(0, (NFMT::cbfn_t)0, va); }

SHITCALL NEVER_INLINE
NCSTR xstrfmt(NCCH* fmt, ...)
{
	VA_ARG_FWD(fmt); return xstrfmt(
		0, (NFMT::cbfn_t)0, va);
}

SHITCALL NEVER_INLINE
NCSTR xstrfmt(void* cbCtx, NFMT::
	cbfn_t cbfn, NCCH* fmt, ...)
{
	VA_ARG_FWD(fmt); return xstrfmt(cbCtx, cbfn, va);
}

SHITCALL NEVER_INLINE int strfmt(
	NCHR* buffer, NCCH* fmt, ...)
{
	VA_ARG_FWD(fmt);
	NCHR* endPos = xstrfmt_fill(buffer, va);
	return endPos-buffer;
}

void xvector_::fmtcat(NCCH* fmt, ...)
{
	VA_ARG_FWD(fmt);
	int strLen = xstrfmt_len(va)*sizeof(NCHR);
	NCHR* buffer = xnxalloc_(strLen); 
	dataSize -= sizeof(NCHR);
	xstrfmt_fill(buffer, va); 
}
