
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

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::dbwrapper;

extern "C" {

M_EXPORT_SYMBOL bool db_connect( ODBLink& dbLink_, char const* dataBase_,
		char const* login_, char const * password_ ) {
	MYSQL* mySQL( NULL );
	dbLink_._conn = mySQL = mysql_init( NULL );
	if ( mySQL ) {
		int unsigned protocol( MYSQL_PROTOCOL_SOCKET );
		if ( ! mysql_options( mySQL, MYSQL_OPT_PROTOCOL, &protocol ) ) {
			if ( mysql_real_connect( mySQL, NULL,
						login_, password_, dataBase_,
						0, NULL, CLIENT_IGNORE_SPACE | CLIENT_IGNORE_SIGPIPE ) )
				dbLink_._valid = true;
		}
	}
	return ( ! dbLink_._valid );
}

M_EXPORT_SYMBOL void db_disconnect( ODBLink& dbLink_ ) {
	if ( dbLink_._conn ) {
		mysql_close( static_cast<MYSQL*>( dbLink_._conn ) );
		dbLink_.clear();
	}
	return;
}

M_EXPORT_SYMBOL int dbrs_errno( ODBLink const& dbLink_, void* ) {
	return ( ::mysql_errno( static_cast<MYSQL*>( dbLink_._conn ) ) );
}

M_EXPORT_SYMBOL char const* dbrs_error( ODBLink const& dbLink_, void* ) {
	return ( ::mysql_error( static_cast<MYSQL*>( dbLink_._conn ) ) );
}

M_EXPORT_SYMBOL void* db_query( ODBLink& dbLink_, char const* query_ ) {
	M_ASSERT( dbLink_._conn && dbLink_._valid );
	mysql_query( static_cast<MYSQL*>( dbLink_._conn ), query_ );
	return ( mysql_store_result( static_cast<MYSQL*>( dbLink_._conn ) ) );
}

M_EXPORT_SYMBOL void rs_unquery( void* data_ ) {
	mysql_free_result( static_cast<MYSQL_RES*>( data_ ) );
	return;
}

M_EXPORT_SYMBOL char const* rs_get( void* data_, int long row_, int column_ ) {
	MYSQL_ROW row;
	mysql_data_seek( static_cast<MYSQL_RES*>( data_ ), row_ );
	row = mysql_fetch_row( static_cast<MYSQL_RES*>( data_ ) );
	return ( row [ column_ ] );
}

M_EXPORT_SYMBOL int rs_fields_count( void* data_ ) {
	return ( data_ ? ::mysql_num_fields( static_cast<MYSQL_RES*>( data_ ) ) : 0 );
}

M_EXPORT_SYMBOL int long dbrs_records_count( ODBLink& dbLink_, void* dataR_ ) {
	M_ASSERT( dbLink_._conn && dbLink_._valid );
	if ( dataR_ )
		return ( static_cast<int long>( mysql_num_rows( static_cast<MYSQL_RES*>( dataR_ ) ) ) );
	else
		return ( static_cast<int long>( mysql_affected_rows( static_cast<MYSQL*>( dbLink_._conn ) ) ) );
}

M_EXPORT_SYMBOL int long dbrs_id( ODBLink& dbLink_, void* ) {
	M_ASSERT( dbLink_._conn && dbLink_._valid );
	return ( static_cast<int long>( mysql_insert_id( static_cast<MYSQL*>( dbLink_._conn ) ) ) );
}

M_EXPORT_SYMBOL char const* rs_column_name( void* dataR_, int field_ ) {
	MYSQL_FIELD* field = NULL;
	field = mysql_fetch_field_direct( static_cast<MYSQL_RES*>( dataR_ ), field_ );
	return ( field->name );
}

int yaal_mysql_driver_main( int, char** ) __attribute__(( __noreturn__ ));
int yaal_mysql_driver_main( int, char** ) {
	::exit( 0 );
}

}

namespace {

class HMySQLInitDeinit {
public:
	HMySQLInitDeinit( void );
	~HMySQLInitDeinit( void );
} mySQLInitDeinit;

HMySQLInitDeinit::HMySQLInitDeinit( void ) {
	mysql_library_init( 0, NULL, NULL );
	return;
}

HMySQLInitDeinit::~HMySQLInitDeinit( void ) {
	mysql_library_end();
	return;
}

}

