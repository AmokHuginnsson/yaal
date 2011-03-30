/*
---           yaal 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	tools/halarm.cxx - this file is integral part of yaal project.

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

#include <ctime>
#include <cstring>
#include <csignal>

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "halarm.hxx"
#include "hcore/numeric.hxx"

using namespace yaal::hcore;
using namespace yaal::meta;

namespace yaal
{

namespace tools
{

yaal::hcore::HMutex HAlarm::_mutex;

HAlarm::HAlarm( int long miliseconds_ )
	: _timer(), _lock( _mutex )
	{
	M_PROLOG
	static int long const MILI_IN_WHOLE = power<10, 3>::value;
	static int long const NANO_IN_MILI = power<10, 6>::value;

	sigevent event;
	::memset( &event, 0, sizeof ( event ) );
	event.sigev_notify = SIGEV_SIGNAL;
	event.sigev_signo = SIGALRM;
	event.sigev_value.sival_ptr = &_timer;
	M_ENSURE( timer_create( CLOCK_REALTIME, &event, &_timer ) == 0 );
	
	sigset_t mask;
	M_ENSURE( sigemptyset( &mask ) == 0 );
	M_ENSURE( sigaddset( &mask, SIGALRM ) == 0 );
	M_ENSURE( pthread_sigmask( SIG_UNBLOCK, &mask, NULL ) == 0 );

	itimerspec timeout;
	::memset( &timeout, 0, sizeof ( timeout ) );
	timeout.it_value.tv_sec = miliseconds_ / MILI_IN_WHOLE;
	timeout.it_value.tv_nsec = ( miliseconds_ % MILI_IN_WHOLE ) * NANO_IN_MILI;
	M_ENSURE( timer_settime( _timer, 0, &timeout, NULL ) == 0 );
	return;
	M_EPILOG
	}

HAlarm::~HAlarm( void )
	{
	M_PROLOG

	itimerspec timeout;
	::memset( &timeout, 0, sizeof ( timeout ) );
	M_ENSURE( timer_settime( _timer, 0, &timeout, NULL ) == 0 );

	sigset_t mask;
	M_ENSURE( sigemptyset( &mask ) == 0 );
	M_ENSURE( sigaddset( &mask, SIGALRM ) == 0 );
	M_ENSURE( pthread_sigmask( SIG_BLOCK, &mask, NULL ) == 0 );

	M_ENSURE( timer_delete( _timer ) == 0 );
	return;
	M_DESTRUCTOR_EPILOG
	}

}

}

