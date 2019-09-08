/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <time.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hscheduledasynccaller.hxx"
#include "hasynccaller.hxx"
#include "hthreadpool.hxx"
#include "hcore/si.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

HScheduledAsyncCaller::HScheduledAsyncCaller( void )
	: _queue()
	, _mutex()
	, _condition( _mutex )
	, _loop( true ) {
	M_PROLOG
	HThreadPool::get_instance().start_task(
		call( &HScheduledAsyncCaller::run, this ),
		call( &HScheduledAsyncCaller::stop, this ),
		call( &HScheduledAsyncCaller::want_restart, this )
	);
	return;
	M_EPILOG
}

HScheduledAsyncCaller::~HScheduledAsyncCaller( void ) {
	M_PROLOG
	return;
	M_DESTRUCTOR_EPILOG
}

void HScheduledAsyncCaller::stop( void ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _loop ) {
		_loop = false;
		_condition.signal();
	}
	return;
	M_EPILOG
}

bool HScheduledAsyncCaller::want_restart( void ) const {
	return ( true );
}

/*
 * Let `time_point_t' be a number of nanoseconds from UNIX epoch.
 */

void HScheduledAsyncCaller::call_at( yaal::hcore::HTime const& time_, call_t call_ ) {
	M_PROLOG
	HLock l( _mutex );
	i64_t secondsFromUnixEpoch( yaal_epoch_to_unix_epoch( time_.raw() ) );
	_queue.push_back( make_pair( secondsFromUnixEpoch * si::NANO_IN_WHOLE, call_ ) );
	_condition.signal();
	return;
	M_EPILOG
}

void HScheduledAsyncCaller::call_in( yaal::hcore::time::duration_t duration_, call_t call_ ) {
	M_PROLOG
	HLock l( _mutex );
	_queue.push_back( make_pair( ::time( nullptr ) * si::NANO_IN_WHOLE + duration_.raw(), call_ ) );
	_condition.signal();
	return;
	M_EPILOG
}

void HScheduledAsyncCaller::flush( void* invoker_ ) {
	M_PROLOG
	HLock l( _mutex );
	for ( queue_t::iterator it = _queue.begin(); it != _queue.end(); ) {
		if ( (*it).second.id() == invoker_ ) {
			it = _queue.erase( it );
		} else {
			++ it;
		}
	}
	return;
	M_EPILOG
}

void HScheduledAsyncCaller::run( void ) {
	M_PROLOG
	HLock l( _mutex );
	HThread::set_name( "HScheduledAsyncCaller" );
	while ( ! _isKilled_ && _loop ) {
		queue_t::iterator it = _queue.begin();
		i64_t now( ::time( nullptr ) * si::NANO_IN_WHOLE );
		while ( ( it != _queue.end() ) && ( (*it).first <= now ) ) {
			HAsyncCaller::get_instance().register_call( 0, (*it).second );
			it = _queue.erase( it );
		}
		if ( it != _queue.end() ) {
			if ( (*it).first > now ) {
				_condition.wait_for( duration( (*it).first - now, time::UNIT::NANOSECOND ) );
			}
		} else {
			_condition.wait_for( /* ever */ );
		}
	}
	return;
	M_EPILOG
}

int HScheduledAsyncCaller::life_time( int ) {
	return ( HAsyncCaller::life_time( 0 ) - 10 );
}

}

}

