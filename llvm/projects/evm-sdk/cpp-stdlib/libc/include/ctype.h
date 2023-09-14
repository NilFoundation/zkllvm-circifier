#ifndef _CTYPE_H
#define _CTYPE_H

#define _UPPER      0x0001
#define _LOWER      0x0002
#define _DIGIT      0x0004
#define _SPACE      0x0008
#define _PUNCT      0x0010
#define _CNTRL      0x0020
#define _BLANK      0x0040
#define _XDIGIT     0x0080
#define _ALPHA      (_UPPER | _LOWER)
#define _ALNUM      (_ALPHA | _DIGIT)
#define _GRAPH      (_ALNUM | _PUNCT)
#define _PRINT      (_GRAPH | _BLANK)

#define __isalnum(c)  ((int) (_ptype[(int) (c)] & _ALNUM))
#define __isalpha(c)  ((int) (_ptype[(int) (c)] & _ALPHA))
#define __iscntrl(c)  ((int) (_ptype[(int) (c)] & _CNTRL))
#define __isdigit(c)  ((int) (_ptype[(int) (c)] & _DIGIT))
#define __isgraph(c)  ((int) (_ptype[(int) (c)] & _GRAPH))
#define __islower(c)  ((int) (_ptype[(int) (c)] & _LOWER))
#define __isprint(c)  ((int) (_ptype[(int) (c)] & _PRINT))
#define __ispunct(c)  ((int) (_ptype[(int) (c)] & _PUNCT))
#define __isspace(c)  ((int) (_ptype[(int) (c)] & _SPACE))
#define __isupper(c)  ((int) (_ptype[(int) (c)] & _UPPER))
#define __isxdigit(c) ((int) (_ptype[(int) (c)] & _XDIGIT))
#define __tolower(c)  ((int) (_plmap[(int) (c)]))
#define __toupper(c)  ((int) (_pumap[(int) (c)]))

int isascii (int c);

#include <sys/cdefs.h>

__BEGIN_DECLS

extern const unsigned short int *_ptype;
extern const short int *_plmap;
extern const short int *_pumap;

inline bool isblank(char ch) {
    return ch ==' ' || ch =='\t';
}

int (isalnum)(int);
int (isalpha)(int);
int (iscntrl)(int);
int (isdigit)(int);
int (isgraph)(int);
int (islower)(int);
int (isprint)(int);
int (ispunct)(int);
int (isspace)(int);
int (isupper)(int);
int (isxdigit)(int);
int (tolower)(int);
int (toupper)(int);

#define isalnum_l(c,l)	isalnum (c)
#define isalpha_l(c,l)	isalpha (c)
#define iscntrl_l(c,l)	iscntrl (c)
#define isdigit_l(c,l)	isdigit (c)
#define islower_l(c,l)	islower (c)
#define isgraph_l(c,l)	isgraph (c)
#define isprint_l(c,l)	isprint (c)
#define ispunct_l(c,l)	ispunct (c)
#define isspace_l(c,l)	isspace (c)
#define isupper_l(c,l)	isupper (c)
#define isxdigit_l(c,l)	isxdigit (c)

__END_DECLS

#endif /* _CTYPE_H */
