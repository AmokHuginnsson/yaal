#ifndef YAAL_MSVCXX_UNISTD_H_INCLUDED
#define YAAL_MSVCXX_UNISTD_H_INCLUDED 1

#define close close_off
#include <cstdio>
#include <cerrno>
#define unlink unlink_gnu
#undef tmpfile
extern "C" FILE* tmpfile (void);

#undef EDEADLOCK
#define select select_off
#include <glibc/unistd.h>
#include <libintl.h>
#undef select
#undef fprintf
#undef close

#undef gethostname
#define gethostname ms_gethostname
int ms_gethostname( char*, int );

#define fork ms_fork

#undef unlink

#include "tools/hpipedchild.hxx"

#define ms_fork HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn::create_spawner( image_, argv_, fileDesIn, fileDesOut, fileDesErr )
class M_YAAL_HCORE_PUBLIC_API HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn
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

int unix_close( int const& );

template<typename T>
inline int close( T const& fd_ )
	{ return ( unix_close( fd_ ) ); }

#endif /* not YAAL_MSVCXX_UNISTD_H_INCLUDED */

