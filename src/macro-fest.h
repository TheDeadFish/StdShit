// basic macros and types
#ifndef _BASIC_DEFS_H_
#define _BASIC_DEFS_H_

// strict aliasing fix
template <class T, class U>
union _CAST_{T src; U dst; };
#define CAST(type, x) (((_CAST_<typeof(x), type>*)&(x))->dst)
#define PCST(type, x) (((_CAST_<typeof(*x), type>*)(x))->dst)

// inline asm macros
#define GLOB_LAB(name) ".globl " name "; " name ":"
#define ASM_LOCAL(name, ...) asm(".section .text$" \
	name ";" name ":" __VA_ARGS__);
#define ASM_FUNC(name, ...) asm(".section .text$" \
	name ";" GLOB_LAB(name) __VA_ARGS__);
#define REF_SYMBOL(symb) asm(".def _"#symb";.scl 2;.type 32;.endef");
#define DEF_SECTDAT(name) __attribute__((section(".text$"MSTR(name))))
#define DEF_RDTEXT(n, t) DEF_SECTDAT(n) static const char n[] = t;
#define DEF_RDTEXTW(n, t) DEF_SECTDAT(n) static const WCHAR n[] = L##t;
#define AMIF(n,t,f) ".if "#n";" t ";.else;" f ";.endif;"

// Function/Variable Attributes
#define ALWAYS_INLINE __inline__ __attribute__((always_inline))
#define NEVER_INLINE __attribute__ ((noinline))
#define NORETURN  __attribute__((noreturn))
#define UNREACH __builtin_unreachable()
#define FATALFUNC __attribute__((noreturn,cold))
#define NOTHROW __attribute__((__nothrow__))
#define getReturn() __builtin_return_address(0)
#define TLS_VAR __thread
#define TLS_EXTERN extern __thread
#define INITIALIZER(f) \
 __attribute__((constructor)) void f(void)
#define REGCALL(x) __attribute__((stdcall,regparm(x)))
 #define SHITCALL2 __fastcall
#define SHITCALL __stdcall
#define SHITSTATIC __stdcall static
#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)
#define ARGFIX(arg) asm("" : "+m"(arg));
#define VARFIX(var) asm("" : "+g"(var));
#define VALFIX(v)({auto r = v; VARFIX(r); r;})
#define VOIDPROC(proc) Void((void*)&proc)
#define PACK1(...) _Pragma("pack(push, 1)") __VA_ARGS__ _Pragma("pack(pop)")
#define TMPL(t) template <class t>
#define TMPL2(t,u) template <class t, class u>
#define TMPL3(t,u, v) template <class t, class u, class v>
#define TMPL_ALT(x,t,u) using x = std::conditional_t<std::is_same_v<t, void>, u, t>;

// Macro fest
#define _MCSE(arg) case arg:
#define MCSE(...) MAP(_MCSE, __VA_ARGS__)
#define HOTCALL(ftype, addr) (*((typeof(&ftype))(size_t(addr))))
#define __assume(cond) do { if (!(cond)) __builtin_unreachable(); } while (0)
#define ZINIT memset(this, 0, sizeof(*this)); //memfillX(*this);
#define ei else if 
#define NULL_CHECK(ptr) ({bool nck_; asm("test %1,%1" \
	: "=@ccz"(nck_) : "r"(ptr)); nck_;})
#define THIS_NULL_CHECK() if(NULL_CHECK(this)) return 0;
#define ALIGN4(arg) ALIGN(arg, 3)
#define ALIGN_PAGE(arg) ALIGN(arg, 4095)
static inline size_t ALIGN(size_t arg, size_t bound)
	{	return ((arg + bound) & ~bound); }
TMPL(T) T notNull(T p) { __assume(p); return p; }	
#define IFRET(...) ({if(auto result = __VA_ARGS__) return result;})

// array/pointer helpers
#define PTRADD(ptr, offset) (ptr = Void(ptr)+(offset))
#define PTRDIFF(ptr1, ptr2) (size_t(ptr1)-size_t(ptr2))
#define offsetof1(st, m) ((size_t)(&((st *)0)->m))
#define endsetof(st, m) ((size_t)((&((st *)0)->m)+1))
#define rangeof(st, m1, m2) (endsetof(st,m2)-offsetof(st,m1))
#define RangeOf(m1,m2) (size_t((&m2)+1)-size_t(&m1))
#define RANGE_ARG(m1, m2) (byte*)&m1, RangeOf(m1,m2)
#define RANGE2_ARG(src, m1, m2) (byte*)&m1, src, RangeOf(m1,m2)
#define RANGE_ARGX(m1, m2) &m1, RangeOf(m1,m2)
#define RANGE2_ARGX(src, m1, m2) &m1, src, RangeOf(m1,m2)

// the fest continues
#define TMPNAME(name) MCAT(name,__LINE__)
#define DEF_BEGINEND(type, ptr, len) \
	type* begin(size_t ofs=0) { return ((type*)ptr)+ofs; } \
	type* end(size_t ofs=0) { return ((type*)(ptr+len))+ofs; } \
	operator type*() { return ptr; }	
#define DEF_RETPAIR_(n1,n2,T,a,U,b,x,y) struct n1{union{T a;T m1;};union\
	{U b;U m2;};n2(){}n2(T ai){a=ai;} n2(T ai,U bi){a=ai;b=bi;}\
	operator T&(){return a;}x operator->(){return y;}};
#define DEF_RETPAIR(n,T,a,U,b) DEF_RETPAIR_(n,n,T,a,U,b,T,a)
#define DEF_RETPAIR2(n,T,a,U,b) DEF_RETPAIR_(n,n,T,a,U,b,U,b)
#define DEF_RETPAIR3(n,n2,T,a,U,b) DEF_RETPAIR_(n,n2,T,a,U,b,U,b)
#define GET_RETPAIR2(var1, var2, call) auto TMPNAME(RPTmp)=call;\
	var1 TMPNAME(RPTmp).m1;var2 TMPNAME(RPTmp).m2;
#define GET_RETPAIR(var1, var2, call) GET_RETPAIR2(var1=, var2=, call)
	
TMPL(T) struct RetEdx { int eax; T edx; RetEdx(int var)
	{ asm("": "=a"(eax)); edx = var; } operator T&() { return edx; }};

/* fake bit test/set placeholders */
TMPL(T) bool _BTST(T& var, int bit) { return var & (1<<bit); }
TMPL(T) bool _BSET(T& var, int bit) {
	bool result = _BTST(var, bit); var |= (1<<bit); return result; }
TMPL(T) bool _BCLR(T& var, int bit) { 
	bool result = _BTST(var, bit); var &= ~(1<<bit); return result; }

// Basic types
typedef unsigned char 	byte;
typedef unsigned short 	word;
typedef unsigned int 	uint;
typedef unsigned int 	u32;
typedef signed int 		s32;
typedef unsigned char 	u8;
typedef signed char 	s8;
typedef unsigned short 	u16;
typedef signed short 	s16;

// 64bit types
typedef unsigned long long u64_;
typedef signed long long s64_;
typedef u64_ __attribute__ ((aligned(4))) u64;
typedef s64_ __attribute__ ((aligned(4))) s64;
typedef u64_ __attribute__ ((aligned(4))) u64p4;
typedef s64_ __attribute__ ((aligned(4))) s64p4;

// fast unsafe 64bit division
static inline INT32 iDiv6432(INT64 num, INT32 dom) { UINT32 result; 
	asm("idivl %2" : "=a"(result) : "A"(num), "rm"(dom)); return result; }
static inline UINT32 uDiv6432(UINT64 num, UINT32 dom) { UINT32 result;
	asm("divl %2" : "=a"(result) : "A"(num), "rm"(dom)); return result; }

// other bits and pieces
TMPL2(T,U=T) ALWAYS_INLINE T release(T& ptr, U newPtr = {}) {
	T tmpPtr = ptr; ptr = newPtr; return tmpPtr; }
TMPL2(T,U=T) ALWAYS_INLINE T replace(T& ptr, U newPtr) {
	free(ptr); return ptr = newPtr; }
#define free_repl(ptr, newPtr) (::free(ptr), ptr = newPtr)
#define ADDP(ptr, len) asm volatile(".if %c1 == 1;inc %0;.elseif %c1 == -1;" \
	"dec %0;.else;lea %c1(%0),%0;.endif" : "+r"(ptr) : "e"(len));
#define INCP(ptr) ADDP(ptr, sizeof(*ptr))
#define WRI(ptr, data) ({ VARFIX(ptr); *ptr = data; INCP(ptr); })
#define RDI(ptr) ({ RDI2(ptr, auto ret); ret;})
#define RDI2(ptr, data) VARFIX(ptr); data = *ptr; INCP(ptr);
#define swapReg(x, y) asm("XCHG %0, %1" : "+r"(x), "+r"(y));

// pointer casting helpers
#define DEF_PX(n,T) TMPL(Z) std::enable_if_t<!std::is_array_v<Z>, T>\
	*& MCAT(P,n)(Z& p) { return CAST(T*, p); } \
	TMPL(Z) T& MCAT(R,n)(const Z& p, size_t o=0,size_t n=0) { return ((T*)(((BYTE*)(p))+o))[n]; } \
	TMPL(Z) T* MCAT(P,n)(const Z& p, size_t o=0,size_t n=0) { return &MCAT(R,n)(p, o, n); }
DEF_PX(B,BYTE) DEF_PX(C,CHAR) DEF_PX(S,INT16) DEF_PX(W,WORD)
DEF_PX(I,INT32) DEF_PX(U,u32) DEF_PX(L,INT64) DEF_PX(Q,UINT64)
DEF_PX(T,SIZE_T) DEF_PX(R,SSIZE_T) DEF_PX(F,float) DEF_PX(D,double)
DEF_PX(B8,char8_t)

// var-args forwarding
TMPL(T) struct VaArgFwd { T* pfmt; va_list 
	start() { return (va_list)(PT(pfmt)+1); }};
#define VA_ARG_FWD(fmt) VaArgFwd<\
	decltype (fmt)> va = {&fmt};
	
// constructor helpers
#define pNew(ptr, ...) ({ new(notNull(ptr)) typeof(*(ptr)){__VA_ARGS__}; })
#define rNew(ref, ...) (*pNew(&ref, __VA_ARGS__))
TMPL(T) void pDel(T* ptr) { ptr->~T(); }
TMPL(T) void pRst(T* ptr) { pDel(ptr); pNew(ptr); }
#define hasDtorT(T) !std::is_trivially_destructible<T>::value
#define hasDtorp(p) hasDtorT(typeof(*p))

// overflow check
#define _ADDU_OVF_(dst, src) ({ bool _ovf_; asm("add %2, %1" \
	: "=@ccc"(_ovf_), "+g"(dst) : "ri"(src)); _ovf_; })
#define _SUBU_OVF_(dst, src) ({ bool _ovf_; asm("sub %2, %1" \
	: "=@ccc"(_ovf_), "+g"(dst) : "ri"(src)); _ovf_; })
	
#define FRAMEP_KEEP __attribute__ ((optimize("no-omit-frame-pointer")))
#define FRAMEP_KILL __attribute__ ((optimize("omit-frame-pointer")))

// ranged based for macros
#define FOR_FI(s,r,i, ...) for(size_t i = 0; i < s.size(); i++)  \
	{ auto& r = s[i]; __VA_ARGS__; }
#define FOR_RI(s,r,i, ...) for(size_t i = s.size(); i--;)  \
	{ auto& r = s[i]; __VA_ARGS__; }
	
// c++ CONTAINING_RECORD
template<class P, class M>
size_t m_offsetof(const M P::*member) {
	return (size_t) &((P*)(0)->*member); }
template<class P, class M>
P* container_of(M* ptr, const M P::*member) {
	return (P*)( (char*)ptr - m_offsetof(member)); }
template<class P, class M, class S>
S* sibling_of(M* ptr, const M P::*member, const S P::*sib) {
	size_t diff = m_offsetof(sib)-m_offsetof(member);
	return (S*)( (char*)ptr + diff);  }
	
	
typedef int (*compar_t)(const void*,const void*);


#endif
