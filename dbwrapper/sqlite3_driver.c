
/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	sqlite3_driver.c - this file is integral part of `stdhapi' project.

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

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sqlite3.h>

#include "../hcore/xalloc.h"

#ifndef NULL
#define NULL	0
#endif /* not NULL */

extern "C"
{

typedef struct
	{
	int f_iErrorCode;
	char * f_pcErrorMessage;
	sqlite3 * f_psDB;
	
	} sqlite_db;
	
typedef struct
	{
	int f_iRows;
	int f_iColumns;
	char * * f_ppcData;
	} sqlite3_result;

sqlite_db * g_psBrokenDB = NULL;

void * db_query ( void *, const char * );
void db_unquery ( void * );

void * db_connect ( const char * a_pcDataBase,
		const char *, const char * )
	{
	void * l_pvPtr = NULL;
	sqlite_db * l_psSQLite = ( sqlite_db * ) xcalloc ( sizeof ( sqlite_db ) );
	l_psSQLite->f_iErrorCode = sqlite3_open ( a_pcDataBase,
			& l_psSQLite->f_psDB );
	if ( l_psSQLite->f_iErrorCode )
		{
		l_psSQLite->f_pcErrorMessage = xstrdup ( sqlite3_errmsg ( l_psSQLite->f_psDB ) );
		g_psBrokenDB = l_psSQLite;
		l_psSQLite = NULL;
		}
	else
		{
		l_pvPtr = db_query ( l_psSQLite, "PRAGMA empty_result_callbacks = ON;" );
		if ( l_pvPtr )
			db_unquery ( l_pvPtr );
		else g_psBrokenDB = l_psSQLite, l_psSQLite = NULL;
		}
	return ( l_psSQLite );
	}

void db_disconnect ( void * a_pvData )
	{
	sqlite_db * l_psSQLite = ( sqlite_db * ) a_pvData;
	sqlite3_close ( l_psSQLite->f_psDB );
	if ( l_psSQLite->f_pcErrorMessage )
		xfree ( l_psSQLite->f_pcErrorMessage );
	xfree ( l_psSQLite );
	return;
	}

int db_errno ( void * a_pvData )
	{
	sqlite_db * l_psSQLite = ( sqlite_db * ) a_pvData;
	if ( ! l_psSQLite )
		l_psSQLite = g_psBrokenDB;
	if ( l_psSQLite )
		{
		if ( l_psSQLite->f_iErrorCode )
			return ( l_psSQLite->f_iErrorCode );
		return ( sqlite3_errcode ( l_psSQLite->f_psDB ) );
		}
	return ( errno );
	}

const char * db_error  ( void * a_pvData )
	{
	sqlite_db * l_psSQLite = ( sqlite_db * ) a_pvData;
	if ( ! l_psSQLite )
		l_psSQLite = g_psBrokenDB;
	if ( l_psSQLite )
		{
		if ( l_psSQLite->f_pcErrorMessage )
			return ( l_psSQLite->f_pcErrorMessage );
		return ( sqlite3_errmsg ( l_psSQLite->f_psDB ) );
		}
	return ( "" );
	}

void * db_query ( void * a_pvData, const char * a_pcQuery )
	{
	sqlite_db * l_psSQLite = ( sqlite_db * ) a_pvData;
	sqlite3_result * l_psResult = NULL;
	l_psResult = ( sqlite3_result * ) xcalloc ( sizeof ( sqlite3_result ) );
	l_psResult->f_iColumns = 0;
	l_psResult->f_iRows = 0;
	l_psResult->f_ppcData = NULL;
	if ( l_psSQLite->f_pcErrorMessage )
		xfree ( l_psSQLite->f_pcErrorMessage );
	l_psSQLite->f_iErrorCode = sqlite3_get_table ( l_psSQLite->f_psDB,
			a_pcQuery, & l_psResult->f_ppcData, & l_psResult->f_iRows,
			& l_psResult->f_iColumns, & l_psSQLite->f_pcErrorMessage );
	return ( l_psResult );
	}

void db_unquery ( void * a_pvData )
	{
	sqlite3_free_table ( ( ( sqlite3_result * ) a_pvData )->f_ppcData );
	xfree ( a_pvData );
	return;
	}

char * rs_get ( void * a_pvData, int a_iRow,  int a_iColumn )
	{
	char * * l_ppcData = NULL;
	sqlite3_result * l_psResult = ( sqlite3_result * )a_pvData;
	l_ppcData = l_psResult->f_ppcData;
	return ( l_ppcData [ ( a_iRow + 1 ) * l_psResult->f_iColumns + a_iColumn ] );
	}

int rs_fields_count ( void * a_pvData )
	{
	return ( ( ( sqlite3_result * ) a_pvData )->f_iColumns );
	}

long int rsdb_records_count ( void * a_pvDataB, void * a_pvDataR )
	{
	if ( a_pvDataR )
		return ( ( ( sqlite3_result * ) a_pvDataR )->f_iRows );
	else
		return ( sqlite3_changes ( ( sqlite3 * ) a_pvDataB ) );
	}

long int rsdb_id ( void * a_pvDataB, void * )
	{
	return ( sqlite3_last_insert_rowid ( ( sqlite3 * ) a_pvDataB ) );
	}

char * rs_column_name ( void * a_pvDataR, int a_iField )
	{
	return ( ( ( sqlite3_result * ) a_pvDataR )->f_ppcData [ a_iField ] );
	}

}

