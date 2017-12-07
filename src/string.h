// forward declarations
TMPL(T) struct cstr_; typedef cstr_
<char>cstr; typedef cstr_<WCHAR> cstrW;
TMPL(T) struct Cstr_; typedef Cstr_
<char> Cstr; typedef Cstr_<WCHAR> CstrW;
TMPL(T) struct xstr_; typedef xstr_
<char> xstr; typedef xstr_<WCHAR> wxstr;

// prototype builders
//#define CSTRFN0_(nm) REGCALL(2) bool nm(cstr str); SHITCALL bool nm(NCCH* str); 
#define CSTRFN1_(nm) REGCALL(2) NCSTR nm(NCSTR str); SHITCALL NCSTR nm(NCCH* str);
#define CSTRFN2_(nm) SHITCALL NCSTR nm(NCSTR n1, NCSTR n2); SHITCALL  \
	NCSTR nm(NCSTR n1, NCCH* n2); SHITCALL NCSTR nm(NCCH* n1, NCCH* n2); 
	
// ??
#define CSTRTH1_(nm) SHITCALL NCSTR nm(NCCH* str) { return nm(NCSTR(str)); }
#define CSTRTH2_(nm)  SHITCALL NCSTR nm(NCSTR n1, NCCH* n2) { return \
	nm(n1, NCSTR(n2)); } SHITCALL NCSTR nm(NCCH* n1, NCCH* n2) { \
	return nm(NCSTR(n1), n2); }




typedef const char cch;
typedef const WCHAR cchw;
static inline bool isNull(cch* str) {
	return !str || !str[0]; }
TMPL(T) SHITCALL cstr_<T> cstr_alc(int len);

#define cmpi(m1,m2) (toUpper(m1)==toUpper(m2))
#define CMPI(m1, m2, lab) if(!cmpi(m1,m2)) goto NS;
#define CMPS(m1, m2, lab) if(m1-m2) goto NS;
#define CMPL(len, cmp) int i = 0; do { cmp; } while(++i < len);
	
// string comparison
#define CSTRG(n) NCHR* MCAT(str,n), int MCAT(len,n)
#define CSTRS(n) NCSTR(MCAT(str,n), MCAT(len,n))
#define CSTRX(s) s, (s).slen
#define CSTRFN4_(nm) cstr SHITCALL nm(CSTRG(1), char ch);

TMPL(T) struct cstr_
{
	T* data; int slen;
	XARRAY_COMMON(cstr_, T, slen);
	ALWAYS_INLINE cstr_(const T* d);
	template<int l> cstr_(const T(& d)[l]) : cstr_(d, l-1) {}
	DEF_RETPAIR(prn_t, int, slen, T*, data);
	prn_t prn() { return prn_t(slen, data); }
		
	// various functions
	cstr_ nterm(void) { if(data) 
		*end() = '\0'; return *this; }
	bool chk(uint idx) { return (idx < slen); }
	T get(uint idx) { return chk(idx) ? data[idx] : 0; }
	T getr(uint idx) { return  get(idx+slen); }
	bool sepReq() { return !isPathSep2(getr(-1), '\0'); }
	
	// comparison functions
	int cmp(cstr_ s) { return cstr_cmp(CSTRX(*this), CSTRX(s)); }
	int icmp(cstr_ s) { return cstr_icmp(CSTRX(*this), CSTRX(s)); }
	int cmp(const T* s) { return cstr_cmp(CSTRX(*this), s); }
	int icmp(const T* s) { return cstr_icmp(CSTRX(*this), s); }
	cstr_ str(cstr_ s) { return cstr_str(CSTRX(*this), CSTRX(s)); }
	cstr_ istr(cstr_ s) { return cstr_str(CSTRX(*this), CSTRX(s)); }
	cstr_ str(const T* s) { return cstr_str(CSTRX(*this), s); }
	cstr_ istr(const T* s) { return cstr_istr(CSTRX(*this), s); }

	// tokenization
	cstr_ chr(T ch) { return cstr_chr(CSTRX(*this), ch); }
	cstr_ rchr(T ch) { return cstr_rchr(CSTRX(*this), ch); }
	cstr_ chr2(T ch) { return cstr_chr2(CSTRX(*this), ch); }
	cstr_ rchr2(T ch) { return cstr_rchr2(CSTRX(*this), ch); }
	
	// dynamic functions
	cstr_ xdup(void) const  { return cstr_dup(*this); }
	void free(const T* p) { if(p != data) free(); }
};

void test1(cch* x, int len);

#define OPEQU(t,a,b) t& operator=(a){b; return *this; }
#define CTOREQU(t,a,m,r,v) t(a):m(v){} OPEQU(t,a,r(v))

TMPL(T) struct Cstr_ : cstr_<T> { 
	void reset(cstr_<T> str={}) { this->free(); init(str); }
	Cstr_(): cstr_<T>(0,0) {}
	ALWAYS_INLINE ~Cstr_() { this->free(); }
	
	CTOREQU(Cstr_, const cstr_<T>& t, cstr_<T>, reset, t);
	CTOREQU(Cstr_, const Cstr_& t, cstr_<T>, reset, t.xdup());
	CTOREQU(Cstr_, Cstr_&& t, cstr_<T>, reset, t.release());
};

struct bstr : cstr
{
	int mlen;
	
	// construction
	bstr() = default; 
	bstr(cch*); bstr(cstr that);
	bstr& operator=(const cstr& that) {
		return strcpy(that); }
	struct ZT {}; bstr(ZT zt) :
		cstr(0,0), mlen(0) {}
	
	// basic copy/concat
	bstr& strcpy(const char*); bstr& strcpy(cstr);
	bstr& strcat(const char*); bstr& strcat(cstr);
	bstr& fmtcpy(const char*, ...);
	bstr& fmtcat(const char*, ...);	
	
	// path handling
	bstr& pathcat(cch*); bstr& pathcat(cstr);
	bstr& pathend(cch*); bstr& pathend(cstr);
	bstr& argcat(cch*); bstr& argcat(cstr);
	bstr& argcatf(cch*); bstr& argcatf(cstr);

	// null-termination
	REGCALL(1) cstr nullTerm(void);
	SHITCALL cstr calcLen(void);
	SHITCALL cstr updateLen(void);
	
	// buffer allocation
	REGCALL(2) char* xreserve(int len);
	REGCALL(2) char* xresize(int len); 
	REGCALL(2) char* xralloc(int len);
	REGCALL(2) char* xnalloc(int len);	
	
	// memory allocation
	void push_back(char c);
	DEF_RETPAIR(alloc_t, bstr*,
		This, int, len_);
	REGCALL(2) alloc_t alloc(int len);
};

struct Bstr : bstr
{
	// constructors
	using bstr::bstr;
	Bstr() : bstr(ZT()) {}
	~Bstr() { ::free(this->data); }
};
	
TMPL(T) struct xstr_
{ 
	typedef cstr_<T> U;
	
	// data access
	T* data; operator T*() { return data; } void init(T* p) { 
	data = p; } void reset(T* p = 0) { free(data); init(p); }
	T* release(T* p = 0) { T* t = data; data = p; return t; }
	cstr xdup() const { return xstrdup(*(U*)this); }
	bool operator==(const T* s) const { return !strcmp(data, s); }
	
	// ctor/dtor/assignment
	constexpr xstr_() : data(0) {} 
	ALWAYS_INLINE ~xstr_() { free(data); } 
	xstr_(T* p) : data(p) {} xstr_& operator=
		(T* p) { reset(p); return *this; }
	xstr_(const xstr_& u) : data(u.xdup()) {} xstr_& operator=
		(const xstr_& u) { reset(u.xdup()); return *this; }
	xstr_(xstr_&& u) : data(u.release()) {} xstr_& operator=(
		xstr_&& u) { reset(u.release()); return *this; } 
	xstr_(U p) : data(p) {} xstr_& operator=(
		U p) { reset(p); return *this; }
		
	// other helpers
	void alloc(int len) { data = xMalloc(len+1); }
};



// nwide conditionals
#define NWIF(x,y) MIF(NWIDE,x,y)
#define NCCH NWIF(cchw, cch)
#define NCHR NWIF(WCHAR, char)
#define NCSTR NWIF(cstrW, cstr)
#define NWTX NWIF("W", "")
#define NWNM(x) NWIF(MCAT(x,W),x)

#ifndef no_xstrfmt
TMPL(T) 
struct xstrfmt_fmtN { enum { FLAG_ABSLEN = 1<<16,
		SPACE_POSI = 1,		FLAG_XCLMTN = 2, 	FLAG_QUOTE = 4,		FLAG_HASH = 8,	 
		FLAG_DOLAR = 16,	FLAG_PRCNT = 32,	FLAG_AMPRSND = 64,	FLAG_APOSTR = 128,
		FLAG_LBRACE = 256,	FLAG_RBRACE = 512, 	UPPER_CASE = 1024, 	FORCE_SIGN = 2048,
		FLAG_COMMA = 4096,	LEFT_JUSTIFY = 8192, FLAG_SLASH = 1<<15, PADD_ZEROS = 1<<16	};
		
	va_list ap; T* dstPosArg; uint flags;
	int width; int precision; int length;
	typedef size_t (*cbfn_t)(xstrfmt_fmtN* ctx,
		T ch); void* cbCtx; cbfn_t cbfn;
};
#define xstrfmt_fmt xstrfmt_fmtN<char>
#define xstrfmt_fmtW xstrfmt_fmtN<wchar_t>
#define Xstrfmt(...) xstr_(xstrfmt(__VA_ARGS__))
#endif

#define NWIDE 0
#include "string8W.h"
#define NWIDE 1
#include "string8W.h"

TMPL(T) cstr_<T>::cstr_(const T* d) : cstr_(cstr_len(d)) {}
