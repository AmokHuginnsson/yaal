#ifndef YAAL_MSVCXX_EMU_UNISTD_HXX_INCLUDED
#define YAAL_MSVCXX_EMU_UNISTD_HXX_INCLUDED 1

#include "config.hxx"
#include "hcore/pod.hxx"

namespace msvcxx
{

int close( int const& );
M_YAAL_HCORE_PUBLIC_API int long read( int const&, void*, int long );
M_YAAL_HCORE_PUBLIC_API int long write( int const&, void const*, int long );
M_YAAL_HCORE_PUBLIC_API int pipe( int* );
M_YAAL_HCORE_PUBLIC_API int dup2( int, int );
M_YAAL_HCORE_PUBLIC_API void log_windows_error( char const* );
void get_memory_size_info( i64_t&, i64_t& );

} /* namespace msvcxx */

#define ms_get_memory_size_info msvcxx::get_memory_size_info

#endif /* #ifndef YAAL_MSVCXX_EMU_UNISTD_HXX_INCLUDED */
