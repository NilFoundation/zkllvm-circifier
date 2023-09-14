#ifndef _STDIO_H
#define _STDIO_H

#define __need_va_list
#define __need_size_t
#define __need_NULL

#include <stdarg.h>
#include <stddef.h>
#include <locale.h>

#define _IOFBF          0
#define _IOLBF          1
#define _IONBF          2

#define BUFSIZ          1024
#define EOF             (-1)
#define FOPEN_MAX       20
#define FILENAME_MAX    260
#define L_tmpnam        255

#define SEEK_SET        0
#define SEEK_CUR        1
#define SEEK_END        2

#define TMP_MAX         32767

#define stdin           _stdin
#define stdout          _stdout
#define stderr          _stderr

#include <sys/cdefs.h>

__BEGIN_DECLS

typedef struct {
    int _file;
    int _flag;
    int _charbuf;
    char *_base;
    char *_end;
    char *_ptr;
    char *_rend;
    char *_wend;
    char *_tmpfname;
} FILE;

__extension__ typedef __int64 fpos_t;

extern FILE *_stdin;
extern FILE *_stdout;
extern FILE *_stderr;

int remove(const char *filename);
int rename(const char *from, const char *to);
FILE *tmpfile(void);
char *tmpnam(char *s);
int fclose(FILE *stream);
int fflush(FILE *stream);
FILE *fopen(const char * filename,
            const char * mode);
FILE *freopen(const char * filename,
              const char * mode,
              FILE * stream);
void setbuf(FILE * stream,
            char * buf);
int setvbuf(FILE * stream,
            char * buf,
            int mode, size_t size);
int fprintf(FILE * stream,
            const char * format, ...);
int fscanf(FILE * stream,
           const char * format, ...);
int printf(const char * format, ...);
int scanf(const char * format, ...);
int snprintf(char * s, size_t n,
             const char * format, ...);
int sprintf(char * s,
            const char * format, ...);
int sscanf(const char * s,
           const char * format, ...);
int vfprintf(FILE * stream,
             const char * format, va_list arg);
int vfscanf(FILE * stream,
            const char * format, va_list arg);
int vprintf(const char * format, va_list arg);
int vscanf(const char * format, va_list arg);
int vsnprintf(char * s, size_t n,
              const char * format, va_list arg);
int vsprintf(char * s,
             const char * format, va_list arg);
int vsscanf(const char * s,
            const char * format, va_list arg);
int fgetc(FILE *stream);
char *fgets(char * s, int n,
            FILE * stream);
int fputc(int c, FILE *stream);
int fputs(const char * s, FILE * stream);
int getc(FILE *stream);
int getchar(void);
int putc(int c, FILE *stream);
int putchar(int c);
int puts(const char *s);
int ungetc(int c, FILE *stream);
size_t fread(void * ptr,
             size_t size, size_t nmemb,
             FILE * stream);
size_t fwrite(const void * ptr,
              size_t size, size_t nmemb,
              FILE * stream);
int fgetpos(FILE * stream,
            fpos_t * pos);
int fseek(FILE *stream, long int offset, int whence);
int fsetpos(FILE *stream, const fpos_t *pos);
long int ftell(FILE *stream);
void rewind(FILE *stream);
void clearerr(FILE *stream);
int feof(FILE *stream);
int ferror(FILE *stream);
void perror(const char *s);

int scanf_l(locale_t , const char * __restrict, ...);
int sprintf_l(char * __restrict, locale_t __restrict, const char * __restrict, ...);
int sscanf_l(const char * __restrict, locale_t __restrict, const char * __restrict, ...);

int	snprintf_l(char * __restrict, size_t, locale_t __restrict, const char * __restrict, ...);
int vfscanf_l(FILE * __restrict, locale_t __restrict, const char * __restrict, va_list);
int vscanf_l(locale_t __restrict, const char * __restrict, va_list);
int vsnprintf_l(char * __restrict, size_t, locale_t __restrict, const char * __restrict, va_list);
int vsscanf_l(const char * __restrict, locale_t __restrict, const char * __restrict, va_list);

int	asprintf_l(char ** __restrict, locale_t __restrict, const char * __restrict, ...);
int vasprintf_l(char ** __restrict, locale_t __restrict, const char * __restrict, va_list);

__END_DECLS

#endif /* _STDIO_H */
