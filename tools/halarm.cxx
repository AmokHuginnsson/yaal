/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <ctime>
#include <cstring>
#include <csignal>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "halarm.hxx"
#include "hcore/numeric.hxx"
#include "hcore/si.hxx"

using namespace yaal::hcore;
using namespace yaal::meta;

namespace yaal {

namespace tools {

static_assert( sizeof ( timer_t ) <= sizeof ( HAlarm::timer_handle_t ), "timer handle too big" );

namespace {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
static int const FWD_CLOCK_REALTIME = CLOCK_REALTIME;
#pragma GCC diagnostic pop
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
int sigaddset_fwd( sigset_t* set, int signo ) {
	return ( sigaddset( set, signo ) );
}
#pragma GCC diagnostic pop

static void dummy_signal_handler( int )
	{ }
}

yaal::hcore::HMutex HAlarm::_mutex;

HAlarm::HAlarm( i64_t milliseconds_ )
	: _timer( -1 )
	, _action( chunk_size<struct sigaction>( 1 ) )
	, _lock( _mutex ) {
	M_PROLOG
	sigevent event;
	::memset( &event, 0, sizeof ( event ) );
	event.sigev_notify = SIGEV_SIGNAL;
	event.sigev_signo = SIGALRM;
	timer_t* t( reinterpret_cast<timer_t*>( &_timer ) );
	event.sigev_value.sival_ptr = t;
	M_ENSURE( timer_create( FWD_CLOCK_REALTIME, &event, t ) == 0 );

	int step( 0 );
	try {
		sigset_t mask;
		M_ENSURE( sigemptyset( &mask ) == 0 );
		M_ENSURE( sigaddset_fwd( &mask, SIGALRM ) == 0 );
		M_ENSURE( pthread_sigmask( SIG_UNBLOCK, &mask, nullptr ) == 0 );
		struct sigaction act;
		::memset( &act, 0, sizeof ( act ) );
		act.sa_handler = dummy_signal_handler;
		++ step;
		M_ENSURE( sigaction( SIGALRM, &act, _action.get<struct sigaction>() ) == 0 );
		++ step;

		itimerspec timeout;
		::memset( &timeout, 0, sizeof ( timeout ) );
		timeout.it_value.tv_sec = static_cast<int long>( milliseconds_ / si::MILLI_IN_WHOLE );
		timeout.it_value.tv_nsec = static_cast<int long>( ( milliseconds_ % si::MILLI_IN_WHOLE ) * si::NANO_IN_MILLI );
		M_ENSURE( timer_settime( *t, 0, &timeout, nullptr ) == 0 );
		++ step;
	} catch ( ... ) {
		if ( step > 0 ) {
			cleanup_sigsetup( step );
		}
		M_ENSURE( timer_delete( *t ) == 0 );
		throw;
	}
	return;
	M_EPILOG
}

HAlarm::~HAlarm( void ) {
	M_PROLOG

	itimerspec timeout;
	::memset( &timeout, 0, sizeof ( timeout ) );
	timer_t* t( reinterpret_cast<timer_t*>( &_timer ) );
	M_ENSURE( timer_settime( *t, 0, &timeout, nullptr ) == 0 );

	cleanup_sigsetup( 2 );

	M_ENSURE( timer_delete( *t ) == 0 );
	return;
	M_DESTRUCTOR_EPILOG
}

void HAlarm::cleanup_sigsetup( int step_ ) {
	M_PROLOG
	if ( step_ > 1 ) {
		sigaction( SIGALRM, _action.get<struct sigaction>(), nullptr );
	}
	sigset_t mask;
	M_ENSURE( sigemptyset( &mask ) == 0 );
	M_ENSURE( sigaddset_fwd( &mask, SIGALRM ) == 0 );
	M_ENSURE( pthread_sigmask( SIG_BLOCK, &mask, nullptr ) == 0 );
	return;
	M_EPILOG
}

}

}

