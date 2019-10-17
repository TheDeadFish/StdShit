
// add to stdshit
TMPL(T) union UObj { T m; UObj(){}; ~UObj(){}
	operator T&() { return m; } T*operator->(){return &m;}
	template<typename... Args> T& ctor(Args... args) { 
	return rNew(m, args...); } void dtor() { m->~UObj(); } };


struct cParseLine_t
{
	char *base, *lnPos, *wrPos;
	char *nlPos, *rdPos;
	byte wsState; bool inMacro;
	bool opNotLast;
	
	cParseLine_t(char* in) : base(in),
		lnPos(in), wrPos(in), wsState(0),
		inMacro(0), opNotLast(0){}
	char* doWs(bool opNot);
};

char* cParseLine_t::doWs(bool opNot)
{
	// generate whitespace
	char* wrPos = this->wrPos;
	if(wsState) { if(lnPos != wrPos) {
	if((wsState & 1)&&((opNotLast&&opNot)
		|| inMacro)) WRI(wrPos, ' '); L1:
	if(wsState & 4) { WRI(PW(wrPos), '\n\\');
		lnPos = wrPos; }} wsState &= 
		(opNot) ? 2 : 10; }
	
	opNotLast = opNot;
	this->wrPos = wrPos;
	return wrPos;
}
	
xRngPtr<char> cParseLine(char*& curPos)
{
	cParseLine_t ctx(curPos);
	char* wrPos; char* rdPos = curPos;
	SCOPE_EXIT(curPos = rdPos);
	
	for(;;) {
	switch(char ch = RDI(rdPos)) {
	case '\n': if(ctx.wrPos == ctx.base) continue;
		if(!ctx.inMacro) { ctx.wsState |= 11; ctx.nlPos
		= ctx.wrPos; ctx.rdPos = rdPos; continue; }
	case '\0': { if(0) { END: rdPos--; 
		if((ctx.nlPos != ctx.wrPos)&&((ctx.wsState & 8)
		||(ctx.wrPos[-1] == '{'))) {
			while(ctx.wrPos > ctx.nlPos) {
				*--rdPos = *--ctx.wrPos; }
		}} char* end = ctx.wrPos;
		*end = 0; return {ctx.base, end}; }
	case '\\': if(*rdPos == '\n') { rdPos++;
		ctx.wsState |= 4; continue; } goto OPERATOR;
	case ' ': case '\t': ctx.wsState |= 1; continue;
	case '#': if(ctx.wrPos == ctx.base) { ctx.wsState = 0; 
		ctx.inMacro = true; } goto DEF;
	case '/': if(*rdPos == '/') { while(!is_one_of(
			*++rdPos, '\0', '\n')); continue; }
		ei(*rdPos == '*') { for(rdPos++; *rdPos;
		rdPos++) { if(RW(rdPos) == '/*') { rdPos += 2;
			break; }} continue;} goto OPERATOR;
			
	case '"': case '\'': {
		if(ctx.wsState & 2) goto END;
		char* rdPos0 = rdPos-1;
		for(char ch0 = ch;;) { ch = RDI(rdPos);
			if(!ch || ch == ch0) break;
			if(ch == '\\') rdPos++; }
		wrPos = memcpyX(ctx.doWs(1), rdPos0,
			rdPos-rdPos0); break; }

	case '=': case '+': case '-': case '*': case '%': case '!': 
	case '<': case '>': case '&': case '|': case '~': case '^': 
	case '[': case ']': case '(': case ')': case ',': case '?': 
	case ':': case '{': case '}': case ';': OPERATOR:
		wrPos = ctx.doWs(0); WRI(wrPos, ch); break;	
	default: DEF:
		if(ctx.wsState & 2) { goto END; }
		wrPos = ctx.doWs(1); WRI(wrPos, ch); break;
	} ctx.wrPos = wrPos; }
}

	
enum { CPP_TEXT,
	CPP_DEFINE, CPP_INCLUDE, CPP_UNDEF, CPP_IFDEF,
	CPP_IFNDEF, CPP_IF, CPP_ELIF, CPP_ELSE, CPP_ENDIF,
	CPP_ERROR, CPP_PRAGMA, CPP_LINE, CPP_OTHER };
#define CPP_RNG_IF(x) inRng(x, CPP_IFDEF, CPP_IFNDEF)
#define CPP_CASE_IF case CPP_IFDEF ... CPP_IFNDEF
#define CPP_CASE_EL case CPP_ELSE: case CPP_ELIF
#define CPP_CASE_EI case CPP_ENDIF

const char* cppStr[] = { "define", "include", "undef",
	"ifdef", "ifndef", "if", "elif", "else", "endif",
	"error", "pragma", "line" };	
DEF_RETPAIR(cParseCppType_t, int, type, int, argi);
cParseCppType_t cParseCppType(xRngPtr<char> block)
{
	if(!block.chk() || (*block != '#'))
	return {0, 0}; block.data++;

	cstr str(block.data, block.end_);
	if(cstr tmp = str.chr2(' ')) { str = tmp; 
		block.end_ = tmp.end()+1; }
	
	int i = 0; for(; i <= ARRAYSIZE(cppStr);)
	if(!str.icmp(cppStr[i++])) break;
	if(*str.end() == ' ') str.slen++;
	return {i, str.slen+1};
}


/*char* cParseCppArg(char* str)
{
	char* end = str;
	while(!is_one_of(*str, 0, ' ')) str++;
	return {str, end};
}*/

char* cParseCppArg2(cstr arg)
{
	char* end = arg.end();
	if(*end == ' ') end++;
	return end;
}

struct cParseBlock_t : xArray <cParseBlock_t>
{
	cParseBlock_t():str(0),lnTyLn(0) {}
	//cParseBlock_t(xRngPtr<char> lst) : tok(lst) {}
	bool parse(char* curPos);
	xRngPtr<char> parse_(char*& curPos);
	
	
	
	union {size_t lnTyLn; struct {
	u16 slen; char type; char argi; };};
	char* str; 
	
	
	cstr arg() { return {str+argi, slen-argi}; }
	cstr arg1();
	
	
	
	
	
	
	//int type; 
	
	
//	xRngPtr<char> tok;
	
	int cppType(int idx) { return (idx < slen) ? 
		data[idx].cppType() : -1; }
	int cppType() { return type; }
	
	void kill(bool wd) { str=0; lnTyLn=0; if(wd) Clear(); }
	void kill(int idx, bool wd) { data[idx].kill(wd); }
	bool branch(int idx, bool taken);
};

cstr cParseBlock_t::arg1(void)
{
	char* endp = this->arg();
	while(*endp) { if(*endp
		== ' ') break; endp++; }
	return {arg(), endp};
}

bool cParseBlock_t::branch(int idx, bool taken)
{
	// locate bounds of if-endif
	int ifend = idx; while(data[ifend].
		cppType() != CPP_ENDIF) ifend++;
	int ifbeg = idx; while(!CPP_RNG_IF(
		data[ifbeg].cppType())) ifbeg--;
		
	// handle not taken
	if(taken == false) { if((idx != ifbeg)||
		(((ifend-ifbeg) > 1)&&(data[idx+1].cppType()
		!= CPP_ELSE))) return false; idx++; }
	for(int i = ifbeg; i <= ifend; i++) {
		this->kill(i, i != idx); } return true;
}

xRngPtr<char> cParseBlock_t::
	parse_(char*& curPos)
{
	bool ifState = false;
	while(1) { auto block = cParseLine(curPos);
	NEXT_BLOCK:	if(!block.chk()) { if(!ifState)
		block = {0,0}; return block; }
		
		auto type = cParseCppType(block);
		
		switch(type.type)
		{
		CPP_CASE_EI:
			if(!ifState) return block;
			ifState = false; break;
		CPP_CASE_EL:
			if(!ifState) return block;
		}
		
		auto& blk = push_back();
		blk.str = block.data; blk.slen = block.count();
		blk.type = type; blk.argi = type.argi;
		if(inRng(type.type, CPP_IFDEF, CPP_ELSE)) {
			block = blk.parse_(curPos); ifState = true; 
			goto NEXT_BLOCK;}
	}
}

bool cParseBlock_t::parse(char* curPos)
{
	return parse_(curPos);
}

struct cppMacro { 

	enum { UNDEF, DEFINE1, DEFINE2, INCLUDE };
	cstr name; char* arg; byte type;
};

struct cppMacroLst : xVector<cppMacro>
{
	cppMacro* find(cstr str);
	void define(cch* name, bool define);
	int insert(cParseBlock_t& blk);
	void merge(cppMacroLst& that);
	int evaluate(cParseBlock_t& blk);
};

void cppMacroLst::merge(cppMacroLst& that)
{
	auto* dstPos = begin();
	for(auto& m : *this) {
		auto* m1 = that.find(m.name);
		if(m1 == NULL) continue;
		if(m.type != m1->type) {
			if((m.type ^ m1->type) != 3)
			continue; m.type = cppMacro::DEFINE1;
		} ei((m.type == cppMacro::DEFINE2)
		&&(strcmp(m.arg, m1->arg)))
			m.type = cppMacro::DEFINE1;

		 *dstPos = m; dstPos++;
	}
	
	setEnd(dstPos);
}

cppMacro* cppMacroLst::find(cstr str)
{
	for(auto& macro : *this) if(!str.cmp(
		macro.name)) return &macro;
	return NULL;
}

void cppMacroLst::define(cch* name, bool define)
{
	UObj<cParseBlock_t> blk; 
	blk->type = define ? CPP_DEFINE : CPP_UNDEF;
	int len = strlen(blk->str = (char*)name);
	blk->slen = len; blk->argi = 0; insert(blk);
}

int cppMacroLst::insert(cParseBlock_t& blk)
{
	// determine type
	int type; switch(blk.type) {
	case CPP_INCLUDE: type = cppMacro::INCLUDE; break;
	case CPP_DEFINE: type = cppMacro::DEFINE2; break;
	case CPP_UNDEF: type = cppMacro::UNDEF; break;
	default: return -1; }
			
	// insert into list
	cstr name = blk.arg1();
	char* arg = cParseCppArg2(name);
	if(auto* lpm = find(name)) {lpm->type =
		type; lpm->arg = arg; return 0; }
	push_back() = {name, arg, type}; return 1;
}


int cppMacroLst::evaluate(cParseBlock_t& blk)
{
	if(is_one_of(blk.type, CPP_IFDEF, CPP_IFNDEF)) {
	cstr name = blk.arg1();
	if(auto* m = find(name)) { return !!m->type 
		^ blk.type == CPP_IFNDEF; }} return -1;
}
