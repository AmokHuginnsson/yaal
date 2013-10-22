/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	mysql_driver.cxx - this file is integral part of `yaal' project.

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

#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "config.hxx"

#ifdef __HOST_OS_TYPE_FREEBSD__
#	define TO_BE_INCLUDED_LATER 1
#endif /* __HOST_OS_TYPE_FREEBSD__ */

#include <mysql/mysql.h>

#include "hcore/memory.hxx"
#include "dbwrapper/db_driver.hxx"
#include "hcore/hprogramoptionshandler.hxx"
#include "hcore/hcore.hxx"
#include "hcore/hlog.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::dbwrapper;

namespace {

char const AUTODETECT_CHARSET[] = "autodetect";
HString _clientCharacterSet_ = AUTODETECT_CHARSET;

}

extern "C" {

M_EXPORT_SYMBOL char* TABLE_LIST_QUERY = const_cast<char*>( "SHOW TABLES;" );
M_EXPORT_SYMBOL char* COLUMN_LIST_QUERY = const_cast<char*>( "SHOW COLUMNS FROM %s;" );
M_EXPORT_SYMBOL int COLUMN_NAME_INDEX = 0;

struct OMySQLResult {
	ODBLink& _link;
	MYSQL_RES* _result;
	MYSQL_ROW _row;
	bool _randomAccess;
	OMySQLResult( ODBLink& link_ )
		: _link( link_ ), _result( NULL ), _row(), _randomAccess( false ) {}
private:
	OMySQLResult( OMySQLResult const& );
	OMySQLResult& operator = ( OMySQLResult const& );
};

M_EXPORT_SYMBOL void driver_init( void );
M_EXPORT_SYMBOL void driver_init( void ) {
	M_PROLOG
	log << "Initializing MySQL driver." << endl;
	M_ENSURE( mysql_library_init( 0, NULL, NULL ) == 0 );
	return;
	M_EPILOG
}

M_EXPORT_SYMBOL void driver_cleanup( void );
M_EXPORT_SYMBOL void driver_cleanup( void ) {
	M_PROLOG
	log << "Cleaning up after MySQL driver." << endl;
	mysql_library_end();
	return;
	M_EPILOG
}

M_EXPORT_SYMBOL bool db_connect( ODBLink&, yaal::hcore::HString const&,
		yaal::hcore::HString const&, yaal::hcore::HString const&, yaal::hcore::HString const& );
M_EXPORT_SYMBOL bool db_connect( ODBLink& dbLink_, yaal::hcore::HString const& dataBase_,
		yaal::hcore::HString const& login_, yaal::hcore::HString const& password_, yaal::hcore::HString const& ) {
	MYSQL* mySQL( NULL );
	dbLink_._conn = mySQL = mysql_init( NULL );
	if ( mySQL ) {
		int unsigned protocol( MYSQL_PROTOCOL_SOCKET );
		if ( ! ( mysql_options( mySQL, MYSQL_OPT_PROTOCOL, &protocol ) || mysql_options( mySQL, MYSQL_SET_CHARSET_NAME, _clientCharacterSet_.raw() ) ) ) {
			if ( mysql_real_connect( mySQL, NULL,
						login_.raw(), password_.raw(), dataBase_.raw(),
						0, NULL, CLIENT_IGNORE_SPACE | CLIENT_IGNORE_SIGPIPE ) )
				dbLink_._valid = true;
		}
	}
	return ( ! dbLink_._valid );
}

M_EXPORT_SYMBOL void db_disconnect( ODBLink& );
M_EXPORT_SYMBOL void db_disconnect( ODBLink& dbLink_ ) {
	if ( dbLink_._conn ) {
		mysql_close( static_cast<MYSQL*>( dbLink_._conn ) );
		dbLink_.clear();
	}
	return;
}

M_EXPORT_SYMBOL int dbrs_errno( ODBLink const&, void* );
M_EXPORT_SYMBOL int dbrs_errno( ODBLink const& dbLink_, void* ) {
	return ( ::mysql_errno( static_cast<MYSQL*>( dbLink_._conn ) ) );
}

M_EXPORT_SYMBOL char const* dbrs_error( ODBLink const&, void* );
M_EXPORT_SYMBOL char const* dbrs_error( ODBLink const& dbLink_, void* ) {
	return ( ::mysql_error( static_cast<MYSQL*>( dbLink_._conn ) ) );
}

M_EXPORT_SYMBOL void* db_fetch_query_result( ODBLink&, char const* );
M_EXPORT_SYMBOL void* db_fetch_query_result( ODBLink& dbLink_, char const* query_ ) {
	M_ASSERT( dbLink_._conn && dbLink_._valid );
	OMySQLResult* result( new ( memory::yaal ) OMySQLResult( dbLink_ ) );
	::mysql_query( static_cast<MYSQL*>( dbLink_._conn ), query_ );
	result->_randomAccess = true;
	result->_result = ::mysql_store_result( static_cast<MYSQL*>( dbLink_._conn ) );
	return ( result );
}

M_EXPORT_SYMBOL void rs_free_query_result( void* );
M_EXPORT_SYMBOL void rs_free_query_result( void* data_ ) {
	OMySQLResult* pr( static_cast<OMySQLResult*>( data_ ) );
	::mysql_free_result( pr->_result );
	M_SAFE( delete pr );
	return;
}

M_EXPORT_SYMBOL void* db_query( ODBLink&, char const* );
M_EXPORT_SYMBOL void* db_query( ODBLink& dbLink_, char const* query_ ) {
	M_ASSERT( dbLink_._conn && dbLink_._valid );
	OMySQLResult* result( new ( memory::yaal ) OMySQLResult( dbLink_ ) );
	::mysql_query( static_cast<MYSQL*>( dbLink_._conn ), query_ );
	result->_randomAccess = false;
	result->_result = ::mysql_use_result( static_cast<MYSQL*>( dbLink_._conn ) );
	return ( result );
}

M_EXPORT_SYMBOL void rs_free_cursor( void* );
M_EXPORT_SYMBOL void rs_free_cursor( void* data_ ) {
	OMySQLResult* pr( static_cast<OMySQLResult*>( data_ ) );
	::mysql_free_result( pr->_result );
	M_SAFE( delete pr );
	return;
}

M_EXPORT_SYMBOL char const* rs_get( void*, int long, int );
M_EXPORT_SYMBOL char const* rs_get( void* data_, int long row_, int column_ ) {
	MYSQL_ROW row;
	mysql_data_seek( static_cast<OMySQLResult*>( data_ )->_result, row_ );
	row = mysql_fetch_row( static_cast<OMySQLResult*>( data_ )->_result );
	return ( row[ column_ ] );
}

M_EXPORT_SYMBOL bool rs_next( void* );
M_EXPORT_SYMBOL bool rs_next( void* data_ ) {
	OMySQLResult* pr( static_cast<OMySQLResult*>( data_ ) );
	pr->_row = mysql_fetch_row( pr->_result );
	return ( pr->_row == NULL );
}

M_EXPORT_SYMBOL char const* rs_get_field( void*, int );
M_EXPORT_SYMBOL char const* rs_get_field( void* data_, int field_ ) {
	OMySQLResult* pr( static_cast<OMySQLResult*>( data_ ) );
	return ( pr->_row[field_] );
}

M_EXPORT_SYMBOL int rs_fields_count( void* );
M_EXPORT_SYMBOL int rs_fields_count( void* data_ ) {
	OMySQLResult* pr( static_cast<OMySQLResult*>( data_ ) );
	return ( ( pr && pr->_result ) ? ::mysql_num_fields( pr->_result ) : 0 );
}

M_EXPORT_SYMBOL int long dbrs_records_count( ODBLink&, void* );
M_EXPORT_SYMBOL int long dbrs_records_count( ODBLink& dbLink_, void* dataR_ ) {
	M_ASSERT( dbLink_._conn && dbLink_._valid );
	OMySQLResult* pr( static_cast<OMySQLResult*>( dataR_ ) );
	if ( pr && pr->_result )
		return ( static_cast<int long>( ::mysql_num_rows( pr->_result ) ) );
	else
		return ( static_cast<int long>( ::mysql_affected_rows( static_cast<MYSQL*>( dbLink_._conn ) ) ) );
}

M_EXPORT_SYMBOL int long dbrs_id( ODBLink&, void* );
M_EXPORT_SYMBOL int long dbrs_id( ODBLink& dbLink_, void* ) {
	M_ASSERT( dbLink_._conn && dbLink_._valid );
	return ( static_cast<int long>( mysql_insert_id( static_cast<MYSQL*>( dbLink_._conn ) ) ) );
}

M_EXPORT_SYMBOL char const* rs_column_name( void*, int );
M_EXPORT_SYMBOL char const* rs_column_name( void* dataR_, int field_ ) {
	MYSQL_FIELD* field = NULL;
	field = mysql_fetch_field_direct( static_cast<OMySQLResult*>( dataR_ )->_result, field_ );
	return ( field->name );
}

}

namespace {

class HMySQLInitDeinit {
public:
	HMySQLInitDeinit( void );
} mySQLInitDeinit;

HMySQLInitDeinit::HMySQLInitDeinit( void ) {
	M_PROLOG
	yaal_options()( "client_character_set",
			program_options_helper::option_value( _clientCharacterSet_ ),
			HProgramOptionsHandler::OOption::TYPE::REQUIRED,
			"character set used by mysql client", "name" );
	yaal_options().process_rc_file( "yaal", "mysql", NULL );
#if defined( HAVE_DECL_MYSQL_AUTODETECT_CHARSET_NAME ) && ( HAVE_DECL_MYSQL_AUTODETECT_CHARSET_NAME == 1 )
	if ( _clientCharacterSet_ == AUTODETECT_CHARSET )
		_clientCharacterSet_ = MYSQL_AUTODETECT_CHARSET_NAME;
#endif /* #ifdef defined( HAVE_DECL_MYSQL_AUTODETECT_CHARSET_NAME ) && ( HAVE_DECL_MYSQL_AUTODETECT_CHARSET_NAME == 1 ) */
	char const* characterSetOverride( ::getenv( "YAAL_MYSQL_CLIENT_CHARACTER_SET" ) );
	if ( characterSetOverride )
		_clientCharacterSet_ = characterSetOverride;
	return;
	M_EPILOG
}

}

extern "C" {

int yaal_mysql_driver_main( int, char** );
int yaal_mysql_driver_main( int, char** ) {
	return ( 0 );
}

}

