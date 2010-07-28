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

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "system.hxx"
#include "algorithm.hxx"
#include "hclock.hxx"

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

int wait_for_io( int* input_, int const& inputCount_, int* output_, int const& outputCount_, int long* timeOut_, bool restartable_ )
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
		timeOut.tv_usec = *timeOut_ * 1000;
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

}

}

}

