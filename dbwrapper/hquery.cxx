/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  hquery.cxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hquery.hxx"
#include "hdatabase.hxx"
#include "hrecordset.hxx"
#include "dbwrapper.hxx"
#include "hcore/hlog.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace dbwrapper {

HQuery::HQuery(
		database_ptr_t database_,
		ODBConnector const* connector_,
		yaal::hcore::HString const& sql_,
		void* query_
) : _dataBase( database_ ), _connector( connector_ ),
	_query( query_ ), _sql( sql_ ),
	_bindBuffer() {
	return;
}

HQuery::~HQuery( void ) {
	if ( _query ) {
		(*_connector->query_free)( _dataBase->_dbLink, _query );
		_query = NULL;
	}
	return;
}

void HQuery::bind( int parameterNo_, yaal::hcore::HString const& parameterValue_ ) {
	M_PROLOG
	(*_connector->query_bind)( _dataBase->_dbLink, _query, parameterNo_, _bindBuffer[parameterNo_] = parameterValue_ );
	return;
	M_EPILOG
}

HRecordSet::ptr_t HQuery::execute( void ) {
	M_PROLOG
	if ( ! _dataBase->_dbLink._valid ) {
		M_THROW( "not connected to database", errno );
	}
	if ( _logSQL_ ) {
		log << "SQL: " << _sql << endl;
	}
	void* result( (_connector->query_execute)( _dataBase->_dbLink, _query ) );
	if ( (_connector->dbrs_errno)( _dataBase->_dbLink, result ) ) {
		throw HSQLException( HString( "SQL error: " ) + (_connector->dbrs_error)( _dataBase->_dbLink, result ) );
	}
	return ( make_pointer<HRecordSet>( _dataBase, _connector, result, HRecordSet::CURSOR::FORWARD ) );
	M_EPILOG
}

}

}

