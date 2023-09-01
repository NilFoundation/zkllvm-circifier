#ifndef _WCHAR_H
#define _WCHAR_H

#include "stdarg.h"
#include "stddef.h"
#include "stdio.h"

typedef struct {} mbstate_t;
typedef unsigned wint_t;

#define	WEOF 	((wint_t)-1)
#define WCHAR_MIN 0
#define WCHAR_MAX 0xffffffffu

int fwprintf(FILE * stream,
             const wchar_t * format, ...);
int fwscanf(FILE * stream,
            const wchar_t * format, ...);
int swprintf(wchar_t * s, size_t n,
             const wchar_t * format, ...);
int swscanf(const wchar_t * s,
            const wchar_t * format, ...);
int vfwprintf(FILE * stream,
              const wchar_t * format, va_list arg);
int vfwscanf(FILE * stream,
             const wchar_t * format, va_list arg);
int vswprintf(wchar_t * s, size_t n,
              const wchar_t * format, va_list arg);
int vswscanf(const wchar_t * s,
             const wchar_t * format, va_list arg);
int vwprintf(const wchar_t * format,
             va_list arg);
int vwscanf(const wchar_t * format,
            va_list arg);
int wprintf(const wchar_t * format, ...);
int wscanf(const wchar_t * format, ...);
wint_t fgetwc(FILE *stream);
wchar_t *fgetws(wchar_t * s, int n,
                FILE * stream);
wint_t fputwc(wchar_t c, FILE *stream);
int fputws(const wchar_t * s,
           FILE * stream);
int fwide(FILE *stream, int mode);
wint_t getwc(FILE *stream);
wint_t getwchar(void);
wint_t putwc(wchar_t c, FILE *stream);
wint_t putwchar(wchar_t c);
wint_t ungetwc(wint_t c, FILE *stream);
double wcstod(const wchar_t * nptr,
              wchar_t ** endptr);
float wcstof(const wchar_t * nptr,
             wchar_t ** endptr);
long double wcstold(const wchar_t * nptr,
                    wchar_t ** endptr);
long int wcstol(const wchar_t * nptr,
                wchar_t ** endptr, int base);
long long int wcstoll(const wchar_t * nptr,
                      wchar_t ** endptr, int base);
unsigned long int wcstoul(const wchar_t * nptr,
                          wchar_t ** endptr, int base);
unsigned long long int wcstoull(
        const wchar_t * nptr,
        wchar_t ** endptr, int base);
wchar_t *wcscpy(wchar_t * s1,
                const wchar_t * s2);
wchar_t *wcsncpy(wchar_t * s1,
                 const wchar_t * s2, size_t n);
wchar_t *wmemcpy(wchar_t * s1,
                 const wchar_t * s2, size_t n);
wchar_t *wmemmove(wchar_t *s1, const wchar_t *s2,
                  size_t n);
wchar_t *wcscat(wchar_t * s1,
                const wchar_t * s2);
wchar_t *wcsncat(wchar_t * s1,
                 const wchar_t * s2, size_t n);
int wcscmp(const wchar_t *s1, const wchar_t *s2);
int wcscoll(const wchar_t *s1, const wchar_t *s2);
int wcsncmp(const wchar_t *s1, const wchar_t *s2,
            size_t n);
size_t wcsxfrm(wchar_t * s1,
               const wchar_t * s2, size_t n);
int wmemcmp(const wchar_t *s1, const wchar_t *s2,
            size_t n);
wchar_t *wcschr(const wchar_t *s, wchar_t c);
size_t wcscspn(const wchar_t *s1, const wchar_t *s2);
wchar_t *wcspbrk(const wchar_t *s1, const wchar_t *s2);
wchar_t *wcsrchr(const wchar_t *s, wchar_t c);
size_t wcsspn(const wchar_t *s1, const wchar_t *s2);
wchar_t *wcsstr(const wchar_t *s1, const wchar_t *s2);
wchar_t *wcstok(wchar_t * s1,
                const wchar_t * s2,
                wchar_t ** ptr);
wchar_t *wmemchr(const wchar_t *s, wchar_t c, size_t n);
size_t wcslen(const wchar_t *s);
wchar_t *wmemset(wchar_t *s, wchar_t c, size_t n);
size_t wcsftime(wchar_t * s, size_t maxsize,
                const wchar_t * format,
                const struct tm * timeptr);
wint_t btowc(int c);
int wctob(wint_t c);
int mbsinit(const mbstate_t *ps);
size_t mbrlen(const char * s, size_t n,
              mbstate_t * ps);
size_t mbrtowc(wchar_t * pwc,
               const char * s, size_t n,
               mbstate_t * ps);
size_t wcrtomb(char * s, wchar_t wc,
               mbstate_t * ps);
size_t mbsrtowcs(wchar_t * dst,
                 const char ** src, size_t len,
                 mbstate_t * ps);
size_t wcsrtombs(char * dst,
                 const wchar_t ** src, size_t len,
                 mbstate_t * ps);

int fwprintf_s(FILE * stream,
               const wchar_t * format, ...);
int fwscanf_s(FILE * stream,
              const wchar_t * format, ...);
int snwprintf_s(wchar_t * s,
                size_t n,
                const wchar_t * format, ...);
int swprintf_s(wchar_t * s, size_t n,
               const wchar_t * format, ...);
int swscanf_s(const wchar_t * s,
              const wchar_t * format, ...);
int vfwprintf_s(FILE * stream,
                const wchar_t * format,
                va_list arg);
int vfwscanf_s(FILE * stream,
               const wchar_t * format, va_list arg);
int vsnwprintf_s(wchar_t * s,
                 size_t n,
                 const wchar_t * format,
                 va_list arg);
int vswprintf_s(wchar_t * s,
                size_t n,
                const wchar_t * format,
                va_list arg);
int vswscanf_s(const wchar_t * s,
               const wchar_t * format,
               va_list arg);
int vwprintf_s(const wchar_t * format,
               va_list arg);
int vwscanf_s(const wchar_t * format,
              va_list arg);
int wprintf_s(const wchar_t * format, ...);
int wscanf_s(const wchar_t * format, ...);
/*

errno_t wcscpy_s(wchar_t * s1,
                 size_t s1max,
                 const wchar_t * s2);
errno_t wcsncpy_s(wchar_t * s1,
                  size_t s1max,
                  const wchar_t * s2,
                  size_t n);
errno_t wmemcpy_s(wchar_t * s1,
                  size_t s1max,
                  const wchar_t * s2,
                  size_t n);
errno_t wmemmove_s(wchar_t *s1, size_t s1max,
                   const wchar_t *s2, size_t n);
errno_t wcscat_s(wchar_t * s1,
                 size_t s1max,
                 const wchar_t * s2);
errno_t wcsncat_s(wchar_t * s1,
                  size_t s1max,
                  const wchar_t * s2,
                  size_t n);
wchar_t *wcstok_s(wchar_t * s1,
                  size_t * s1max,
                  const wchar_t * s2,
                  wchar_t ** ptr);
size_t wcsnlen_s(const wchar_t *s, size_t maxsize);
errno_t wcrtomb_s(size_t * retval,
                  char * s, size_t smax,
                  wchar_t wc, mbstate_t * ps);
errno_t mbsrtowcs_s(size_t * retval,
                    wchar_t * dst, size_t dstmax,
                    const char ** src, size_t len,
                    mbstate_t * ps);
errno_t wcsrtombs_s(size_t * retval,
                    char * dst, size_t dstmax,
                    const wchar_t ** src, size_t len,
                    mbstate_t * ps);
*/

#endif  // _WCHAR_H
