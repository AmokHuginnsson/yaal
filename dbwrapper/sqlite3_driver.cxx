/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	sqlite3_driver.cxx - this file is integral part of `yaal' project.

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

using namespace yaal;
using namespace yaal::hcore;

extern "C" {

struct OSQLite {
	int _errorCode;
	HString _errorMessage;
	sqlite3* _dB;
	OSQLite( void )
		: _errorCode( 0 ), _errorMessage(), _dB( NULL ) {}
private:
	OSQLite( OSQLite const& );
	OSQLite& operator = ( OSQLite const& );
};
	
struct OSQLiteResult {
	int _rows;
	int _columns;
	char** _data;
	int _errorCode;
	HString _errorMessage;
	OSQLiteResult( void )
		: _rows( 0 ), _columns( 0 ), _data( NULL ),
		_errorCode( 0 ), _errorMessage() {}
private:
	OSQLiteResult( OSQLiteResult const& );
	OSQLiteResult& operator = ( OSQLiteResult const& );
};

OSQLite* _brokenDB_ = NULL;

void* yaal_sqlite3_db_query( void*, char const* );
void yaal_sqlite3_rs_unquery( void* );
void yaal_sqlite3_db_disconnect( void* );

/* sqlite driver uses convention that database file name should have
 * .sqlite extension or no extension at all, and this default extension is added
 * to user supplied database name by driver during db_connect. */

M_EXPORT_SYMBOL void* db_connect( char const* dataBase_,
		char const*, char const* ) {
	void* ptr = NULL;
	char const fileNameExt[] = ".sqlite";
	struct stat stat;
	OSQLite* sQLite = NULL;
	if ( _brokenDB_ ) {
		yaal_sqlite3_db_disconnect( _brokenDB_ );
		_brokenDB_ = NULL;
	}
	sQLite = new ( memory::yaal ) OSQLite;
	HString dataBase( dataBase_ );
	if ( ::stat( dataBase.raw(), &stat ) ) {
		dataBase += fileNameExt; 
		if ( ::stat( dataBase.raw(), &stat ) ) {
			sQLite->_errorMessage.format( "Database file `%s' does not exists.", dataBase.raw() );
			_brokenDB_ = sQLite;
			return ( NULL );
		}
	}
	sQLite->_errorCode = ::sqlite3_open( dataBase.raw(),
			&sQLite->_dB );
	if ( sQLite->_errorCode ) {
		sQLite->_errorMessage = ::sqlite3_errmsg( sQLite->_dB );
		_brokenDB_ = sQLite;
		sQLite = NULL;
	} else {
		ptr = yaal_sqlite3_db_query( sQLite, "PRAGMA empty_result_callbacks = ON;" );
		if ( ptr )
			yaal_sqlite3_rs_unquery( ptr );
		else
			_brokenDB_ = sQLite, sQLite = NULL;
	}
	return ( sQLite );
}

void yaal_sqlite3_db_disconnect( void* data_ ) {
	OSQLite* sQLite = static_cast<OSQLite*>( data_ );
	if ( sQLite->_dB )
		sqlite3_close( sQLite->_dB );
	M_SAFE( delete sQLite );
	return;
}
M_EXPORT_SYMBOL void db_disconnect( void* data_ ) {
	yaal_sqlite3_db_disconnect( data_ );
}

M_EXPORT_SYMBOL int dbrs_errno( void* db_, void* result_ ) {
	OSQLite* sQLite = static_cast<OSQLite*>( db_ );
	OSQLiteResult* r = static_cast<OSQLiteResult*>( result_ );
	if ( ! sQLite )
		sQLite = _brokenDB_;
	int code( errno );
	if ( r && r->_errorCode )
		code = r->_errorCode;
	else if ( sQLite ) {
		if ( sQLite->_errorCode )
			code = sQLite->_errorCode;
		else
			code = sqlite3_errcode( sQLite->_dB );
	}
	return ( code );
}

M_EXPORT_SYMBOL char const* dbrs_error( void* db_, void* result_ ) {
	OSQLite* sQLite = static_cast<OSQLite*>( db_ );
	OSQLiteResult* r = static_cast<OSQLiteResult*>( result_ );
	if ( ! sQLite )
		sQLite = _brokenDB_;
	char const* msg( "" );
	if ( r && ! r->_errorMessage.is_empty() )
		msg = r->_errorMessage.raw();
	else if ( sQLite ) {
		if ( ! sQLite->_errorMessage.is_empty() )
			return ( sQLite->_errorMessage.raw() );
		return ( sqlite3_errmsg( sQLite->_dB ) );
	}
	return ( msg );
}

void* yaal_sqlite3_db_query( void* data_, char const* query_ ) {
	OSQLite* sQLite( static_cast<OSQLite*>( data_ ) );
	OSQLiteResult* result( new ( memory::yaal ) OSQLiteResult );
	result->_columns = 0;
	result->_rows = 0;
	result->_data = NULL;
	char* errmsg = NULL;
	result->_errorCode = sqlite3_get_table( sQLite->_dB,
			query_, &result->_data, &result->_rows,
			&result->_columns, &errmsg );
	result->_errorMessage = errmsg;
	return ( result );
}
M_EXPORT_SYMBOL void* db_query( void* data_, char const* query_ ) {
	return ( yaal_sqlite3_db_query( data_, query_ ) );
}

void yaal_sqlite3_rs_unquery( void* data_ ) {
	OSQLiteResult* pr = static_cast<OSQLiteResult*>( data_ );
	sqlite3_free_table( pr->_data );
	M_SAFE( delete pr );
	return;
}
M_EXPORT_SYMBOL void rs_unquery( void* data_ ) {
	yaal_sqlite3_rs_unquery( data_ );
}

M_EXPORT_SYMBOL char const* rs_get( void* data_, int long row_, int column_ ) {
	char** data = NULL;
	OSQLiteResult* result = static_cast<OSQLiteResult*>( data_ );
	data = result->_data;
	return ( data[ ( row_ + 1 ) * result->_columns + column_ ] );
}

M_EXPORT_SYMBOL int rs_fields_count( void* data_ ) {
	return ( static_cast<OSQLiteResult*>( data_ )->_columns );
}

M_EXPORT_SYMBOL int long dbrs_records_count( void* dataB_, void* dataR_ ) {
	OSQLiteResult* result( static_cast<OSQLiteResult*>( dataR_ ) );
	if ( result && ( result->_columns > 0 ) )
		return ( result->_rows );
	else
		return ( ::sqlite3_changes( static_cast<OSQLite*>( dataB_ )->_dB ) );
}

M_EXPORT_SYMBOL int long dbrs_id( void* dataB_, void* ) {
	/* FIXME change driver interface to allow 64bit insert row id (from autoincrement) */
	return ( static_cast<int long>( sqlite3_last_insert_rowid( static_cast<OSQLite*>( dataB_ )->_dB ) ) );
}

M_EXPORT_SYMBOL char const* rs_column_name( void* dataR_, int field_ ) {
	return ( static_cast<OSQLiteResult*>( dataR_ )->_data[ field_ ] );
}

int yaal_sqlite3_driver_main( int, char** ) {
	return ( 0 );
}

}

