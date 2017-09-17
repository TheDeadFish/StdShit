// prototype builders
#define CSTRFN0_(nm) REGCALL(2) bool nm(cstr str); SHITCALL bool nm(cch* str); 
#define CSTRFN1_(nm) REGCALL(2) cstr nm(cstr str); SHITCALL cstr nm(cch* str);
#define CSTRFN2_(nm) SHITCALL cstr nm(cstr n1, cstr n2); SHITCALL  \
	cstr nm(cstr n1, cch* n2); SHITCALL cstr nm(cch* n1, cch* n2); 
	
// ??
#define CSTRTH1_(nm) SHITCALL cstr nm(cch* str) { return nm(cstr(str)); }
#define CSTRTH2_(nm)  SHITCALL cstr nm(cstr n1, cch* n2) { return \
	nm(n1, cstr(n2)); } SHITCALL cstr nm(cch* n1, cch* n2) { \
	return nm(cstr(n1), n2); }

	
	
	
struct cstr;
#define cstr_len(si)({ cstr so; asm("push %1;" \
	"call _cstr_len;": "=A"(so): "g"(si)); so;})
REGCALL(2) cstr cstr_dup(cstr str);
SHITCALL cstr xstrdup(const char*);

typedef const char cch;
static inline bool isNull(cch* str) {
	return !str || !str[0]; }
	
// fast string compare helpers
#define isAlpha8(val) ({ bool ia8R; char ia8T; asm("and $-33, %b0; sub $65"\
	",%b0; cmp $25, %b0;" : "=q"(ia8T), "=@ccbe"(ia8R) : "0"(val)); ia8R; })
#define cmpi8(val,mem) ({ bool ret; char tmp;  \
asm("mov %2, %%ah" : "=a"(tmp) : "0"(val), "g"(mem));  \
asm("xor %%al, %%ah;" :"+a"(tmp), "=@ccz"(ret)); if(!ret) { \
asm("and $-33, %h0;" : "+a"(tmp), "=@ccz"(ret)); if(ret) { \
	ret = isAlpha8(tmp); }} ret; }) 

#define CMPI(m1, m2, lab) if(!cmpi8(m1,m2)) goto NS;
#define CMPS(m1, m2, lab) if(m1-m2) goto NS;
#define CMPL(len, cmp) int i = 0; do { cmp; } while(++i < len);
	
// string comparison
#define CSTRG(n) char* MCAT(str,n), int MCAT(len,n)
#define CSTRS(n) cstr(MCAT(str,n), MCAT(len,n))
#define CSTRX(s) (s).data, (s).slen
int SHITCALL cstr_cmp(CSTRG(1), CSTRG(2));
int SHITCALL cstr_icmp(CSTRG(1), CSTRG(2));
int SHITCALL cstr_cmp(CSTRG(1), cch* str2);
int SHITCALL cstr_icmp(CSTRG(1), cch* str2);

cstr SHITCALL cstr_str(CSTRG(1), CSTRG(2));
cstr SHITCALL cstr_istr(CSTRG(1), CSTRG(2));
cstr SHITCALL cstr_str(CSTRG(1), cch* str2);
cstr SHITCALL cstr_istr(CSTRG(1), cch* str2);


// tokenization/splitting
SHITCALL cstr cstr_split(cstr& str, char ch);
cstr SHITCALL cstr_chr(CSTRG(1), char ch);
cstr SHITCALL cstr_rchr(CSTRG(1), char ch);

struct cstr
{
	char* data; int slen;
	XARRAY_COMMON(cstr, char, slen);
	ALWAYS_INLINE cstr(cch* d) : cstr(cstr_len(d)) {} 
	template<int l> cstr(cch(& d)[l]) : cstr(d, l-1) {}
	DEF_RETPAIR(prn_t, int, slen, char*, data);
	prn_t prn() { return prn_t(slen, data); }
		
	// various functions
	cstr nterm(void) { if(data) 
		*end() = '\0'; return *this; }
	bool chk(uint idx) { return (idx < slen); }
	char get(uint idx) { return chk(idx) ? data[idx] : 0; }
	char getr(uint idx) { return  get(idx+slen); }
	bool sepReq() { return !isPathSep2(getr(-1), '\0'); }
	
	// comparison functions
	int cmp(cstr s) { return cstr_cmp(CSTRX(*this), CSTRX(s)); }
	int icmp(cstr s) { return cstr_icmp(CSTRX(*this), CSTRX(s)); }
	int cmp(cch* s) { return cstr_cmp(CSTRX(*this), s); }
	int icmp(cch* s) { return cstr_icmp(CSTRX(*this), s); }
	cstr str(cstr s) { return cstr_str(CSTRX(*this), CSTRX(s)); }
	cstr istr(cstr s) { return cstr_str(CSTRX(*this), CSTRX(s)); }
	cstr str(cch* s) { return cstr_str(CSTRX(*this), s); }
	cstr istr(cch* s) { return cstr_istr(CSTRX(*this), s); }

	// tokenization
	cstr chr(char ch) { return cstr_chr(CSTRX(*this), ch); }
	cstr rchr(char ch) { return cstr_rchr(CSTRX(*this), ch); }
	
	// dynamic functions
	cstr xdup(void) const  { return cstr_dup(*this); }
	void free(cch* p) { if(p != data) free(); }
};

#define OPEQU(t,a,b) t& operator=(a){b; return *this; }
#define CTOREQU(t,a,m,r,v) t(a):m(v){} OPEQU(t,a,r(v))

struct Cstr : cstr { 
	void reset(cstr str={}) { free(); init(str); }
	Cstr(): cstr(0,0) {}
	ALWAYS_INLINE ~Cstr() { free(); }
	
	CTOREQU(Cstr, const cstr& t, cstr, reset, t);
	CTOREQU(Cstr, const Cstr& t, cstr, reset, t.xdup());
	CTOREQU(Cstr, Cstr&& t, cstr, reset, t.release());
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

// Path Handling
CSTRFN1_(getPath) CSTRFN1_(getName) 
CSTRFN1_(getName2) CSTRFN1_(getExt)
CSTRFN2_(replName) CSTRFN2_(pathCat)
static inline cstr getPath0(cstr str) {
	return getPath(str).nterm(); }


	
TMPL2(T,U) struct xstr_
{ 
	// data access
	T* data; operator T*() { return data; } void init(T* p) { 
	data = p; } void reset(T* p = 0) { free(data); init(p); }
	T* release(T* p = 0) { T* t = data; data = p; return t; }
	cstr xdup() const { return xstrdup(data); }
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
};

typedef xstr_<char, cstr> xstr;
