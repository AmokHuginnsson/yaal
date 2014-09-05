/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	sqlite3_driver.cxx - this file is integral part of `yaal' project.

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
#include <cstdlib>
#include <errno.h>
#include <cstring>
#include <sys/stat.h>
#include <sqlite3.h>

#include "hcore/base.hxx"
#include "hcore/hexception.hxx"
#include "hcore/memory.hxx"
#include "hcore/compat.hxx"
#include "hcore/hstring.hxx"
#include "dbwrapper/db_driver.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::dbwrapper;

extern "C" {

M_EXPORT_SYMBOL char* TABLE_LIST_QUERY = const_cast<char*>( "SELECT name FROM"
		  " (SELECT * FROM sqlite_master UNION ALL"
			"  SELECT * FROM sqlite_temp_master)"
			" WHERE type='table'"
			" ORDER BY name;" );
M_EXPORT_SYMBOL char* COLUMN_LIST_QUERY = const_cast<char*>( "PRAGMA TABLE_INFO( %s );" );
M_EXPORT_SYMBOL int COLUMN_NAME_INDEX = 1;

struct OSQLite {
	int _errorCode;
	HString _errorMessage;
	sqlite3* _db;
	OSQLite( void )
		: _errorCode( 0 ), _errorMessage(), _db( NULL ) {}
private:
	OSQLite( OSQLite const& );
	OSQLite& operator = ( OSQLite const& );
};

struct OSQLiteResult {
	int _rows;
	int _columns;
	void* _data;
	int _errorCode;
	HString _errorMessage;
	bool _randomAccess;
	OSQLiteResult( void )
		: _rows( 0 ), _columns( 0 ), _data( NULL ),
		_errorCode( 0 ), _errorMessage(), _randomAccess( false ) {}
private:
	OSQLiteResult( OSQLiteResult const& );
	OSQLiteResult& operator = ( OSQLiteResult const& );
};

void* yaal_sqlite3_db_fetch_query_result( ODBLink&, char const* );
void yaal_sqlite3_rs_free_query_result( void* );
void* yaal_db_query( ODBLink&, char const* );
void yaal_rs_free_cursor( void* );
void yaal_sqlite3_db_disconnect( ODBLink& );

/* sqlite driver uses convention that database file name should have
 * .sqlite extension or no extension at all, and this default extension is added
 * to user supplied database name by driver during db_connect. */

M_EXPORT_SYMBOL bool db_connect( ODBLink&, yaal::hcore::HString const&,
		yaal::hcore::HString const&, yaal::hcore::HString const&, yaal::hcore::HString const& );
M_EXPORT_SYMBOL bool db_connect( ODBLink& dbLink_, HString const& dataBase_,
		HString const&, HString const&, HString const& ) {
	do {
		HScopedValueReplacement<int> saveErrno( errno, 0 );
		struct stat stat;
		OSQLite* sQLite( NULL );
		dbLink_._conn = sQLite = new ( memory::yaal ) OSQLite;
		HString dataBase( dataBase_ );
		if ( ::stat( dataBase.raw(), &stat ) ) {
			char const fileNameExt[] = ".sqlite";
			dataBase += fileNameExt;
			if ( ::stat( dataBase.raw(), &stat ) ) {
				sQLite->_errorMessage.format( "Database file `%s' does not exists.", dataBase.raw() );
				break;
			}
		}
		sQLite->_errorCode = ::sqlite3_open( dataBase.raw(), &sQLite->_db );
		if ( sQLite->_errorCode )
			sQLite->_errorMessage = ::sqlite3_errmsg( sQLite->_db );
		else {
			void* ptr( yaal_sqlite3_db_fetch_query_result( dbLink_, "PRAGMA empty_result_callbacks = ON;" ) );
			if ( ptr ) {
				yaal_sqlite3_rs_free_query_result( ptr );
				dbLink_._valid = true;
			}
		}
	} while ( false );
	return ( dbLink_._valid );
}

void yaal_sqlite3_db_disconnect( ODBLink& dbLink_ ) {
	OSQLite* sQLite( static_cast<OSQLite*>( dbLink_._conn ) );
	M_ASSERT( sQLite );
	if ( sQLite->_db )
		sqlite3_close( sQLite->_db );
	M_SAFE( delete sQLite );
	dbLink_.clear();
	return;
}
M_EXPORT_SYMBOL void db_disconnect( ODBLink& );
M_EXPORT_SYMBOL void db_disconnect( ODBLink& dbLink_ ) {
	yaal_sqlite3_db_disconnect( dbLink_ );
}

M_EXPORT_SYMBOL int dbrs_errno( ODBLink const&, void* );
M_EXPORT_SYMBOL int dbrs_errno( ODBLink const& dbLink_, void* result_ ) {
	OSQLite* sQLite( static_cast<OSQLite*>( dbLink_._conn ) );
	OSQLiteResult* r( static_cast<OSQLiteResult*>( result_ ) );
	M_ASSERT( sQLite );
	int code( errno );
	if ( r && r->_errorCode )
		code = r->_errorCode;
	else if ( sQLite ) {
		if ( sQLite->_errorCode )
			code = sQLite->_errorCode;
		else
			code = sqlite3_errcode( sQLite->_db );
	}
	return ( code );
}

M_EXPORT_SYMBOL char const* dbrs_error( ODBLink const&, void* );
M_EXPORT_SYMBOL char const* dbrs_error( ODBLink const& dbLink_, void* result_ ) {
	OSQLite* sQLite = static_cast<OSQLite*>( dbLink_._conn );
	OSQLiteResult* r = static_cast<OSQLiteResult*>( result_ );
	M_ASSERT( sQLite );
	char const* msg( "" );
	if ( r && ! r->_errorMessage.is_empty() )
		msg = r->_errorMessage.raw();
	else if ( sQLite ) {
		if ( ! sQLite->_errorMessage.is_empty() )
			return ( sQLite->_errorMessage.raw() );
		return ( sqlite3_errmsg( sQLite->_db ) );
	}
	return ( msg );
}

void* yaal_sqlite3_db_fetch_query_result( ODBLink& dbLink_, char const* query_ ) {
	OSQLite* sQLite( static_cast<OSQLite*>( dbLink_._conn ) );
	OSQLiteResult* result( new ( memory::yaal ) OSQLiteResult );
	result->_columns = 0;
	result->_rows = 0;
	result->_data = NULL;
	result->_randomAccess = true;
	char* errmsg( NULL );
	char** data = NULL; /* msvcxx initialization workaround */
	result->_errorCode = sqlite3_get_table( sQLite->_db,
			query_, &data, &result->_rows,
			&result->_columns, &errmsg );
	result->_data = data;
	result->_errorMessage = errmsg;
	return ( result );
}
M_EXPORT_SYMBOL void* db_fetch_query_result( ODBLink&, char const* );
M_EXPORT_SYMBOL void* db_fetch_query_result( ODBLink& dbLink_, char const* query_ ) {
	M_ASSERT( dbLink_._conn && dbLink_._valid );
	return ( yaal_sqlite3_db_fetch_query_result( dbLink_, query_ ) );
}

void yaal_sqlite3_rs_free_query_result( void* data_ ) {
	OSQLiteResult* pr = static_cast<OSQLiteResult*>( data_ );
	M_ASSERT( pr->_randomAccess );
	sqlite3_free_table( static_cast<char**>( pr->_data ) );
	M_SAFE( delete pr );
	return;
}
M_EXPORT_SYMBOL void rs_free_query_result( void* );
M_EXPORT_SYMBOL void rs_free_query_result( void* data_ ) {
	yaal_sqlite3_rs_free_query_result( data_ );
}

void* yaal_db_query( ODBLink& dbLink_, char const* query_ ) {
	M_ASSERT( dbLink_._conn && dbLink_._valid );
	HScopedValueReplacement<int> saveErrno( errno, 0 );
	OSQLite* sQLite( static_cast<OSQLite*>( dbLink_._conn ) );
	OSQLiteResult* result( new ( memory::yaal ) OSQLiteResult );
	result->_columns = 0;
	result->_rows = 0;
	result->_data = NULL;
	result->_randomAccess = false;
	sqlite3_stmt* stmt( NULL );
	result->_errorCode = sqlite3_prepare_v2( sQLite->_db, query_, -1, &stmt, NULL );
	result->_data = stmt;
	result->_columns = sqlite3_column_count( stmt );
	if ( result->_errorCode != SQLITE_OK )
		result->_errorMessage = sqlite3_errmsg( sQLite->_db );
	return ( result );
}

M_EXPORT_SYMBOL void* db_query( ODBLink&, char const* );
M_EXPORT_SYMBOL void* db_query( ODBLink& dbLink_, char const* query_ ) {
	return ( yaal_db_query( dbLink_, query_ ) );
}

M_EXPORT_SYMBOL void* db_prepare_query( ODBLink&, char const* );
M_EXPORT_SYMBOL void* db_prepare_query( ODBLink& dbLink_, char const* query_ ) {
	return ( yaal_db_query( dbLink_, query_ ) );
}

M_EXPORT_SYMBOL void query_bind( ODBLink&, void*, int, yaal::hcore::HString const& );
M_EXPORT_SYMBOL void query_bind( ODBLink&, void* data_, int argNo_, yaal::hcore::HString const& value_ ) {
	OSQLiteResult* result( static_cast<OSQLiteResult*>( data_ ) );
	sqlite3_bind_text( static_cast<sqlite3_stmt*>( result->_data ), argNo_, value_.c_str(), static_cast<int>( value_.get_length() ), SQLITE_STATIC );
	return;
}

M_EXPORT_SYMBOL void* query_execute( ODBLink&, void* );
M_EXPORT_SYMBOL void* query_execute( ODBLink&, void* data_ ) {
	return ( data_ );
}

M_EXPORT_SYMBOL void query_free( ODBLink&, void* );
M_EXPORT_SYMBOL void query_free( ODBLink&, void* data_ ) {
	yaal_rs_free_cursor( data_ );
	return;
}

void yaal_rs_free_cursor( void* data_ ) {
	OSQLiteResult* result( static_cast<OSQLiteResult*>( data_ ) );
	sqlite3_finalize( static_cast<sqlite3_stmt*>( result->_data ) );
	M_SAFE( delete result );
	return;
}
M_EXPORT_SYMBOL void rs_free_cursor( void* );
M_EXPORT_SYMBOL void rs_free_cursor( void* data_ ) {
	yaal_rs_free_cursor( data_ );
	return;
}

M_EXPORT_SYMBOL char const* rs_get( void*, int long, int );
M_EXPORT_SYMBOL char const* rs_get( void* data_, int long row_, int column_ ) {
	OSQLiteResult* result( static_cast<OSQLiteResult*>( data_ ) );
	M_ASSERT( result->_randomAccess );
	char** data = static_cast<char**>( result->_data ); /* msvcxx initialization workaround */
	return ( data[ ( row_ + 1 ) * result->_columns + column_ ] );
}

M_EXPORT_SYMBOL bool rs_next( void* );
M_EXPORT_SYMBOL bool rs_next( void* data_ ) {
	HScopedValueReplacement<int> saveErrno( errno, 0 );
	OSQLiteResult* result( static_cast<OSQLiteResult*>( data_ ) );
	return ( sqlite3_step( static_cast<sqlite3_stmt*>( result->_data ) ) == SQLITE_ROW );
}

M_EXPORT_SYMBOL char const* rs_get_field( void*, int );
M_EXPORT_SYMBOL char const* rs_get_field( void* data_, int field_ ) {
	OSQLiteResult* result( static_cast<OSQLiteResult*>( data_ ) );
	return ( reinterpret_cast<char const*>( sqlite3_column_text( static_cast<sqlite3_stmt*>( result->_data ), field_ ) ) );
}

M_EXPORT_SYMBOL int rs_fields_count( void* );
M_EXPORT_SYMBOL int rs_fields_count( void* data_ ) {
	return ( static_cast<OSQLiteResult*>( data_ )->_columns );
}

M_EXPORT_SYMBOL int long dbrs_records_count( ODBLink&, void* );
M_EXPORT_SYMBOL int long dbrs_records_count( ODBLink& dbLink_, void* dataR_ ) {
	M_ASSERT( dbLink_._conn && dbLink_._valid );
	OSQLiteResult* result( static_cast<OSQLiteResult*>( dataR_ ) );
	if ( result && ( result->_columns > 0 ) )
		return ( result->_rows );
	else
		return ( ::sqlite3_changes( static_cast<OSQLite*>( dbLink_._conn )->_db ) );
}

M_EXPORT_SYMBOL int long dbrs_id( ODBLink&, void* );
M_EXPORT_SYMBOL int long dbrs_id( ODBLink& dbLink_, void* ) {
	M_ASSERT( dbLink_._conn && dbLink_._valid );
	/* FIXME change driver interface to allow 64bit insert row id (from autoincrement) */
	return ( static_cast<int long>( sqlite3_last_insert_rowid( static_cast<OSQLite*>( dbLink_._conn )->_db ) ) );
}

M_EXPORT_SYMBOL char const* rs_column_name( void*, int );
M_EXPORT_SYMBOL char const* rs_column_name( void* dataR_, int field_ ) {
	OSQLiteResult* result( static_cast<OSQLiteResult*>( dataR_ ) );
	return ( result->_randomAccess ? static_cast<char**>( result->_data )[ field_ ] : sqlite3_column_name( static_cast<sqlite3_stmt*>( result->_data ), field_ ) );
}

int yaal_sqlite3_driver_main( int, char** );
int yaal_sqlite3_driver_main( int, char** ) {
	return ( 0 );
}

}

