/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  system.cxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <csignal>
#include <cstring>
#include <unistd.h>
#include <sys/time.h>
#include <pwd.h>
#include <grp.h>
#include <sys/statvfs.h>
#include <sys/resource.h>

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
	return ( ::kill( pid_, signal_ ) );
}

namespace {
#pragma GCC diagnostic ignored "-Wold-style-cast"
template<typename T1>
inline void FWD_FD_ZERO( T1 val1_ ) {
	FD_ZERO( val1_ );
}
#pragma GCC diagnostic ignored "-Wsign-conversion"
template<typename T1, typename T2>
inline void FWD_FD_SET( T1 val1_, T2 val2_ ) {
	FD_SET( val1_, val2_ );
}
template<typename T1, typename T2>
inline bool FWD_FD_ISSET( T1 val1_, T2 val2_ ) {
	return ( FD_ISSET( val1_, val2_ ) );
}
#pragma GCC diagnostic error "-Wsign-conversion"
#ifdef __HOST_OS_TYPE_DARWIN__
mach_msg_type_number_t HOST_VM_INFO_COUNT_FWD{ HOST_VM_INFO_COUNT };
#endif /* #ifdef __HOST_OS_TYPE_DARWIN__ */
#pragma GCC diagnostic error "-Wold-style-cast"
}

int wait_for_io( int* input_, int inputCount_, int* output_, int outputCount_, int long* timeOut_, bool restartable_ ) {
	M_ASSERT( ( inputCount_ >= 0 ) && ( outputCount_ >= 0 ) && ( ( inputCount_ + outputCount_ ) > 0 ) );
	M_ASSERT( ! inputCount_ || input_ );
	M_ASSERT( ! outputCount_ || output_ );
	HClock clock;
	fd_set readers;
	fd_set writers;
	if ( inputCount_ )
		FWD_FD_ZERO( &readers );
	if ( outputCount_ )
		FWD_FD_ZERO( &writers );
	for ( int i( 0 ); i < inputCount_; ++ i )
		FWD_FD_SET( input_[ i ], &readers );
	for ( int i( 0 ); i < outputCount_; ++ i )
		FWD_FD_SET( output_[ i ], &writers );
	timeval timeOut, * timeOutP = timeOut_ ? &timeOut : nullptr;
	if ( timeOut_ ) {
		timeOut.tv_usec = static_cast<suseconds_t>( ( *timeOut_ % 1000 ) * 1000 );
		timeOut.tv_sec = *timeOut_ / 1000;
	}
	int ret( 0 );
	HScopedValueReplacement<int> saveErrno( errno, 0 );
	do {
		ret = ::select( FD_SETSIZE, inputCount_ ? &readers : nullptr, outputCount_ ? &writers : nullptr, nullptr, timeOutP );
	} while ( restartable_
			&& ( ret == -1 )
			&& ( errno == EINTR )
			&& ( ! timeOut_ || ( clock.get_time_elapsed( time::UNIT::MILISECOND ) < *timeOut_ ) ) );
	for ( int i( 0 ); i < inputCount_; ++ i ) {
		if ( ! FWD_FD_ISSET( input_[ i ], &readers ) )
			input_[ i ] = -1;
	}
	for ( int i( 0 ); i < outputCount_; ++ i ) {
		if ( ! FWD_FD_ISSET( output_[ i ], &writers ) )
			output_[ i ] = -1;
	}
	if ( timeOut_ )
		*timeOut_ -= min( *timeOut_, static_cast<int long>( clock.get_time_elapsed( time::UNIT::MILISECOND ) ) );
	return ( ret );
}

namespace {

static int const GETPW_R_SIZE   = 1024;
static int const GETGR_R_SIZE   = 1024;

}

HString get_user_name( int uid_ ) {
	M_PROLOG
	passwd accountInfo;
	int bufferSize( static_cast<int>( ::sysconf( _SC_GETPW_R_SIZE_MAX ) ) );
	if ( bufferSize <= 0 )
		bufferSize = GETPW_R_SIZE;
	HChunk buffer( bufferSize );
	passwd* any( nullptr );
	M_ENSURE( ! getpwuid_r( static_cast<uid_t>( uid_ ), &accountInfo, buffer.get<char>(), static_cast<size_t>( bufferSize ), &any ) );
	return ( any ? HString( accountInfo.pw_name ) : HString( uid_ ) );
	M_EPILOG
}

HString get_group_name( int gid_ ) {
	M_PROLOG
	group groupInfo;
	int bufferSize( static_cast<int>( ::sysconf( _SC_GETGR_R_SIZE_MAX ) ) );
	if ( bufferSize <= 0 )
		bufferSize = GETGR_R_SIZE;
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
		while ( ( hit < countof ( tagVars ) ) && ( meminfo.read_line( line ) > 0 ) ) {
			t.assign( line );
			if ( t.begin() != t.end() ) {
				tokens = *t.begin();
				for ( TagVar const& tagVar : tagVars ) {
					if ( ! strcasecmp( tokens, tagVar._tag ) ) {
						HTokenizer::iterator it( t.begin() );
						++ it;
						if ( it != t.end() ) {
							tokens = *it;
							tokens.trim();
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
	freeMemory = vm.t_free * pagesize;
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
	if ( ( rlVM.rlim_cur > 0 ) && ( ( static_cast<i64_t>( rlVM.rlim_cur ) - usedMemory ) < availableMemory ) ) {
		availableMemory = static_cast<i64_t>( rlVM.rlim_cur ) - usedMemory;
	}
#endif /* #if ( HAVE_DECL_RLIMIT_AS == 1 ) */
#ifndef __HOST_OS_TYPE_CYGWIN__
	rlimit rlData = { 0, 0 };
	M_ENSURE( ::getrlimit( RLIMIT_DATA, &rlData ) == 0 );
	if ( ( rlData.rlim_cur > 0 ) && ( ( static_cast<i64_t>( rlData.rlim_cur ) - usedMemory ) < availableMemory ) ) {
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
	M_ENSURE( ::statvfs( path_.raw(), &svfs ) == 0 );
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

}

}

}

