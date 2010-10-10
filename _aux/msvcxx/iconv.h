#ifndef YAAL_MSVCXX_ICONV_H_INCLUDED
#define YAAL_MSVCXX_ICONV_H_INCLUDED 1

#include <../include/iconv.h>

extern int* iconv_errno( void );
#undef errno
#define errno *iconv_errno()

#define YAAL_MSVCXX_ERRNO_HXX_INCLUDED 1

#endif /* #ifndef YAAL_MSVCXX_ICONV_H_INCLUDED */

