/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hscheduledasynccaller.cxx - this file is integral part of `yaal' project.

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

#include <time.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hscheduledasynccaller.hxx"

using namespace yaal::hcore;

namespace yaal
{

namespace tools
{

HScheduledAsyncCaller::HScheduledAsyncCaller( void ) : f_oCondition( f_oMutex )
	{
	M_PROLOG
	start();
	return;
	M_EPILOG
	}

HScheduledAsyncCaller::~HScheduledAsyncCaller( void )
	{
	M_PROLOG
	stop();
	return;
	M_EPILOG
	}

void HScheduledAsyncCaller::do_signal( void )
	{
	M_PROLOG
	f_oCondition.signal();
	return;
	M_EPILOG
	}

void* HScheduledAsyncCaller::do_work( void )
	{
	M_PROLOG
	HLock l( f_oMutex );
	while ( f_bLoop )
		{
		queue_t::iterator it = f_oQueue.begin();
		while ( ( it != f_oQueue.end() ) && ( (*it).first <= time( NULL ) ) )
			{
			(*it).second->invoke();
			it = f_oQueue.erase( it );
			}
		if ( it != f_oQueue.end() )
			{
			time_t delay = 0;
			if ( ( delay = ( (*it).first - time( NULL ) ) ) > 0 )
				f_oCondition.wait( static_cast<int long unsigned>( delay ), 0 );
			}
		else
			f_oCondition.wait( 10000, 0 );
		}
	return ( 0 );
	M_EPILOG
	}

}

}

