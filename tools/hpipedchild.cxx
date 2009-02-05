/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hpipedchild.cxx - this file is integral part of `yaal' project.

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

#include <cstdio>
#include <cstdlib>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h> /* for FreeBSD */

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "hpipedchild.hxx"
#include "hfsitem.hxx"

using namespace yaal::hcore;

namespace yaal
{

namespace tools
{

HPipedChild::HPipedChild( void )
	: HStreamInterface(), f_iPid( 0 ),
	f_iPipeIn( -1 ), f_iPipeOut( -1 ), f_iPipeErr( -1 ),
	f_eCSOI( STREAM::D_OUT ), f_oSecondLineCache( f_oCache ), f_iSecondLineOffset( f_iOffset )
	{
	return;
	}

HPipedChild::~HPipedChild( void )
	{
	M_PROLOG
	if ( f_iPid > 0 )
		finish();
	return;
	M_EPILOG
	}

HPipedChild::STATUS HPipedChild::finish( void )
	{
	M_PROLOG
	if ( f_iPipeErr >= 0 )
		TEMP_FAILURE_RETRY( ::close( f_iPipeErr ) );
	f_iPipeErr = -1;
	if ( f_iPipeOut >= 0 )
		TEMP_FAILURE_RETRY( ::close( f_iPipeOut ) );
	f_iPipeOut = -1;
	if ( f_iPipeIn >= 0 )
		TEMP_FAILURE_RETRY( ::close( f_iPipeIn ) );
	f_iPipeIn = -1;
	STATUS s;
	if ( f_iPid > 0 )
		{
		::kill( f_iPid, SIGKILL );
		int status = 0;
		::waitpid( f_iPid, &status, 0 );
		if ( WIFEXITED( status ) )
			{
			s.type = STATUS::TYPE::D_NORMAL;
			s.value = WEXITSTATUS( status );
			}
		else if ( WIFSIGNALED( status ) )
			{
			s.type = STATUS::TYPE::D_ABORT;
			s.value = WTERMSIG( status );
			}
		}
	f_iPid = 0;
	return ( s );
	M_EPILOG
	}

void HPipedChild::spawn( HString const& a_oImage, argv_t const& a_oArgv )
	{
	M_PROLOG
	int l_piFileDesIn[ 2 ], l_piFileDesOut[ 2 ], l_piFileDesErr[ 2 ];
	HFSItem image( a_oImage );
	M_ENSURE( !! image && image.is_executable() );
	if ( ::pipe( l_piFileDesIn ) || ::pipe ( l_piFileDesOut ) || ::pipe( l_piFileDesErr ) )
		M_THROW( "pipe", errno );
	f_iPid = ::fork();
	if ( f_iPid < 0 )
		M_THROW( "fork", errno );
	if ( ! f_iPid )
		{
		if ( TEMP_FAILURE_RETRY( ::close( l_piFileDesIn[ 1 ] ) )
				|| TEMP_FAILURE_RETRY( ::close( l_piFileDesOut[ 0 ] ) )
				|| TEMP_FAILURE_RETRY( ::close( l_piFileDesErr[ 0 ] ) ) )
			M_THROW( "close", errno );
		if ( ( ::dup2( l_piFileDesIn[ 0 ], fileno( stdin ) ) < 0 )
				|| ( ::dup2( l_piFileDesOut[ 1 ], fileno( stdout ) ) < 0 )
				|| ( ::dup2( l_piFileDesErr[ 1 ], fileno( stderr ) ) < 0 ) )
			M_THROW( "dup2", errno );
		char** argv = xcalloc<char*>( a_oArgv.size() + 2 );
		argv[ 0 ] = xstrdup( a_oImage.raw() );
		int i = 1;
		for ( argv_t::const_iterator it = a_oArgv.begin(); it != a_oArgv.end(); ++ it, ++ i )
			argv[ i ] = xstrdup( it->raw() );
		::execv( a_oImage.raw(), argv );
		M_THROW( "execlp", errno );
		}
	else
		{
		if ( TEMP_FAILURE_RETRY( ::close( l_piFileDesIn[ 0 ] ) )
				|| TEMP_FAILURE_RETRY( ::close( l_piFileDesOut[ 1 ] ) )
				|| TEMP_FAILURE_RETRY( ::close( l_piFileDesErr[ 1 ] ) ) )
			M_THROW( "close", errno );
		f_iPipeIn = l_piFileDesIn[ 1 ];
		f_iPipeOut = l_piFileDesOut[ 0 ];
		f_iPipeErr = l_piFileDesErr[ 0 ];
		}
	return;
	M_EPILOG
	}

int long HPipedChild::do_read( void* const a_pcBuffer, int long const& a_lSize )
	{
	M_PROLOG
	M_ASSERT( ( f_iPipeOut >= 0 ) && ( f_iPipeErr >= 0 ) );
	int fd = ( ( f_eCSOI == STREAM::D_OUT ) ? f_iPipeOut : f_iPipeErr );
	return ( ::read( fd, a_pcBuffer, a_lSize ) );
	M_EPILOG
	}

int long HPipedChild::do_write( void const* const a_pcString, int long const& a_lSize )
	{
	M_PROLOG
	M_ASSERT( f_iPipeIn >= 0 );
	int long iWritten = 0;
	do
		{
		iWritten += TEMP_FAILURE_RETRY( ::write( f_iPipeIn,
					static_cast<char const* const>( a_pcString ) + iWritten,
					a_lSize - iWritten ) );
		}
	while ( iWritten < a_lSize );
	return ( iWritten );
	M_EPILOG
	}

void HPipedChild::do_flush( void ) const
	{
	}

bool HPipedChild::read_poll( void* a_pvTime )
	{
	M_PROLOG
	int l_iError = - 1;
	fd_set l_xFdSet;
	timeval* l_pxWait = static_cast<timeval*>( a_pvTime );
	int fd = ( ( f_eCSOI == STREAM::D_OUT ) ? f_iPipeOut : f_iPipeErr );
	do
		{
		FD_ZERO( &l_xFdSet );
		FD_SET( fd, &l_xFdSet );
		l_iError = ::select( FD_SETSIZE, &l_xFdSet, NULL, NULL, l_pxWait );
		if ( l_iError < 0 )
			break;
		else if ( ( l_iError > 0 )
				&& FD_ISSET( fd, &l_xFdSet ) )
			break;
		else if ( ! l_iError )
			break;
		}
	while ( l_pxWait->tv_sec || l_pxWait->tv_usec );
	return ( l_iError <= 0 );
	M_EPILOG
	}

bool HPipedChild::is_running( void )
	{
	return ( f_iPid > 0 );
	}

void HPipedChild::set_csoi( STREAM::stream_t const& a_eCSOI )
	{
	M_PROLOG
	M_ASSERT( ( a_eCSOI == STREAM::D_OUT ) || ( a_eCSOI == STREAM::D_ERR ) );
	if ( a_eCSOI != f_eCSOI )
		{
		using yaal::swap;
		swap( f_iOffset, f_iSecondLineOffset );
		swap( f_oCache, f_oSecondLineCache );
		}
	f_eCSOI = a_eCSOI;
	return;
	M_EPILOG
	}

}

}

