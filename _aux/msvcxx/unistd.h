#ifndef YAAL_MSVCXX_UNISTD_H_INCLUDED
#define YAAL_MSVCXX_UNISTD_H_INCLUDED 1

#define unlink unlink_gnu

#include <glibc/unistd.h>

#define fork ms_fork

#undef unlink

#include "tools/hpipedchild.hxx"

#define ms_fork HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn::create_spawner( a_oImage, a_oArgv, l_piFileDesIn, l_piFileDesOut, l_piFileDesErr )
class HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn
	{
	int* _in;
	int* _out;
	int* _err;
	yaal::hcore::HString _path;
	yaal::tools::HPipedChild::argv_t _argv;
public:
	static HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn create_spawner( yaal::hcore::HString const&, yaal::tools::HPipedChild::argv_t const&, int*, int*, int* );
	HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn( yaal::hcore::HString const&, yaal::tools::HPipedChild::argv_t const&, int*, int*, int* );
	int operator()( void );
	};

#define socket unix_socket
#define bind unix_bind
#define listen unix_listen
#define accept unix_accept
#define connect unix_connect
#define shutdown unix_shutdown
#define setsockopt unix_setsockopt
#define htons unix_htons
#define ntohs unix_ntohs
#define inet_ntop unix_inet_ntop

#endif /* not YAAL_MSVCXX_UNISTD_H_INCLUDED */

