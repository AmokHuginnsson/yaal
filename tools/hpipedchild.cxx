/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hpipedchild.cxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <cstdio>
#include <cstdlib>
#include <sys/wait.h>
#include <unistd.h>

#include "hcore/hexception.h"
M_VCSID ( "$Id$" )
#include "hpipedchild.h"

namespace yaal
{

namespace tools
{

HPipedChild::HPipedChild( void )
	: HStreamInterface(), f_iPid( 0 ), f_iPipeIn( -1 ), f_iPipeOut( -1 )
	{
	return;
	}

HPipedChild::~HPipedChild( void )
	{
	if ( f_iPid > 0 )
		finish();
	return;
	}

void HPipedChild::finish( void )
	{
	if ( f_iPipeOut >= 0 )
		::close( f_iPipeOut );
	f_iPipeOut = -1;
	if ( f_iPipeIn >= 0 )
		::close( f_iPipeIn );
	f_iPipeIn = -1;
	if ( f_iPid > 0 )
		{
		::kill( f_iPid, SIGKILL );
		::waitpid( f_iPid, NULL, 0 );
		}
	f_iPid = 0;
	}

void HPipedChild::spawn( char const* const a_pcImage )
	{
	M_PROLOG
	int l_piFileDesLeft [ 2 ], l_piFileDesRight [ 2 ];
	if ( ::pipe( l_piFileDesLeft ) || pipe ( l_piFileDesRight ) )
		M_THROW ( "pipe", errno );
	f_iPid = ::fork();
	if ( f_iPid < 0 )
		M_THROW( "fork", errno );
	if ( ! f_iPid )
		{
		if ( ::close( l_piFileDesLeft [ 0 ] ) || ::close( l_piFileDesRight [ 1 ] ) )
			M_THROW( "close", errno );
		if ( ( ::dup2( l_piFileDesRight [ 0 ], ::fileno( stdin ) ) < 0 )
				|| ( ::dup2( l_piFileDesLeft [ 1 ], ::fileno( stdout ) ) < 0 ) )
			M_THROW( "dup2", errno );
		::execl( a_pcImage, a_pcImage, NULL );
		M_THROW( "execlp", errno );
		}
	else
		{
		if ( ::close( l_piFileDesLeft [ 1 ] ) || ::close( l_piFileDesRight [ 0 ] ) )
			M_THROW( "close", errno );
		f_iPipeIn = l_piFileDesLeft [ 0 ];
		f_iPipeOut = l_piFileDesRight [ 1 ];
		}
	return;
	M_EPILOG
	}

int HPipedChild::do_read( void* const a_pcBuffer, int const a_iSize )
	{
	M_PROLOG
	M_ASSERT( f_iPipeIn >= 0 );
	return ( ::read( f_iPipeIn, a_pcBuffer, a_iSize ) );
	M_EPILOG
	}

int HPipedChild::do_write( void const* const a_pcString, int const a_iSize )
	{
	M_PROLOG
	M_ASSERT( f_iPipeOut >= 0 );
	return ( ::write( f_iPipeOut, a_pcString, a_iSize ) );
	M_EPILOG
	}

void HPipedChild::do_flush( void ) const
	{
	}

bool HPipedChild::read_poll( void* a_pvTime )
	{
	int l_iError = - 1;
	fd_set l_xFdSet;
	timeval* l_pxWait = static_cast<timeval*>( a_pvTime );
	while ( l_pxWait->tv_sec || l_pxWait->tv_usec )
		{
		FD_ZERO( & l_xFdSet );
		FD_SET( f_iPipeIn, &l_xFdSet );
		l_iError = ::select( FD_SETSIZE, &l_xFdSet, NULL, NULL, l_pxWait );
		if ( l_iError < 0 )
			break;
		else if ( ( l_iError > 0 )
				&& FD_ISSET( f_iPipeIn, &l_xFdSet ) )
			break;
		else if ( ! l_iError )
			break;
		}
	return ( l_iError <= 0 );
	}

bool HPipedChild::is_running( void )
	{
	return ( f_iPid > 0 );
	}

}

}

