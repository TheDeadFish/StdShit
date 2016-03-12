// basic stuff
#define MCAT2(name, ...) name##__VA_ARGS__
#define MCAT(name, ...) MCAT2(name, __VA_ARGS__)
#define EXTRACT(...) EXTRACT __VA_ARGS__
#define NOTHING_EXTRACT
#define UNPAREN(x) MCAT(NOTHING_, EXTRACT x)

// varidac macros, by Anonymous Coward
// http://stackoverflow.com/a/6936114
#define VA_NARGS_IMPL(_1, _2, _3, _4, _5, N, ...) N
#define VA_NARGS(...) VA_NARGS_IMPL(X,##__VA_ARGS__, 4, 3, 2, 1, 0)
#define VARARG_IMPL2(base, count, ...) base##count(__VA_ARGS__)
#define VARARG_IMPL(base, count, ...) VARARG_IMPL2(base, count, __VA_ARGS__) 
#define VARARG(base, ...) VARARG_IMPL(base, VA_NARGS(__VA_ARGS__), __VA_ARGS__)
