/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hscheduledasynccaller.cxx - this file is integral part of `yaal' project.

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

#include <time.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hscheduledasynccaller.hxx"
#include "hasynccaller.hxx"
#include "hthreadpool.hxx"

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

void HScheduledAsyncCaller::register_call( priority_t prio, call_t call ) {
	M_PROLOG
	HLock l( _mutex );
	_queue.push_back( make_pair( prio, call ) );
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
		while ( ( it != _queue.end() ) && ( (*it).first <= time( NULL ) ) ) {
			HAsyncCaller::get_instance().register_call( 0, (*it).second );
			it = _queue.erase( it );
		}
		if ( it != _queue.end() ) {
			time_t delay = 0;
			if ( ( delay = ( (*it).first - time( NULL ) ) ) > 0 ) {
				_condition.wait( static_cast<int long unsigned>( delay ), 0 );
			}
		} else {
			_condition.wait( 10000, 0 );
		}
	}
	return;
	M_EPILOG
}

int HScheduledAsyncCaller::life_time( int ) {
	return ( HAsyncCaller::life_time( 0 ) + 10 );
}

}

}

