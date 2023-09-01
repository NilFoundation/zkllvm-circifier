#ifndef _MATH_H
#define _MATH_H

#define HUGE_VAL    (1e+300 * 1e+300)

// TODO: Should not be used: no float types in EVM
#define INFINITY     0
#define NAN          0

#include <sys/cdefs.h>

__BEGIN_DECLS

/******************************************************************************
 * Floating point data types                                                  *
 ******************************************************************************/

/*  Define float_t and double_t per C standard, ISO/IEC 9899:2011 7.12 2,
    taking advantage of GCC's __FLT_EVAL_METHOD__ (which a compiler may
    define anytime and GCC does) that shadows FLT_EVAL_METHOD (which a
    compiler must define only in float.h).                                    */
#if __FLT_EVAL_METHOD__ == 0
typedef float float_t;
typedef double double_t;
#elif __FLT_EVAL_METHOD__ == 1
typedef double float_t;
typedef double double_t;
#elif __FLT_EVAL_METHOD__ == 2 || __FLT_EVAL_METHOD__ == -1
typedef long double float_t;
typedef long double double_t;
#else /* __FLT_EVAL_METHOD__ */
#   error "Unsupported value of __FLT_EVAL_METHOD__."
#endif /* __FLT_EVAL_METHOD__ */

#if defined(__GNUC__)
#   define    HUGE_VAL     __builtin_huge_val()
#   define    HUGE_VALF    __builtin_huge_valf()
#   define    HUGE_VALL    __builtin_huge_vall()
#   define    NAN          __builtin_nanf("0x7fc00000")
#else
#   define    HUGE_VAL     1e500
#   define    HUGE_VALF    1e50f
#   define    HUGE_VALL    1e5000L
#   define    NAN          __nan()
#endif

#define INFINITY    HUGE_VALF

/******************************************************************************
 *      Taxonomy of floating point data types                                 *
 ******************************************************************************/

#define FP_NAN          1
#define FP_INFINITE     2
#define FP_ZERO         3
#define FP_NORMAL       4
#define FP_SUBNORMAL    5
#define FP_SUPERNORMAL  6 /* legacy PowerPC support; this is otherwise unused */

#if defined __arm64__ || defined __ARM_VFPV4__
/*  On these architectures, fma(), fmaf( ), and fmal( ) are generally about as
    fast as (or faster than) separate multiply and add of the same operands.  */
#   define FP_FAST_FMA     1
#   define FP_FAST_FMAF    1
#   define FP_FAST_FMAL    1
#elif (defined __i386__ || defined __x86_64__) && (defined __FMA__ || defined __AVX512F__)
/*  When targeting the FMA ISA extension, fma() and fmaf( ) are generally
    about as fast as (or faster than) separate multiply and add of the same
    operands, but fmal( ) may be more costly.                                 */
#   define FP_FAST_FMA     1
#   define FP_FAST_FMAF    1
#   undef  FP_FAST_FMAL
#else
/*  On these architectures, fma( ), fmaf( ), and fmal( ) function calls are
    significantly more costly than separate multiply and add operations.      */
#   undef  FP_FAST_FMA
#   undef  FP_FAST_FMAF
#   undef  FP_FAST_FMAL
#endif

/* The values returned by `ilogb' for 0 and NaN respectively. */
#define FP_ILOGB0      (-2147483647 - 1)
#define FP_ILOGBNAN    (-2147483647 - 1)

/* Bitmasks for the math_errhandling macro.  */
#define MATH_ERRNO        1    /* errno set by math functions.  */
#define MATH_ERREXCEPT    2    /* Exceptions raised by math functions.  */

typedef float float_t;
typedef double double_t;

int fpclassify(double x);
int isfinite(double x);
int isinf(double x);
int isnan(double x);
int isnormal(double x);
int signbit(double x);
double acos(double x);
float acosf(float x);
long double acosl(long double x);
double asin(double x);
float asinf(float x);
long double asinl(long double x);
double atan(double x);
float atanf(float x);
long double atanl(long double x);
double atan2(double y, double x);
float atan2f(float y, float x);
long double atan2l(long double y, long double x);
double cos(double x);
float cosf(float x);
long double cosl(long double x);
double sin(double x);
float sinf(float x);
long double sinl(long double x);
double tan(double x);
float tanf(float x);
long double tanl(long double x);
double acosh(double x);
float acoshf(float x);
long double acoshl(long double x);
double asinh(double x);
float asinhf(float x);
long double asinhl(long double x);
double atanh(double x);
float atanhf(float x);
long double atanhl(long double x);
double cosh(double x);
float coshf(float x);
long double coshl(long double x);
double sinh(double x);
float sinhf(float x);
long double sinhl(long double x);
double tanh(double x);
float tanhf(float x);
long double tanhl(long double x);
double exp(double x);
float expf(float x);
long double expl(long double x);
double exp2(double x);
float exp2f(float x);
long double exp2l(long double x);
double expm1(double x);
float expm1f(float x);
long double expm1l(long double x);
double frexp(double value, int *exp);
float frexpf(float value, int *exp);
long double frexpl(long double value, int *exp);
int ilogb(double x);
int ilogbf(float x);
int ilogbl(long double x);
double ldexp(double x, int exp);
float ldexpf(float x, int exp);
long double ldexpl(long double x, int exp);
double log(double x);
float logf(float x);
long double logl(long double x);
double log10(double x);
float log10f(float x);
long double log10l(long double x);
double log1p(double x);
float log1pf(float x);
long double log1pl(long double x);
double log2(double x);
float log2f(float x);
long double log2l(long double x);
double logb(double x);
float logbf(float x);
long double logbl(long double x);
double modf(double value, double *iptr);
float modff(float value, float *iptr);
long double modfl(long double value, long double *iptr);
double scalbn(double x, int n);
float scalbnf(float x, int n);
long double scalbnl(long double x, int n);
double scalbln(double x, long int n);
float scalblnf(float x, long int n);
long double scalblnl(long double x, long int n);
double cbrt(double x);
float cbrtf(float x);
long double cbrtl(long double x);
double fabs(double x);
float fabsf(float x);
long double fabsl(long double x);
double hypot(double x, double y);
float hypotf(float x, float y);
long double hypotl(long double x, long double y);
double pow(double x, double y);
float powf(float x, float y);
long double powl(long double x, long double y);
double sqrt(double x);
float sqrtf(float x);
long double sqrtl(long double x);
double erf(double x);
float erff(float x);
long double erfl(long double x);
double erfc(double x);
float erfcf(float x);
long double erfcl(long double x);
double lgamma(double x);
float lgammaf(float x);
long double lgammal(long double x);
double tgamma(double x);
float tgammaf(float x);
long double tgammal(long double x);
double ceil(double x);
float ceilf(float x);
long double ceill(long double x);
double floor(double x);
float floorf(float x);
long double floorl(long double x);
double nearbyint(double x);
float nearbyintf(float x);
long double nearbyintl(long double x);
double rint(double x);
float rintf(float x);
long double rintl(long double x);
long int lrint(double x);
long int lrintf(float x);
long int lrintl(long double x);
long long int llrint(double x);
long long int llrintf(float x);
long long int llrintl(long double x);
double round(double x);
float roundf(float x);
long double roundl(long double x);
long int lround(double x);
long int lroundf(float x);
long int lroundl(long double x);
long long int llround(double x);
long long int llroundf(float x);
long long int llroundl(long double x);
double trunc(double x);
float truncf(float x);
long double truncl(long double x);
double fmod(double x, double y);
float fmodf(float x, float y);
long double fmodl(long double x, long double y);
double remainder(double x, double y);
float remainderf(float x, float y);
long double remainderl(long double x, long double y);
double remquo(double x, double y, int *quo);
float remquof(float x, float y, int *quo);
long double remquol(long double x, long double y,
                    int *quo);
double copysign(double x, double y);
float copysignf(float x, float y);
long double copysignl(long double x, long double y);
double nan(const char *tagp);
float nanf(const char *tagp);
long double nanl(const char *tagp);
double nextafter(double x, double y);
float nextafterf(float x, float y);
long double nextafterl(long double x, long double y);
double nexttoward(double x, long double y);
float nexttowardf(float x, long double y);
long double nexttowardl(long double x, long double y);
double fdim(double x, double y);
float fdimf(float x, float y);
long double fdiml(long double x, long double y);
double fmax(double x, double y);
float fmaxf(float x, float y);
long double fmaxl(long double x, long double y);
double fmin(double x, double y);
float fminf(float x, float y);
long double fminl(long double x, long double y);
double fma(double x, double y, double z);
float fmaf(float x, float y, float z);
long double fmal(long double x, long double y,
                 long double z);
int isgreater(double x, double y);
int isgreaterequal(double x, double y);
int isless(double x, double y);
int islessequal(double x, double y);
int islessgreater(double x, double y);
int isunordered(double x, double y);

__END_DECLS

#endif /* _MATH_H */
