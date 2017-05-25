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
	
// string comparison
#define CSTRG(n) char* MCAT(str,n), int MCAT(len,n)
#define CSTRS(n) cstr(MCAT(str,n), MCAT(len,n))
#define CSTRX(s) (s).data, (s).slen
int SHITCALL cstr_cmp(CSTRG(1), CSTRG(2));
int SHITCALL cstr_icmp(CSTRG(1), CSTRG(2));
int SHITCALL cstr_cmp(CSTRG(1), cch* str2);
int SHITCALL cstr_icmp(CSTRG(1), cch* str2);

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
	
	
	
	
	// dynamic functions
	cstr xdup(void) const  { return cstr_dup(*this); }
	void free(cch* p) { if(p != data) free(); }
};

struct Cstr : cstr { 
	Cstr(const cstr& that) : cstr(that) {}
	Cstr(Cstr&& that) { this->init(that.release()); }
	Cstr(const Cstr& that) { this->init(cstr_dup(that)); }
	ALWAYS_INLINE ~Cstr() { free(); } };

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
CSTRFN1_(getPath) CSTRFN1_(getName) CSTRFN1_(getName2)
CSTRFN2_(replName) CSTRFN2_(pathCat)
static inline cstr getPath0(cstr str) {
	return getPath(str).nterm(); }

struct xstr 
{ 
	// data access
	char* data; operator char*() { return data; } void init(char* p) { 
	data = p; } void reset(char* p = 0) { free(data); init(p); }
	char* release(char* p = 0) { char* t = data; data = p; return t; }
	cstr xdup() const { return xstrdup(data); }
	bool operator==(cch* s) const { return !strcmp(data, s); }
	
	// ctor/dtor/assignment
	constexpr xstr() : data(0) {} ~xstr() { free(data); } 
	xstr(char* p) : data(p) {} xstr& operator=
		(char* p) { reset(p); return *this; }
	xstr(const xstr& u) : data(u.xdup()) {} xstr& operator=
		(const xstr& u) { reset(u.xdup()); return *this; }
	xstr(xstr&& u) : data(u.release()) {} xstr& operator=(
		xstr&& u) { reset(u.release()); return *this; } 
};