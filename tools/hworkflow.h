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
#include "hcore/hmap.h"

namespace yaal
{

namespace tools
{

class HTaskInterface
	{
public:
	virtual ~HTaskInterface( void ){}
	void finish( void );
protected:
	virtual void do_finish( void ) = 0;
	};

class HWorkFlowInterface
	{
public:
	virtual ~HWorkFlowInterface( void ){}
	void task_finished( HTaskInterface* );
private:
	virtual void do_task_finished( HTaskInterface* ) = 0;
	};

class HWorkFlow : public HWorkFlowInterface
	{
	class HTask;
	typedef yaal::hcore::HPointer<HTask> task_ptr_t;
	typedef yaal::hcore::HCallInterface::ptr_t call_t;
	typedef yaal::hcore::HList<call_t> queue_t;
	typedef yaal::hcore::HThreadT<HWorkFlow> work_flow_t;
	typedef yaal::hcore::HMap<HTaskInterface*, task_ptr_t> pool_t;
	typedef yaal::hcore::HList<HTaskInterface*> join_queue_t;
	bool f_bLoop;
	/*! Task queue.
	 */
	queue_t f_oQueue;
	int f_iWorkerPoolSize;
	int f_iActiveWorkers;
	/*! Running tasks executors.
	 */
	pool_t f_oPool;
	join_queue_t f_oJoinQueue;
	yaal::hcore::HSemaphore f_oSemaphore;
	yaal::hcore::HMutex f_oMutex;
	work_flow_t f_oWorkFlow;
public:
	HWorkFlow( int = 1 );
	virtual ~HWorkFlow( void );
	void run( void );
	void push_task( call_t );
private:
	virtual void do_task_finished( HTaskInterface* );
	int operator()( yaal::hcore::HThread const* );
	friend class yaal::hcore::HThreadT<HWorkFlow>;
	};

class HWorkFlow::HTask : public HTaskInterface
	{
private:
	typedef yaal::hcore::HThreadT<HWorkFlow::HTask> task_t;
	HWorkFlowInterface* f_poWorkFlow;
	call_t f_oCall;
	task_t f_oTask;
public:
	HTask( HWorkFlowInterface*, call_t );
	void spawn( void );
	virtual void do_finish( void );
private:
	int operator()( yaal::hcore::HThread const* );
	friend class yaal::hcore::HThreadT<HTask>;
	HTask( HTask const& );
	HTask& operator = ( HTask const& );
	};

}

}

#endif /* not __YAAL_TOOLS_HWORKFLOW_H */

