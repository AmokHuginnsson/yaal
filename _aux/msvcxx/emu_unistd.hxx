#ifndef YAAL_MSVCXX_EMU_UNISTD_HXX_INCLUDED
#define YAAL_MSVCXX_EMU_UNISTD_HXX_INCLUDED 1

namespace msvcxx
{

char const* windows_strerror( int );
void log_windows_error( char const* );

} /* namespace msvcxx */

#endif /* #ifndef YAAL_MSVCXX_EMU_UNISTD_HXX_INCLUDED */
