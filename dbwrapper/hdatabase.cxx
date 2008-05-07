/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hdatabase.cxx - this file is integral part of `yaal' project.

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

#include <string.h>

#include "hcore/hexception.h"
M_VCSID ( "$Id$" )
#include "hcore/hlog.h"
#include "hdatabase.h"
#include "db_driver_loader.h"

using namespace yaal::hcore;

namespace yaal
{

namespace dbwrapper
{

HDataBase::HDataBase( void ) : HPointerFromThisInterface<HDataBase>(), f_pvCoreData( NULL )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HDataBase::~HDataBase( void )
	{
	M_PROLOG
	if ( f_pvCoreData )
		dbwrapper::db_disconnect( f_pvCoreData );
	f_pvCoreData = NULL;
	return;
	M_EPILOG
	}

int HDataBase::connect( char const* a_pcDataBase, char const* a_pcLogin,
		char const* a_pcPassword )
	{
	M_PROLOG
	f_pvCoreData = dbwrapper::db_connect( a_pcDataBase, a_pcLogin, a_pcPassword );
	if ( ! f_pvCoreData )
		M_THROW( dbwrapper::db_error( f_pvCoreData ),
				dbwrapper::db_errno( f_pvCoreData ) );
	return ( 0 );
	M_EPILOG
	}

HRecordSet HDataBase::query( char const* a_pcQuery )
	{
	M_PROLOG
	if ( ! f_pvCoreData )
		M_THROW( "not connected to database", errno );
	if ( HLog::f_lLogMask & LOG_TYPE::D_SQL )
		log << "SQL: " << a_pcQuery << endl;
	void* l_pvResult = dbwrapper::db_query( f_pvCoreData, a_pcQuery );
	if ( ! l_pvResult )
		throw HSQLException( HString( "SQL error: " ) + dbwrapper::db_error ( f_pvCoreData ) );
	return ( HRecordSet( get_pointer(), l_pvResult ) );
	M_EPILOG
	}

char const * HDataBase::get_error ( void ) const
	{
	M_PROLOG
	return ( dbwrapper::db_error( f_pvCoreData ) );
	M_EPILOG
	}

HDataBase::ptr_t HDataBase::get_connector( void )
	{
	return ( ptr_t( new HDataBase() ) );
	}

}

}

