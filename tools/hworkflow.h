/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hworkflow.h - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#ifndef __YAAL_TOOLS_HWORKFLOW_H
#define __YAAL_TOOLS_HWORKFLOW_H

#include "hcore/hpointer.h"
#include "hcore/hcall.h"
#include "hcore/hthread.h"
#include "hcore/hlist.h"

namespace yaal
{

namespace tools
{

class HWorkerInterface
	{
public:
	virtual ~HWorkerInterface( void ){}
	void finish( void );
protected:
	virtual void do_finish( void ) = 0;
	};

class HWorkFlowInterface
	{
public:
	typedef yaal::hcore::HCallInterface::ptr_t task_t;
	virtual ~HWorkFlowInterface( void ){}
	task_t pop_task( void );
private:
	virtual task_t do_pop_task( void ) = 0;
	};

class HWorkFlow : public HWorkFlowInterface
	{
public:
	struct MODE
		{
		typedef enum
			{
			D_FLAT,
			D_PIPE
			} mode_t;
		};
private:
	class HWorker;
	typedef yaal::hcore::HThreadT<HWorkFlow> work_flow_t;
	typedef yaal::hcore::HPointer<HWorker> worker_ptr_t;
	typedef yaal::hcore::HList<worker_ptr_t> pool_t;
	typedef yaal::hcore::HList<task_t> queue_t;
	MODE::mode_t f_eMode;
	bool f_bLoop;
	int f_iWorkerPoolSize;
	int f_iActiveWorkers;
	/*! Tasks executors.
	 */
	pool_t f_oPool;
	/*! Task queue.
	 */
	queue_t f_oQueue;
	yaal::hcore::HSemaphore f_oSemaphore;
	yaal::hcore::HMutex f_oMutex;
	work_flow_t f_oWorkFlow;
public:
	HWorkFlow( MODE::mode_t const&, int = 1 );
	virtual ~HWorkFlow( void );
	void run( void );
	void push_task( task_t );
private:
	virtual task_t do_pop_task( void );
	int operator()( yaal::hcore::HThread const* );
	friend class yaal::hcore::HThreadT<HWorkFlow>;
	};

class HWorkFlow::HWorker : public HWorkerInterface
	{
private:
	typedef yaal::hcore::HThreadT<HWorkFlow::HWorker> worker_t;
	HWorkFlowInterface* f_poWorkFlow;
	worker_t f_oWorker;
public:
	HWorker( HWorkFlowInterface* );
	void spawn( void );
	virtual void do_finish( void );
private:
	int operator()( yaal::hcore::HThread const* );
	friend class yaal::hcore::HThreadT<HWorker>;
	HWorker( HWorker const& );
	HWorker& operator = ( HWorker const& );
	};

}

}

#endif /* not __YAAL_TOOLS_HWORKFLOW_H */

