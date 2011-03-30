/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hasynccaller.cxx - this file is integral part of `yaal' project.

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
M_VCSID( "$Id: "__TID__" $" )
#include "hasynccaller.hxx"

using namespace yaal::hcore;

namespace yaal
{

namespace tools
{

HAbstractAsyncCaller::HAbstractAsyncCaller( void )
	: _queue(), _thread(), _mutex(), _loop( false )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

void HAbstractAsyncCaller::stop( void )
	{
	M_PROLOG
	_loop = false;
	do_signal();
	_thread.finish();
	return;
	M_EPILOG
	}

void HAbstractAsyncCaller::start( void )
	{
	M_PROLOG
	_loop = true;
	_thread.spawn( call( &HAbstractAsyncCaller::run, this ) );
	return;
	M_EPILOG
	}

void HAbstractAsyncCaller::register_call( priority_t prio, call_t call )
	{
	M_PROLOG
		{
		HLock l( _mutex );
		_queue.push_back( prio, call );
		}
	do_signal();
	return;
	M_EPILOG
	}

void HAbstractAsyncCaller::flush( void* invoker_ )
	{
	M_PROLOG
	HLock l( _mutex );
	for ( queue_t::iterator it = _queue.begin(); it != _queue.end(); )
		{
		if ( (*it).second.id() == invoker_ )
			it = _queue.erase( it );
		else
			++ it;
		}
	return;
	M_EPILOG
	}

void* HAbstractAsyncCaller::run( void )
	{
	M_PROLOG
	return ( do_work() );
	M_EPILOG
	}

HAsyncCaller::HAsyncCaller( void ) : _semaphore()
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
	M_DESTRUCTOR_EPILOG
	}

void HAsyncCaller::do_signal( void )
	{
	M_PROLOG
	_semaphore.signal();
	return;
	M_EPILOG
	}

void* HAsyncCaller::do_work( void )
	{
	M_PROLOG
	HThread::set_name( "HAsyncCaller" );
	while ( _loop )
		{
		_semaphore.wait();
		HLock l( _mutex );
		queue_t::iterator it = _queue.begin();
		if ( it != _queue.end() )
			{
			(*it).second();
			_queue.erase( it );
			}
		}
	return ( 0 );
	M_EPILOG
	}

int HAsyncCaller::life_time( int )
	{
	return ( 50 );
	}

}

}

