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
M_VCSID( "$Id: "__TID__" $" )
#include "hcore/hlog.hxx"
#include "hdatabase.hxx"
#include "db_driver_loader.hxx"

using namespace yaal::hcore;

namespace yaal
{

namespace dbwrapper
{

HDataBase::HDataBase( void ) : HPointerFromThisInterface<HDataBase>(),
	_connector( NULL ), _coreData( NULL )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HDataBase::~HDataBase( void )
	{
	M_PROLOG
	if ( _coreData )
		(_connector->db_disconnect)( _coreData );
	_coreData = NULL;
	return;
	M_DESTRUCTOR_EPILOG
	}

int HDataBase::connect( yaal::hcore::HString const& dataBase_, yaal::hcore::HString const& login_,
		yaal::hcore::HString const& password_ )
	{
	M_PROLOG
	_coreData = (_connector->db_connect)( dataBase_.raw(), login_.raw(), password_.raw() );
	if ( ! _coreData )
		M_THROW( (_connector->dbrs_error)( _coreData, NULL ),
				(_connector->dbrs_errno)( _coreData, NULL ) );
	return ( 0 );
	M_EPILOG
	}

HRecordSet::ptr_t HDataBase::query( HString const& query_ )
	{
	M_PROLOG
	if ( ! _coreData )
		M_THROW( "not connected to database", errno );
	if ( HLog::_logMask & LOG_TYPE::SQL )
		log << "SQL: " << query_ << endl;
	void* result = (_connector->db_query)( _coreData, query_.raw() );
	if ( (_connector->dbrs_errno)( _coreData, result ) )
		throw HSQLException( HString( "SQL error: " ) + (_connector->dbrs_error)( _coreData, result ) );
	HRecordSet::ptr_t rs;
	if ( result )
		rs = HRecordSet::ptr_t( new HRecordSet( get_pointer(), _connector, result ) );
	return ( rs );
	M_EPILOG
	}

char const* HDataBase::get_error( void ) const
	{
	M_PROLOG
	return ( (_connector->dbrs_error)( _coreData, NULL ) );
	M_EPILOG
	}

int HDataBase::get_errno( void ) const
	{
	M_PROLOG
	return ( (_connector->dbrs_errno)( _coreData, NULL ) );
	M_EPILOG
	}

HDataBase::ptr_t HDataBase::get_connector( ODBConnector::DRIVER::enum_t driverId_ )
	{
	M_PROLOG
	ptr_t p( new HDataBase() );
	p->_connector = load_driver( driverId_ );
	return ( p );
	M_EPILOG
	}

ODBConnector const* HDataBase::connector( void ) const
	{
	return ( _connector );
	}

}

}

