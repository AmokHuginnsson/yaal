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

#include <cstdlib>
#include <cstring>
#include <errno.h>

#include "config.hxx"

#ifdef HAVE_POSTGRESQL_LIBPQ_FE_H
#	include <postgresql/libpq-fe.h>
#elif defined ( HAVE_LIBPQ_FE_H )
#	include <libpq-fe.h>
#else /* HAVE_LIBPQ_FE_H */
#	error "No libpq-fe.h header available."
#endif /* not HAVE_LIBPQ_FE_H */

#include "hcore/memory.hxx"
#include "dbwrapper/db_driver.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::dbwrapper;

extern "C" {

M_EXPORT_SYMBOL char* TABLE_LIST_QUERY = const_cast<char*>( "SELECT table_name, table_schema"
		" FROM information_schema.tables"
		" WHERE table_schema NOT IN ('pg_catalog', 'information_schema');" );
M_EXPORT_SYMBOL char* COLUMN_LIST_QUERY = const_cast<char*>( "SELECT a.attnum, a.attname AS field, t.typname AS type,"
		" a.attlen AS length, a.atttypmod AS length_var,"
		" a.attnotnull AS not_null, a.atthasdef as has_default"
		" FROM pg_class c, pg_attribute a, pg_type t"
		" WHERE c.relname = '%s'"
		" AND a.attnum > 0"
		" AND a.attrelid = c.oid"
		" AND a.atttypid = t.oid"
		" ORDER BY a.attnum;" );
M_EXPORT_SYMBOL int COLUMN_NAME_INDEX = 1;

struct OPostgreSQLResult {
	ODBLink& _link;
	PGresult* _result;
	int _index;
	int _total;
	bool _randomAccess;
	OPostgreSQLResult( ODBLink& link_ )
		: _link( link_ ), _result( NULL ), _index( 0 ), _total( 0 ), _randomAccess( false ) {}
private:
	OPostgreSQLResult( OPostgreSQLResult const& );
	OPostgreSQLResult& operator = ( OPostgreSQLResult const& );
};

M_EXPORT_SYMBOL bool db_connect( ODBLink&, yaal::hcore::HString const&,
		yaal::hcore::HString const&, yaal::hcore::HString const&, yaal::hcore::HString const& );
M_EXPORT_SYMBOL bool db_connect( ODBLink& dbLink_, yaal::hcore::HString const& dataBase_,
		yaal::hcore::HString const& login_, yaal::hcore::HString const& password_, yaal::hcore::HString const& ) {
	PGconn* connection( NULL );
	dbLink_._conn = connection = PQsetdbLogin( NULL /* host */,
			NULL /* port */, NULL /* options */,
			NULL /* debugging tty */,
			dataBase_.raw(), login_.raw(), password_.raw() );
	if ( PQstatus( connection ) == CONNECTION_OK )
		dbLink_._valid = true;
	return ( dbLink_._valid );
}

M_EXPORT_SYMBOL void db_disconnect( ODBLink& );
M_EXPORT_SYMBOL void db_disconnect( ODBLink& dbLink_ ) {
	if ( dbLink_._conn ) {
		PQfinish( static_cast<PGconn*>( dbLink_._conn ) );
		dbLink_.clear();
	}
	return;
}

M_EXPORT_SYMBOL int dbrs_errno( ODBLink const&, void* );
M_EXPORT_SYMBOL int dbrs_errno( ODBLink const& dbLink_, void* result_ ) {
	M_ASSERT( dbLink_._conn );
	int err( 0 );
	if ( result_ ) {
		ExecStatusType status = PQresultStatus( static_cast<OPostgreSQLResult*>( result_ )->_result );
		err = ( ( status == PGRES_COMMAND_OK ) || ( status == PGRES_TUPLES_OK ) ) ? 0 : status;
	} else {
		ConnStatusType connStatus( PQstatus( static_cast<PGconn*>( dbLink_._conn ) ) );
		err = ( connStatus == CONNECTION_OK ) ? 0 : connStatus;
	}
	return ( err );
}

M_EXPORT_SYMBOL char const* dbrs_error( ODBLink const&, void* );
M_EXPORT_SYMBOL char const* dbrs_error( ODBLink const& dbLink_, void* result_ ) {
	M_ASSERT( dbLink_._conn );
	return ( result_ ? ::PQresultErrorMessage( static_cast<OPostgreSQLResult*>( result_ )->_result ) : ::PQerrorMessage( static_cast<PGconn*>( dbLink_._conn ) ) );
}

M_EXPORT_SYMBOL void* db_fetch_query_result( ODBLink&, char const* );
M_EXPORT_SYMBOL void* db_fetch_query_result( ODBLink& dbLink_, char const* query_ ) {
	M_ASSERT( dbLink_._conn && dbLink_._valid );
	OPostgreSQLResult* result( new ( memory::yaal ) OPostgreSQLResult( dbLink_ ) );
	result->_result = PQexec( static_cast<PGconn*>( dbLink_._conn ), query_ );
	result->_randomAccess = true;
	return ( result );
}

M_EXPORT_SYMBOL void rs_free_query_result( void* );
M_EXPORT_SYMBOL void rs_free_query_result( void* data_ ) {
	OPostgreSQLResult* pr( static_cast<OPostgreSQLResult*>( data_ ) );
	M_ASSERT( pr->_randomAccess );
	PQclear( pr->_result );
	M_SAFE( delete pr );
	return;
}

M_EXPORT_SYMBOL void* db_query( ODBLink&, char const* );
M_EXPORT_SYMBOL void* db_query( ODBLink& dbLink_, char const* query_ ) {
	M_ASSERT( dbLink_._conn && dbLink_._valid );
	OPostgreSQLResult* result( new ( memory::yaal ) OPostgreSQLResult( dbLink_ ) );
	PGconn* conn( static_cast<PGconn*>( dbLink_._conn ) );
	PGresult* r( ::PQprepare( conn, "", query_, 0, NULL ) );
	result->_result = ::PQdescribePrepared( conn, "" );
	result->_randomAccess = false;
	::PQsendQueryPrepared( conn, "", 0, NULL, NULL, NULL, 0 );
	::PQclear( r );
/*	::PQsetSingleRowMode( conn ); */
	return ( result );
}

M_EXPORT_SYMBOL void rs_free_cursor( void* );
M_EXPORT_SYMBOL void rs_free_cursor( void* data_ ) {
	OPostgreSQLResult* pr( static_cast<OPostgreSQLResult*>( data_ ) );
	M_ASSERT( ! pr->_randomAccess );
	::PQclear( pr->_result );
	M_SAFE( delete pr );
	return;
}

M_EXPORT_SYMBOL char const* rs_get( void*, int long, int );
M_EXPORT_SYMBOL char const* rs_get( void* data_, int long row_, int column_ ) {
	return ( ::PQgetvalue( static_cast<OPostgreSQLResult*>( data_ )->_result, static_cast<int>( row_ ), column_ ) );
}

M_EXPORT_SYMBOL bool rs_next( void* );
M_EXPORT_SYMBOL bool rs_next( void* data_ ) {
	OPostgreSQLResult* pr( static_cast<OPostgreSQLResult*>( data_ ) );
	M_ASSERT( ! pr->_randomAccess );
	bool gotMore( false );
	if ( ++ pr->_index < pr->_total ) {
		gotMore = true;
	} else {
		::PQclear( pr->_result );
		gotMore = ( pr->_result = ::PQgetResult( static_cast<PGconn*>( pr->_link._conn ) ) ) != NULL;
		if ( gotMore ) {
			pr->_index = 0;
			pr->_total = ::PQntuples( pr->_result );
			gotMore = pr->_index < pr->_total;
		} else
			pr->_total = 0;
	}
	return ( gotMore );
}

M_EXPORT_SYMBOL char const* rs_get_field( void*, int );
M_EXPORT_SYMBOL char const* rs_get_field( void* data_, int field_ ) {
	OPostgreSQLResult* pr( static_cast<OPostgreSQLResult*>( data_ ) );
	return ( ::PQgetvalue( pr->_result, pr->_index, field_ ) );
}

M_EXPORT_SYMBOL int rs_fields_count( void* );
M_EXPORT_SYMBOL int rs_fields_count( void* data_ ) {
	OPostgreSQLResult* pr( static_cast<OPostgreSQLResult*>( data_ ) );
	return ( ::PQnfields( pr->_result ) );
}

M_EXPORT_SYMBOL int long dbrs_records_count( ODBLink&, void* );
M_EXPORT_SYMBOL int long dbrs_records_count( ODBLink&, void* dataR_ ) {
	OPostgreSQLResult* pr( static_cast<OPostgreSQLResult*>( dataR_ ) );
	char* tmp = ::PQcmdTuples( pr->_result );
	if ( tmp && tmp [ 0 ] )
		return ( ::strtol( tmp, NULL, 10 ) );
	else
		return ( ::PQntuples( pr->_result ) );
}

M_EXPORT_SYMBOL int long dbrs_id( ODBLink&, void* );
M_EXPORT_SYMBOL int long dbrs_id( ODBLink& dbLink_, void* ) {
	M_ASSERT( dbLink_._conn && dbLink_._valid );
	PGresult* result( PQexec( static_cast<PGconn*>( dbLink_._conn ), "SELECT lastval();" ) );
	int long id( -1 );
	if ( result ) {
		char const* value( PQgetvalue( result, 0, 0 ) );
		if ( value )
			id = ::strtol( value, NULL, 10 );
		PQclear( result );
	}
	return ( id );
}

M_EXPORT_SYMBOL char const* rs_column_name( void*, int );
M_EXPORT_SYMBOL char const* rs_column_name( void* dataR_, int field_ ) {
	OPostgreSQLResult* pr( static_cast<OPostgreSQLResult*>( dataR_ ) );
	return ( ::PQfname( pr->_result, field_ ) );
}

int yaal_postgresql_driver_main( int, char** );
int yaal_postgresql_driver_main( int, char** ) {
	return ( 0 );
}

}

