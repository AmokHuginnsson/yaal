/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	system.cxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
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

#include "config.hxx"

#ifdef __HOST_OS_TYPE_FREEBSD__
#include <sys/sysctl.h>
#include <sys/vmmeter.h>
#include <vm/vm_param.h>
#endif /* #ifdef __HOST_OS_TYPE_FREEBSD__ */

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "system.hxx"
#include "algorithm.hxx"
#include "hclock.hxx"
#include "hchunk.hxx"
#include "hfile.hxx"
#include "htokenizer.hxx"

namespace yaal
{

namespace hcore
{

namespace system
{

int close( int fd_ )
	{
	return ( ::close( fd_ ) );
	}

int getpid( void )
	{
	return ( ::getpid() );
	}

int kill( int pid_, int signal_ )
	{
	return ( ::kill( pid_, signal_ ) );
	}

int wait_for_io( int* input_, int inputCount_, int* output_, int outputCount_, int long* timeOut_, bool restartable_ )
	{
	M_ASSERT( ( inputCount_ >= 0 ) && ( outputCount_ >= 0 ) && ( ( inputCount_ + outputCount_ ) > 0 ) );
	M_ASSERT( ! inputCount_ || input_ );
	M_ASSERT( ! outputCount_ || output_ );
	HClock clock;
	fd_set readers;
	fd_set writers;
	if ( inputCount_ )
		FD_ZERO( &readers );
	if ( outputCount_ )
		FD_ZERO( &writers );
/* FD_SET is a macro and first argument is evaluated twice ! */
	for ( int i( 0 ); i < inputCount_; ++ i )
		FD_SET( input_[ i ], &readers );
	for ( int i( 0 ); i < outputCount_; ++ i )
		FD_SET( output_[ i ], &writers );
	timeval timeOut, * timeOutP = timeOut_ ? &timeOut : NULL;
	if ( timeOut_ )
		{
		timeOut.tv_usec = ( *timeOut_ % 1000 ) * 1000;
		timeOut.tv_sec = *timeOut_ / 1000;
		}
	int ret( 0 );
	do
		{
		ret = ::select( FD_SETSIZE, inputCount_ ? &readers : NULL, outputCount_ ? &writers : NULL, NULL, timeOutP );
		}
	while ( restartable_
			&& ( ret == -1 )
			&& ( errno == EINTR )
			&& ( ! timeOut_ || ( clock.get_time_elapsed( HClock::UNIT::MILISECOND ) < *timeOut_ ) ) );
	for ( int i( 0 ); i < inputCount_; ++ i )
		{
		if ( ! FD_ISSET( input_[ i ], &readers ) )
			input_[ i ] = -1;
		}
	for ( int i( 0 ); i < outputCount_; ++ i )
		{
		if ( ! FD_ISSET( output_[ i ], &writers ) )
			output_[ i ] = -1;
		}
	if ( timeOut_ )
		*timeOut_ -= min( *timeOut_, clock.get_time_elapsed( HClock::UNIT::MILISECOND ) );
	return ( ret );
	}

namespace
{

static int const GETPW_R_SIZE   = 1024;
static int const GETGR_R_SIZE   = 1024;

}

HString get_user_name( int uid_ )
	{
	M_PROLOG
	passwd accountInfo;
	int bufferSize( static_cast<int>( ::sysconf( _SC_GETPW_R_SIZE_MAX ) ) );
	if ( bufferSize <= 0 )
		bufferSize = GETPW_R_SIZE;
	HChunk buffer( bufferSize );
	passwd* any( NULL );
	M_ENSURE( ! getpwuid_r( uid_, &accountInfo, buffer.get<char>(), bufferSize, &any ) );
	return ( any ? HString( accountInfo.pw_name ) : HString( uid_ ) );
	M_EPILOG
	}

HString get_group_name( int gid_ )
	{
	M_PROLOG
	group groupInfo;
	int bufferSize( static_cast<int>( ::sysconf( _SC_GETGR_R_SIZE_MAX ) ) );
	if ( bufferSize <= 0 )
		bufferSize = GETGR_R_SIZE;
	HChunk buffer( bufferSize );
	group* any( NULL );
	M_ENSURE( ! getgrgid_r( gid_, &groupInfo, buffer.get<char>(), bufferSize, &any ) );
	return ( any ? HString( groupInfo.gr_name ) : HString( gid_ ) );
	M_EPILOG
	}

int long get_available_memory_size( void )
	{
	M_PROLOG
#if defined ( __HOST_OS_TYPE_LINUX__ ) || defined ( __HOST_OS_TYPE_CYGWIN__ )
	int long availableMemory( 0 );
	try
		{
		HFile meminfo( "/proc/meminfo", HFile::OPEN::READING );
		HString line;
		HTokenizer t( line, ":" );
		HString tokens;
		while ( meminfo.read_line( line ) > 0 )
			{
			t.assign( line );
			if ( t.begin() != t.end() )
				{
				tokens = *t.begin();
				if ( ! strcasecmp( tokens, "MemFree" ) )
					{
					HTokenizer::iterator it( t.begin() );
					++ it;
					if ( it != t.end() )
						{
						tokens = *it;
						tokens.trim();
						availableMemory = lexical_cast<int long>( tokens ) * 1024;
						}
					}
				}
			}
		}
	catch ( HException const& )
		{
		}
	return ( availableMemory );
#elif defined ( __HOST_OS_TYPE_FREEBSD__ ) /* #if defined ( __HOST_OS_TYPE_LINUX__ ) || defined ( __HOST_OS_TYPE_CYGWIN__ ) */
	vmtotal vm;
	int mib[] = { CTL_HW, HW_PAGESIZE };
	int long pagesize( 0 );
	size_t size( sizeof ( pagesize ) );
	M_ENSURE( sysctl( mib, 2, &pagesize, &size, NULL, 0 ) == 0 );
	mib[ 0 ] = CTL_VM;
	mib[ 1 ] = VM_TOTAL;
	size = sizeof ( vmtotal );
	M_ENSURE( sysctl( mib, 2, &vm, &size, NULL, 0 ) == 0 );
	return ( vm.t_free * pagesize );
#elif defined ( __HOST_OS_TYPE_SOLARIS__ ) /* #elif defined ( __HOST_OS_TYPE_FREEBSD__ ) #if defined ( __HOST_OS_TYPE_LINUX__ ) || defined ( __HOST_OS_TYPE_CYGWIN__ ) */
	return ( sysconf( _SC_AVPHYS_PAGES ) * sysconf( _SC_PAGESIZE ) );
#elif defined ( __HOST_OS_TYPE_WINDOWS__ ) /* #elif defined ( __HOST_OS_TYPE_SOLARIS__ ) #elif defined ( __HOST_OS_TYPE_FREEBSD__ ) #if defined ( __HOST_OS_TYPE_LINUX__ ) || defined ( __HOST_OS_TYPE_CYGWIN__ ) */
	return ( ms_get_available_memory_size() );
#else
	return ( 0 );
#endif
	M_EPILOG
	}

int long get_available_disk_space( yaal::hcore::HString const& path_ )
	{
	M_PROLOG
	struct statvfs svfs;
	::memset( &svfs, 0, sizeof ( svfs ) );
	M_ENSURE( ::statvfs( path_.raw(), &svfs ) == 0 );
	return ( svfs.f_bavail * svfs.f_bsize );
	M_EPILOG
	}

}

}

}

