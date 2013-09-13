/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hworkflow.cxx - this file is integral part of `yaal' project.

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
#include "hworkflow.hxx"
#include "hcore/memory.hxx"

using namespace yaal::hcore;

namespace yaal {

namespace tools {

HWorkFlowInterface::task_t HWorkFlowInterface::pop_task( void ) {
	M_PROLOG
	return ( do_pop_task() );
	M_EPILOG
}

HWorkFlow::HWorkFlow( int workerPoolSize_ )
	: _workerPoolSize( workerPoolSize_ ), _activeWorkers( 0 ),
	_busyWorkers( 0 ), _pool(), _queue(), _semaphore(), _mutex() {
	M_PROLOG
	M_ASSERT( _workerPoolSize > 0 );
	return;
	M_EPILOG
}

HWorkFlow::~HWorkFlow( void ) {
	M_PROLOG
	for ( pool_t::iterator it( _pool.begin() ), end( _pool.end() ); it != end; ++ it )
		_semaphore.signal();
	for ( pool_t::iterator it( _pool.begin() ), end( _pool.end() ); it != end; ++ it )
		(*it)->finish();
	return;
	M_DESTRUCTOR_EPILOG
}

void HWorkFlow::push_task( task_t call_ ) {
	M_PROLOG
	HLock l( _mutex );
	M_ASSERT( _busyWorkers <= _activeWorkers );
	if ( ( _busyWorkers == _activeWorkers ) && ( _activeWorkers < _workerPoolSize ) ) {
		worker_ptr_t w( make_pointer<HWorker>( this ) );
		_pool.push_back( w );
		++ _activeWorkers;
		++ _busyWorkers;
		w->spawn();
	}
	_queue.push_back( call_ );
	_semaphore.signal();
	return;
	M_EPILOG
}

void HWorkFlow::cancel_all( void ) {
	M_PROLOG
	HLock l( _mutex );
	_queue.clear();
	M_EPILOG
}

HWorkFlowInterface::task_t HWorkFlow::do_pop_task( void ) {
	M_PROLOG
	/* make worker as idle (not busy) */ {
		HLock l( _mutex );
		-- _busyWorkers;
	}
	HWorkFlow::task_t t;
	_semaphore.wait();
	HLock l( _mutex );
	if ( ! _queue.is_empty() ) {
		t = _queue.front();
		_queue.pop_front();
		++ _busyWorkers;
	}
	return ( t );
	M_EPILOG
}

HWorkFlow::HWorker::HWorker( HWorkFlowInterface* workFlow_ )
	: _workFlow( workFlow_ ), _thread() {
	return;
}

void HWorkFlow::HWorker::spawn( void ) {
	M_PROLOG
	_thread.spawn( call( &HWorkFlow::HWorker::run, this ) );
	return;
	M_EPILOG
}

void HWorkFlow::HWorker::finish( void ) {
	M_PROLOG
	_thread.finish();
	return;
	M_EPILOG
}

void HWorkFlow::HWorker::run( void ) {
	M_PROLOG
	HThread::set_name( "HWorkFlow" );
	HWorkFlow::task_t t;
	while ( ! _isKilled_ && ( !! ( t = _workFlow->pop_task() ) ) ) {
		t();
		t.reset();
	}
	return;
	M_EPILOG
}

}

}

