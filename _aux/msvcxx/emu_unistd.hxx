#ifndef YAAL_MSVCXX_EMU_UNISTD_HXX_INCLUDED
#define YAAL_MSVCXX_EMU_UNISTD_HXX_INCLUDED 1

#include "config.hxx"
#include "hcore/pod.hxx"

namespace msvcxx {

int close( int const& );
M_YAAL_HCORE_PUBLIC_API int long read( int, void*, int long );
M_YAAL_HCORE_PUBLIC_API int long write( int, void const*, int long );
M_YAAL_HCORE_PUBLIC_API int long sendto( int, void const*, int long, int, sockaddr const*, int );
M_YAAL_HCORE_PUBLIC_API int long recvfrom( int, void*, int long, int, sockaddr*, int* );
M_YAAL_HCORE_PUBLIC_API int pipe( int* );
M_YAAL_HCORE_PUBLIC_API int isatty( int );
M_YAAL_HCORE_PUBLIC_API int dup2( int, int );
M_YAAL_HCORE_PUBLIC_API void log_windows_error( char const* );
M_YAAL_HCORE_PUBLIC_API int lockf( int, int, int long long );
void get_memory_size_info( yaal::i64_t&, yaal::i64_t& );

} /* namespace msvcxx */

#define ms_get_memory_size_info msvcxx::get_memory_size_info

#endif /* #ifndef YAAL_MSVCXX_EMU_UNISTD_HXX_INCLUDED */
