
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

#include "../hcore/hexception.h"
#include "../hcore/hlog.h"
#include "../hcore/hstring.h"
#include "../hcore/rc_file.h"

#include "db_driver_loader.h"

#ifndef NULL
#define NULL	0
#endif /* not NULL */

const char g_pcDone [ ] = "done.\n";

const char * g_ppcDriver [ 24 ] =
	{
	"libmysql_driver.so",
	"libpostgresql_driver.so",
	NULL
	};

void * g_pvDlHandle = NULL;


namespace dbwrapper
	{

bool	n_bLogSQL = false;
int		n_iDataBaseDriver = 0;

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

typedef long int ( * t7 ) ( void *, void * = NULL );
long int ( * rsdb_count ) ( void *, void * = NULL );

typedef long int ( * t8 ) ( void *, void * );
long int ( * rsdb_id ) ( void *, void * );

typedef char * ( * t9 ) ( void *, int );
char * ( * rs_column_name ) ( void *, int );

	}

extern "C"
{

extern void _init ( void );
extern void _fini ( void );

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

void set_dbwrapper_variables ( HString & a_roOption, HString & a_roValue )
	{
	if ( ! strcasecmp ( a_roOption, "log_sql" ) )
		rc_file::rc_set_variable ( a_roValue, dbwrapper::n_bLogSQL );
	else if ( ! strcasecmp ( a_roOption, "data_base_driver" ) )
		{
		if ( ! strcmp ( a_roValue, "MySQL" ) )
			dbwrapper::n_iDataBaseDriver = D_MYSQL;
		else if ( ! strcmp ( a_roValue, "PostgreSQL" ) )
			dbwrapper::n_iDataBaseDriver = D_POSTGRESQL;
		
		}
	return;
	}

void dbwrapper_init ( void )
	{
	int l_iCtr = 0;
/* I do not know why but when _init ( ) is called explictly, like here would,
 * process hangs just before dbwrapper_fini ( ) */
/*	_init ( ); */
	g_iErrNo = 0;
	dbwrapper::db_connect = NULL;
	dbwrapper::db_disconnect = NULL;
	dbwrapper::db_errno = NULL;
	dbwrapper::db_error = NULL;
	dbwrapper::db_query = NULL;
	dbwrapper::db_unquery = NULL;
	dbwrapper::rs_get = NULL;
	dbwrapper::rsdb_count = NULL;
	dbwrapper::rsdb_id = NULL;
	dbwrapper::rs_column_name = NULL;
	fprintf ( stderr, "Loading dynamic database driver ... " );
	rc_file::process_rc_file ( "stdhapi", set_dbwrapper_variables );
	if ( dbwrapper::n_iDataBaseDriver )
		{
		l_iCtr = dbwrapper::n_iDataBaseDriver - 1;
		g_pvDlHandle = dlopen ( g_ppcDriver [ l_iCtr ], RTLD_NOW | RTLD_GLOBAL );
		if ( ! g_pvDlHandle )
			{
			l_iCtr = 0;
			dbwrapper_error ( );
			}
		else l_iCtr ++;
		}
	while ( ! g_pvDlHandle && g_ppcDriver [ l_iCtr ] )
		{
		g_pvDlHandle = dlopen ( g_ppcDriver [ l_iCtr ++ ], RTLD_NOW | RTLD_GLOBAL );
		if ( ! g_pvDlHandle )dbwrapper_error ( );
		}
	if ( ! g_pvDlHandle )dbwrapper_exit ( );
	else
		::log << "Loading [" << g_ppcDriver [ l_iCtr - 1 ] << "] driver." << endl;
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
	else if ( ! ( dbwrapper::rsdb_count = (dbwrapper::t7) dlsym ( g_pvDlHandle,
					"rsdb_count" ) ) )dbwrapper_error ( );
	else if ( ! ( dbwrapper::rsdb_id = (dbwrapper::t8) dlsym ( g_pvDlHandle,
					"rsdb_id" ) ) )dbwrapper_error ( );
	else if ( ! ( dbwrapper::rs_column_name = (dbwrapper::t9)dlsym ( g_pvDlHandle,
					"rs_column_name" ) ) )dbwrapper_error ( );
	else if ( ! ( dbwrapper::db_connect = (dbwrapper::t0) dlsym ( g_pvDlHandle,
					"db_connect" ) ) )dbwrapper_error ( );
	if ( dbwrapper::db_connect )fprintf ( stderr, g_pcDone );
	else dbwrapper_exit ( );
	return;
	}

void dbwrapper_fini ( void )
	{
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
	else fprintf ( stderr, "Bailing out.\n" );
	g_pvDlHandle = NULL;
	_fini ( );
	return;
	}

}

