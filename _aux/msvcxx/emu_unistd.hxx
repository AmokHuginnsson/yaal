#ifndef YAAL_MSVCXX_EMU_UNISTD_HXX_INCLUDED
#define YAAL_MSVCXX_EMU_UNISTD_HXX_INCLUDED 1

namespace msvcxx
{

int close( int const& );
M_YAAL_HCORE_PUBLIC_API int long read( int const&, void*, int long );
M_YAAL_HCORE_PUBLIC_API int long write( int const&, void const*, int long );
M_YAAL_HCORE_PUBLIC_API int pipe( int* );
M_YAAL_HCORE_PUBLIC_API int dup2( int, int );
M_YAAL_HCORE_PUBLIC_API void log_windows_error( char const* );
int long get_available_memory_size( void );

} /* namespace msvcxx */

#define ms_get_available_memory_size msvcxx::get_available_memory_size

#endif /* #ifndef YAAL_MSVCXX_EMU_UNISTD_HXX_INCLUDED */
