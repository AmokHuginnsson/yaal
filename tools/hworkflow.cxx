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

void HWorkFlowInterface::task_finished( HTaskInterface* a_poTask )
	{
	M_PROLOG
	do_task_finished( a_poTask );
	return;
	M_EPILOG
	}

HWorkFlow::HWorkFlow( MODE::mode_t const& a_eMode, int a_iWorkerPoolSize )
	: f_eMode( a_eMode ), f_bLoop( a_eMode == MODE::D_PIPE ), f_oQueue(),
	f_iWorkerPoolSize( a_iWorkerPoolSize ), f_iActiveWorkers( 0 ),
	f_oPool(), f_oJoinQueue(),
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
	while ( f_bLoop || ( ! f_bLoop && ( ! f_oQueue.is_empty() || f_iActiveWorkers ) ) )
		{
		f_oSemaphore.wait();
			{
			HLock l( f_oMutex );
			for ( join_queue_t::iterator it = f_oJoinQueue.begin(); it != f_oJoinQueue.end(); ++ it )
				{
				(*it)->finish();
				-- f_iActiveWorkers;
				f_oPool.remove( *it );
				}
			f_oJoinQueue.clear();
			if ( ! f_bLoop && ( f_eMode == MODE::D_PIPE ) )
				break;
			if ( ! f_oQueue.is_empty() && ( f_iActiveWorkers < f_iWorkerPoolSize ) )
				{
				task_ptr_t t( new HTask( this, f_oQueue.head() ) );
				f_oQueue.pop_front();
				f_oPool.insert( t.raw(), t );
				++ f_iActiveWorkers;
				t->spawn();
				}
			}
		}
	for ( pool_t::iterator it = f_oPool.begin(); it != f_oPool.end(); ++ it )
		it->first->finish();
	return ( 0 );
	M_EPILOG
	}

void HWorkFlow::run( void )
	{
	operator()( NULL );
	}

void HWorkFlow::push_task( call_t a_oCall )
	{
	M_PROLOG
	HLock l( f_oMutex );
	f_oQueue.push_back( a_oCall );
	f_oSemaphore.signal();
	return;
	M_EPILOG
	}

void HWorkFlow::do_task_finished( HTaskInterface* a_poTask )
	{
	M_PROLOG
	HLock l( f_oMutex );
	f_oJoinQueue.push_back( a_poTask );
	f_oSemaphore.signal();
	return;
	M_EPILOG
	}

HWorkFlow::HTask::HTask( HWorkFlowInterface* a_poWorkFlow, HWorkFlow::call_t a_oCall )
	: f_poWorkFlow( a_poWorkFlow ), f_oCall( a_oCall ), f_oTask( *this )
	{
	return;
	}

void HWorkFlow::HTask::spawn( void )
	{
	M_PROLOG
	f_oTask.spawn();
	return;
	M_EPILOG
	}

void HTaskInterface::finish( void )
	{
	M_PROLOG
	do_finish();
	return;
	M_EPILOG
	}

void HWorkFlow::HTask::do_finish( void )
	{
	M_PROLOG
	f_oTask.finish();
	return;
	M_EPILOG
	}

int HWorkFlow::HTask::operator()( yaal::hcore::HThread const* )
	{
	M_PROLOG
	f_oCall->invoke();
	f_poWorkFlow->task_finished( this );
	return ( 0 );
	M_EPILOG
	}

}

}

