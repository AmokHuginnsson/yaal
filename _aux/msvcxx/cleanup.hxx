/* No header guards cause cleanup may by necessary multiple times. */

#undef close
#undef CALLBACK
#undef ERROR
#undef VOID
#undef CHAR
#undef STRICT
#undef OPTIONAL
#undef TRUE
#undef FALSE
#undef ALTERNATE
#undef OK
#undef min
#undef max
#undef PARITY_ODD
#undef DELETE
#undef INFINITY
#undef OUT
#undef ERR
#undef clear
#undef erase
#undef move
#undef write
#undef read
#undef sendto
#undef recvfrom
#undef interface

#ifndef YAAL_MSVCXX_CLEANUP_HXX_INCLUDED
#define YAAL_MSVCXX_CLEANUP_HXX_INCLUDED

static int const OK = 0;
static int const ERR = -1;
#define strtoll _strtoi64
#define strtoull _strtoui64

#endif /* not YAAL_MSVCXX_CLEANUP_HXX_INCLUDED */
