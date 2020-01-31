/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hasynccaller.hxx"
#include "hthreadpool.hxx"
#include "hcore/system.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

HAsyncCaller::HAsyncCaller( void )
	: _queue()
	, _stopRequests( 0 )
	, _semaphore()
	, _mutex() {
	M_PROLOG
	HThreadPool& tp( HThreadPool::get_instance() );
	for ( int i( 0 ); i < system::get_core_count_info(); ++ i ) {
		tp.schedule_task(
			HWorkFlow::SCHEDULE_POLICY::EAGER,
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
	while ( ! _isKilled_ ) {
		_semaphore.wait();
		HLock l( _mutex );
		if ( _stopRequests > 0 ) {
			-- _stopRequests;
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
	++ _stopRequests;
	_semaphore.signal();
	return;
	M_EPILOG
}

bool HAsyncCaller::want_restart( void ) const {
	return ( true );
}

int HAsyncCaller::life_time( int ) {
	return ( HThreadPool::life_time( 0 ) - 20 );
}

}

}

