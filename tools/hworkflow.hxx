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
#define YAAL_TOOLS_HWORKFLOW_HXX_INCLUDED

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
	typedef yaal::hcore::HBoundCallInterface<>::ptr_t task_t;
	virtual ~HWorkFlowInterface( void ){}
	task_t pop_task( void );
private:
	virtual task_t do_pop_task( void ) = 0;
	};

/*! \brief Thread pool idiom implementation.
 */
class HWorkFlow : public HWorkFlowInterface
	{
private:
	class HWorker;
	typedef yaal::hcore::HPointer<HWorker> worker_ptr_t;
	typedef yaal::hcore::HList<worker_ptr_t> pool_t;
	typedef yaal::hcore::HList<task_t> queue_t;
	int f_iWorkerPoolSize;
	int f_iActiveWorkers;
	int f_iBusyWorkers;
	/*! Tasks executors.
	 */
	pool_t f_oPool;
	/*! Task queue.
	 */
	queue_t f_oQueue;
	yaal::hcore::HSemaphore f_oSemaphore;
	yaal::hcore::HMutex f_oMutex;
public:
	HWorkFlow( int = 1 );
	virtual ~HWorkFlow( void );
	void push_task( task_t );
private:
	virtual task_t do_pop_task( void );
	};

/*! \brief Finest unit of working capacity.
 */
class HWorkFlow::HWorker
	{
private:
	typedef yaal::hcore::HThreadT<HWorkFlow::HWorker> worker_t;
	HWorkFlowInterface* f_poWorkFlow;
	worker_t f_oWorker;
public:
	HWorker( HWorkFlowInterface* );
	void spawn( void );
	void finish( void );
private:
	int operator()( yaal::hcore::HThread const* );
	friend class yaal::hcore::HThreadT<HWorker>;
	HWorker( HWorker const& );
	HWorker& operator = ( HWorker const& );
	};

}

}

#endif /* not YAAL_TOOLS_HWORKFLOW_HXX_INCLUDED */

