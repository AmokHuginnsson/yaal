
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

#include "../config.h"

#include "../hcore/xalloc.h"
#include "../hcore/hexception.h"
#include "../hcore/hlog.h"
#include "../hcore/hstring.h"
#include "../hcore/rc_file.h"

#include "db_driver_loader.h"

const char g_pcDone [ ] = "done.\n";

char * g_pcDefaultSockPath = NULL;
const char * g_ppcDriver [ 24 ] =
	{
	NULL,
	"libmysql_driver.so",
	"libpostgresql_driver.so",
	NULL
	};

void * g_pvDlHandle = NULL;

namespace dbwrapper
	{

bool	n_bLogSQL = false;
int		n_iDataBaseDriver = 0;

OVariable n_psVariables [ ] =
	{
		{ D_TYPE_BOOL, "log_sql", & n_bLogSQL },
		{ D_TYPE_CHAR_POINTER, "mysql_socket", & g_pcDefaultSockPath },
		{ 0, NULL, NULL }
	};

typedef void * ( * t0 ) ( const char *, const char *, const char * );
void * ( * db_connect ) ( const char *, const char *, const char * );

typedef void ( * t1 ) ( void * );
void ( * db_disconnect ) ( void * );

typedef int ( * t2 ) ( void * );
int ( * db_errno ) ( void * );

typedef const char * ( * t3 ) ( void * );
const char * ( * db_error ) ( void * );

typedef void * ( * t4 ) ( void *, const char * );
void * ( * db_query ) ( void *, const char * );

typedef void ( * t5 ) ( void * );
void ( * db_unquery ) ( void * );

typedef char * ( * t6 ) ( void *, int, int );
char * ( * rs_get ) ( void *, int, int );

typedef int ( * t7 ) ( void * );
int ( * rs_fields_count ) ( void * );

typedef long int ( * t8 ) ( void *, void * = NULL );
long int ( * rsdb_records_count ) ( void *, void * = NULL );

typedef long int ( * t9 ) ( void *, void * );
long int ( * rsdb_id ) ( void *, void * );

typedef char * ( * tA ) ( void *, int );
char * ( * rs_column_name ) ( void *, int );

	}

/* Null-dummy driver */

void * dummy_db_connect ( const char *, const char *, const char * )
	{
	::log << "Error: Data base request (db_connect) while no driver loaded.";
	::log << endl;
	return ( NULL );
	}

void dummy_db_disconnect ( void * )
	{
	::log << "Error: Data base request (db_disconnect) while no driver loaded.";
	::log << endl;
	return;
	}

int dummy_db_errno ( void * )
	{
	::log << "Error: Data base request (db_errno) while no driver loaded.";
	::log << endl;
	return ( 0 );
	}

const char * dummy_db_error ( void * )
	{
	::log << "Error: Data base request (db_error) while no driver loaded.";
	::log << endl;
	return ( NULL );
	}

void * dummy_db_query ( void *, const char * )
	{
	::log << "Error: Data base request (db_query) while no driver loaded.";
	::log << endl;
	return ( NULL );
	}

void dummy_db_unquery ( void * )
	{
	::log << "Error: Data base request (db_unquery) while no driver loaded.";
	::log << endl;
	return;
	}

char * dummy_rs_get ( void *, int, int )
	{
	::log << "Error: Data base request (rs_get) while no driver loaded." << endl;
	return ( NULL );
	}

int dummy_rs_fields_count ( void * )
	{
	::log << "Error: Data base request (rs_fields_count) while no driver loaded.";
	::log << endl;
	return ( 0 );
	}

long int dummy_rsdb_records_count ( void *, void * = NULL )
	{
	::log << "Error: Data base request (rsdb_records_count)";
	::log << " while no driver loaded." << endl;
	return ( 0 );
	}

long int dummy_rsdb_id ( void *, void * )
	{
	::log << "Error: Data base request (rsdb_id) while no driver loaded." << endl;
	return ( 0 );
	}

char * dummy_rs_column_name ( void *, int )
	{
	::log << "Error: Data base request (rs_column_name) while no driver loaded.";
	::log << endl;
	return ( NULL );
	}

/* end of null-dummy driver */

using namespace dbwrapper;

void dbwrapper_error ( void )
	{
	HString l_oMessage;
	l_oMessage = dlerror ( );
	::log << l_oMessage << endl;
	fprintf ( stderr, "(%s), ", ( char * ) l_oMessage );
	return;
	}

void dbwrapper_exit ( void ) __attribute__  ((noreturn));
void dbwrapper_exit ( void )
	{
	fprintf ( stderr, "failed.\n" );
	exit ( 1 );
	}

void load_driver ( void )
	{
	int l_iCtr = 0;
	g_iErrNo = 0;
	fprintf ( stderr, "Loading dynamic database driver ... " );
	if ( dbwrapper::n_iDataBaseDriver )
		{
		l_iCtr = dbwrapper::n_iDataBaseDriver;
		g_pvDlHandle = dlopen ( g_ppcDriver [ l_iCtr ++ ], RTLD_NOW | RTLD_GLOBAL );
		if ( ! g_pvDlHandle )l_iCtr = 1;
		while ( ! g_pvDlHandle && g_ppcDriver [ l_iCtr ] )
			{
			dbwrapper_error ( );
			if ( ( l_iCtr == dbwrapper::n_iDataBaseDriver ) && l_iCtr ++ )continue;
			g_pvDlHandle = dlopen ( g_ppcDriver [ l_iCtr ++ ],
					RTLD_NOW | RTLD_GLOBAL );
			}
		if ( ! g_pvDlHandle )dbwrapper_exit ( );
		else
			{
			if ( ( ( l_iCtr - 1 ) == D_DB_DRIVER_MYSQL ) && ! g_pcDefaultSockPath )
				g_pcDefaultSockPath = xstrdup ( "/var/run/mysqld/mysqld.sock" );
			::log << "Loading [" << g_ppcDriver [ l_iCtr - 1 ] << "] driver." << endl;
			}
		fprintf ( stderr, g_pcDone );
		fprintf ( stderr, "Linking symbols ... " );
		if ( ! ( dbwrapper::db_disconnect = (dbwrapper::t1) dlsym ( g_pvDlHandle,
						"db_disconnect" ) ) )dbwrapper_error ( );
		else if ( ! ( dbwrapper::db_errno = (dbwrapper::t2) dlsym ( g_pvDlHandle,
						"db_errno" ) ) )dbwrapper_error ( );
		else if ( ! ( dbwrapper::db_error = (dbwrapper::t3) dlsym ( g_pvDlHandle,
						"db_error" ) ) )dbwrapper_error ( );
		else if ( ! ( dbwrapper::db_query = (dbwrapper::t4) dlsym ( g_pvDlHandle,
						"db_query" ) ) )dbwrapper_error ( );
		else if ( ! ( dbwrapper::db_unquery = (dbwrapper::t5) dlsym ( g_pvDlHandle,
						"db_unquery" ) ) )dbwrapper_error ( );
		else if ( ! ( dbwrapper::rs_get = (dbwrapper::t6) dlsym ( g_pvDlHandle,
						"rs_get" ) ) )dbwrapper_error ( );
		else if ( ! ( rs_fields_count = (dbwrapper::t7) dlsym ( g_pvDlHandle,
						"rs_fields_count" ) ) )dbwrapper_error ( );
		else if ( ! ( rsdb_records_count = (dbwrapper::t8) dlsym ( g_pvDlHandle,
						"rsdb_records_count" ) ) )dbwrapper_error ( );
		else if ( ! ( dbwrapper::rsdb_id = (dbwrapper::t9) dlsym ( g_pvDlHandle,
						"rsdb_id" ) ) )dbwrapper_error ( );
		else if ( ! ( rs_column_name = ( dbwrapper::tA ) dlsym ( g_pvDlHandle,
						"rs_column_name" ) ) )dbwrapper_error ( );
		else if ( ! ( dbwrapper::db_connect = (dbwrapper::t0) dlsym ( g_pvDlHandle,
						"db_connect" ) ) )dbwrapper_error ( );
		if ( dbwrapper::db_connect )fprintf ( stderr, g_pcDone );
		else dbwrapper_exit ( );
		}
	else
		{
		dbwrapper::db_connect = dummy_db_connect;
		dbwrapper::db_disconnect = dummy_db_disconnect;
		dbwrapper::db_errno = dummy_db_errno;
		dbwrapper::db_error = dummy_db_error;
		dbwrapper::db_query = dummy_db_query;
		dbwrapper::db_unquery = dummy_db_unquery;
		dbwrapper::rs_get = dummy_rs_get;
		dbwrapper::rs_fields_count = dummy_rs_fields_count;
		dbwrapper::rsdb_records_count = dummy_rsdb_records_count;
		dbwrapper::rsdb_id = dummy_rsdb_id;
		dbwrapper::rs_column_name = dummy_rs_column_name;
		}
	return;
	}

/* Driver autoloaders ... */

void * null_db_connect ( const char * a_pcDataBase,
		const char * a_pcLogin, const char * a_pcPassword )
	{
	load_driver ( );
	return ( db_connect ( a_pcDataBase, a_pcLogin, a_pcPassword ) );
	}

void null_db_disconnect ( void * a_pvDataBase )
	{
	load_driver ( );
	db_disconnect ( a_pvDataBase );
	return;
	}

int null_db_errno ( void * a_pvDataBase )
	{
	load_driver ( );
	return ( db_errno ( a_pvDataBase ) );
	}

const char * null_db_error ( void * a_pvDataBase )
	{
	load_driver ( );
	return ( db_error ( a_pvDataBase ) );
	}

void * null_db_query ( void * a_pvDataBase, const char * a_pcQuery )
	{
	load_driver ( );
	return ( db_query ( a_pvDataBase, a_pcQuery ) );
	}

void null_db_unquery ( void * a_pvDataBase )
	{
	load_driver ( );
	db_unquery ( a_pvDataBase );
	return;
	}

char * null_rs_get ( void * a_pvResult, int a_iRow, int a_iCol )
	{
	load_driver ( );
	return ( rs_get ( a_pvResult, a_iRow, a_iCol ) );
	}

int null_rs_fields_count ( void * a_pvResult )
	{
	load_driver ( );
	return ( rs_fields_count ( a_pvResult ) );
	}

long int null_rsdb_records_count ( void * a_pvDataBase, void * a_pvResult )
	{
	load_driver ( );
	return ( rsdb_records_count ( a_pvDataBase, a_pvResult ) );
	}

long int null_rsdb_id ( void * a_pvDataBase, void * a_pvResult )
	{
	load_driver ( );
	return ( rsdb_id ( a_pvDataBase, a_pvResult ) );
	}

char * null_rs_column_name ( void * a_pvResult, int a_iColumn )
	{
	load_driver ( );
	return ( rs_column_name ( a_pvResult, a_iColumn ) );
	}

/* end of driver autoloader section */

bool set_dbwrapper_variables ( HString & a_roOption, HString & a_roValue )
	{
	if ( ! strcasecmp ( a_roOption, "data_base_driver" ) )
		{
		if ( ! ( strcasecmp ( a_roValue, "none" )
					&& strcasecmp ( a_roValue, "null" ) ) )
			dbwrapper::n_iDataBaseDriver = D_DB_DRIVER_NONE;
		else if ( ! strcmp ( a_roValue, "MySQL" ) )
			dbwrapper::n_iDataBaseDriver = D_DB_DRIVER_MYSQL;
		else if ( ! strcmp ( a_roValue, "PostgreSQL" ) )
			dbwrapper::n_iDataBaseDriver = D_DB_DRIVER_POSTGRESQL;
		else
			{
			::log << "Error: `" << a_roValue << "' is unknown driver." << endl;
			exit ( 1 );
			}
		}
	else return ( true );
	return ( false );
	}

void dbwrapper_init ( void ); __attribute__ ( ( constructor ) )
void dbwrapper_init ( void )
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
	rc_file::process_rc_file ( "stdhapi", "dbwrapper",
			dbwrapper::n_psVariables, set_dbwrapper_variables );
	return;
	}

void dbwrapper_fini ( void ); __attribute__ ( ( destructor ) )
void dbwrapper_fini ( void )
	{
	if ( g_pcDefaultSockPath )
		xfree ( ( void * ) g_pcDefaultSockPath );
	if ( g_pvDlHandle )
		{
		fprintf ( stderr, "Unloading dynamic database driver ... " );
#ifndef __HOST_OS_TYPE_FREEBSD__
		if ( dlclose ( g_pvDlHandle ) )
			{
			dbwrapper_error ( );
			dbwrapper_exit ( );
			}
		else
#endif /* not __HOST_OS_TYPE_FREEBSD__ */
			fprintf ( stderr, g_pcDone );
		}
	g_pvDlHandle = NULL;
	return;
	}

/* older versions of g++ fail to handle __attribute__((constructor))
   if no static object exists */

#if __GNUC__ < 3 || \
	 ( __GNUC__ == 3 && __GNUC_MINOR__ < 3 )

HString g_oDummyDBWRAPPER;

#endif

