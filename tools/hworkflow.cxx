/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hworkflow.hxx"
#include "hcore/memory.hxx"

using namespace yaal::hcore;

namespace yaal {

namespace tools {

class HWorkFlow::HTask {
private:
	call_t _call;
	call_t _asyncStop;
	want_restart_t _wantRestart;
public:
	HTask( call_t call_, call_t asyncStop_, want_restart_t wantRestart_ )
		: _call( call_ )
		, _asyncStop( asyncStop_ )
		, _wantRestart( wantRestart_ ) {
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
	void const* id( void ) const {
		M_PROLOG
		return ( _call.id() );
		M_EPILOG
	}
	void reset( void ) {
		M_PROLOG
		_asyncStop.reset();
		M_EPILOG
	}
private:
	HTask( HTask const& ) = delete;
	HTask& operator = ( HTask const& ) = delete;
};

/*! \brief Finest unit of working capacity.
 */
class HWorkFlow::HWorker {
public:
	typedef HWorker this_type;
	typedef std::atomic<bool> async_flag_t;
private:
	HWorkFlow* _workFlow;
	yaal::hcore::HThread _thread;
	HWorkFlow::STATE _state;
	HWorkFlow::task_t _task;
	async_flag_t _hasTask;
	async_flag_t _canJoin;
	mutable yaal::hcore::HMutex _mutex;
public:
	HWorker( HWorkFlow* );
	void spawn( void );
	void spawn( HWorkFlow::task_t );
	void finish( void );
	void async_stop( HWorkFlow::STATE );
	void run( void );
	void cancel_task( void const* );
	bool has_task( void ) const {
		return ( _hasTask );
	}
	bool can_join( void ) const {
		return ( _canJoin );
	}
private:
	HWorker( HWorker const& ) = delete;
	HWorker& operator = ( HWorker const& ) = delete;
};

HWorkFlow::HWorkFlow( int workerPoolSize_ )
	: _workerPoolSize( workerPoolSize_ )
	, _busyWorkers( 0 )
	, _state( STATE::RUNNING )
	, _workers()
	, _tasks()
	, _semaphore()
	, _mutex() {
	M_PROLOG
	M_ASSERT( _workerPoolSize > 0 );
	return;
	M_EPILOG
}

HWorkFlow::~HWorkFlow( void ) {
	M_PROLOG
	bool startIsRequired( false );
	/* check if start is required */ {
		HLock l( _mutex );
		M_ASSERT( ( _state == STATE::RUNNING ) || ( _state == STATE::STOPPED ) || ( _state == STATE::CLOSED ) );
		if ( _state == STATE::STOPPED ) {
			if ( _tasks.is_empty() ) {
				for ( worker_t& w : _workers ) {
					if ( w->has_task() ) {
						startIsRequired = true;
						break;
					}
				}
			} else {
				startIsRequired = true;
			}
		}
	}
	if ( startIsRequired ) {
		start();
	}
	bool stopIsRequired( false );
	/* check if stop is required */ {
		HLock l( _mutex );
		if ( _state == STATE::RUNNING ) {
			stopIsRequired = true;
		}
	}
	if ( stopIsRequired ) {
		windup( WINDUP_MODE::CLOSE );
	}
	return;
	M_DESTRUCTOR_EPILOG
}

void HWorkFlow::schedule_task( SCHEDULE_POLICY schedulePolicy_, call_t task_, call_t asyncStop_, want_restart_t wantRestart_ ) {
	M_PROLOG
	switch ( schedulePolicy_ ) {
		case ( SCHEDULE_POLICY::EAGER ): {
			add_task_eager( task_, asyncStop_, wantRestart_ );
		} break;
		case ( SCHEDULE_POLICY::LAZY ): {
			add_task_lazy( task_, asyncStop_, wantRestart_ );
		} break;
	}
	return;
	M_EPILOG
}

void HWorkFlow::add_task_lazy( call_t task_, call_t asyncStop_, want_restart_t wantRestart_ ) {
	M_PROLOG
	HLock l( _mutex );
	int activeWorkers( static_cast<int>( _workers.get_size() ) );
	M_ASSERT( _busyWorkers <= activeWorkers );
	if ( ( _state == STATE::ABORTING ) || ( _state == STATE::CLOSING ) || ( _state == STATE::CLOSED ) ) {
		throw HWorkFlowException( "Not accepting new tasks." );
	}
	if ( ( _state == STATE::RUNNING )
		&& ( activeWorkers < _workerPoolSize )
		&& ( ( _busyWorkers + static_cast<int>( _tasks.size() ) ) >= activeWorkers ) ) {
		worker_t w( make_resource<HWorker>( this ) );
		w->spawn();
		_workers.emplace_back( yaal::move( w ) );
		++ _busyWorkers;
	}
	task_t t( make_resource<HTask>( task_, asyncStop_, wantRestart_ ) );
	_tasks.emplace_back( yaal::move( t ) );
	_semaphore.signal();
	return;
	M_EPILOG
}

void HWorkFlow::add_task_eager( call_t task_, call_t asyncStop_, want_restart_t wantRestart_ ) {
	M_PROLOG
	HLock l( _mutex );
	M_ASSERT( _busyWorkers <= static_cast<int>( _workers.get_size() ) );
	if ( _state != STATE::RUNNING ) {
		throw HWorkFlowException( "Cannot start new task now." );
	}
	task_t t( make_resource<HTask>( task_, asyncStop_, wantRestart_ ) );
	if ( ( _busyWorkers + _tasks.get_size() ) >= _workers.get_size() ) {
		worker_t w( make_resource<HWorker>( this ) );
		w->spawn( yaal::move( t ) );
		_workers.emplace_back( yaal::move( w ) );
		++ _busyWorkers;
		++ _workerPoolSize;
	} else {
		_tasks.emplace_back( yaal::move( t ) );
		_semaphore.signal();
	}
	return;
	M_EPILOG
}

void HWorkFlow::cancel_task( void const* subject_ ) {
	M_PROLOG
	HLock l( _mutex );
	for (
		task_queue_t::iterator taskIt( _tasks.begin() ), taskEndId( _tasks.end() );
		taskIt != taskEndId;
		/* inc in the loop due to erase */
	) {
		if ( (*taskIt)->id() == subject_ ) {
			taskIt = _tasks.erase( taskIt );
			_semaphore.wait();
		} else {
			++ taskIt;
		}
	}
	for ( worker_t& w : _workers ) {
		w->cancel_task( subject_ );
	}
	return;
	M_EPILOG
}

void HWorkFlow::start( void ) {
	M_PROLOG
	HLock l( _mutex );
	if ( ( _state != STATE::STOPPED ) && ( _state != STATE::CLOSED ) ) {
		throw HWorkFlowException( "Parallel start." );
	}
	_state = STATE::RUNNING;
	int activeWorkers( static_cast<int>( _workers.get_size() ) );
	for ( int i( activeWorkers ); i < min( _workerPoolSize, static_cast<int>( _tasks.get_size() ) ); ++ i ) {
		worker_t w( make_resource<HWorker>( this ) );
		_workers.emplace_back( yaal::move( w ) );
		++ _busyWorkers;
	}
	for ( worker_t& w : _workers ) {
		w->spawn();
	}
	return;
	M_EPILOG
}

void HWorkFlow::schedule_windup( WINDUP_MODE windupMode_ ) {
	M_PROLOG
	/* Set state. */ {
		HLock l( _mutex );
		if ( _state != STATE::RUNNING ) {
			throw HWorkFlowException( "Parallel stop." );
		}
		switch ( windupMode_ ) {
			case ( WINDUP_MODE::ABORT ):     _state = STATE::ABORTING;     break;
			case ( WINDUP_MODE::INTERRUPT ): _state = STATE::INTERRUPTING; break;
			case ( WINDUP_MODE::SUSPEND ):   _state = STATE::STOPPING;     break;
			case ( WINDUP_MODE::CLOSE ):     _state = STATE::CLOSING;      break;
		}
	}
	for ( int i( 0 ), SIZE( static_cast<int>( _workers.get_size() ) ); i < SIZE; ++ i ) {
		_semaphore.signal();
	}
	if ( ( windupMode_ == WINDUP_MODE::ABORT ) || ( windupMode_ == WINDUP_MODE::INTERRUPT ) ) {
		for ( worker_t& w : _workers ) {
			w->async_stop( _state );
		}
	}
	return;
	M_EPILOG
}

void HWorkFlow::join() {
	M_PROLOG
	HLock l( _mutex );
	M_ASSERT( ( _state == STATE::ABORTING ) || ( _state == STATE::INTERRUPTING ) || ( _state == STATE::STOPPING ) || ( _state == STATE::CLOSING ) );
	l.unlock();
	for ( worker_t& w : _workers ) {
		w->finish();
	}
	l.lock();
	if ( _state == STATE::ABORTING ) {
		_semaphore.reset();
		_tasks.clear();
	}
	_state = ( ( _state == STATE::INTERRUPTING ) || ( _state == STATE::STOPPING ) ) ? STATE::STOPPED : STATE::CLOSED;
	return;
	M_EPILOG
}

bool HWorkFlow::can_join( void ) {
	M_PROLOG
	HLock l( _mutex );
	bool canJoin( ( _state == STATE::ABORTING ) || ( _state == STATE::INTERRUPTING ) || ( _state == STATE::STOPPING ) || ( _state == STATE::CLOSING ) );
	if ( canJoin ) {
		for ( worker_t& w : _workers ) {
			if ( ! w->can_join() ) {
				canJoin = false;
				break;
			}
		}
	}
	return canJoin;
	M_EPILOG
}

HWorkFlow::STATE HWorkFlow::state( void ) const {
	M_PROLOG
	return ( _state );
	M_EPILOG
}

void HWorkFlow::windup( WINDUP_MODE windupMode_ ) {
	M_PROLOG
	schedule_windup( windupMode_ );
	join();
	return;
	M_EPILOG
}

HWorkFlow::task_t HWorkFlow::pop_task( void ) {
	M_PROLOG
	HLock l( _mutex );
	M_ASSERT( ( _state != STATE::CLOSED ) && ( _state != STATE::STOPPED ) );
	-- _busyWorkers;
	l.unlock();
	_semaphore.wait();
	l.lock();
	M_ASSERT( ( _state != STATE::CLOSED ) && ( _state != STATE::STOPPED ) );
	HWorkFlow::task_t t;
	if ( ! _tasks.is_empty() && ( ( _state == STATE::RUNNING ) || ( _state == STATE::STOPPING ) || ( _state == STATE::CLOSING ) ) ) {
		t = yaal::move( _tasks.front() );
		_tasks.pop_front();
		++ _busyWorkers;
	}
	return t;
	M_EPILOG
}

HWorkFlow::HWorker::HWorker( HWorkFlow* workFlow_ )
	: _workFlow( workFlow_ )
	, _thread()
	, _state{ HWorkFlow::STATE::STOPPED }
	, _task()
	, _hasTask{ false }
	, _canJoin{ true }
	, _mutex() {
	return;
}

void HWorkFlow::HWorker::spawn( void ) {
	M_PROLOG
	HLock l( _mutex );
	M_ENSURE( _state == HWorkFlow::STATE::STOPPED );
	_state = HWorkFlow::STATE::RUNNING;
	_thread.spawn( call( &HWorkFlow::HWorker::run, this ) );
	return;
	M_EPILOG
}

void HWorkFlow::HWorker::spawn( HWorkFlow::task_t task_ ) {
	M_PROLOG
	HLock l( _mutex );
	M_ENSURE( _state == HWorkFlow::STATE::STOPPED );
	_task = yaal::move( task_ );
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
	_canJoin = false;
	while ( ! _isKilled_ ) {
		HLock l( _mutex );
		if ( ! _task ) {
			l.unlock();
			task_t t = _workFlow->pop_task();
			l.lock();
			if ( !! t ) {
				_task = yaal::move( t );
				_hasTask = true;
			} else {
				break;
			}
		}
		HTask* t( _task.raw() );
		l.unlock();
		t->call();
		l.lock();
		if ( _state != HWorkFlow::STATE::ABORTING ) {
			if ( ! _task->want_restart() ) {
				_task.reset();
				_hasTask = false;
			}
			if ( _state != HWorkFlow::STATE::RUNNING ) {
				break;
			}
		} else {
			_task.reset();
			_hasTask = false;
			break;
		}
	}
	_canJoin = true;
	return;
	M_EPILOG
}

void HWorkFlow::HWorker::cancel_task( void const* id_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( !! _task && ( _task->id() == id_ ) ) {
		_task->reset();
	}
	M_EPILOG
}

}

}

