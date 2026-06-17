// https://github.com/Hirrolot/awesome-c-preprocessor
#define nil 0
#define null NULL
#define not !
#define and &&
#define or  ||
#define forever for (;;)
#define nul ((void *)0)
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define BIT(n) (1UL << (n))
#define BIT_SET(x, n)   ((x) |= BIT(n))
#define BIT_CLEAR(x, n) ((x) &= ~BIT(n))
#define BIT_TEST(x, n)  (!!((x) & BIT(n)))
#define MIN(a, b) \
    ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })
#define MAX(a, b) \
    ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })
#define DEFER(start, end) for(int _i = (start, 0); !_i; _i = (end, 1))
//Usage: DEFER(mutex_lock(&m), mutex_unlock(&m)) { /* critical section */ }
#define IS_WITHIN(val, min, max) ((val) >= (min) && (val) <= (max))
#define FOREACH_ELEM(ptr, arr) \
    for (ptr = (arr); ptr < (arr) + ARRAY_SIZE(arr); ptr++)
#define FOR_EACH_MAP(array, size, var_name, body) \
    for (size_t _i = 0; _i < (size); _i++) {      \
        __typeof__(array[0]) *var_name = &array[_i]; \
        body;                                     \
    }
#define DEBUG_MODE 0
#define TYPE_NAME(x) _Generic((x), \
    int: "int", \
    float: "float", \
    double: "double", \
    default: "unknown")
#define LAMBDA(return_type, args, body) \
    ({ return_type __fn__ args body &__fn__; })
    /*
    int result = LAMBDA(int, (int x, int y), { return x + y; })(5, 10);
    // Pass an anonymous function as a function pointer
    void execute_callback(int (*f)(int));
    execute_callback(LAMBDA(int, (int a), { return a * 2; }));*/
#define lambda(lambda$_ret, lambda$_args, lambda$_body)\
  ({\
    lambda$_ret lambda$__anon$ lambda$_args\
      lambda$_body\
    &lambda$__anon$;\
  })

#define RGBA(r, g, b, a) (((r)&0xFF)<<24 | ((g)&0xFF)<<16 | ((b)&0xFF)<<8 | ((a)&0xFF))

#define KB(x) ((x) * 1024UL)
#define MB(x) (KB(x) * 1024UL)
#define GB(x) (MB(x) * 1024UL)
