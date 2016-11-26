
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

typedef const char cch;
static inline bool isNull(cch* str) {
	return !str || !str[0]; }

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
	
	// dynamic functions
	cstr xdup(void) const  {
		return cstr_dup(*this); }
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
