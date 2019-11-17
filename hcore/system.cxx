/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstdlib>
#include <csignal>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <pwd.h>
#include <grp.h>
#include <sys/statvfs.h>
#include <sys/resource.h>
#include <sys/stat.h>
#ifdef __HOST_OS_TYPE_CYGWIN__
#include <ctime> /* nanosleep */
#endif /* #ifdef __HOST_OS_TYPE_CYGWIN__ */

#include "config.hxx"

#ifdef __HOST_OS_TYPE_FREEBSD__
#include <sys/sysctl.h>
#include <sys/vmmeter.h>
#include <vm/vm_param.h>
#endif /* #ifdef __HOST_OS_TYPE_FREEBSD__ */
#ifdef __HOST_OS_TYPE_DARWIN__
#include <sys/sysctl.h>
#include <sys/vmmeter.h>
#include <mach/mach_init.h>
#include <mach/mach_host.h>
#include <libproc.h>
#endif /* #ifdef __HOST_OS_TYPE_DARWIN__ */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "system.hxx"
#include "algorithm.hxx"
#include "hclock.hxx"
#include "hchunk.hxx"
#include "hfile.hxx"
#include "htokenizer.hxx"

namespace yaal {

namespace hcore {

namespace system {

int close( int fd_ ) {
	return ( ::close( fd_ ) );
}

int getpid( void ) {
	return ( ::getpid() );
}

int kill( int pid_, int signal_ ) {
	int result( ::kill( pid_, signal_ ) );
#ifdef __HOST_OS_TYPE_CYGWIN__
	/* Work around for buggy child process handling in Cygwin. */
	int retriesLeft( 32 );
	while ( ( retriesLeft -- > 0 ) && ( result == -1 ) && ( ( errno == ESRCH ) || ( errno == EACCES ) ) ) {
		timespec ts{ 0, 1000 * 1000 }; /* sleep for 1 millisecond */
		::nanosleep( &ts, nullptr );
		result = ::kill( pid_, signal_ );
	}
#endif /* #ifdef __HOST_OS_TYPE_CYGWIN__ */
	return ( result );
}

namespace {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
template<typename T1>
inline void FWD_FD_ZERO( T1 val1_ ) {
	FD_ZERO( val1_ );
}
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
template<typename T1, typename T2>
inline void FWD_FD_SET( T1 val1_, T2 val2_ ) {
	FD_SET( val1_, val2_ );
}
template<typename T1, typename T2>
inline bool FWD_FD_ISSET( T1 val1_, T2 val2_ ) {
	return ( FD_ISSET( val1_, val2_ ) );
}
#pragma GCC diagnostic pop
#ifdef __HOST_OS_TYPE_DARWIN__
mach_msg_type_number_t HOST_VM_INFO_COUNT_FWD{ HOST_VM_INFO_COUNT };
#endif /* #ifdef __HOST_OS_TYPE_DARWIN__ */
#pragma GCC diagnostic pop
}

int wait_for_io( int* input_, int inputCount_, int* output_, int outputCount_, int long* timeOut_, bool restartable_ ) {
	M_ASSERT( ( inputCount_ >= 0 ) && ( outputCount_ >= 0 ) && ( ( inputCount_ + outputCount_ ) > 0 ) );
	M_ASSERT( ! inputCount_ || input_ );
	M_ASSERT( ! outputCount_ || output_ );
	HClock clock;
	fd_set readers;
	fd_set writers;
	if ( inputCount_ ) {
		FWD_FD_ZERO( &readers );
	}
	if ( outputCount_ ) {
		FWD_FD_ZERO( &writers );
	}
	for ( int i( 0 ); i < inputCount_; ++ i ) {
		FWD_FD_SET( input_[ i ], &readers );
	}
	for ( int i( 0 ); i < outputCount_; ++ i ) {
		FWD_FD_SET( output_[ i ], &writers );
	}
	timeval timeOut;
	timeval* timeOutP( timeOut_ ? &timeOut : nullptr );
	if ( timeOut_ ) {
		timeOut.tv_usec = static_cast<suseconds_t>( ( *timeOut_ % 1000 ) * 1000 );
		timeOut.tv_sec = *timeOut_ / 1000;
	}
	int ret( 0 );
	i64_t elapsed( 0 );
	do {
		ret = ::select( FD_SETSIZE, inputCount_ ? &readers : nullptr, outputCount_ ? &writers : nullptr, nullptr, timeOutP );
		elapsed = clock.get_time_elapsed( time::UNIT::MILLISECOND );
	} while (
		restartable_
		&& ( ret == -1 )
		&& ( errno == EINTR )
		&& ( ! timeOut_ || ( elapsed < *timeOut_ ) )
	);
	for ( int i( 0 ); i < inputCount_; ++ i ) {
		if ( ! FWD_FD_ISSET( input_[ i ], &readers ) ) {
			input_[ i ] = -1;
		}
	}
	for ( int i( 0 ); i < outputCount_; ++ i ) {
		if ( ! FWD_FD_ISSET( output_[ i ], &writers ) ) {
			output_[ i ] = -1;
		}
	}
	if ( timeOut_ ) {
		*timeOut_ -= min( *timeOut_, static_cast<int long>( clock.get_time_elapsed( time::UNIT::MILLISECOND ) ) );
	}
	return ( ret );
}

user_id_t get_user_id( void ) {
	return ( getuid() );
}

namespace {

static int const GETPW_R_SIZE   = 1024;
static int const GETGR_R_SIZE   = 1024;

HString get_user_name_impl( user_id_t uid_, bool throwOnFailure_ ) {
	M_PROLOG
	passwd accountInfo;
	int bufferSize( static_cast<int>( ::sysconf( _SC_GETPW_R_SIZE_MAX ) ) );
	if ( bufferSize <= 0 ) {
		bufferSize = GETPW_R_SIZE;
	}
	HChunk buffer( bufferSize );
	passwd* any( nullptr );
	M_ENSURE( ! getpwuid_r( static_cast<uid_t>( uid_ ), &accountInfo, buffer.get<char>(), static_cast<size_t>( bufferSize ), &any ) );
	if ( throwOnFailure_ && ! any ) {
		throw HRuntimeException( "Cannot resolve user ID." );
	}
	return ( any ? HString( accountInfo.pw_name ) : HString( uid_ ) );
	M_EPILOG
}

}

HString get_user_name( user_id_t uid_ ) {
	M_PROLOG
	return ( get_user_name_impl( uid_, false ) );
	M_EPILOG
}

user_id_t get_group_id( void ) {
	return ( getgid() );
}

HString get_group_name( user_id_t gid_ ) {
	M_PROLOG
	group groupInfo;
	int bufferSize( static_cast<int>( ::sysconf( _SC_GETGR_R_SIZE_MAX ) ) );
	if ( bufferSize <= 0 ) {
		bufferSize = GETGR_R_SIZE;
	}
	HChunk buffer( bufferSize );
	group* any( nullptr );
	M_ENSURE( ! getgrgid_r( static_cast<gid_t>( gid_ ), &groupInfo, buffer.get<char>(), static_cast<size_t>( bufferSize ), &any ) );
	return ( any ? HString( groupInfo.gr_name ) : HString( gid_ ) );
	M_EPILOG
}

yaal::hcore::HString get_host_name( void ) {
	static int const HOSTNAME_SIZE = 128;
	static HChunk hostname( HOSTNAME_SIZE );
	M_ENSURE( ::gethostname( hostname.get<char>(), HOSTNAME_SIZE - 1 ) == 0 );
	return ( hostname.get<char>() );
}

namespace {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
static i64_t const FWD_RLIM_INFINITY = static_cast<i64_t>( RLIM_INFINITY );
#pragma GCC diagnostic pop
}

HResourceInfo get_memory_size_info( void ) {
	M_PROLOG
	i64_t availableMemory( 0 );
	i64_t freeMemory( 0 );
	i64_t totalMemory( 0 );
	i64_t usedMemory( 0 );
	struct rusage ru;
	::memset( &ru, 0, sizeof ( ru ) );
	M_ENSURE( ::getrusage( RUSAGE_SELF, &ru ) == 0 );
#if defined ( __HOST_OS_TYPE_LINUX__ ) || defined ( __HOST_OS_TYPE_CYGWIN__ )
	usedMemory = ru.ru_maxrss * 1024;
	/* FIXME: sysconf() interface is also available on Linux and is way faster.
	 * We shall make use of it.
	 */
	try {
		HFile meminfo( "/proc/meminfo", HFile::OPEN::READING );
		HString line;
		HTokenizer t( line, ":" );
		HString tokens;
		i64_t cachedMemory( 0 );
		struct TagVar {
			char const* _tag;
			i64_t* _var;
		};
		TagVar tagVars[] = {
			{ "MemFree", &freeMemory },
			{ "MemTotal", &totalMemory },
			{ "Cached", &cachedMemory }
		};
		int hit( 0 );
		while ( ( hit < yaal::size( tagVars ) ) && meminfo.read_line( line ).good() ) {
			t.assign( line );
			if ( t.begin() != t.end() ) {
				tokens = *t.begin();
				for ( TagVar const& tagVar : tagVars ) {
					if ( ! stricasecmp( tokens, tagVar._tag ) ) {
						HTokenizer::iterator it( t.begin() );
						++ it;
						if ( it != t.end() ) {
							tokens = *it;
							tokens.trim( " \t" );
							*(tagVar._var) = lexical_cast<i64_t>( tokens ) * 1024;
							++ hit;
						}
					}
				}
			}
		}
		freeMemory += cachedMemory;
	} catch ( HException const& ) {
	}
#elif defined ( __HOST_OS_TYPE_FREEBSD__ ) /* #if defined ( __HOST_OS_TYPE_LINUX__ ) || defined ( __HOST_OS_TYPE_CYGWIN__ ) */
	usedMemory = ru.ru_maxrss * 1024;
	int mib[] = { CTL_HW, HW_PAGESIZE };
	int long pagesize( 0 );
	size_t size( sizeof ( pagesize ) );
	M_ENSURE( sysctl( mib, 2, &pagesize, &size, nullptr, 0 ) == 0 );
	mib[ 1 ] = HW_PHYSMEM;
	int long physmem( 0 );
	size = sizeof ( physmem );
	M_ENSURE( sysctl( mib, 2, &physmem, &size, nullptr, 0 ) == 0 );
	mib[ 0 ] = CTL_VM;
	mib[ 1 ] = VM_TOTAL;
	vmtotal vm;
	::memset( &vm, 0, sizeof ( vm ) );
	size = sizeof ( vmtotal );
	M_ENSURE( sysctl( mib, 2, &vm, &size, nullptr, 0 ) == 0 );
	freeMemory = static_cast<i64_t>( vm.t_free ) * pagesize;
	totalMemory = physmem;
#elif defined ( __HOST_OS_TYPE_SOLARIS__ ) /* #elif defined ( __HOST_OS_TYPE_FREEBSD__ ) #if defined ( __HOST_OS_TYPE_LINUX__ ) || defined ( __HOST_OS_TYPE_CYGWIN__ ) */
	usedMemory = ru.ru_maxrss * ::getpagesize();
	freeMemory = sysconf( _SC_AVPHYS_PAGES ) * sysconf( _SC_PAGESIZE );
	totalMemory = sysconf( _SC_PHYS_PAGES ) * sysconf( _SC_PAGESIZE );
#elif defined ( __HOST_OS_TYPE_DARWIN__ ) /* #elif defined ( __HOST_OS_TYPE_SOLARIS__ ) #elif defined ( __HOST_OS_TYPE_FREEBSD__ ) #if defined ( __HOST_OS_TYPE_LINUX__ ) || defined ( __HOST_OS_TYPE_CYGWIN__ ) */
	usedMemory = ru.ru_maxrss;
	int mib[] = { CTL_HW, HW_PAGESIZE };
	int long pagesize( 0 );
	size_t size( sizeof ( pagesize ) );
	M_ENSURE( sysctl( mib, 2, &pagesize, &size, nullptr, 0 ) == 0 );
	mib[ 1 ] = HW_MEMSIZE;
	int long physmem( 0 );
	size = sizeof ( physmem );
	M_ENSURE( sysctl( mib, 2, &physmem, &size, nullptr, 0 ) == 0 );
	totalMemory = physmem;
	mach_msg_type_number_t count = HOST_VM_INFO_COUNT_FWD;
	vm_statistics_data_t vmstat;
	kern_return_t kr(
		host_statistics(
			mach_host_self(),
			HOST_VM_INFO,
			reinterpret_cast<host_info_t>( &vmstat ),
			&count
		)
	);
	if ( kr == KERN_SUCCESS ) {
		freeMemory = vmstat.free_count * pagesize;
	} else {
		freeMemory = totalMemory;
	}
#elif defined ( __HOST_OS_TYPE_WINDOWS__ ) /* #elif defined ( __HOST_OS_TYPE_SOLARIS__ ) #elif defined ( __HOST_OS_TYPE_FREEBSD__ ) #if defined ( __HOST_OS_TYPE_LINUX__ ) || defined ( __HOST_OS_TYPE_CYGWIN__ ) */
	usedMemory = ru.ru_maxrss;
	ms_get_memory_size_info( freeMemory, totalMemory );
#endif
	availableMemory = freeMemory;
#if ( HAVE_DECL_RLIMIT_AS == 1 )
	rlimit rlVM = { 0, 0 };
	M_ENSURE( ::getrlimit( RLIMIT_AS, &rlVM ) == 0 );
	if (
		( static_cast<i64_t>( rlVM.rlim_cur ) != FWD_RLIM_INFINITY )
		&& ( ( static_cast<i64_t>( rlVM.rlim_cur ) - usedMemory ) < availableMemory )
	) {
		availableMemory = static_cast<i64_t>( rlVM.rlim_cur ) - usedMemory;
	}
#endif /* #if ( HAVE_DECL_RLIMIT_AS == 1 ) */
#ifndef __HOST_OS_TYPE_CYGWIN__
	rlimit rlData = { 0, 0 };
	M_ENSURE( ::getrlimit( RLIMIT_DATA, &rlData ) == 0 );
	if (
		( static_cast<i64_t>( rlData.rlim_cur ) != FWD_RLIM_INFINITY )
		&& ( ( static_cast<i64_t>( rlData.rlim_cur ) - usedMemory ) < availableMemory )
	) {
		availableMemory = static_cast<i64_t>( rlData.rlim_cur ) - usedMemory;
	}
#endif /* #ifndef __HOST_OS_TYPE_CYGWIN__ */
	return ( HResourceInfo( availableMemory, freeMemory, totalMemory ) );
	M_EPILOG
}

HResourceInfo get_disk_space_info( yaal::hcore::HString const& path_ ) {
	M_PROLOG
	struct statvfs svfs;
	::memset( &svfs, 0, sizeof ( svfs ) );
	HUTF8String utf8( path_ );
	M_ENSURE( ::statvfs( utf8.c_str(), &svfs ) == 0, path_ );
	return ( HResourceInfo( static_cast<i64_t>( svfs.f_bavail ) * static_cast<i64_t>( svfs.f_frsize ),
				static_cast<i64_t>( svfs.f_bfree ) * static_cast<i64_t>( svfs.f_frsize ),
				static_cast<i64_t>( svfs.f_blocks ) * static_cast<i64_t>( svfs.f_frsize ) ) );
	M_EPILOG
}

int get_core_count_info( void ) {
	M_PROLOG
	return ( static_cast<int>( ::sysconf( _SC_NPROCESSORS_ONLN ) ) );
	M_EPILOG
}

int get_page_size( void ) {
	M_PROLOG
	return ( static_cast<int>( ::sysconf( _SC_PAGESIZE ) ) );
	M_EPILOG
}

yaal::hcore::HString get_self_exec_path( void ) {
	M_PROLOG
	HString path;
#if defined ( __HOST_OS_TYPE_LINUX__ ) || defined ( __HOST_OS_TYPE_CYGWIN__ )
	static int const MAX_PATH_LEN( 4096 );
	char buf[MAX_PATH_LEN + 1] = { 0 };
	static_cast<void>( readlink( "/proc/self/exe", buf, sizeof ( buf ) ) == 0 );
	path = buf;
#endif
#ifdef __HOST_OS_TYPE_FREEBSD__
	static int const MAX_PATH_LEN( 4096 );
	int mib[] = { CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1 };
	char buf[MAX_PATH_LEN];
	size_t cb = sizeof ( buf );
	sysctl( mib, 4, buf, &cb, NULL, 0 );
	path = buf;
#endif
#ifdef __HOST_OS_TYPE_DARWIN__
	char buf[PROC_PIDPATHINFO_MAXSIZE];
	if ( proc_pidpath( getpid(), buf, sizeof ( buf ) ) == 0 ) {
		path = buf;
	}
#endif
#ifdef __HOST_OS_TYPE_SOLARIS__
	path = getexecname();
#endif
#ifdef __HOST_OS_TYPE_WINDOWS__
	ms_get_module_file_name( path );
#endif
	return ( path );
	M_EPILOG
}

void set_close_on_exec( int fd_ ) {
	M_PROLOG
	int fdFlags( ::fcntl( fd_, F_GETFD, 0 ) );
	M_ENSURE( fdFlags >= 0 );
	M_ENSURE( ::fcntl( fd_, F_SETFD, fdFlags | FD_CLOEXEC ) == 0 );
	return;
	M_EPILOG
}

yaal::hcore::HString home_path( void ) {
	char const* const homePath( ::getenv( HOME_ENV_VAR ) );
	if ( homePath ) {
		return ( homePath );
	}
	char const* loginFromEnv( ::getenv( "LOGIN" ) );
	if ( ! loginFromEnv ) {
		loginFromEnv = ::getenv( "USERNAME" );
	}
	if ( ! loginFromEnv ) {
		loginFromEnv = ::getenv( "USER" );
	}
	if ( ! loginFromEnv ) {
		loginFromEnv = ::getenv( "LOGNAME" );
	}
	HString login;
	if ( loginFromEnv ) {
		login.assign( loginFromEnv );
	} else {
		try {
			login.assign( get_user_name_impl( get_user_id(), true ) );
		} catch ( HException const& ) {
			return ( HString() );
		}
	}
	HString hp( "/home/" );
	hp.append( login );
	return ( hp );
}

mode_t get_umask( void ) {
	mode_t const lockOutUmask( S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH );
	mode_t curUmask( static_cast<mode_t>( ::umask( lockOutUmask ) ) );
	M_ENSURE( static_cast<mode_t>( ::umask( static_cast<::mode_t>( curUmask ) ) ) == lockOutUmask );
	return ( static_cast<mode_t>( curUmask ) );
}

void set_umask( mode_t umask_ ) {
	::umask( static_cast<::mode_t>( umask_ ) );
	return;
}

inline int resource_limit_type( RESOURCE_LIMIT_TYPE resourceLimitType_ ) {
	int rlt( 0 );
	switch ( resourceLimitType_ ) {
		case ( RESOURCE_LIMIT_TYPE::MEMORY_SIZE ):   rlt = RLIMIT_DATA;   break;
		case ( RESOURCE_LIMIT_TYPE::STACK_SIZE ):    rlt = RLIMIT_STACK;  break;
		case ( RESOURCE_LIMIT_TYPE::CPU_TIME ):      rlt = RLIMIT_CPU;    break;
		case ( RESOURCE_LIMIT_TYPE::OPEN_FILES ):    rlt = RLIMIT_NOFILE; break;
		case ( RESOURCE_LIMIT_TYPE::FILE_SIZE ):     rlt = RLIMIT_FSIZE;  break;
		case ( RESOURCE_LIMIT_TYPE::PROCESS_COUNT ): rlt = RLIMIT_NPROC;  break;
		case ( RESOURCE_LIMIT_TYPE::CORE_SIZE ):     rlt = RLIMIT_CORE;   break;
	}
	return ( rlt );
}

HResourceLimit get_limit( RESOURCE_LIMIT_TYPE resourceLimitType_ ) {
	M_PROLOG
	rlimit rl = { 0, 0 };
	M_ENSURE( ::getrlimit( resource_limit_type( resourceLimitType_ ), &rl ) == 0 );
	return (
		HResourceLimit(
			rl.rlim_cur != RLIM_INFINITY ? static_cast<i64_t>( rl.rlim_cur ) : HResourceLimit::UNLIMITED,
			rl.rlim_max != RLIM_INFINITY ? static_cast<i64_t>( rl.rlim_max ) : HResourceLimit::UNLIMITED
		)
	);
	M_EPILOG
}

void set_limit( RESOURCE_LIMIT_TYPE resourceLimitType_, HResourceLimit const& resourceLimit_ ) {
	M_PROLOG
	rlimit rl = {
		resourceLimit_.soft() != HResourceLimit::UNLIMITED ? static_cast<rlim_t>( resourceLimit_.soft() ) : RLIM_INFINITY,
		resourceLimit_.hard() != HResourceLimit::UNLIMITED ? static_cast<rlim_t>( resourceLimit_.hard() ) : RLIM_INFINITY
	};
	M_ENSURE( ::setrlimit( resource_limit_type( resourceLimitType_ ), &rl ) == 0 );
	return;
	M_EPILOG
}

}

}

}

