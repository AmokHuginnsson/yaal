/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	sqlite3_driver.c - this file is integral part of `yaal' project.

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
#include <sys/stat.h>
#include <sqlite3.h>

#include "hcore/xalloc.h"

using namespace yaal::hcore;

extern "C"
{

typedef struct
	{
	int f_iErrorCode;
	char * f_pcErrorMessage;
	sqlite3 * f_psDB;
	} OSQLite;
	
typedef struct
	{
	int f_iRows;
	int f_iColumns;
	char * * f_ppcData;
	} OSQLiteResult;

OSQLite * g_psBrokenDB = NULL;

void * db_query ( void *, char const * );
void db_unquery ( void * );
void db_disconnect ( void * );

/* sqlite3 driver uses convention that database file name should have
 * .sqlite3 or .db3 extension, and this default extension is added
 * to user supplied database name by driver during db_connect. */

void * db_connect ( char const * a_pcDataBase,
		char const *, char const * )
	{
	int l_iNmLnght = 0;
	void * l_pvPtr = NULL;
	char * l_pcDataBase = NULL;
	char const l_pcFileNameExt [ ] = ".sqlite3";
	struct stat l_sStat;
	OSQLite * l_psSQLite = NULL;
	if ( g_psBrokenDB )
		{
		db_disconnect ( g_psBrokenDB );
		g_psBrokenDB = NULL;
		}
	l_psSQLite = xcalloc < OSQLite > ( 1 );
	l_iNmLnght = strlen ( a_pcDataBase );
	l_pcDataBase = xcalloc < char > ( l_iNmLnght + strlen ( l_pcFileNameExt ) + 1 );
	strcpy ( l_pcDataBase, a_pcDataBase );
	strcat ( l_pcDataBase, l_pcFileNameExt );
	if ( stat ( l_pcDataBase, & l_sStat ) )
		{
		strcpy ( l_pcDataBase + l_iNmLnght, ".db3" );
		if ( stat ( l_pcDataBase, & l_sStat ) )
			{
			asprintf ( & l_psSQLite->f_pcErrorMessage,
					"Database file `%s' does not exists.", l_pcDataBase );
			xfree ( l_pcDataBase );
			g_psBrokenDB = l_psSQLite;
			return ( NULL );
			}
		}
	l_psSQLite->f_iErrorCode = sqlite3_open ( l_pcDataBase,
			& l_psSQLite->f_psDB );
	xfree ( l_pcDataBase );
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
		else
			g_psBrokenDB = l_psSQLite, l_psSQLite = NULL;
		}
	return ( l_psSQLite );
	}

void db_disconnect ( void * a_pvData )
	{
	OSQLite * l_psSQLite = static_cast < OSQLite * > ( a_pvData );
	if ( l_psSQLite->f_psDB )
		sqlite3_close ( l_psSQLite->f_psDB );
	if ( l_psSQLite->f_pcErrorMessage )
		xfree ( l_psSQLite->f_pcErrorMessage );
	xfree ( l_psSQLite );
	return;
	}

int db_errno ( void * a_pvData )
	{
	OSQLite * l_psSQLite = static_cast < OSQLite * > ( a_pvData );
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

char const * db_error  ( void * a_pvData )
	{
	OSQLite * l_psSQLite = static_cast < OSQLite * > ( a_pvData );
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

void * db_query ( void * a_pvData, char const * a_pcQuery )
	{
	OSQLite * l_psSQLite = ( OSQLite * ) a_pvData;
	OSQLiteResult * l_psResult = NULL;
	l_psResult = xcalloc < OSQLiteResult > ( 1 );
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
	sqlite3_free_table ( static_cast < OSQLiteResult * > ( a_pvData )->f_ppcData );
	xfree ( a_pvData );
	return;
	}

char * rs_get ( void * a_pvData, int a_iRow, int a_iColumn )
	{
	char * * l_ppcData = NULL;
	OSQLiteResult * l_psResult = static_cast < OSQLiteResult * > ( a_pvData );
	l_ppcData = l_psResult->f_ppcData;
	return ( l_ppcData [ ( a_iRow + 1 ) * l_psResult->f_iColumns + a_iColumn ] );
	}

int rs_fields_count ( void * a_pvData )
	{
	return ( static_cast < OSQLiteResult * > ( a_pvData )->f_iColumns );
	}

int long dbrs_records_count ( void * a_pvDataB, void * a_pvDataR )
	{
	if ( a_pvDataR )
		return ( static_cast < OSQLiteResult * > ( a_pvDataR )->f_iRows );
	else
		return ( sqlite3_changes ( static_cast < OSQLite * > ( a_pvDataB )->f_psDB ) );
	}

int long dbrs_id ( void * a_pvDataB, void * )
	{
	return ( sqlite3_last_insert_rowid ( static_cast < OSQLite * > ( a_pvDataB )->f_psDB ) );
	}

char * rs_column_name ( void * a_pvDataR, int a_iField )
	{
	return ( static_cast < OSQLiteResult * > ( a_pvDataR )->f_ppcData [ a_iField ] );
	}

}

