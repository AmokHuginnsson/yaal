
/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	db_driver_loader.c - this file is integral part of `stdhapi' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libintl.h>

#include "../config.h"

#include "../hcore/hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "../hcore/xalloc.h"
#include "../hcore/hstring.h"
#include "../hcore/hlog.h"
#include "db_driver_loader.h"
#include "dbwrapper.h"

#define M_DB_ERR(msg) "Error: Data base request ("msg") while no driver loaded."

char const x_tag_g_pcDone [ ] = "done.\r\n", * g_pcDone = x_tag_g_pcDone;

static char const * g_ppcDriver [ 24 ] =
	{
	NULL,
	"libmysql_driver.so",
	"libpostgresql_driver.so",
	"libsqlite_driver.so",
	"libsqlite3_driver.so",
	NULL
	};

namespace dbwrapper
	{
	
void * n_pvDlHandle = NULL;

typedef void * ( * t0 ) ( char const *, char const *, char const * );
void * ( * db_connect ) ( char const *, char const *, char const * );

typedef void ( * t1 ) ( void * );
void ( * db_disconnect ) ( void * );

typedef int ( * t2 ) ( void * );
int ( * db_errno ) ( void * );

typedef char const * ( * t3 ) ( void * );
char const * ( * db_error ) ( void * );

typedef void * ( * t4 ) ( void *, char const * );
void * ( * db_query ) ( void *, char const * );

typedef void ( * t5 ) ( void * );
void ( * db_unquery ) ( void * );

typedef char * ( * t6 ) ( void *, int, int );
char * ( * rs_get ) ( void *, int, int );

typedef int ( * t7 ) ( void * );
int ( * rs_fields_count ) ( void * );

typedef long int ( * t8 ) ( void *, void * );
long int ( * rsdb_records_count ) ( void *, void * );

typedef long int ( * t9 ) ( void *, void * );
long int ( * rsdb_id ) ( void *, void * );

typedef char * ( * tA ) ( void *, int );
char * ( * rs_column_name ) ( void *, int );

	}

using namespace dbwrapper;

namespace dbwrapper
{
	
/* Null driver */

void * null_db_connect ( char const *, char const *, char const * )
	{
	log ( D_LOG_ERROR ) << M_DB_ERR ( "(db_connect)" ) << endl;
	return ( NULL );
	}

void null_db_disconnect ( void * )
	{
	log ( D_LOG_ERROR ) << M_DB_ERR ( "(db_disconnect)" ) << endl;
	return;
	}

int null_db_errno ( void * )
	{
	log ( D_LOG_ERROR ) << M_DB_ERR ( "(db_errno)" ) << endl;
	return ( 0 );
	}

char const * null_db_error ( void * )
	{
	log ( D_LOG_ERROR ) << M_DB_ERR ( "db_error)" ) << endl;
	return ( _ ( "null database driver loaded" ) );
	}

void * null_db_query ( void *, char const * )
	{
	log ( D_LOG_ERROR ) << M_DB_ERR ( "(db_query)" ) << endl;
	return ( NULL );
	}

void null_db_unquery ( void * )
	{
	log ( D_LOG_ERROR ) << M_DB_ERR ( "(db_unquery)" ) << endl;
	return;
	}

char * null_rs_get ( void *, int, int )
	{
	log ( D_LOG_ERROR ) << M_DB_ERR ( "(rs_get)" ) << endl;
	return ( NULL );
	}

int null_rs_fields_count ( void * )
	{
	log ( D_LOG_ERROR ) << M_DB_ERR ( "(rs_fields_count)" ) << endl;
	return ( 0 );
	}

long int null_rsdb_records_count ( void *, void * = NULL )
	{
	log ( D_LOG_ERROR ) << M_DB_ERR ( "(rsdb_records_count)" ) << endl;
	return ( 0 );
	}

long int null_rsdb_id ( void *, void * )
	{
	log ( D_LOG_ERROR ) << M_DB_ERR ( "(rsdb_id)" ) << endl;
	return ( 0 );
	}

char * null_rs_column_name ( void *, int )
	{
	log ( D_LOG_ERROR ) << M_DB_ERR ( "(rs_column_name)" ) << endl;
	return ( NULL );
	}

/* end of null-dummy driver */

void dbwrapper_error ( void )
	{
	HString l_oMessage;
	l_oMessage = dlerror ( );
	log ( D_LOG_ERROR ) << l_oMessage << endl;
	fprintf ( stderr, "(%s), ", ( char * ) l_oMessage );
	return;
	}

void dbwrapper_exit ( void ) __attribute__  ((noreturn));
void dbwrapper_exit ( void )
	{
	fprintf ( stderr, "failed.\r\n" );
	exit ( 1 );
	}

void load_driver ( void )
	{
	M_PROLOG
	int l_iCtr = 0;
	g_iErrNo = 0;
	fprintf ( stderr, "Loading dynamic database driver ... " );
	if ( dbwrapper::n_iDataBaseDriver )
		{
		l_iCtr = dbwrapper::n_iDataBaseDriver;
		n_pvDlHandle = dlopen ( g_ppcDriver [ l_iCtr ++ ], RTLD_NOW | RTLD_GLOBAL );
		if ( ! n_pvDlHandle )l_iCtr = 1;
		while ( ! n_pvDlHandle && g_ppcDriver [ l_iCtr ] )
			{
			if ( ( l_iCtr == dbwrapper::n_iDataBaseDriver ) && l_iCtr ++ )continue;
			dbwrapper_error ( );
			n_pvDlHandle = dlopen ( g_ppcDriver [ l_iCtr ++ ],
					RTLD_NOW | RTLD_GLOBAL );
			}
		if ( ! n_pvDlHandle )dbwrapper_exit ( );
		else
			{
			log ( D_LOG_NOTICE ) << "Loading [" << g_ppcDriver [ l_iCtr - 1 ];
			log << "] driver." << endl;
			}
		fprintf ( stderr, x_tag_g_pcDone );
		fprintf ( stderr, "Linking symbols ... " );
		if ( ! ( dbwrapper::db_disconnect = (dbwrapper::t1) dlsym ( n_pvDlHandle,
						"db_disconnect" ) ) )dbwrapper_error ( );
		else if ( ! ( dbwrapper::db_errno = (dbwrapper::t2) dlsym ( n_pvDlHandle,
						"db_errno" ) ) )dbwrapper_error ( );
		else if ( ! ( dbwrapper::db_error = (dbwrapper::t3) dlsym ( n_pvDlHandle,
						"db_error" ) ) )dbwrapper_error ( );
		else if ( ! ( dbwrapper::db_query = (dbwrapper::t4) dlsym ( n_pvDlHandle,
						"db_query" ) ) )dbwrapper_error ( );
		else if ( ! ( dbwrapper::db_unquery = (dbwrapper::t5) dlsym ( n_pvDlHandle,
						"db_unquery" ) ) )dbwrapper_error ( );
		else if ( ! ( dbwrapper::rs_get = (dbwrapper::t6) dlsym ( n_pvDlHandle,
						"rs_get" ) ) )dbwrapper_error ( );
		else if ( ! ( rs_fields_count = (dbwrapper::t7) dlsym ( n_pvDlHandle,
						"rs_fields_count" ) ) )dbwrapper_error ( );
		else if ( ! ( rsdb_records_count = (dbwrapper::t8) dlsym ( n_pvDlHandle,
						"rsdb_records_count" ) ) )dbwrapper_error ( );
		else if ( ! ( dbwrapper::rsdb_id = (dbwrapper::t9) dlsym ( n_pvDlHandle,
						"rsdb_id" ) ) )dbwrapper_error ( );
		else if ( ! ( rs_column_name = ( dbwrapper::tA ) dlsym ( n_pvDlHandle,
						"rs_column_name" ) ) )dbwrapper_error ( );
		else if ( ! ( dbwrapper::db_connect = (dbwrapper::t0) dlsym ( n_pvDlHandle,
						"db_connect" ) ) )dbwrapper_error ( );
		if ( dbwrapper::db_connect != dbwrapper::autoloader_db_connect )
			fprintf ( stderr, x_tag_g_pcDone );
		else
			M_THROW ( _ ( "can not load database driver" ), n_iDataBaseDriver );
		}
	else
		{
		dbwrapper::db_connect = null_db_connect;
		dbwrapper::db_disconnect = null_db_disconnect;
		dbwrapper::db_errno = null_db_errno;
		dbwrapper::db_error = null_db_error;
		dbwrapper::db_query = null_db_query;
		dbwrapper::db_unquery = null_db_unquery;
		dbwrapper::rs_get = null_rs_get;
		dbwrapper::rs_fields_count = null_rs_fields_count;
		dbwrapper::rsdb_records_count = null_rsdb_records_count;
		dbwrapper::rsdb_id = null_rsdb_id;
		dbwrapper::rs_column_name = null_rs_column_name;
		}
	return;
	M_EPILOG
	}

/* Driver autoloaders ... */

void * autoloader_db_connect ( char const * a_pcDataBase,
		char const * a_pcLogin, char const * a_pcPassword )
	{
	load_driver ( );
	return ( db_connect ( a_pcDataBase, a_pcLogin, a_pcPassword ) );
	}

void autoloader_db_disconnect ( void * a_pvDataBase )
	{
	load_driver ( );
	db_disconnect ( a_pvDataBase );
	return;
	}

int autoloader_db_errno ( void * a_pvDataBase )
	{
	load_driver ( );
	return ( db_errno ( a_pvDataBase ) );
	}

char const * autoloader_db_error ( void * a_pvDataBase )
	{
	load_driver ( );
	return ( db_error ( a_pvDataBase ) );
	}

void * autoloader_db_query ( void * a_pvDataBase, char const * a_pcQuery )
	{
	load_driver ( );
	return ( db_query ( a_pvDataBase, a_pcQuery ) );
	}

void autoloader_db_unquery ( void * a_pvDataBase )
	{
	load_driver ( );
	db_unquery ( a_pvDataBase );
	return;
	}

char * autoloader_rs_get ( void * a_pvResult, int a_iRow, int a_iCol )
	{
	load_driver ( );
	return ( rs_get ( a_pvResult, a_iRow, a_iCol ) );
	}

int autoloader_rs_fields_count ( void * a_pvResult )
	{
	load_driver ( );
	return ( rs_fields_count ( a_pvResult ) );
	}

long int autoloader_rsdb_records_count ( void * a_pvDataBase,
		void * a_pvResult )
	{
	load_driver ( );
	return ( rsdb_records_count ( a_pvDataBase, a_pvResult ) );
	}

long int autoloader_rsdb_id ( void * a_pvDataBase, void * a_pvResult )
	{
	load_driver ( );
	return ( rsdb_id ( a_pvDataBase, a_pvResult ) );
	}

char * autoloader_rs_column_name ( void * a_pvResult, int a_iColumn )
	{
	load_driver ( );
	return ( rs_column_name ( a_pvResult, a_iColumn ) );
	}

/* end of driver autoloader section */

}

