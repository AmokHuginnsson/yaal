/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

  hdatabase.cxx - this file is integral part of `yaal' project.

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

#include <string.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hdatabase.hxx"
#include "hcore/memory.hxx"
#include "hcore/hlog.hxx"
#include "db_driver_loader.hxx"
#include "dbwrapper.hxx"

using namespace yaal::hcore;

namespace yaal {

namespace dbwrapper {

HDataBase::HDataBase( void )
	: HPointerFromThisInterface<HDataBase>()
	, _connector( nullptr )
	, _dbLink() {
	M_PROLOG
	return;
	M_EPILOG
}

HDataBase::~HDataBase( void ) {
	M_PROLOG
	if ( _dbLink._conn ) {
		(_connector->db_disconnect)( _dbLink );
	}
	_dbLink.clear();
	return;
	M_DESTRUCTOR_EPILOG
}

void HDataBase::connect( yaal::hcore::HString const& dataBase_, yaal::hcore::HString const& login_,
		yaal::hcore::HString const& password_, yaal::hcore::HString const& server_ ) {
	M_PROLOG
	M_ASSERT( _connector && _connector->db_connect );
	if ( ! (_connector->db_connect)( _dbLink, dataBase_, login_, password_, server_ ) ) {
		M_THROW( (_connector->dbrs_error)( _dbLink, nullptr ), (_connector->dbrs_errno)( _dbLink, nullptr ) );
	}
	return;
	M_EPILOG
}

HRecordSet::ptr_t HDataBase::execute_query( HString const& query_, HRecordSet::CURSOR cursor_ ) {
	M_PROLOG
	if ( ! _dbLink._valid ) {
		M_THROW( "not connected to database", errno );
	}
	if ( _logSQL_ ) {
		log << "SQL: " << query_ << endl;
	}
	void* result( cursor_ == HRecordSet::CURSOR::RANDOM_ACCESS ? (_connector->db_fetch_query_result)( _dbLink, query_.raw() ) : (_connector->db_query)( _dbLink, query_.raw() ) );
	if ( (_connector->dbrs_errno)( _dbLink, result ) ) {
		HString message( "SQL execute error: '"_ys.append( query_ ).append( "': " ).append( (_connector->dbrs_error)( _dbLink, result ) ) );
		log( LOG_LEVEL::ERROR ) << message << endl;
		if ( result ) {
			if ( cursor_ == HRecordSet::CURSOR::RANDOM_ACCESS ) {
				(_connector->rs_free_query_result)( result );
			} else {
				(_connector->rs_free_cursor)( result );
			}
		}
		throw HSQLException( message );
	}
	return ( make_pointer<HRecordSet>( get_pointer(), _connector, result, cursor_ ) );
	M_EPILOG
}

HQuery::ptr_t HDataBase::prepare_query( HString const& query_ ) {
	M_PROLOG
	if ( ! _dbLink._valid ) {
		M_THROW( "not connected to database", errno );
	}
	void* result( (_connector->db_prepare_query)( _dbLink, query_.raw() ) );
	if ( (_connector->dbrs_errno)( _dbLink, result ) ) {
		HString message( "SQL prepare error: '"_ys.append( query_ ).append( "': " ).append( (_connector->dbrs_error)( _dbLink, result ) ) );
		log( LOG_LEVEL::ERROR ) << message << endl;
		if ( result ) {
			(_connector->rs_free_cursor)( result );
		}
		throw HSQLException( message );
	}
	return ( make_pointer<HQuery>( get_pointer(), _connector, query_, result ) );
	M_EPILOG
}

char const* HDataBase::get_error( void ) const {
	M_PROLOG
	return ( (_connector->dbrs_error)( _dbLink, nullptr ) );
	M_EPILOG
}

int HDataBase::get_errno( void ) const {
	M_PROLOG
	return ( (_connector->dbrs_errno)( _dbLink, nullptr ) );
	M_EPILOG
}

HDataBase::ptr_t HDataBase::get_connector( ODBConnector::DRIVER::enum_t driverId_ ) {
	M_PROLOG
	ptr_t p( make_pointer<HDataBase>() );
	M_ENSURE( p->_connector = load_driver( driverId_ ) );
	return ( p );
	M_EPILOG
}

HDataBase::table_list_t HDataBase::get_tables( void ) const {
	M_PROLOG
	M_ASSERT( _connector->_tableListQuery );
	table_list_t tl;
	HRecordSet::ptr_t rs( const_cast<HDataBase*>( this )->execute_query( _connector->_tableListQuery ) );
	for ( HRecordSet::iterator it( rs->begin() ), end( rs->end() ); it != end; ++ it ) {
		tl.push_back( *it[0] );
	}
	return ( tl );
	M_EPILOG
}

HDataBase::column_list_t HDataBase::get_columns( yaal::hcore::HString const& tableName_ ) const {
	M_PROLOG
	M_ASSERT( _connector->_columnListQuery );
	column_list_t cl;
	HString q;
	q.format( _connector->_columnListQuery, tableName_.raw() );
	HRecordSet::ptr_t rs( const_cast<HDataBase*>( this )->execute_query( q ) );
	for ( HRecordSet::iterator it( rs->begin() ), end( rs->end() ); it != end; ++ it ) {
		cl.push_back( *it[_connector->_columnNameIndex] );
	}
	return ( cl );
	M_EPILOG
}

}

}

