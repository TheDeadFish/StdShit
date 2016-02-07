// Awesome delegates
// Version 2.2, 30/01/2016
// DeadFish Shitware

#ifndef _DELEAGATE_H_
#define _DELEAGATE_H_
#include <type_traits>
#include <tuple>

#define MakeDelegate(obj, func) \
	decltype(Delegate_(obj, func))::Bind<func>(obj)

template <class R, class... P>
struct Delegate
{
	Delegate() {} Delegate(void* stub) { stub_ptr = (stub_type)stub; }
	Delegate(void* object, void* stub) {
		object_ptr = object; stub_ptr = (stub_type)stub;	}
	TMPL(T)
	void set(T object, R (__thiscall *stub)(T, P... params))
	{	object_ptr = (void*)object;
		stub_ptr = (stub_type)stub;	}
	TMPL(T)
	void set(T* object, R (T::*stub)(P... params))
	{	object_ptr = (void*)object;
		stub_ptr = (void*)(object_ptr->*stub); }
	R operator()(P... params) const
    {	return (*stub_ptr)(object_ptr, params...); }
	bool isValid(void)
	{	return (stub_ptr != 0);	}
	
// private
	typedef R (__thiscall *stub_type)
		(void* object_ptr, P... params);
	void* object_ptr;
    stub_type stub_ptr;
};

template <class R, class... P>
struct MemberFunc
{
	MemberFunc() {}
	MemberFunc(void *stub) { stub_ptr = (stub_type)stub; }
	R operator()(void* object_ptr, P... params) const
    {	return (*stub_ptr)(object_ptr, params...); }
	bool isValid(void)
	{	return (stub_ptr != 0);	}

// private
	typedef R (__thiscall *stub_type)
		(void* object_ptr, P... params);
	stub_type stub_ptr;	
};


// cdecl / stdcall with context
#define DELEGATE_WCTX(nm, cv, ck) template <class T, class R, typename... P> struct nm {template \
<R (cv *TMethod)(T*, P...)> struct Bind { Bind(T* ctx) { object_ptr = (void*)ctx; } void* \
object_ptr; template <class RO, class... PO> static RO __thiscall stub(void* ctx, PO... params) {\
return TMethod((T*)ctx, params...); } template <class RO, class... PO> operator Delegate<RO, \
PO...>() { UNPAREN(ck); return Delegate<RO, PO...>(object_ptr, (void*)&stub<RO, PO...>); }};}; \
template <class T, class R, typename... P> nm<T, R, P...> Delegate_(T* obj, R (cv *TMethod)(T*, \
P... params)) { return nm<T, R, P...>(); }
DELEGATE_WCTX(Delegate_cdecl,,); DELEGATE_WCTX(Delegate_std,__stdcall,); 
DELEGATE_WCTX(Delegate_this,__thiscall,(if( std::is_same
<std::tuple<P...>,std::tuple<PO...> >::value ) return Delegate<RO, PO...>(0, (void*)TMethod);));

// cdecl / stdcall without context
#define DELEGATE_WOCTX(nm, cv, ck) template <class R, typename... P> struct nm { template <R (cv \
*TMethod)(P...)> struct Bind { Bind(int dummy) {} template <class RO, class... PO> static RO \
__thiscall stub(void* ctx, PO... params) {	return TMethod(params...); } template <class RO,  \
class... PO> operator Delegate<RO, PO...>() { UNPAREN(ck); return Delegate<RO, PO...>((void*)0, \
(void*)&stub<RO, PO...>); }};}; template <class R, typename... P> nm<R, P...> Delegate_( \
int dummy, R (cv *TMethod)(P... params)) { return nm<R, P...>(); }
DELEGATE_WOCTX(Delegate_noctx,,) DELEGATE_WOCTX(Delegate_noctx2, __stdcall, (if( std::is_same
<std::tuple<P...>,std::tuple<PO...> >::value ) return Delegate<RO, PO...>(0, (void*)TMethod);))

// Delegate __member function
template <class S, class T, class R, typename... P>
struct Delegate_member {
template <R (T::*TMethod)(P...)>
struct Bind {
	Bind(S* ctx) { object_ptr = (void*)static_cast<T*>(ctx); }
	void* object_ptr;
	template <class RO, class... PO>
	static RO __thiscall stub(void* ctx, PO... params)
	{	return (((T*)ctx)->*TMethod)(params...); }
	template <class RO, class... PO>
	operator Delegate<RO, PO...>() {
		if( std::is_same<std::tuple<P...>, std::tuple<PO...> >::value )
			return Delegate<RO, PO...>(object_ptr,
					(void*)(((T*)object_ptr)->*TMethod));
		return Delegate<RO, PO...>(
			object_ptr, (void*)&stub<RO, PO...>); }
	operator MemberFunc<R, P...>() {
		Delegate<R, P...> delgate = *this;
		return MemberFunc<R, P...>((void*)delgate.stub_ptr); }
};};
template <class S, class T, class R, typename... P> Delegate_member<S, T, R, P...> Delegate_(
S* obj, R (T::*TMethod)(P... params)) { return Delegate_member<S, T, R, P...>(); }

// Delegate __lambda function
template <typename T, typename U>
struct helper : helper<T, decltype(&U::operator())>{};
template <typename T, typename C, typename R, typename... A>
struct helper<T, R(C::*)(A...) const> {
    static const bool value = std::is_convertible<T, R(*)(A...)>::value; };
template<typename T> struct is_stateless {
    static const bool value = helper<T,T>::value; };
	
template <class F>
struct Delegate_lambda { const F& func;
	Delegate_lambda(const F& func_) : func(func_) {}
	template <class RO, class... PO>
	static RO __thiscall stub(void* ctx, PO... params)
	{	return (*(F*)ctx)(params...); }	
	template <class RO, class... PO>
	operator Delegate<RO, PO...>() { return Delegate<RO, PO...>(
		is_stateless<F>::value ? 0 : (void*)&func, (void*)&stub<RO, PO...>); }
};
template <class F>
Delegate_lambda<F> MakeDlgtLmbd(const F& func)
{	return Delegate_lambda<F>(func); }

template <class T, class F>
struct Delegate_lambda2 { const F& func; T* obj;
	Delegate_lambda2(T* obj_, const F& func_) : obj(obj_), func(func_) {}
	template <class RO, class... PO>
	static RO __thiscall stub(void* ctx, PO... params)
	{	return (*(F*)0)((T*)ctx, params...); }
	template <class RO, class... PO>
	operator Delegate<RO, PO...>() { return Delegate<RO, PO...>(
			(void*)obj, (void*)&stub<RO, PO...>); }
};
template <class T, class F>
Delegate_lambda2<T, F> MakeDlgtLmbd(T* obj, const F& func)
{	return Delegate_lambda2<T, F>(obj, func); }

/* delegates V1.0 compatibility
#define Delegate0 Delegate 
#define MakeDelegate0(obj, func) MakeDelegate(obj, func)
#define Delegate1 Delegate 
#define MakeDelegate1(obj, func) MakeDelegate(obj, func)
#define Delegate2 Delegate
#define MakeDelegate2(obj, func) MakeDelegate(obj, func)

// delegates V1.0 Interface
#define DF_INTERFACE(type)			\
class type{							\
public:								\
	enum { val = __COUNTER__};		\
	void* object_ptr;				\
	void reset(void){				\
		for(int i = 1; i < sizeof(*this)/4; i++) 	\
			(&object_ptr)[i] = 0;}
#define DF_METHOD0(R)		\
	DfMethod<__COUNTER__-val, R>
#define DF_METHOD1(R, P1)		\
	DfMethod<__COUNTER__-val, R, P1>
#define DF_METHOD2(R, P1, P2)		\
	DfMethod<__COUNTER__-val, R, P1, P2>
#define DF_INTERFACE_END() };

template <int i, class R, class... P>
class DfMethod
{
public:
	void operator=(int j)
	{	stub_ptr = (stub_type*)j;	}
	void operator=(Delegate<R, P...> delegate)
	{	stub_ptr = delegate.stub_ptr;	}
	R operator()(P... params) const
    {	void* object_ptr = *((void**)this-i);
        return (*stub_ptr)(object_ptr, params...);   }
	bool isValid(void)
	{	return (stub_ptr != 0);	}
	
// private
	typedef R (__thiscall *stub_type)(
		void* object_ptr, P... params);
	stub_type stub_ptr;
}; */

#endif
