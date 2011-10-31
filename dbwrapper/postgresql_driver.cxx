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

extern "C" {

namespace {

PGconn* _brokenDB_ = NULL;

}

M_EXPORT_SYMBOL void* db_connect( char const* dataBase_,
		char const* login_, char const* password_ ) {
	PGconn * connection = NULL;
	if ( _brokenDB_ ) {
		PQfinish( _brokenDB_ );
		_brokenDB_ = NULL;
	}
	connection = PQsetdbLogin( NULL /* host */,
			NULL /* port */, NULL /* options */,
			NULL /* debugging tty */,
			dataBase_, login_, password_ );
	if ( PQstatus( connection ) != CONNECTION_OK )
		_brokenDB_ = connection, connection = NULL;
	return ( connection );
}

M_EXPORT_SYMBOL void db_disconnect( void* data_ ) {
	if ( data_ )
		PQfinish( static_cast<PGconn*>( data_ ) );
	return;
}

M_EXPORT_SYMBOL int dbrs_errno( void*, void* result_ ) {
	ExecStatusType status = PQresultStatus( static_cast<PGresult*>( result_ ) );
	return ( ! ( ( status == PGRES_COMMAND_OK ) || ( status == PGRES_TUPLES_OK ) ) );
}

M_EXPORT_SYMBOL char const* dbrs_error( void* db_, void* result_ ) {
	if ( ! db_ )
		db_ = _brokenDB_;
	return ( result_ ? ::PQresultErrorMessage( static_cast<PGresult*>( result_ ) ) : ::PQerrorMessage( static_cast<PGconn*>( db_ ) ) );
}

M_EXPORT_SYMBOL void* db_query( void* data_, char const* query_ ) {
	return ( PQexec( static_cast<PGconn*>( data_ ), query_ ) );
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

M_EXPORT_SYMBOL int long dbrs_records_count( void*, void* dataR_ ) {
	char* tmp = ::PQcmdTuples( static_cast<PGresult*>( dataR_ ) );
	if ( tmp && tmp [ 0 ] )
		return ( ::strtol( tmp, NULL, 10 ) );
	else
		return ( ::PQntuples( static_cast<PGresult*>( dataR_ ) ) );
}

M_EXPORT_SYMBOL int long dbrs_id( void* db_, void* ) {
	PGresult* result( PQexec( static_cast<PGconn*>( db_ ), "SELECT lastval();" ) );
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
