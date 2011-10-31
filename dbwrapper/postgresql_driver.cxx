/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	mysql_driver.cxx - this file is integral part of `yaal' project.

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

M_EXPORT_SYMBOL bool db_connect( ODBLink& dbLink_, char const* dataBase_,
		char const* login_, char const* password_ ) {
	PGconn* connection( NULL );
	dbLink_._conn = connection = PQsetdbLogin( NULL /* host */,
			NULL /* port */, NULL /* options */,
			NULL /* debugging tty */,
			dataBase_, login_, password_ );
	if ( PQstatus( connection ) == CONNECTION_OK )
		dbLink_._valid = true;
	return ( ! dbLink_._valid );
}

M_EXPORT_SYMBOL void db_disconnect( ODBLink& dbLink_ ) {
	if ( dbLink_._conn ) {
		PQfinish( static_cast<PGconn*>( dbLink_._conn ) );
		dbLink_.clear();
	}
	return;
}

M_EXPORT_SYMBOL int dbrs_errno( ODBLink const& dbLink_, void* result_ ) {
	M_ASSERT( dbLink_._conn );
	int err( 0 );
	if ( result_ ) {
		ExecStatusType status = PQresultStatus( static_cast<PGresult*>( result_ ) );
		err = ( ( status == PGRES_COMMAND_OK ) || ( status == PGRES_TUPLES_OK ) ) ? 0 : status;
	} else {
		ConnStatusType connStatus( PQstatus( static_cast<PGconn*>( dbLink_._conn ) ) );
		err = ( connStatus == CONNECTION_OK ) ? 0 : connStatus;
	}
	return ( err );
}

M_EXPORT_SYMBOL char const* dbrs_error( ODBLink const& dbLink_, void* result_ ) {
	M_ASSERT( dbLink_._conn );
	return ( result_ ? ::PQresultErrorMessage( static_cast<PGresult*>( result_ ) ) : ::PQerrorMessage( static_cast<PGconn*>( dbLink_._conn ) ) );
}

M_EXPORT_SYMBOL void* db_query( ODBLink& dbLink_, char const* query_ ) {
	M_ASSERT( dbLink_._conn && dbLink_._valid );
	return ( PQexec( static_cast<PGconn*>( dbLink_._conn ), query_ ) );
}

M_EXPORT_SYMBOL void rs_unquery( void* data_ ) {
	PQclear( static_cast<PGresult*>( data_ ) );
	return;
}

M_EXPORT_SYMBOL char const* rs_get( void* data_, int long row_, int column_ ) {
	return ( ::PQgetvalue( static_cast<PGresult*>( data_ ), static_cast<int>( row_ ), column_ ) );
}

M_EXPORT_SYMBOL int rs_fields_count( void* data_ ) {
	return ( ::PQnfields( static_cast<PGresult*>( data_ ) ) );
}

M_EXPORT_SYMBOL int long dbrs_records_count( ODBLink&, void* dataR_ ) {
	char* tmp = ::PQcmdTuples( static_cast<PGresult*>( dataR_ ) );
	if ( tmp && tmp [ 0 ] )
		return ( ::strtol( tmp, NULL, 10 ) );
	else
		return ( ::PQntuples( static_cast<PGresult*>( dataR_ ) ) );
}

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

M_EXPORT_SYMBOL char const* rs_column_name( void* dataR_, int field_ ) {
	return ( ::PQfname( static_cast<PGresult*>( dataR_ ), field_ ) );
}

int yaal_postgresql_driver_main( int, char** ) __attribute__(( __noreturn__ ));
int yaal_postgresql_driver_main( int, char** ) {
	::exit( 0 );
}

}
