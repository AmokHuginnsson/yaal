
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

M_EXPORT_SYMBOL void* db_query( ODBLink&, char const* );
M_EXPORT_SYMBOL void* db_query( ODBLink& dbLink_, char const* query_ ) {
	M_ASSERT( dbLink_._conn && dbLink_._valid );
	mysql_query( static_cast<MYSQL*>( dbLink_._conn ), query_ );
	return ( mysql_store_result( static_cast<MYSQL*>( dbLink_._conn ) ) );
}

M_EXPORT_SYMBOL void rs_unquery( void* );
M_EXPORT_SYMBOL void rs_unquery( void* data_ ) {
	mysql_free_result( static_cast<MYSQL_RES*>( data_ ) );
	return;
}

M_EXPORT_SYMBOL char const* rs_get( void*, int long, int );
M_EXPORT_SYMBOL char const* rs_get( void* data_, int long row_, int column_ ) {
	MYSQL_ROW row;
	mysql_data_seek( static_cast<MYSQL_RES*>( data_ ), row_ );
	row = mysql_fetch_row( static_cast<MYSQL_RES*>( data_ ) );
	return ( row [ column_ ] );
}

M_EXPORT_SYMBOL int rs_fields_count( void* );
M_EXPORT_SYMBOL int rs_fields_count( void* data_ ) {
	return ( data_ ? ::mysql_num_fields( static_cast<MYSQL_RES*>( data_ ) ) : 0 );
}

M_EXPORT_SYMBOL int long dbrs_records_count( ODBLink&, void* );
M_EXPORT_SYMBOL int long dbrs_records_count( ODBLink& dbLink_, void* dataR_ ) {
	M_ASSERT( dbLink_._conn && dbLink_._valid );
	if ( dataR_ )
		return ( static_cast<int long>( mysql_num_rows( static_cast<MYSQL_RES*>( dataR_ ) ) ) );
	else
		return ( static_cast<int long>( mysql_affected_rows( static_cast<MYSQL*>( dbLink_._conn ) ) ) );
}

M_EXPORT_SYMBOL int long dbrs_id( ODBLink&, void* );
M_EXPORT_SYMBOL int long dbrs_id( ODBLink& dbLink_, void* ) {
	M_ASSERT( dbLink_._conn && dbLink_._valid );
	return ( static_cast<int long>( mysql_insert_id( static_cast<MYSQL*>( dbLink_._conn ) ) ) );
}

M_EXPORT_SYMBOL char const* rs_column_name( void*, int );
M_EXPORT_SYMBOL char const* rs_column_name( void* dataR_, int field_ ) {
	MYSQL_FIELD* field = NULL;
	field = mysql_fetch_field_direct( static_cast<MYSQL_RES*>( dataR_ ), field_ );
	return ( field->name );
}

}

namespace {

class HMySQLInitDeinit {
public:
	HMySQLInitDeinit( void );
	~HMySQLInitDeinit( void );
} mySQLInitDeinit;

HMySQLInitDeinit::HMySQLInitDeinit( void ) {
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
	if ( ! ::getenv( "BUGGY_MYSQL_CLIENT" ) )
		mysql_library_init( 0, NULL, NULL );
	return;
}

HMySQLInitDeinit::~HMySQLInitDeinit( void ) {
	if ( ! ::getenv( "BUGGY_MYSQL_CLIENT" ) )
		mysql_library_end();
	return;
}

}

extern "C" {

int yaal_mysql_driver_main( int, char** );
int yaal_mysql_driver_main( int, char** ) {
	return ( 0 );
}

}

