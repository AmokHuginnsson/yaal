
/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	mysql_driver.c - this file is integral part of `stdhapi' project.

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
#include <string.h>

#include "../hcore/xalloc.h"

#include "../config.h"

#ifdef __HOST_OS_TYPE_FREEBSD__
#	define TO_BE_INCLUDED_LATER 1
#endif /* __HOST_OS_TYPE_FREEBSD__ */

#include <mysql/mysql.h>

#ifndef NULL
#define NULL	0
#endif /* not NULL */

extern "C"
{

MYSQL * g_psBrokenDB = NULL;

void db_disconnect ( void * );

void * db_connect ( const char * a_pcDataBase,
		const char * a_pcLogin, const char * a_pcPassword )
	{
	MYSQL * l_psMySQL = NULL;
	if ( g_psBrokenDB )
		{
		db_disconnect ( g_psBrokenDB );
		g_psBrokenDB = NULL;
		}
	l_psMySQL = mysql_init ( NULL );
	if ( l_psMySQL )
		{
		if ( mysql_options ( l_psMySQL, MYSQL_OPT_NAMED_PIPE, NULL ) )
			g_psBrokenDB = l_psMySQL, l_psMySQL = NULL;
		else if ( ! mysql_real_connect ( l_psMySQL, NULL, a_pcLogin, a_pcPassword,
				a_pcDataBase, 0, NULL, CLIENT_IGNORE_SPACE ) )
			g_psBrokenDB = l_psMySQL, l_psMySQL = NULL;
		}
	return ( l_psMySQL );
	}

void db_disconnect ( void * a_pvData )
	{
	mysql_close ( ( MYSQL * ) a_pvData );
	return;
	}

int db_errno ( void * a_pvData )
	{
	if ( ! a_pvData )a_pvData = g_psBrokenDB;
	return ( mysql_errno ( ( MYSQL * ) a_pvData ) );
	}

const char * db_error  ( void * a_pvData )
	{
	if ( ! a_pvData )a_pvData = g_psBrokenDB;
	return ( mysql_error ( ( MYSQL * ) a_pvData ) );
	}

void * db_query ( void * a_pvData, const char * a_pcQuery )
	{
	mysql_query ( ( MYSQL * ) a_pvData, a_pcQuery );
	return ( mysql_store_result ( ( MYSQL * ) a_pvData ) );
	}

void db_unquery ( void * a_pvData )
	{
	mysql_free_result ( ( MYSQL_RES * ) a_pvData );
	return;
	}

char * rs_get ( void * a_pvData, int a_iRow,  int a_iColumn )
	{
	MYSQL_ROW l_ppcRow;
	mysql_data_seek ( ( MYSQL_RES * ) a_pvData, a_iRow );
	l_ppcRow = mysql_fetch_row ( ( MYSQL_RES * ) a_pvData );
	return ( l_ppcRow [ a_iColumn ] );
	}

int rs_fields_count ( void * a_pvData )
	{
	return ( mysql_num_fields ( ( MYSQL_RES * ) a_pvData ) );
	}

long int rsdb_records_count ( void * a_pvDataB, void * a_pvDataR )
	{
	if ( a_pvDataR )
		return ( mysql_num_rows ( ( MYSQL_RES * ) a_pvDataR ) );
	else
		return ( mysql_affected_rows ( ( MYSQL * ) a_pvDataB ) );
	}

long int rsdb_id ( void * a_pvDataB, void * )
	{
	return ( mysql_insert_id ( ( MYSQL * ) a_pvDataB ) );
	}

char * rs_column_name ( void * a_pvDataR, int a_iField )
	{
	MYSQL_FIELD * l_psField = NULL;
	l_psField = mysql_fetch_field_direct ( ( MYSQL_RES * ) a_pvDataR, a_iField );
	return ( l_psField->name );
	}

}

