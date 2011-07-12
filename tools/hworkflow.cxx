/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hworkflow.cxx - this file is integral part of `yaal' project.

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

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hworkflow.hxx"
#include "hcore/memory.hxx"

using namespace yaal::hcore;

namespace yaal
{

namespace tools
{

HWorkFlowInterface::task_t HWorkFlowInterface::pop_task( void )
	{
	M_PROLOG
	return ( do_pop_task() );
	M_EPILOG
	}

HWorkFlow::HWorkFlow( int workerPoolSize_ )
	: _workerPoolSize( workerPoolSize_ ), _activeWorkers( 0 ),
	_busyWorkers( 0 ), _pool(), _queue(), _semaphore(), _mutex()
	{
	M_PROLOG
	M_ASSERT( _workerPoolSize > 0 );
	return;
	M_EPILOG
	}

HWorkFlow::~HWorkFlow( void )
	{
	M_PROLOG
		{
		HLock l( _mutex );
		_workerPoolSize = 0;
		}
	for ( pool_t::iterator it( _pool.begin() ), end( _pool.end() ); it != end; ++ it )
		{
		_semaphore.signal();
		(*it)->finish();
		}
	return;
	M_DESTRUCTOR_EPILOG
	}

void HWorkFlow::push_task( task_t call_ )
	{
	M_PROLOG
	HLock l( _mutex );
	M_ASSERT( _busyWorkers <= _activeWorkers );
	if ( ( _busyWorkers == _activeWorkers ) && ( _activeWorkers < _workerPoolSize ) )
		{
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

HWorkFlowInterface::task_t HWorkFlow::do_pop_task( void )
	{
	M_PROLOG
	bool running = true;
		{
		HLock l( _mutex );
		-- _busyWorkers;
		running = _workerPoolSize ? true : false;
		}
	if ( running )
		_semaphore.wait();
	HLock l( _mutex );
	HWorkFlow::task_t t;
	if ( ! _queue.is_empty() )
		{
		t = _queue.front();
		_queue.pop_front();
		++ _busyWorkers;
		}
	return ( t );
	M_EPILOG
	}

HWorkFlow::HWorker::HWorker( HWorkFlowInterface* workFlow_ )
	: _workFlow( workFlow_ ), _thread()
	{
	return;
	}

void HWorkFlow::HWorker::spawn( void )
	{
	M_PROLOG
	_thread.spawn( call( &HWorkFlow::HWorker::run, this ) );
	return;
	M_EPILOG
	}

void HWorkFlow::HWorker::finish( void )
	{
	M_PROLOG
	_thread.finish();
	return;
	M_EPILOG
	}

void* HWorkFlow::HWorker::run( void )
	{
	M_PROLOG
	HThread::set_name( "HWorkFlow" );
	HWorkFlow::task_t t;
	while ( ! _isKilled_ && ( !! ( t = _workFlow->pop_task() ) ) )
		{
		t();
		t.reset();
		}
	return ( 0 );
	M_EPILOG
	}

}

}

