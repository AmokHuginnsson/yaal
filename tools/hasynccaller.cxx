/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hasynccaller.cxx - this file is integral part of `yaal' project.

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

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hasynccaller.hxx"
#include "hthreadpool.hxx"
#include "hcore/system.hxx"

using namespace yaal::hcore;

namespace yaal {

namespace tools {

HAsyncCaller::HAsyncCaller( void )
	: _queue()
	, _syncOn( 0 )
	, _semaphore()
	, _mutex() {
	M_PROLOG
	HThreadPool& tp( HThreadPool::get_instance() );
	for ( int i( 0 ); i < system::get_core_count_info(); ++ i ) {
		tp.start_task(
			call( &HAsyncCaller::run, this ),
			call( &HAsyncCaller::stop, this ),
			call( &HAsyncCaller::want_restart, this )
		);
	}
	return;
	M_EPILOG
}

HAsyncCaller::~HAsyncCaller( void ) {
	M_PROLOG
	stop();
	return;
	M_DESTRUCTOR_EPILOG
}

void HAsyncCaller::register_call( priority_t prio_, call_t call_ ) {
	M_PROLOG
	HLock l( _mutex );
	_queue.push_back( make_pair( prio_, call_ ) );
	_semaphore.signal();
	return;
	M_EPILOG
}

void HAsyncCaller::run( void ) {
	M_PROLOG
	HThread::set_name( "HAsyncCaller" );
	/* _syncOn lock scope */ {
		HLock l( _mutex );
		++ _syncOn;
	}
	while ( true ) {
		_semaphore.wait();
		HLock l( _mutex );
		if ( ( _syncOn <= 0 ) || _queue.is_empty() ) {
			break;
		}
		queue_t::iterator it = _queue.begin();
		call_t c( (*it).second );
		_queue.erase( it );
		l.unlock();
		c();
	}
	return;
	M_EPILOG
}

void HAsyncCaller::stop( void ) {
	M_PROLOG
	HLock l( _mutex );
	-- _syncOn;
	return;
	M_EPILOG
}

bool HAsyncCaller::want_restart( void ) const {
	return ( true );
}

int HAsyncCaller::life_time( int ) {
	return ( HThreadPool::life_time( 0 ) + 10 );
}

}

}

