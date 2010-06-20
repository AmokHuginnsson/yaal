#ifndef YAAL_MSVCXX_UNISTD_H_INCLUDED
#define YAAL_MSVCXX_UNISTD_H_INCLUDED 1

#define unlink unlink_gnu

#include <glibc/unistd.h>

#undef unlink

#endif /* not YAAL_MSVCXX_UNISTD_H_INCLUDED */
