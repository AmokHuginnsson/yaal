/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hworkflow.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_TOOLS_HWORKFLOW_HXX_INCLUDED
#define YAAL_TOOLS_HWORKFLOW_HXX_INCLUDED 1

#include "hcore/hpointer.hxx"
#include "hcore/hboundcall.hxx"
#include "hcore/hthread.hxx"
#include "hcore/hlist.hxx"

namespace yaal
{

namespace tools
{

/*! \brief Interface for HWorkFlow class to be used by HWorker class.
 */
class HWorkFlowInterface
	{
public:
	typedef HWorkFlowInterface this_type;
	typedef yaal::hcore::HBoundCall<> task_t;
	virtual ~HWorkFlowInterface( void ){}
	task_t pop_task( void );
private:
	virtual task_t do_pop_task( void ) = 0;
	};

typedef yaal::hcore::HExceptionT<HWorkFlowInterface> HWorkFlowInterfaceException;

/*! \brief Thread pool idiom implementation.
 */
class HWorkFlow : public HWorkFlowInterface
	{
public:
	typedef HWorkFlow this_type;
	typedef HWorkFlowInterface base_type;
	typedef yaal::hcore::HPointer<HWorkFlow> ptr_t;
	class HWorker;
private:
	typedef yaal::hcore::HPointer<HWorker> worker_ptr_t;
	typedef yaal::hcore::HList<worker_ptr_t> pool_t;
	typedef yaal::hcore::HList<task_t> queue_t;
	int _workerPoolSize;
	int _activeWorkers;
	int _busyWorkers;
	/*! Tasks executors.
	 */
	pool_t _pool;
	/*! Task queue.
	 */
	queue_t _queue;
	yaal::hcore::HSemaphore _semaphore;
	yaal::hcore::HMutex _mutex;
public:
	HWorkFlow( int = 1 );
	virtual ~HWorkFlow( void );
	void push_task( task_t );
private:
	virtual task_t do_pop_task( void );
	};

typedef yaal::hcore::HExceptionT<HWorkFlow, HWorkFlowInterfaceException> HWorkFlowException;

/*! \brief Finest unit of working capacity.
 */
class HWorkFlow::HWorker
	{
public:
	typedef HWorker this_type;
private:
	HWorkFlowInterface* _workFlow;
	yaal::hcore::HThread _thread;
private:
	HWorker( HWorkFlowInterface* );
	void spawn( void );
	void finish( void );
	void* run( void );
	HWorker( HWorker const& );
	HWorker& operator = ( HWorker const& );
	friend class HWorkFlow;
	};

typedef yaal::hcore::HExceptionT<HWorkFlow::HWorker> HWorkerException;

}

}

#endif /* #ifndef YAAL_TOOLS_HWORKFLOW_HXX_INCLUDED */

