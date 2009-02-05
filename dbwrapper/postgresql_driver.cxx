
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

#include "hcore/xalloc.hxx"

extern "C"
{

PGconn * g_psBrokenDB = NULL;

void db_disconnect ( void * );

void * db_connect ( char const * a_pcDataBase,
		char const * a_pcLogin, char const * a_pcPassword )
	{
	PGconn * l_psConnection = NULL;
	if ( g_psBrokenDB )
		{
		db_disconnect ( g_psBrokenDB );
		g_psBrokenDB = NULL;
		}
	l_psConnection = PQsetdbLogin ( NULL,	/* host */
																	NULL,					/* port */
																	NULL,					/* options */
																	NULL,					/* debugging tty */
																	a_pcDataBase, 
																	a_pcLogin, 
																	a_pcPassword );
	if ( PQstatus ( l_psConnection ) != CONNECTION_OK )
		g_psBrokenDB = l_psConnection, l_psConnection = NULL;
	return ( l_psConnection );
	}

void db_disconnect ( void * a_pvData )
	{
	if ( a_pvData )
		PQfinish ( ( PGconn * ) a_pvData );
	return;
	}

int db_errno( void* )
	{
	return ( errno );
	}

char const* db_error( void* a_pvData )
	{
	if ( ! a_pvData )
		a_pvData = g_psBrokenDB;
	return ( ::PQerrorMessage( static_cast<PGconn*>( a_pvData ) ) );
	}

void * db_query ( void * a_pvData, char const * a_pcQuery )
	{
	return ( PQexec ( ( PGconn * ) a_pvData, a_pcQuery ) );
	}

void db_unquery ( void * a_pvData )
	{
	PQclear ( ( PGresult * ) a_pvData );
	return;
	}

char* rs_get( void* a_pvData, int long a_iRow, int a_iColumn )
	{
	return ( ::PQgetvalue( static_cast<PGresult*>( a_pvData ), static_cast<int>( a_iRow ), a_iColumn ) );
	}

int rs_fields_count( void* a_pvData )
	{
	return ( ::PQnfields( static_cast<PGresult*>( a_pvData ) ) );
	}

int long dbrs_records_count( void*, void* a_pvDataR )
	{
	char* l_pcTmp = ::PQcmdTuples( static_cast<PGresult*>( a_pvDataR ) );
	if ( l_pcTmp && l_pcTmp [ 0 ] )
		return ( ::strtol( l_pcTmp, NULL, 10 ) );
	else
		return ( ::PQntuples( static_cast<PGresult*>( a_pvDataR ) ) );
	}

int long dbrs_id( void*, void* a_pvDataR )
	{
	return ( ::PQoidValue( static_cast<PGresult*>( a_pvDataR ) ) );
	}

char* rs_column_name( void* a_pvDataR, int a_iField )
	{
	return ( ::PQfname( static_cast<PGresult*>( a_pvDataR ), a_iField ) );
	}

int yaal_postgresql_driver_main( int, char** ) __attribute__(( __noreturn__ ));
int yaal_postgresql_driver_main( int, char** )
	{
	::exit( 0 );
	}

}
