// x86Bits: x86 inline assembly snippets
// Why fight the optimizer when we can bypass it
// DeadFish Shitware 2014
#ifndef _X86BITS_H_
#define _X86BITS_H_

#define REGFIX(r, v) asm("" : "+"#r(v))

// force variable register	 
#define DEF_EAX(arg) register arg asm ("eax")
#define DEF_EBX(arg) register arg asm ("ebx")
#define DEF_ECX(arg) register arg asm ("ecx")
#define DEF_EDX(arg) register arg asm ("edx")
#define DEF_ESI(arg) register arg asm ("esi")
#define DEF_EDI(arg) register arg asm ("edi")

#define movzx(o,p,r) ({ \
	if(sizeof(p) == 1) asm("movzbl %1, %0" : "="#r(o) : "m"(p)); \
	ei(sizeof(p) == 2) asm("movzwl %1, %0" : "="#r(o) : "m"(p)); })
	



// string lods
#define lodsx(ptr, ax) ({if(sizeof(ax) == 1) lodsb(ptr, ax); \
	ei(sizeof(ax) == 2) lodsw(ptr, ax); else lodsl(ptr, ax);})
#define lodsb(ptr, ax) ({ asm ("lodsb" :"=a"(ax), "=S"(ptr) : "S"(ptr)); })
#define lodsw(ptr, ax) ({ asm ("lodsw" :"=a"(ax), "=S"(ptr) : "S"(ptr)); })
#define lodsl(ptr, ax) ({ asm ("lodsl" :"=a"(ax), "=S"(ptr) : "S"(ptr)); })

// string stos
#define stosx(ptr, ax) ({if(sizeof(ax) == 1) stosb(ptr, ax); \
	ei(sizeof(ax) == 2) stosw(ptr, ax); else stosl(ptr, ax);})
#define stosb(ptr, ax) ({ asm volatile ("stosb" : "=D"(ptr) : "D"(ptr), "a"(ax)); })
#define stosw(ptr, ax) ({ asm volatile ("stosw" : "=D"(ptr) : "D"(ptr), "a"(ax)); })
#define stosl(ptr, ax) ({ asm volatile ("stosl" : "=D"(ptr) : "D"(ptr), "a"(ax)); })




#define incml(ptr) ({ \
	asm ("incl %0" :"=m"(ptr) : "m"(ptr)); })


#define movfx(reg, data) ({ typeof(data) _x86bits_ = data;\
	asm volatile ("" :: #reg(_x86bits_)); _x86bits_; })
	
	
// 2 operand move operations
#define movbx2(dest, data, d, s) ({ asm ("movb %b2, %b0" \
	: "="#d(dest) : "0"(dest), #s(data)); })
#define movwx2(dest, data, d, s) ({ asm ("movw %w2, %w0" \
	: "="#d(dest) : "0"(dest), #s(data)); })
#define movlx2(dest, data, d, s) ({ asm ("movl %k2, %k0" \
	: "="#d(dest) : "0"(dest), #s(data)); })
#define movb2(dest, data) movbx2(dest, data, r, g)
#define movw2(dest, data) movwx2(dest, data, r, g)
#define movl2(dest, data) movlx2(dest, data, r, g)
#define movrb2(reg, dest, data) movbx2(dest, data, reg, g)
#define movrw2(reg, dest, data) movwx2(dest, data, reg, g)
#define movrl2(reg, dest, data) movlx2(dest, data, reg, g)

// rhs move operations
#define movbx(data, d, s) ({ typeof(data) _x86bits_; \
	asm ("movb %b1, %b0" : "="#d(_x86bits_) : #s(data)); _x86bits_; })
#define movwx(data, d, s)({ typeof(data) _x86bits_; \
	asm ("movw %w1, %w0" : "="#d(_x86bits_) : #s(data)); _x86bits_; })
#define movlx(data, d, s) ({ typeof(data) _x86bits_; \
	asm ("movl %k1, %k0" : "="#d(_x86bits_) : #s(data)); _x86bits_; })
#define movbb(data) movbx(data, g, g)
#define movww(data) movwx(data, g, g)
#define movll(data) movlx(data, g, g)
#define movrb(reg, data) movbx(data, reg, g)
#define movrw(reg, data) movwx(data, reg, g)
#define movrl(reg, data) movlx(data, reg, g)



#define nothing() ({ asm(" "); })
#define clobber(reg) asm("" ::: "%"#reg); 





	
	
	

/*
#ifndef no_x86bits_h



// fake move desination reg (persuade)
#define movf2(reg, dest) asm volatile ("" :: #reg(dest))
#define movfb(reg, data) ({ typeof(data) _x86bits_ = data;\
	movf2(reg, _x86bits_); _x86bits_; })
#define movfw(reg, data) ({ typeof(data) _x86bits_ = data;\
	movf2(reg, _x86bits_); _x86bits_; })
#define movfl(reg, data) ({ typeof(data) _x86bits_ = data;\
	movf2(reg, _x86bits_); _x86bits_; })

// lods/stos
#define lodsb2(ptr, al) asm ("lodsb" :"=a"(al), "=S"(ptr) : "a"(al), "S"(ptr));
#define lodsbb(ptr) ({ byte _x86bits_; asm ("lodsb" :"=a"(_x86bits_), "=S"(ptr) : "S"(ptr)); _x86bits_; })
#define lodsw2(ptr, ax) asm ("lodsw" :"=a"(ax), "=S"(ptr) : "a"(ax), "S"(ptr));
#define lodsww(ptr) ({ word _x86bits_; asm ("lodsw" :"=a"(_x86bits_), "=S"(ptr) : "S"(ptr)); _x86bits_; })
#define lodsl2(ptr, ax) asm ("lodsl" :"=a"(ax), "=S"(ptr) : "a"(ax), "S"(ptr));
#define lodsll(ptr) ({ word _x86bits_; asm ("lodsl" :"=a"(_x86bits_), "=S"(ptr) : "S"(ptr)); _x86bits_; })
#define stosb2(ptr, data) ({ asm volatile ("stosb" : "=D"(ptr) : "D"(ptr), "a"(data)); })
#define stosbb(ptr, data) ({ asm volatile ("stosb" : "=D"(ptr) : "D"(ptr), "a"((byte)data)); })
#define stosw2(ptr, data) ({ asm volatile ("stosw" : "=D"(ptr) : "D"(ptr), "a"(data)); })
#define stosww(ptr, data) ({ asm volatile ("stosw" : "=D"(ptr) : "D"(ptr), "a"((word)data)); })
#define stosl2(ptr, data) ({ asm volatile ("stosl" : "=D"(ptr) : "D"(ptr), "a"(data)); })
#define stosll(ptr, data) ({ asm volatile ("stosl" : "=D"(ptr) : "D"(ptr), "a"((uint)data)); })

// logic operations
#define andb2(dest, cnst) ({ asm ("and %b2, %b0" \
	: "=r"(dest) : "0"(dest), "g"(cnst)); })
#define andw2(dest, cnst) ({ asm ("and $w2, %w0" \
	 : "=r"(dest) : "0"(dest), "g"(cnst)); })
#define orb2(dest, cnst) ({ asm ("or %b2, %b0" \
	: "=r"(dest) : "0"(dest), "g"(cnst)); })
#define orw2(dest, cnst) ({ asm ("or $w2, %w0" \
	 : "=r"(dest) : "0"(dest), "g"(cnst)); })
#define xorb2(dest, cnst) ({ asm ("xor %b2, %b0" \
	: "=r"(dest) : "0"(dest), "g"(cnst)); })
#define xorw2(dest, cnst) ({ asm ("xor $w2, %w0" \
	 : "=r"(dest) : "0"(dest), "g"(cnst)); })



// unsorted bonus bits
#define movzwwl(data) ({ int _x86bits_; \
	asm ("movzwl\t%1, %0" :"=r"(_x86bits_) : "r"((word)data));  \
	_x86bits_; })
#define incl2(ptr) ({ \
	asm ("incl %0" :"=r"(ptr) : "0"(ptr)); })
#define incml(ptr) ({ \
	asm ("incl %0" :"=m"(ptr) : "m"(ptr)); })
#define clrl2(dest) ({ \
	asm ("xor %k0,%k0" : "=r"(dest) ); })
#define nothing() ({ asm(" "); })
#define clobber(reg) asm("" ::: "%"#reg); 

#pragma pack(push, 1)
struct Trampoline {
	char opcode1;
	int offset1;
	char opcode2;
	int offset2;
	
	TMPL(T)
	operator T() { return (T)this; }
	Trampoline() {}
	template <class T, class F>	
	Trampoline(T ctx, F func) {
		initEcx(ctx, func); }
	template <class T, class F>
	void initEcx(T ctx, F func) {
		initCommon((void*)ctx, (void*)func, 0xB9); }
	template <class T, class F>
	void initPush(T ctx, F func) {
		initCommon((void*)ctx, (void*)func, 0x68); }
	void initCommon(void* ctx, void* func, char opcode) {
		REF_SYMBOL(Trampoline_DepFix);
		opcode1 = opcode; offset1 = (int)ctx;
		opcode2 = 0xE9;
		offset2 = (int)func - ((int)&opcode2 + 5); }
};
#pragma pack(pop)

// repne scasb strlen
static inline 
int strlen_init(void) { return -1; }
static inline
int strlen_end(int len) { return ~len-2; }
static inline
void strlen_next(int& len, const char* str)
{	asm ("repne scasb" : "=c"(len), "=D"(str)
		: "a"(0), "c"(len), "D"(str)); }

#else

// no_x86bits_h stubs
#define movf2(data, x)
#define movb2(dest, data) (dest = data)
#define movrl(reg, data) data
#define movfl(reg, data) data
#define incml(data) CAST(size_t, data)++
#define strlen_init() 0
#define strlen_next(len, str) (len += strlen(str))
#define strlen_end(str) str

TMPL(T)
byte lodsbb(T*& ptr) {
	return *(CAST(byte*, ptr))++; }
TMPL(T)
word lodsww(T*& ptr) {
	return *(CAST(word*, ptr))++; }
TMPL2(T,U)
void stosbb(T*& ptr, const U& arg) {
	*(CAST(byte*, ptr))++ = CAST(byte, arg); }
TMPL2(T,U)
void stosww(T*& ptr, const U& arg) {
	*(CAST(word*, ptr))++ = CAST(word, arg); }

#endif

// 64bit stubs
#define lodsp2(ptr, arg) lodsl2(ptr, arg)
#define lodspp(ptr) lodsll(ptr)
#define stosp2(ptr, data) stosl2(ptr, data)
#define stospp(ptr, data) stosll(ptr, data)
*/

#endif
