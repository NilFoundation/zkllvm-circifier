#ifndef _STDLIB_H
#define _STDLIB_H

#define __need_size_t
#define __need_wchar_t
#define __need_NULL

#include <stddef.h>
#include <locale.h>

#define EXIT_SUCCESS    0
#define EXIT_FAILURE    1
#define RAND_MAX        32767

#define MB_CUR_MAX      _mb_cur_max

#include <sys/cdefs.h>

__BEGIN_DECLS

typedef struct {
    int quot;
    int rem;
} div_t;

typedef struct {
    long int quot;
    long int rem;
} ldiv_t;

typedef struct {
    long long int quot;
    long long int rem;
} lldiv_t;

extern int _mb_cur_max;

double atof(const char *nptr);
int atoi(const char *nptr);
long int atol(const char *nptr);
long long int atoll(const char *nptr);
double strtod(const char * nptr,
              char ** endptr);
float strtof(const char * nptr,
             char ** endptr);
long double strtold(const char * nptr,
                    char ** endptr);
long int strtol(const char * nptr,
                char ** endptr, int base);
long long int strtoll(const char * nptr,
                      char ** endptr, int base);
unsigned long int strtoul(
        const char * nptr,
        char ** endptr, int base);
unsigned long long int strtoull(
        const char * nptr,
        char ** endptr, int base);
int rand(void);
void srand(unsigned int seed);
void *aligned_alloc(size_t alignment, size_t size);
void *calloc(size_t nmemb, size_t size);
void free(void *ptr);
void *malloc(size_t size);
void *realloc(void *ptr, size_t size);
_Noreturn void abort(void);
int atexit(void (*func)(void));
int at_quick_exit(void (*func)(void));
_Noreturn void exit(int status);
_Noreturn void _Exit(int status);
char *getenv(const char *name);
_Noreturn void quick_exit(int status);
int system(const char *string);
void *bsearch(const void *key, const void *base,
              size_t nmemb, size_t size,
              int (*compar)(const void *, const void *));
void qsort(void *base, size_t nmemb, size_t size,
           int (*compar)(const void *, const void *));
int abs(int j);
long int labs(long int j);
long long int llabs(long long int j);
div_t div(int numer, int denom);
ldiv_t ldiv(long int numer, long int denom);
lldiv_t lldiv(long long int numer,
              long long int denom);
int mblen(const char *s, size_t n);
int mbtowc(wchar_t * pwc,
           const char * s, size_t n);
int wctomb(char *s, wchar_t wchar);
size_t mbstowcs(wchar_t * pwcs,
                const char * s, size_t n);
size_t wcstombs(char * s,
                const wchar_t * pwcs, size_t n);

long int strtol_l (const char * __nptr,
                   char ** __endptr, int __base,
                   locale_t __loc);

extern unsigned long int strtoul_l (const char * __nptr,
                                    char ** __endptr,
                                    int __base, locale_t __loc);

extern long long int strtoll_l (const char * __nptr,
                                char ** __endptr, int __base,
                                locale_t __loc);

extern unsigned long long int strtoull_l (const char * __nptr,
                                          char ** __endptr,
                                          int __base, locale_t __loc);

extern double strtod_l (const char * __nptr,
                        char ** __endptr, locale_t __loc);

extern float strtof_l (const char * __nptr,
                       char ** __endptr, locale_t __loc);

extern long double strtold_l (const char * __nptr,
                              char ** __endptr,
                              locale_t __loc);

/*

void abort_handler_s(
        const char * msg,
        void * ptr,
        errno_t error);
void ignore_handler_s(
        const char * msg,
        void * ptr,
        errno_t error);
errno_t getenv_s(size_t * len,
                 char * value, rsize_t maxsize,
                 const char * name);
void *bsearch_s(const void *key, const void *base,
                rsize_t nmemb, rsize_t size,
                int (*compar)(const void *k, const void *y,
                              void *context),
                void *context);
errno_t qsort_s(void *base, rsize_t nmemb, rsize_t size,
                int (*compar)(const void *x, const void *y,
                              void *context),
                void *context);
errno_t wctomb_s(int * status,
                 char * s,
                 rsize_t smax,
                 wchar_t wc);
errno_t mbstowcs_s(size_t * retval,
                   wchar_t * dst, rsize_t dstmax,
                   const char * src, rsize_t len);
errno_t wcstombs_s(size_t * retval,
                   char * dst, rsize_t dstmax,
                   const wchar_t * src, rsize_t len);
*/


__END_DECLS

#endif /* _STDLIB_H */
