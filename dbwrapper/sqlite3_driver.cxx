/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	sqlite3_driver.cxx - this file is integral part of `yaal' project.

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

#include <cstdio>
#include <cstdlib>
#include <errno.h>
#include <cstring>
#include <sys/stat.h>
#include <sqlite3.h>

#include "hcore/compat.hxx"
#include "hcore/hstring.hxx"

using namespace yaal::hcore;

extern "C"
{

struct OSQLite
	{
	int f_iErrorCode;
	HString f_oErrorMessage;
	sqlite3* f_psDB;
	OSQLite( void )
		: f_iErrorCode( 0 ), f_oErrorMessage(), f_psDB( NULL ) {}
private:
	OSQLite( OSQLite const& );
	OSQLite& operator = ( OSQLite const& );
	};
	
struct OSQLiteResult
	{
	int f_iRows;
	int f_iColumns;
	char** f_ppcData;
	int f_iErrorCode;
	HString f_oErrorMessage;
	OSQLiteResult( void )
		: f_iRows( 0 ), f_iColumns( 0 ), f_ppcData( NULL ),
		f_iErrorCode( 0 ), f_oErrorMessage() {}
private:
	OSQLiteResult( OSQLiteResult const& );
	OSQLiteResult& operator = ( OSQLiteResult const& );
	};

OSQLite* g_psBrokenDB = NULL;

void* db_query( void*, char const* );
void rs_unquery( void* );
void db_disconnect( void* );

/* sqlite3 driver uses convention that database file name should have
 * .sqlite3 or .db3 extension, and this default extension is added
 * to user supplied database name by driver during db_connect. */

void* db_connect( char const* a_pcDataBase,
		char const*, char const* )
	{
	void* l_pvPtr = NULL;
	char const l_pcFileNameExt[] = ".sqlite3";
	struct stat l_sStat;
	OSQLite* l_psSQLite = NULL;
	if ( g_psBrokenDB )
		{
		db_disconnect( g_psBrokenDB );
		g_psBrokenDB = NULL;
		}
	l_psSQLite = new OSQLite;
	HString l_oDataBase( a_pcDataBase );
	l_oDataBase += l_pcFileNameExt; 
	if ( ::stat( l_oDataBase.raw(), &l_sStat ) )
		{
		l_oDataBase = a_pcDataBase;
		l_oDataBase += ".db3";
		if ( ::stat( l_oDataBase.raw(), &l_sStat ) )
			{
			l_psSQLite->f_oErrorMessage.format( "Database file `%s' does not exists.", l_oDataBase.raw() );
			g_psBrokenDB = l_psSQLite;
			return ( NULL );
			}
		}
	l_psSQLite->f_iErrorCode = ::sqlite3_open( l_oDataBase.raw(),
			&l_psSQLite->f_psDB );
	if ( l_psSQLite->f_iErrorCode )
		{
		l_psSQLite->f_oErrorMessage = ::sqlite3_errmsg( l_psSQLite->f_psDB );
		g_psBrokenDB = l_psSQLite;
		l_psSQLite = NULL;
		}
	else
		{
		l_pvPtr = db_query( l_psSQLite, "PRAGMA empty_result_callbacks = ON;" );
		if ( l_pvPtr )
			rs_unquery( l_pvPtr );
		else
			g_psBrokenDB = l_psSQLite, l_psSQLite = NULL;
		}
	return ( l_psSQLite );
	}

void db_disconnect( void* a_pvData )
	{
	OSQLite* l_psSQLite = static_cast<OSQLite*>( a_pvData );
	if ( l_psSQLite->f_psDB )
		sqlite3_close( l_psSQLite->f_psDB );
	delete l_psSQLite;
	return;
	}

int dbrs_errno( void* db_, void* result_ )
	{
	OSQLite* l_psSQLite = static_cast<OSQLite*>( db_ );
	OSQLiteResult* r = static_cast<OSQLiteResult*>( result_ );
	if ( ! l_psSQLite )
		l_psSQLite = g_psBrokenDB;
	int code( errno );
	if ( r && r->f_iErrorCode )
		code = r->f_iErrorCode;
	else if ( l_psSQLite )
		{
		if ( l_psSQLite->f_iErrorCode )
			code = l_psSQLite->f_iErrorCode;
		else
			code = sqlite3_errcode( l_psSQLite->f_psDB );
		}
	return ( code );
	}

char const* dbrs_error( void* db_, void* result_ )
	{
	OSQLite* l_psSQLite = static_cast<OSQLite*>( db_ );
	OSQLiteResult* r = static_cast<OSQLiteResult*>( result_ );
	if ( ! l_psSQLite )
		l_psSQLite = g_psBrokenDB;
	char const* msg( "" );
	if ( r && ! r->f_oErrorMessage.is_empty() )
		msg = r->f_oErrorMessage.raw();
	else if ( l_psSQLite )
		{
		if ( ! l_psSQLite->f_oErrorMessage.is_empty() )
			return ( l_psSQLite->f_oErrorMessage.raw() );
		return ( sqlite3_errmsg( l_psSQLite->f_psDB ) );
		}
	return ( msg );
	}

void* db_query( void* a_pvData, char const* a_pcQuery )
	{
	OSQLite* l_psSQLite = static_cast<OSQLite*>( a_pvData );
	OSQLiteResult * l_psResult = NULL;
	l_psResult = new OSQLiteResult;
	l_psResult->f_iColumns = 0;
	l_psResult->f_iRows = 0;
	l_psResult->f_ppcData = NULL;
	char* errmsg = NULL;
	l_psResult->f_iErrorCode = sqlite3_get_table( l_psSQLite->f_psDB,
			a_pcQuery, &l_psResult->f_ppcData, &l_psResult->f_iRows,
			&l_psResult->f_iColumns, &errmsg );
	l_psResult->f_oErrorMessage = errmsg;
	return ( l_psResult );
	}

void rs_unquery( void* a_pvData )
	{
	OSQLiteResult* pr = static_cast<OSQLiteResult*>( a_pvData );
	sqlite3_free_table( pr->f_ppcData );
	delete pr;
	return;
	}

char* rs_get( void* a_pvData, int long a_iRow, int a_iColumn )
	{
	char** l_ppcData = NULL;
	OSQLiteResult* l_psResult = static_cast<OSQLiteResult*>( a_pvData );
	l_ppcData = l_psResult->f_ppcData;
	return ( l_ppcData[ ( a_iRow + 1 ) * l_psResult->f_iColumns + a_iColumn ] );
	}

int rs_fields_count( void* a_pvData )
	{
	return ( static_cast<OSQLiteResult*>( a_pvData )->f_iColumns );
	}

int long dbrs_records_count( void* a_pvDataB, void* a_pvDataR )
	{
	if ( a_pvDataR )
		return ( static_cast<OSQLiteResult*>( a_pvDataR )->f_iRows );
	else
		return ( ::sqlite3_changes( static_cast<OSQLite*>( a_pvDataB )->f_psDB ) );
	}

int long dbrs_id( void* a_pvDataB, void* )
	{
	/* FIXME change driver interface to allow 64bit insert row id (from autoincrement) */
	return ( static_cast<int long>( sqlite3_last_insert_rowid( static_cast<OSQLite*>( a_pvDataB )->f_psDB ) ) );
	}

char* rs_column_name( void* a_pvDataR, int a_iField )
	{
	return ( static_cast<OSQLiteResult*>( a_pvDataR )->f_ppcData [ a_iField ] );
	}

int yaal_sqlite3_driver_main( int, char** )
	{
	return ( 0 );
	}

}

