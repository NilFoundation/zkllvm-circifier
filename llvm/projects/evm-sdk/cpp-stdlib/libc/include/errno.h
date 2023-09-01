#ifndef _ERRNO_H
#define _ERRNO_H

#define EDOM    33
#define ERANGE  34

#define errno   _errno

#include <sys/cdefs.h>

__BEGIN_DECLS

extern int _errno;

typedef int errno_t;

__END_DECLS

#endif /* _ERRNO_H */
