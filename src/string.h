
struct cstr;
SHITCALL cstr cstr_len(const char*);
typedef const char cch;



struct cstr
{
	char* data; int slen;

	

	// creation / assignment
	cstr() = default; cstr(const cstr& that) = default;
	cstr(cch* d) : cstr(cstr_len(d)) {} 
	cstr(cch* d, int l) : data((char*)d), slen(l) {}
	cstr(cch* d, cch* e) : data((char*)d), slen(e-d) {}
	template<int l> cstr(cch(& d)[l]) : cstr(d, l-1) {}
	template<typename... Args> cstr& init(Args... args)
		{ return *this = cstr(args...); }
		
	// unsafe indexed splitting
	cstr left(int i) { return cstr(data, i); }
	cstr right(int i) { return cstr(data+i, slen-i); }
	cstr endRel(int i) { return cstr(end(), i-slen); }
	
	// pointer / length access
	void setbase(char* pos) {
		slen += data-pos; data = pos; }
	void setend(char* pos) { slen = pos-data; }
	int offset(char* pos) { pos - data; }
	DEF_BEGINEND(char, data, slen);
	DEF_RETPAIR(prn_t, int, slen, char*, data);
	prn_t prn() { return prn_t(slen, data); }
	
	// trimming optimization
	struct Ptr { char* data; char* end; bool chk() { return end > data; }
	ALWAYS_INLINE char& f() { return *data; } char& l() { return end[-1]; }
	ALWAYS_INLINE char& fi() { return *data++; } char& ld() { return *--end; }};
	ALWAYS_INLINE Ptr ptr() { return Ptr{data, end()}; }
	ALWAYS_INLINE void set(Ptr ptr) { init(ptr.data, ptr.end); }
	ALWAYS_INLINE void sete(Ptr ptr) { setend(ptr.end); }	
};

struct Cstr : cstr { 
	Cstr(const cstr& that) : cstr(that) {}
	Cstr(Cstr&& that) = default;
	Cstr(const Cstr& that) = delete;
	~Cstr() { free(this->data); } };
	
	
	





		
// path operations
SHITCALL cstr getPath(cstr);
SHITCALL cstr getName(cstr);

struct bstr : cstr
{
	int mlen;
	
	// basic copy/concat
	bstr& strcpy(const char*); bstr& strcpy(cstr);
	bstr& strcat(const char*); bstr& strcat(cstr);
	bstr& fmtcpy(const char*, ...);
	bstr& fmtcat(const char*, ...);	
	
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
