/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hworkflow.cxx - this file is integral part of `yaal' project.

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

#include "hcore/base.h"
M_VCSID( "$Id: "__ID__" $" )
#include "hworkflow.h"

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

HWorkFlow::HWorkFlow( MODE::mode_t const& a_eMode, int a_iWorkerPoolSize )
	: f_eMode( a_eMode ), f_bLoop( a_eMode == MODE::D_PIPE ),
	f_iWorkerPoolSize( a_iWorkerPoolSize ), f_iActiveWorkers( 0 ),
	f_oPool(), f_oQueue(),
	f_oSemaphore(), f_oMutex(), f_oWorkFlow( *this )
	{
	M_PROLOG
	M_ASSERT( f_iWorkerPoolSize > 0 );
	if ( f_eMode == MODE::D_PIPE )
		f_oWorkFlow.spawn();
	return;
	M_EPILOG
	}

HWorkFlow::~HWorkFlow( void )
	{
	M_PROLOG
	if ( f_eMode == MODE::D_PIPE )
		{
		f_bLoop = false;
		f_oSemaphore.signal();
		f_oWorkFlow.finish();
		}
	return;
	M_EPILOG
	}

int HWorkFlow::operator()( yaal::hcore::HThread const* )
	{
	M_PROLOG
	bool l_bLoop = false;
		{
		HLock l( f_oMutex );
		l_bLoop = f_bLoop || ( ! f_bLoop && ( ! f_oQueue.is_empty() || f_iActiveWorkers ) );
		}
	while ( l_bLoop )
		{
		HLock l( f_oMutex );
		if ( ! f_oQueue.is_empty() && ( f_iActiveWorkers < f_iWorkerPoolSize ) )
			{
			worker_ptr_t w( new HWorker( this ) );
			f_oPool.push_back( w );
			++ f_iActiveWorkers;
			w->spawn();
			}
		l_bLoop = f_bLoop || ( ( f_eMode != MODE::D_PIPE ) && ( ! f_oQueue.is_empty() || f_iActiveWorkers ) );
		}
	for ( pool_t::iterator it = f_oPool.begin(); it != f_oPool.end(); ++ it )
		(*it)->finish();
	return ( 0 );
	M_EPILOG
	}

void HWorkFlow::run( void )
	{
	operator()( NULL );
	}

void HWorkFlow::push_task( task_t a_oCall )
	{
	M_PROLOG
	HLock l( f_oMutex );
	f_oQueue.push_back( a_oCall );
	f_oSemaphore.signal();
	return;
	M_EPILOG
	}

HWorkFlowInterface::task_t HWorkFlow::do_pop_task( void )
	{
	M_PROLOG
	HLock l( f_oMutex );
	f_oSemaphore.wait();
	HWorkFlow::task_t t;
	if ( ! f_oQueue.is_empty() )
		{
		t = f_oQueue.front();
		f_oQueue.pop_front();
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

void HWorkerInterface::finish( void )
	{
	M_PROLOG
	do_finish();
	return;
	M_EPILOG
	}

void HWorkFlow::HWorker::do_finish( void )
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
		t->invoke();
	return ( 0 );
	M_EPILOG
	}

}

}

