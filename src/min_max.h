// replacment for std::max and std::min
// those functions are completly useless
// much like everything else in <algorithm>
// who the hell designed stl, total stupidity

#ifndef _MINMAX_H_
#define _MINMAX_H_
#include <type_traits>
#include <tuple>

template <class T, class U>
typename std::common_type<T&&, U&&>::type
min(T&& a, U&& b){
    if (b < a)
        return std::forward<U>(b);
    return std::forward<T>(a); }
template <class T, class U>
typename std::common_type<T&&, U&&>::type
max(T&& a, U&& b){
    if (b > a)
        return std::forward<U>(b);
    return std::forward<T>(a); }
	
template <class T, class U, class V>
typename std::common_type<T&&,
	typename std::common_type<U&&,V&&>::type>::type
min_max(T&& val, U&& low, V&& high) {
	if( val < low ) return std::forward<U>(low);
	if( val > high ) return std::forward<V>(high);
	return std::forward<T>(val); }
	
template <class T, class U>
T& min_ref(T& obj, const U& val) { if(obj > val) obj = val; return obj; }
template <class T, class U>
T& min1_ref(T& obj, const U& val) { if(obj >= val) obj = val-1; return obj; }
template <class T, class U>
T& max_ref(T& obj, const U& val) { if(obj < val) obj = val; return obj; }



#endif
