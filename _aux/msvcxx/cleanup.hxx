/* No header guards cause cleanup may by necessary multiple times. */

#undef close
#undef CALLBACK
#undef ERROR
#undef VOID
#undef CHAR
#undef STRICT
#undef OPTIONAL
#undef TRUE
#undef ALTERNATE
#undef OK
#undef min
#undef max
#undef PARITY_ODD
#undef DELETE
#undef OUT
#undef ERR
#undef clear
#undef erase
#undef move
#undef STRICT

#ifndef YAAL_MSVCXX_CLEANUP_HXX_INCLUDED
#define YAAL_MSVCXX_CLEANUP_HXX_INCLUDED

static int const OK = 0;
static int const ERR = -1;

#endif /* not YAAL_MSVCXX_CLEANUP_HXX_INCLUDED */