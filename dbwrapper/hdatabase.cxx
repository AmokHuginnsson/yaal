/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hdatabase.cxx - this file is integral part of `yaal' project.

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

#include <string.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "hcore/hlog.hxx"
#include "hdatabase.hxx"
#include "db_driver_loader.hxx"

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

int HDataBase::connect( yaal::hcore::HString const& a_oDataBase, yaal::hcore::HString const& a_oLogin,
		yaal::hcore::HString const& a_oPassword )
	{
	M_PROLOG
	f_pvCoreData = dbwrapper::db_connect( a_oDataBase.raw(), a_oLogin.raw(), a_oPassword.raw() );
	if ( ! f_pvCoreData )
		M_THROW( dbwrapper::db_error( f_pvCoreData ),
				dbwrapper::db_errno( f_pvCoreData ) );
	return ( 0 );
	M_EPILOG
	}

HRecordSet::ptr_t HDataBase::query( HString const& a_oQuery )
	{
	M_PROLOG
	if ( ! f_pvCoreData )
		M_THROW( "not connected to database", errno );
	if ( HLog::f_lLogMask & LOG_TYPE::D_SQL )
		log << "SQL: " << a_oQuery << endl;
	void* l_pvResult = dbwrapper::db_query( f_pvCoreData, a_oQuery.raw() );
	if ( ! l_pvResult )
		throw HSQLException( HString( "SQL error: " ) + dbwrapper::db_error ( f_pvCoreData ) );
	HRecordSet::ptr_t rs( new HRecordSet( get_pointer(), l_pvResult ) );
	return ( rs );
	M_EPILOG
	}

char const* HDataBase::get_error( void ) const
	{
	M_PROLOG
	return ( dbwrapper::db_error( f_pvCoreData ) );
	M_EPILOG
	}

int HDataBase::get_errno( void ) const
	{
	M_PROLOG
	return ( dbwrapper::db_errno( f_pvCoreData ) );
	M_EPILOG
	}

HDataBase::ptr_t HDataBase::get_connector( void )
	{
	M_PROLOG
	return ( ptr_t( new HDataBase() ) );
	M_EPILOG
	}

}

}

