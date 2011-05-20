#ifndef YAAL_MSVCXX_EMU_UNISTD_HXX_INCLUDED
#define YAAL_MSVCXX_EMU_UNISTD_HXX_INCLUDED 1

#include "hcore/system.hxx"

namespace msvcxx
{

int close( int const& );
M_YAAL_HCORE_PUBLIC_API int long read( int const&, void*, int long );
M_YAAL_HCORE_PUBLIC_API int long write( int const&, void const*, int long );
M_YAAL_HCORE_PUBLIC_API int pipe( int* );
M_YAAL_HCORE_PUBLIC_API int dup2( int, int );
M_YAAL_HCORE_PUBLIC_API void log_windows_error( char const* );
yaal::hcore::system::HResourceInfo get_memory_size_info( void );

} /* namespace msvcxx */

#define ms_get_memory_size_info msvcxx::get_memory_size_info

#endif /* #ifndef YAAL_MSVCXX_EMU_UNISTD_HXX_INCLUDED */
