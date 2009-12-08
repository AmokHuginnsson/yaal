/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	db_driver_loader.cxx - this file is integral part of `yaal' project.

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
#include <cstring>
#include <cstdlib>
#include <dlfcn.h>
#include <libintl.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "hcore/hstring.hxx"
#include "hcore/hlog.hxx"
#include "tools/hplugin.hxx"
#include "db_driver_loader.hxx"
#include "dbwrapper.hxx"

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
	LIB_PREFIX"sqlite3_driver."LIB_EXT,
	LIB_PREFIX"sqlite_driver."LIB_EXT,
	LIB_PREFIX"mysql_driver."LIB_EXT,
	LIB_PREFIX"postgresql_driver."LIB_EXT,
	LIB_PREFIX"oracle_driver."LIB_EXT,
	NULL
	};

typedef HMap<ODBConnector::DRIVER::enum_t, HPlugin::ptr_t> drivers_t;
drivers_t n_oDBDrivers;

/* Null driver */

void* null_db_connect( char const*, char const*, char const* )
	{
	M_PROLOG
	log( LOG_TYPE::ERROR ) << etag << "(db_connect)" << eend << endl;
	return ( NULL );
	M_EPILOG
	}

void null_db_disconnect( void* )
	{
	M_PROLOG
	log( LOG_TYPE::ERROR ) << etag << "(db_disconnect)" << eend << endl;
	return;
	M_EPILOG
	}

int null_db_errno( void* )
	{
	M_PROLOG
	log( LOG_TYPE::ERROR ) << etag << "(db_errno)" << eend << endl;
	return ( 0 );
	M_EPILOG
	}

char const* null_db_error( void* )
	{
	M_PROLOG
	log( LOG_TYPE::ERROR ) << etag << "db_error)" << eend << endl;
	return ( _( "null database driver loaded" ) );
	M_EPILOG
	}

void* null_db_query( void*, char const* )
	{
	M_PROLOG
	log( LOG_TYPE::ERROR ) << etag << "(db_query)" << eend << endl;
	return ( NULL );
	M_EPILOG
	}

void null_db_unquery( void* )
	{
	M_PROLOG
	log( LOG_TYPE::ERROR ) << etag << "(db_unquery)" << eend << endl;
	return;
	M_EPILOG
	}

char* null_rs_get( void*, int long, int )
	{
	M_PROLOG
	log( LOG_TYPE::ERROR ) << etag << "(rs_get)" << eend << endl;
	return ( NULL );
	M_EPILOG
	}

int null_rs_fields_count( void* )
	{
	M_PROLOG
	log( LOG_TYPE::ERROR ) << etag << "(rs_fields_count)" << eend << endl;
	return ( 0 );
	M_EPILOG
	}

int long null_dbrs_records_count( void*, void* )
	{
	M_PROLOG
	log( LOG_TYPE::ERROR ) << etag << "(dbrs_records_count)" << eend << endl;
	return ( 0 );
	M_EPILOG
	}

int long null_dbrs_id( void*, void* )
	{
	M_PROLOG
	log( LOG_TYPE::ERROR ) << etag << "(dbrs_id)" << eend << endl;
	return ( 0 );
	M_EPILOG
	}

char* null_rs_column_name( void*, int )
	{
	M_PROLOG
	log( LOG_TYPE::ERROR ) << etag << "(rs_column_name)" << eend << endl;
	return ( NULL );
	M_EPILOG
	}

/* end of null-dummy driver */

void dbwrapper_error( void )
	{
	M_PROLOG
	HString l_oMessage;
	l_oMessage = dlerror();
	log( LOG_TYPE::ERROR ) << l_oMessage << endl;
	::fprintf( stderr, "(%s), ", l_oMessage.raw() );
	return;
	M_EPILOG
	}

void dbwrapper_exit( void ) __attribute__ ((noreturn));
void dbwrapper_exit( void )
	{
	::fprintf( stderr, "failed.\r\n" );
	exit( 1 );
	}

bool load_driver( ODBConnector& connector_, ODBConnector::DRIVER::enum_t const& driverId_ )
	{
	M_PROLOG
	int l_iCtr = 0;
	errno = 0;
	fprintf ( stderr, "Loading dynamic database driver ... " );
	bool fail = false;
	if ( driverId_ != ODBConnector::DRIVER::NONE )
		{
		if ( driverId_ == ODBConnector::DRIVER::AUTO )
			{
			for ( int i = 1; i < ODBConnector::DRIVER::TERMINATOR; ++ i )
				if ( ! load_driver( connector_, static_cast<ODBConnector::DRIVER::enum_t>( i ) ) )
					break;
			}
		else
			{
			drivers_t::const_iterator it = n_oDBDrivers.find( driverId_ );
			HPlugin::ptr_t driver;
			if ( it == n_oDBDrivers.end() )
				{
				try
					{
					driver = HPlugin::ptr_t( new HPlugin() );
					driver->load( g_ppcDriver[ driverId_ ] );
					n_oDBDrivers.insert( make_pair( driverId_, driver ) );
					}
				catch ( HPluginException& )
					{
					fail = true;
					}
				}
			else
				driver = it->second;
			if ( ! driver->is_loaded() )
				dbwrapper_exit();
			else
				{
				log ( LOG_TYPE::NOTICE ) << "Loading [" << g_ppcDriver[ l_iCtr - 1 ];
				log << "] driver." << endl;
				}
			fprintf( stderr, g_pcDone );
			fprintf( stderr, "Linking symbols ... " );
			try
				{
				driver->resolve( SYMBOL_PREFIX"db_disconnect", connector_.db_disconnect );
				driver->resolve( SYMBOL_PREFIX"db_errno", connector_.db_errno );
				driver->resolve( SYMBOL_PREFIX"db_error", connector_.db_error );
				driver->resolve( SYMBOL_PREFIX"db_query", connector_.db_query );
				driver->resolve( SYMBOL_PREFIX"db_unquery", connector_.db_unquery );
				driver->resolve( SYMBOL_PREFIX"rs_get", connector_.rs_get );
				driver->resolve( SYMBOL_PREFIX"rs_fields_count", connector_.rs_fields_count );
				driver->resolve( SYMBOL_PREFIX"dbrs_records_count", connector_.dbrs_records_count );
				driver->resolve( SYMBOL_PREFIX"dbrs_id", connector_.dbrs_id );
				driver->resolve( SYMBOL_PREFIX"rs_column_name", connector_.rs_column_name );
				driver->resolve( SYMBOL_PREFIX"db_connect", connector_.db_connect );
				}
			catch ( HPluginException& )
				{
				M_THROW( _( "cannot load database driver" ), n_eDataBaseDriver );
				}
			if ( connector_.db_connect != null_db_connect )
				fprintf( stderr, g_pcDone );
			}
		}
	return ( fail );
	M_EPILOG
	}

ODBConnector::ODBConnector( void )
	: db_connect( null_db_connect ),
	db_disconnect( null_db_disconnect ),
	db_errno( null_db_errno ),
	db_error( null_db_error ),
	db_query( null_db_query ),
	db_unquery( null_db_unquery ),
	rs_get( null_rs_get ),
	rs_fields_count( null_rs_fields_count ),
	dbrs_records_count( null_dbrs_records_count ),
	dbrs_id( null_dbrs_id ),
	rs_column_name( null_rs_column_name )
	{
	}

/* end of driver null section */

} /* namespace dbwrapper */

} /* namespace yaal */

