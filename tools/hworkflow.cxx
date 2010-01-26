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
#include "hworkflow.hxx"

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

HWorkFlow::HWorkFlow( int a_iWorkerPoolSize )
	: f_iWorkerPoolSize( a_iWorkerPoolSize ), f_iActiveWorkers( 0 ),
	f_iBusyWorkers( 0 ), f_oPool(), f_oQueue(), f_oSemaphore(), f_oMutex()
	{
	M_PROLOG
	M_ASSERT( f_iWorkerPoolSize > 0 );
	return;
	M_EPILOG
	}

HWorkFlow::~HWorkFlow( void )
	{
	M_PROLOG
		{
		HLock l( f_oMutex );
		f_iWorkerPoolSize = 0;
		}
	for ( pool_t::iterator it = f_oPool.begin(); it != f_oPool.end(); ++ it )
		{
		f_oSemaphore.signal();
		(*it)->finish();
		}
	return;
	M_EPILOG
	}

void HWorkFlow::push_task( task_t a_oCall )
	{
	M_PROLOG
	HLock l( f_oMutex );
	M_ASSERT( f_iBusyWorkers <= f_iActiveWorkers );
	if ( ( f_iBusyWorkers == f_iActiveWorkers ) && ( f_iActiveWorkers < f_iWorkerPoolSize ) )
		{
		worker_ptr_t w( new HWorker( this ) );
		f_oPool.push_back( w );
		++ f_iActiveWorkers;
		++ f_iBusyWorkers;
		w->spawn();
		}
	f_oQueue.push_back( a_oCall );
	f_oSemaphore.signal();
	return;
	M_EPILOG
	}

HWorkFlowInterface::task_t HWorkFlow::do_pop_task( void )
	{
	M_PROLOG
	bool running = true;
		{
		HLock l( f_oMutex );
		-- f_iBusyWorkers;
		running = f_iWorkerPoolSize ? true : false;
		}
	if ( running )
		f_oSemaphore.wait();
	HLock l( f_oMutex );
	HWorkFlow::task_t t;
	if ( ! f_oQueue.is_empty() )
		{
		t = f_oQueue.front();
		f_oQueue.pop_front();
		++ f_iBusyWorkers;
		}
	return ( t );
	M_EPILOG
	}

HWorkFlow::HWorker::HWorker( HWorkFlowInterface* a_poWorkFlow )
	: f_poWorkFlow( a_poWorkFlow ), f_oWorker( *this )
	{
	return;
	}

void HWorkFlow::HWorker::spawn( void )
	{
	M_PROLOG
	f_oWorker.spawn();
	return;
	M_EPILOG
	}

void HWorkFlow::HWorker::finish( void )
	{
	M_PROLOG
	f_oWorker.finish();
	return;
	M_EPILOG
	}

int HWorkFlow::HWorker::operator()( yaal::hcore::HThread const* )
	{
	M_PROLOG
	HWorkFlow::task_t t;
	while ( !! ( t = f_poWorkFlow->pop_task() ) )
		{
		t->invoke();
		t.reset();
		}
	return ( 0 );
	M_EPILOG
	}

}

}

