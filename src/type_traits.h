#ifndef _TYPE_TRAITS_H_
#define _TYPE_TRAITS_H_
#include <type_traits>

// missing on gcc 1.7.2
template <bool T, typename V>
using enable_if_t = typename std::enable_if<T, V>::type;
template< bool B, class T, class F >
using conditional_t = typename std::conditional<B,T,F>::type;
TMPL(T)
enable_if_t<!std::is_class<T>::value, bool> isNeg(const T& value) 
	{ return (typename std::make_signed<T>::type)(value) < 0; }
TMPL(T) uns(T v) { return (std::make_unsigned_t<T>)v; }

// is_explicitly_convertible is worthless
// must be replaced, total shit
struct is_cstyle_castable_impl {
	template<typename _From, typename _To, typename
		= decltype((_To)(std::declval<_From>()))>
	static std::true_type __test(int);
    template<typename, typename>
      static std::false_type __test(...); };
template<typename _From, typename _To>
struct is_cstyle_castable
    : public std::integral_constant<bool, (decltype(
		is_cstyle_castable_impl::__test<_From,_To>(0))::value)>
    { };

#endif
