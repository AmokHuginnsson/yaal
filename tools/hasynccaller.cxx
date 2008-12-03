/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hasynccaller.cxx - this file is integral part of `yaal' project.

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

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "hasynccaller.hxx"

using namespace yaal::hcore;

namespace yaal
{

namespace tools
{

HAbstractAsyncCaller::HAbstractAsyncCaller( void ) : f_oQueue(), f_oCaller( *this ), f_oMutex(), f_bLoop( false )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

void HAbstractAsyncCaller::stop( void )
	{
	M_PROLOG
	f_bLoop = false;
	do_signal();
	f_oCaller.finish();
	return;
	M_EPILOG
	}

void HAbstractAsyncCaller::start( void )
	{
	M_PROLOG
	f_bLoop = true;
	f_oCaller.spawn();
	return;
	M_EPILOG
	}

void HAbstractAsyncCaller::register_call( priority_t prio, call_t call )
	{
	M_PROLOG
		{
		HLock l( f_oMutex );
		f_oQueue.push_back( prio, call );
		}
	do_signal();
	return;
	M_EPILOG
	}

void HAbstractAsyncCaller::flush( void* a_pvInvoker )
	{
	M_PROLOG
	HLock l( f_oMutex );
	for ( queue_t::iterator it = f_oQueue.begin(); it != f_oQueue.end(); )
		{
		if ( (*it).second->id() == a_pvInvoker )
			it = f_oQueue.erase( it );
		else
			++ it;
		}
	return;
	M_EPILOG
	}

int HAbstractAsyncCaller::operator()( yaal::hcore::HThread const* )
	{
	M_PROLOG
	return ( do_work() );
	M_EPILOG
	}

int HAbstractAsyncCaller::life_time( int )
	{
	return ( 50 );
	}

HAsyncCaller::HAsyncCaller( void ) : f_oSemaphore()
	{
	M_PROLOG
	start();
	return;
	M_EPILOG
	}

HAsyncCaller::~HAsyncCaller( void )
	{
	M_PROLOG
	stop();
	return;
	M_EPILOG
	}

void HAsyncCaller::do_signal( void )
	{
	M_PROLOG
	f_oSemaphore.signal();
	return;
	M_EPILOG
	}

int HAsyncCaller::do_work( void )
	{
	M_PROLOG
	while ( f_bLoop )
		{
		f_oSemaphore.wait();
		HLock l( f_oMutex );
		queue_t::iterator it = f_oQueue.begin();
		if ( it != f_oQueue.end() )
			{
			(*it).second->invoke();
			f_oQueue.erase( it );
			}
		}
	return ( 0 );
	M_EPILOG
	}

}

}

