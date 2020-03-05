#ifndef YAAL_MSVCXX_EMU_UNISTD_HXX_INCLUDED
#define YAAL_MSVCXX_EMU_UNISTD_HXX_INCLUDED 1

#include "hcore/pod.hxx"
#include "tools/hpipedchild.hxx"

namespace yaal {
namespace hcore {
class HString;
}
}

namespace msvcxx {

int close( int );
M_YAAL_HCORE_PUBLIC_API int long read( int, void*, int long );
M_YAAL_HCORE_PUBLIC_API int long write( int, void const*, int long );
M_YAAL_HCORE_PUBLIC_API int long sendto( int, void const*, int long, int, sockaddr const*, int );
M_YAAL_HCORE_PUBLIC_API int long recvfrom( int, void*, int long, int, sockaddr*, int* );
M_YAAL_HCORE_PUBLIC_API int pipe( int* );
M_YAAL_HCORE_PUBLIC_API int isatty( int );
M_YAAL_HCORE_PUBLIC_API int dup( int );
M_YAAL_HCORE_PUBLIC_API int dup2( int, int );
M_YAAL_HCORE_PUBLIC_API void log_windows_error( char const* );
M_YAAL_HCORE_PUBLIC_API int lockf( int, int, int long long );
M_YAAL_HCORE_PUBLIC_API int spawn_and_exit( char const*, char const* const* );
void get_memory_size_info( yaal::i64_t&, yaal::i64_t& );
void get_module_file_name( yaal::hcore::HString& );

class M_YAAL_HCORE_PUBLIC_API HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn {
	yaal::hcore::HString _path;
	yaal::tools::HPipedChild::argv_t _argv;
	int _pgid;
	int* _in;
	int* _out;
	int* _err;
	int* _message;
	bool _joinedErr;
public:
	static HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn create_spawner( yaal::hcore::HString const&, yaal::tools::HPipedChild::argv_t const&, int, int*, int*, int*, int*, bool );
	HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn( yaal::hcore::HString const&, yaal::tools::HPipedChild::argv_t const&, int, int*, int*, int*, int*, bool );
	int operator()( void );
};

} /* namespace msvcxx */

#define ms_get_memory_size_info msvcxx::get_memory_size_info
#define ms_get_module_file_name msvcxx::get_module_file_name

#endif /* #ifndef YAAL_MSVCXX_EMU_UNISTD_HXX_INCLUDED */
