
// ansi/unicode macros
#define NCHAR MIF(NWIDE, wchar_t, char)
#define DCHAR MIF(NWIDE, DWORD, WORD)
#define _N(str) MIF(NWIDE, MCAT(L,str), str)
#define NCHFN(x) MCAT(x, MIF(NWIDE,W,A))



TMPL(T) struct cstr_;
typedef cstr_<char> cstrA;
typedef cstr_<wchar_t> cstrW;
#define cstrT cstr_<NCHAR>

SHITCALL cstrA cstr_len(const char*);
SHITCALL cstrW cstr_len(const wchar_t*);

TMPL(T) struct cstr_
{
	// basic members
	typedef T vt; typedef const vt cvt;
	vt* data; uint slen;
	
	// creation / assignment
	cstr_() = default; cstr_(const cstr_& that) = default;
	cstr_(cvt* d) : cstr_(cstr_len(d)) {} 
	cstr_(cvt* d, int l) : data((vt*)d), slen(l) {}
	cstr_(cvt* d, cvt* e) : data((vt*)d), slen(e-d) {}
	template<int l> cstr_(const T(& d)[l]) : cstr_(d, l-1) {}
	template<typename... Args> cstr_& init(Args... args)
		{ return *this = cstr_(args...); }
		
	// unsafe indexed splitting
	cstr_ left(int i) { return cstr_(data, i); }
	cstr_ right(int i) { return cstr_(data+i, slen-i); }
	cstr_ endRel(int i) { return cstr_(end(), i-slen); }
	
	// pointer / length access
	void setbase(vt* pos) {
		slen += data-pos; data = pos; }
	void setend(vt* pos) { slen = pos-data; }
	int offset(vt* pos) { pos - data; }
	DEF_BEGINEND(vt, data, slen);
	DEF_RETPAIR(prn_t, int, slen, vt*, data);
	prn_t prn() { return prn_t(slen, data); }
	
	// trimming optimization
	struct Ptr { vt* data; vt* end; bool chk() { return end > data; }
	ALWAYS_INLINE vt& f() { return *data; } vt& l() { return end[-1]; }
	ALWAYS_INLINE vt& fi() { return *data++; } vt& ld() { return *--end; }};
	ALWAYS_INLINE Ptr ptr() { return Ptr{data, end()}; }
	ALWAYS_INLINE void set(Ptr ptr) { init(ptr.data, ptr.end); }
	ALWAYS_INLINE void sete(Ptr ptr) { setend(ptr.end); }	
};




		
// path operations
SHITCALL cstrA getPath(cstrA); SHITCALL cstrW getPath(cstrW);
SHITCALL cstrA getName(cstrA); SHITCALL cstrW getName(cstrW);


TMPL(T) struct bstr_;
typedef bstr_<char> bstrA;
typedef bstr_<wchar_t> bstrW;
#define bstrT bstr_<NCHAR>

#define CSTR_FN2(r, n, t) r n(const t*); r n(const t*, int len); \
	r n(cstr_<t> str) { return n(str.data, str.len); }


TMPL(T) struct bstr_ : cstr_<T>
{
	int mlen;
	
	// basic copy/concat
	bstr_& strcpy(const T*); bstr_& strcpy(cstr_<T>);
	bstr_& strcat(const T*); bstr_& strcat(cstr_<T>);
	bstr_& fmtcpy(const T*, ...);
	bstr_& fmtcat(const T*, ...);	
	
	// null-termination
	REGCALL(1) cstr_<T> nullTerm(void);
	SHITCALL cstr_<T> calcLen(void);
	SHITCALL cstr_<T> updateLen(void);
	
	// buffer allocation
	REGCALL(2) T* xreserve(int len);
	REGCALL(2) T* xresize(int len); 
	REGCALL(2) T* xralloc(int len);
	REGCALL(2) T* xnalloc(int len);	
	
	// memory allocation
	DEF_RETPAIR(alloc_t, bstr_*,
		This, int, len_);
	REGCALL(2) alloc_t alloc(int len);
};
