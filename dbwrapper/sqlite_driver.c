
/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	sqlite_driver.c - this file is integral part of `stdhapi' project.

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

#include <xalloc.h>

#include <sqlite.h>

#ifndef NULL
#define NULL	0
#endif /* not NULL */

extern "C"
{

typedef struct
	{
	int f_iErrorCode;
	char * f_pcErrorMessage;
	sqlite * f_psDB;
	
	} sqlite_db;
	
typedef struct
	{
	int f_iRows;
	int f_iColumns;
	char * * f_ppcData;
	} sqlite_result;

sqlite_db * g_psBrokenDB = NULL;

void * db_connect ( const char * a_pcDataBase,
		const char *, const char * )
	{
	sqlite_db * l_psSQLite = ( sqlite_db * ) xcalloc ( sizeof ( sqlite_db ) );
	l_psSQLite->f_psDB = sqlite_open ( a_pcDataBase, 0,
			& l_psSQLite->f_pcErrorMessage );
	if ( ! l_psSQLite->f_psDB )
		g_psBrokenDB = l_psSQLite, l_psSQLite = NULL;
	return ( l_psSQLite );
	}

void db_disconnect ( void * a_pvData )
	{
	sqlite_db * l_psSQLite = ( sqlite_db * ) a_pvData;
	sqlite_close ( l_psSQLite->f_psDB );
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
		if ( l_psSQLite->f_iErrorCode )
			return ( l_psSQLite->f_iErrorCode );
	return ( errno );
	}

const char * db_error  ( void * a_pvData )
	{
	sqlite_db * l_psSQLite = ( sqlite_db * ) a_pvData;
	if ( ! l_psSQLite )
		l_psSQLite = g_psBrokenDB;
	if ( l_psSQLite )
		return ( l_psSQLite->f_pcErrorMessage );
	return ( "" );
	}

void * db_query ( void * a_pvData, const char * a_pcQuery )
	{
	sqlite_db * l_psSQLite = ( sqlite_db * ) a_pvData;
	sqlite_result * l_psResult = NULL;
	l_psResult = ( sqlite_result * ) xcalloc ( sizeof ( sqlite_result ) );
	l_psResult->f_iColumns = 0;
	l_psResult->f_iRows = 0;
	l_psResult->f_ppcData = NULL;
	if ( l_psSQLite->f_pcErrorMessage )
		xfree ( l_psSQLite->f_pcErrorMessage );
	l_psSQLite->f_iErrorCode = sqlite_get_table ( l_psSQLite, a_pcQuery,
			& l_psResult->f_ppcData, & l_psResult->f_iRows,
			& l_psResult->f_iColumns, & l_psSQLite->f_pcErrorMessage );
	return ( l_psResult );
	}

void db_unquery ( void * a_pvData )
	{
	sqlite_free_table ( ( ( sqlite_result * ) a_pvData )->f_ppcData );
	xfree ( a_pvData );
	return;
	}

char * rs_get ( void * a_pvData, int a_iRow,  int a_iColumn )
	{
	char * * l_ppcData = NULL;
	sqlite_result * l_psResult = ( sqlite_result * )a_pvData;
	l_ppcData = l_psResult->f_ppcData;
	return ( l_ppcData [ ( a_iRow + 1 ) * l_psResult->f_iColumns + a_iColumn ] );
	}

int rs_fields_count ( void * a_pvData )
	{
	return ( ( ( sqlite_result * ) a_pvData )->f_iColumns );
	}

long int rsdb_records_count ( void * a_pvDataB, void * a_pvDataR )
	{
	if ( a_pvDataR )
		return ( ( ( sqlite_result * ) a_pvDataR )->f_iRows );
	else
		return ( sqlite_changes ( ( sqlite * ) a_pvDataB ) );
	}

long int rsdb_id ( void * a_pvDataB, void * )
	{
	return ( sqlite_last_insert_rowid ( ( sqlite * ) a_pvDataB ) );
	}

char * rs_column_name ( void * a_pvDataR, int a_iField )
	{
	return ( ( ( sqlite_result * ) a_pvDataR )->f_ppcData [ a_iField ] );
	}

}

