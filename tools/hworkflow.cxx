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

class HWorkFlowInterface::HTask {
private:
	call_t _call;
	call_t _asyncStop;
	want_restart_t _wantRestart;
public:
	HTask(
		call_t call_,
		call_t asyncStop_ = call_t(),
		want_restart_t wantRestart_ = want_restart_t()
	) : _call( call_ ),
	_asyncStop( asyncStop_ ),
	_wantRestart( wantRestart_ ) {
		return;
	}
	void call( void ) {
		M_PROLOG
		_call();
		return;
		M_EPILOG
	}
	void async_stop( void ) {
		M_PROLOG
		if ( !! _asyncStop ) {
			_asyncStop();
		}
		return;
		M_EPILOG
	}
	bool want_restart( void ) const {
		M_PROLOG
		return ( !! _wantRestart && _wantRestart() );
		M_EPILOG
	}
};

HWorkFlowInterface::task_t HWorkFlowInterface::pop_task( void ) {
	M_PROLOG
	return ( do_pop_task() );
	M_EPILOG
}

void HWorkFlowInterface::push_task( task_t task_ ) {
	M_PROLOG
	do_push_task( yaal::move( task_ ) );
	return;
	M_EPILOG
}

HWorkFlow::HWorkFlow( int workerPoolSize_ )
	: _workerPoolSize( workerPoolSize_ )
	, _activeWorkers( 0 )
	, _busyWorkers( 0 )
	, _state( STATE::RUNNING )
	, _pool()
	, _queue()
	, _semaphore()
	, _mutex() {
	M_PROLOG
	M_ASSERT( _workerPoolSize > 0 );
	return;
	M_EPILOG
}

HWorkFlow::~HWorkFlow( void ) {
	M_PROLOG
	windup( WINDUP_MODE::CLOSE );
	return;
	M_DESTRUCTOR_EPILOG
}

void HWorkFlow::push_task( call_t task_, call_t asyncStop_, want_restart_t wantRestart_ ) {
	M_PROLOG
	task_t t( make_resource<HTask>( task_, asyncStop_, wantRestart_ ) );
	do_push_task( yaal::move( t ) );
	return;
	M_EPILOG
}

void HWorkFlow::do_push_task( task_t task_ ) {
	M_PROLOG
	HLock l( _mutex );
	M_ASSERT( _busyWorkers <= _activeWorkers );
	if ( ( _state == STATE::RUNNING )
		&& ( _busyWorkers == _activeWorkers )
		&& ( _activeWorkers < _workerPoolSize ) ) {
		worker_t w( make_resource<HWorker>( this ) );
		w->spawn();
		_pool.emplace_back( yaal::move( w ) );
		++ _activeWorkers;
		++ _busyWorkers;
	}
	if ( ( _state == STATE::ABORTING ) || ( _state == STATE::CLOSED ) ) {
		throw HWorkFlowException( "Not accepting new tasks." );
	}
	_queue.emplace_back( yaal::move( task_ ) );
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

void HWorkFlow::start( void ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _state != STATE::STOPPED ) {
		throw HWorkFlowException( "Parallel start." );
	}
	_state = STATE::RUNNING;
	for ( worker_t& w : _pool ) {
		w->spawn();
	}
	return;
	M_EPILOG
}

void HWorkFlow::windup( WINDUP_MODE windupMode_ ) {
	M_PROLOG
	/* Set state. */ {
		HLock l( _mutex );
		if ( _state != STATE::RUNNING ) {
			throw HWorkFlowException( "Parallel stop." );
		}
		_state = ( windupMode_ == WINDUP_MODE::ABORT ) ? STATE::ABORTING : STATE::STOPPING;
	}
	if ( ( windupMode_ == WINDUP_MODE::ABORT ) || ( windupMode_ == WINDUP_MODE::INTERRUPT ) ) {
		for ( worker_t& w : _pool ) {
			w->async_stop( _state );
		}
	}
	if ( _state == STATE::ABORTING ) {
		cancel_all();
	}
	for ( int i( 0 ), SIZE( static_cast<int>( _pool.get_size() ) ); i < SIZE; ++ i ) {
		_semaphore.signal();
	}
	for ( worker_t& w : _pool ) {
		w->finish();
	}
	/* Set state. */ {
		HLock l( _mutex );
		_state = ( ( windupMode_ == WINDUP_MODE::INTERRUPT ) || ( windupMode_ == WINDUP_MODE::SUSPEND ) ) ? STATE::STOPPED : STATE::CLOSED;
	}
	return;
	M_EPILOG
}

HWorkFlowInterface::task_t HWorkFlow::do_pop_task( void ) {
	M_PROLOG
	/* make worker as idle (not busy) */ {
		HLock l( _mutex );
		M_ASSERT( ( _state != STATE::CLOSED ) && ( _state != STATE::STOPPED ) );
		-- _busyWorkers;
	}
	HWorkFlow::task_t t;
	_semaphore.wait();
	HLock l( _mutex );
	M_ASSERT( ( _state != STATE::CLOSED ) && ( _state != STATE::STOPPED ) );
	/*
	 * It is possible for HWorker to get one task of the queue
	 * and execute it even if windup(WINDUP_MODE::INTERRUPT) was invoked before.
	 * This is expected and correct behavior.
	 *
	 * To prevent this we would have to either check for
	 * _state in HWorker::run() just after pop_task()
	 * and possibly re-push task to the queue,
	 * or add additional STATE == STATE::INTERRUPTING.
	 *
	 * We cannot deny HWorker getting vaild task of the queue
	 * on _state == STATE::STOPPING if we want to support WINDUP_MODE::SUSPEND
	 * and WINDUP_MODE::CLOSE.
	 */
	if ( ! _queue.is_empty() && ( _state != STATE::ABORTING ) ) {
		t = yaal::move( _queue.front() );
		_queue.pop_front();
		++ _busyWorkers;
	}
	return ( t );
	M_EPILOG
}

HWorkFlow::HWorker::HWorker( HWorkFlowInterface* workFlow_ )
	: _workFlow( workFlow_ )
	, _thread()
	, _state{ HWorkFlow::STATE::STOPPED }
	, _task()
	, _mutex() {
	return;
}

void HWorkFlow::HWorker::spawn( void ) {
	M_PROLOG
	_state = HWorkFlow::STATE::RUNNING;
	_thread.spawn( call( &HWorkFlow::HWorker::run, this ) );
	return;
	M_EPILOG
}

void HWorkFlow::HWorker::finish( void ) {
	M_PROLOG
	_thread.finish();
	_state = HWorkFlow::STATE::STOPPED;
	return;
	M_EPILOG
}

void HWorkFlow::HWorker::async_stop( HWorkFlow::STATE state_ ) {
	M_PROLOG
	HLock l( _mutex );
	_state = state_;
	if ( !! _task ) {
		_task->async_stop();
	}
	return;
	M_EPILOG
}

void HWorkFlow::HWorker::run( void ) {
	M_PROLOG
	HThread::set_name( "HWorkFlow" );
	HWorkFlow::task_t task;
	while ( ! _isKilled_ ) {
		HTask* t( nullptr );
		/* Pop task. */ {
			HLock l( _mutex );
			_task =_workFlow->pop_task();
			if ( ! _task ) {
				break;
			}
			t = _task.raw();
		}
		t->call();
		HLock l( _mutex );
		if ( _state != HWorkFlow::STATE::ABORTING ) {
			if ( _task->want_restart() ) {
				_workFlow->push_task( yaal::move( _task ) );
			} else {
				_task.reset();
			}
			if ( _state != HWorkFlow::STATE::RUNNING ) {
				break;
			}
		} else {
			_task.reset();
			break;
		}
	}
	return;
	M_EPILOG
}

}

}

