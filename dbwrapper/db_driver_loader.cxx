/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	db_driver_loader.cxx - this file is integral part of `yaal' project.

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

#include "hcore/hexception.h"
M_VCSID ( "$Id$" )
#include "hcore/hstring.h"
#include "hcore/hlog.h"
#include "tools/hplugin.h"
#include "db_driver_loader.h"
#include "dbwrapper.h"

using namespace yaal::hcore;
using namespace yaal::tools;

namespace yaal
{

namespace dbwrapper
{

namespace
{

char const* const etag = "Error: Data base request (";
char const* const eend = ") while no driver loaded.";

}

char const* const g_pcDone = "done.\r\n";

static char const* g_ppcDriver[ 7 ] =
	{
	NULL,
	"libsqlite3_driver.so",
	"libsqlite_driver.so",
	"libmysql_driver.so",
	"libpostgresql_driver.so",
	"liboracle_driver.so",
	NULL
	};

HPlugin n_oDBDriver;

/* Null driver */

void* null_db_connect( char const*, char const*, char const* )
	{
	log( LOG_TYPE::D_ERROR ) << etag << "(db_connect)" << eend << endl;
	return ( NULL );
	}

void null_db_disconnect( void* )
	{
	log( LOG_TYPE::D_ERROR ) << etag << "(db_disconnect)" << eend << endl;
	return;
	}

int null_db_errno( void const* )
	{
	log( LOG_TYPE::D_ERROR ) << etag << "(db_errno)" << eend << endl;
	return ( 0 );
	}

char const* null_db_error( void const* )
	{
	log( LOG_TYPE::D_ERROR ) << etag << "db_error)" << eend << endl;
	return ( _( "null database driver loaded" ) );
	}

void* null_db_query( void*, char const* )
	{
	log( LOG_TYPE::D_ERROR ) << etag << "(db_query)" << eend << endl;
	return ( NULL );
	}

void null_db_unquery( void* )
	{
	log( LOG_TYPE::D_ERROR ) << etag << "(db_unquery)" << eend << endl;
	return;
	}

char* null_rs_get( void*, int, int )
	{
	log( LOG_TYPE::D_ERROR ) << etag << "(rs_get)" << eend << endl;
	return ( NULL );
	}

int null_rs_fields_count( void const* )
	{
	log( LOG_TYPE::D_ERROR ) << etag << "(rs_fields_count)" << eend << endl;
	return ( 0 );
	}

int long null_dbrs_records_count( void const*, void const* )
	{
	log( LOG_TYPE::D_ERROR ) << etag << "(dbrs_records_count)" << eend << endl;
	return ( 0 );
	}

int long null_dbrs_id( void const*, void const* )
	{
	log( LOG_TYPE::D_ERROR ) << etag << "(dbrs_id)" << eend << endl;
	return ( 0 );
	}

char* null_rs_column_name( void const*, int )
	{
	log( LOG_TYPE::D_ERROR ) << etag << "(rs_column_name)" << eend << endl;
	return ( NULL );
	}

/* end of null-dummy driver */

void dbwrapper_error( void )
	{
	HString l_oMessage;
	l_oMessage = dlerror();
	log( LOG_TYPE::D_ERROR ) << l_oMessage << endl;
	::fprintf( stderr, "(%s), ", static_cast<char const* const>( l_oMessage ) );
	return;
	}

void dbwrapper_exit ( void ) __attribute__  ((noreturn));
void dbwrapper_exit ( void )
	{
	::fprintf( stderr, "failed.\r\n" );
	exit ( 1 );
	}

void load_driver( void );
void* autoloader_db_connect( char const* a_pcDataBase,
		char const* a_pcLogin, char const* a_pcPassword )
	{
	load_driver();
	return ( db_connect( a_pcDataBase, a_pcLogin, a_pcPassword ) );
	}

void load_driver( void )
	{
	M_PROLOG
	int l_iCtr = 0;
	errno = 0;
	fprintf ( stderr, "Loading dynamic database driver ... " );
	if ( dbwrapper::n_iDataBaseDriver )
		{
		l_iCtr = dbwrapper::n_iDataBaseDriver;
		try
			{
			n_oDBDriver.load( g_ppcDriver [ l_iCtr ++ ] );
			}
		catch ( HPluginException& )
			{
			}
		while ( ! n_oDBDriver.is_loaded() && g_ppcDriver[ l_iCtr ] )
			{
			if ( ( l_iCtr == dbwrapper::n_iDataBaseDriver ) && l_iCtr ++ )
				continue;
			dbwrapper_error();
			try
				{
				n_oDBDriver.load( g_ppcDriver [ l_iCtr ++ ] );
				}
			catch ( HPluginException& )
				{
				}
			}
		if ( ! n_oDBDriver.is_loaded() )
			dbwrapper_exit();
		else
			{
			log ( LOG_TYPE::D_NOTICE ) << "Loading [" << g_ppcDriver [ l_iCtr - 1 ];
			log << "] driver." << endl;
			}
		fprintf ( stderr, g_pcDone );
		fprintf ( stderr, "Linking symbols ... " );
		try
			{
			n_oDBDriver.resolve( "db_disconnect", db_disconnect );
			n_oDBDriver.resolve( "db_errno", db_errno );
			n_oDBDriver.resolve( "db_error", db_error );
			n_oDBDriver.resolve( "db_query", db_query );
			n_oDBDriver.resolve( "db_unquery", db_unquery );
			n_oDBDriver.resolve( "rs_get", rs_get );
			n_oDBDriver.resolve( "rs_fields_count", rs_fields_count );
			n_oDBDriver.resolve( "dbrs_records_count", dbrs_records_count );
			n_oDBDriver.resolve( "dbrs_id", dbrs_id );
			n_oDBDriver.resolve( "rs_column_name", rs_column_name );
			n_oDBDriver.resolve( "db_connect", db_connect );
			}
		catch ( HPluginException& e )
			{
			M_THROW( _( "can not load database driver" ), n_iDataBaseDriver );
			}
		if ( db_connect != autoloader_db_connect )
			fprintf( stderr, g_pcDone );
		}
	else
		{
		db_connect = null_db_connect;
		db_disconnect = null_db_disconnect;
		db_errno = null_db_errno;
		db_error = null_db_error;
		db_query = null_db_query;
		db_unquery = null_db_unquery;
		rs_get = null_rs_get;
		rs_fields_count = null_rs_fields_count;
		dbrs_records_count = null_dbrs_records_count;
		dbrs_id = null_dbrs_id;
		rs_column_name = null_rs_column_name;
		}
	return;
	M_EPILOG
	}

/* Driver autoloaders ... */

void autoloader_db_disconnect( void* a_pvDataBase )
	{
	load_driver();
	db_disconnect( a_pvDataBase );
	return;
	}

int autoloader_db_errno( void const* a_pvDataBase )
	{
	load_driver();
	return ( db_errno( a_pvDataBase ) );
	}

char const* autoloader_db_error( void const* a_pvDataBase )
	{
	load_driver();
	return ( db_error( a_pvDataBase ) );
	}

void* autoloader_db_query( void* a_pvDataBase, char const* a_pcQuery )
	{
	load_driver();
	return ( db_query( a_pvDataBase, a_pcQuery ) );
	}

void autoloader_db_unquery( void* a_pvDataBase )
	{
	load_driver();
	db_unquery( a_pvDataBase );
	return;
	}

char* autoloader_rs_get( void* a_pvResult, int a_iRow, int a_iCol )
	{
	load_driver();
	return ( rs_get( a_pvResult, a_iRow, a_iCol ) );
	}

int autoloader_rs_fields_count( void const* a_pvResult )
	{
	load_driver();
	return ( rs_fields_count( a_pvResult ) );
	}

int long autoloader_dbrs_records_count( void const* a_pvDataBase,
		void const* a_pvResult )
	{
	load_driver();
	return ( dbrs_records_count( a_pvDataBase, a_pvResult ) );
	}

int long autoloader_dbrs_id( void const* a_pvDataBase, void const* a_pvResult )
	{
	load_driver();
	return ( dbrs_id( a_pvDataBase, a_pvResult ) );
	}

char* autoloader_rs_column_name( void const* a_pvResult, int a_iColumn )
	{
	load_driver();
	return ( rs_column_name( a_pvResult, a_iColumn ) );
	}

db_connect_t db_connect = autoloader_db_connect;
db_disconnect_t db_disconnect = autoloader_db_disconnect;
db_errno_t db_errno = autoloader_db_errno;
db_error_t db_error = autoloader_db_error;
db_query_t db_query = autoloader_db_query;
db_unquery_t db_unquery = autoloader_db_unquery;
rs_get_t rs_get = autoloader_rs_get;
rs_fields_count_t rs_fields_count = autoloader_rs_fields_count;
dbrs_records_count_t dbrs_records_count = autoloader_dbrs_records_count;
dbrs_id_t dbrs_id = autoloader_dbrs_id;
rs_column_name_t rs_column_name = autoloader_rs_column_name;

/* end of driver autoloader section */

} /* namespace dbwrapper */

} /* namespace yaal */

