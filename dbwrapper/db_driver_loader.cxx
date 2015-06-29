/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

  db_driver_loader.cxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <cstdlib>
#include <cstdio>
#include <libintl.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "db_driver_loader.hxx"
#include "hcore/memory.hxx"
#include "hcore/hstring.hxx"
#include "hcore/hlog.hxx"
#include "hcore/hfile.hxx"
#include "tools/hplugin.hxx"
#include "tools/hstringstream.hxx"
#include "dbwrapper.hxx"

using namespace yaal::hcore;
using namespace yaal::tools;

namespace yaal {

namespace dbwrapper {

namespace {

char const* const etag = "Error: Data base request (";
char const* const eend = ") while no driver loaded.";

}

char const* const _done_ = "done.\r\n";

static char const* _driver_[ 8 ] = {
	"default",
	"null",
	LIB_PREFIX "yaal_sqlite3_driver" LIB_INFIX "." LIB_EXT,
	LIB_PREFIX "yaal_postgresql_driver" LIB_INFIX "." LIB_EXT,
	LIB_PREFIX "yaal_mysql_driver" LIB_INFIX "." LIB_EXT,
	LIB_PREFIX "yaal_firebird_driver" LIB_INFIX "." LIB_EXT,
	LIB_PREFIX "yaal_oracle_driver" LIB_INFIX "." LIB_EXT,
	NULL
};

drivers_t _dBDrivers_;

/* Null driver */

namespace {

bool null_db_connect( ODBLink&, yaal::hcore::HString const&, yaal::hcore::HString const&, yaal::hcore::HString const&, yaal::hcore::HString const& ) {
	M_PROLOG
	log( LOG_LEVEL::ERROR ) << etag << "db_connect" << eend << endl;
	return ( true );
	M_EPILOG
}

void null_db_disconnect( ODBLink& ) {
	M_PROLOG
	log( LOG_LEVEL::ERROR ) << etag << "(db_disconnect)" << eend << endl;
	return;
	M_EPILOG
}

int null_dbrs_errno( ODBLink const&, void* ) {
	M_PROLOG
	log( LOG_LEVEL::ERROR ) << etag << "db_errno" << eend << endl;
	return ( 0 );
	M_EPILOG
}

char const* null_dbrs_error( ODBLink const&, void* ) {
	M_PROLOG
	log( LOG_LEVEL::ERROR ) << etag << "db_error" << eend << endl;
	return ( _( "null database driver loaded" ) );
	M_EPILOG
}

void* null_db_fetch_query_result( ODBLink&, char const* ) {
	M_PROLOG
	log( LOG_LEVEL::ERROR ) << etag << "db_fetch_query_result" << eend << endl;
	return ( NULL );
	M_EPILOG
}

void null_rs_free_query_result( void* ) {
	M_PROLOG
	log( LOG_LEVEL::ERROR ) << etag << "rs_free_query_result" << eend << endl;
	return;
	M_EPILOG
}

void* null_db_query( ODBLink&, char const* ) {
	M_PROLOG
	log( LOG_LEVEL::ERROR ) << etag << "db_query" << eend << endl;
	return ( NULL );
	M_EPILOG
}

void* null_db_prepare_query( ODBLink&, char const* ) {
	M_PROLOG
	log( LOG_LEVEL::ERROR ) << etag << "db_prepare_query" << eend << endl;
	return ( NULL );
	M_EPILOG
}

void null_query_bind( ODBLink&, void*, int, yaal::hcore::HString const& ) {
	M_PROLOG
	log( LOG_LEVEL::ERROR ) << etag << "query_bind" << eend << endl;
	return;
	M_EPILOG
}

void* null_query_execute( ODBLink&, void* ) {
	M_PROLOG
	log( LOG_LEVEL::ERROR ) << etag << "query_execute" << eend << endl;
	return ( NULL );
	M_EPILOG
}

void null_query_free( ODBLink&, void* ) {
	M_PROLOG
	log( LOG_LEVEL::ERROR ) << etag << "query_free" << eend << endl;
	return;
	M_EPILOG
}

void null_rs_free_cursor( void* ) {
	M_PROLOG
	log( LOG_LEVEL::ERROR ) << etag << "rs_free_cursor" << eend << endl;
	return;
	M_EPILOG
}

char const* null_rs_get( void*, int long, int ) {
	M_PROLOG
	log( LOG_LEVEL::ERROR ) << etag << "rs_get" << eend << endl;
	return ( NULL );
	M_EPILOG
}

bool null_rs_next( void* ) {
	M_PROLOG
	log( LOG_LEVEL::ERROR ) << etag << "rs_next" << eend << endl;
	return ( true );
	M_EPILOG
}

char const* null_rs_get_field( void*, int ) {
	M_PROLOG
	log( LOG_LEVEL::ERROR ) << etag << "rs_get_field" << eend << endl;
	return ( NULL );
	M_EPILOG
}

int null_rs_fields_count( void* ) {
	M_PROLOG
	log( LOG_LEVEL::ERROR ) << etag << "rs_fields_count" << eend << endl;
	return ( 0 );
	M_EPILOG
}

int long null_dbrs_records_count( ODBLink&, void* ) {
	M_PROLOG
	log( LOG_LEVEL::ERROR ) << etag << "dbrs_records_count" << eend << endl;
	return ( 0 );
	M_EPILOG
}

int long null_dbrs_id( ODBLink&, void* ) {
	M_PROLOG
	log( LOG_LEVEL::ERROR ) << etag << "dbrs_id" << eend << endl;
	return ( 0 );
	M_EPILOG
}

char const* null_rs_column_name( void*, int ) {
	M_PROLOG
	log( LOG_LEVEL::ERROR ) << etag << "rs_column_name" << eend << endl;
	return ( NULL );
	M_EPILOG
}

/* end of null-dummy driver */

void dbwrapper_error( void ) {
	M_PROLOG
	HString message;
	message = HPlugin().error_message( 0 );
	log( LOG_LEVEL::ERROR ) << message << endl;
	::fprintf( stderr, "(%s), ", message.raw() );
	return;
	M_EPILOG
}

void dbwrapper_exit( void ) __attribute__ ((noreturn));
void dbwrapper_exit( void ) {
	cerr << "failed." << endl;
	exit( 1 );
}

ODBConnector const* try_load_driver( ODBConnector::DRIVER::enum_t driverId_ ) {
	M_PROLOG
	M_ENSURE( ( driverId_ >= ODBConnector::DRIVER::SQLITE3 ) && ( driverId_ < ODBConnector::DRIVER::TERMINATOR ) );
	drivers_t::const_iterator it = _dBDrivers_.find( driverId_ );
	driver_t driver;
	if ( it == _dBDrivers_.end() ) /* given driver has not been loaded yet */ {
		try {
			driver = make_pair( make_pointer<HPlugin>(), make_pointer<ODBConnector>() );
			log( LOG_LEVEL::NOTICE ) << "Loading [" << _driver_[ driverId_ + 1 ] << "] driver ... ";
			driver.first->load( _driver_[ driverId_ + 1 ] );
			cout << "(linking symbols ...) " << flush;
			driver.first->try_resolve( SYMBOL_PREFIX"driver_init", driver.second->driver_init );
			driver.first->try_resolve( SYMBOL_PREFIX"driver_cleanup", driver.second->driver_cleanup );
			driver.first->resolve( SYMBOL_PREFIX"db_disconnect", driver.second->db_disconnect );
			driver.first->resolve( SYMBOL_PREFIX"dbrs_errno", driver.second->dbrs_errno );
			driver.first->resolve( SYMBOL_PREFIX"dbrs_error", driver.second->dbrs_error );
			driver.first->resolve( SYMBOL_PREFIX"db_fetch_query_result", driver.second->db_fetch_query_result );
			driver.first->resolve( SYMBOL_PREFIX"rs_free_query_result", driver.second->rs_free_query_result );
			driver.first->resolve( SYMBOL_PREFIX"db_query", driver.second->db_query );
			driver.first->resolve( SYMBOL_PREFIX"db_prepare_query", driver.second->db_prepare_query );
			driver.first->resolve( SYMBOL_PREFIX"query_bind", driver.second->query_bind );
			driver.first->resolve( SYMBOL_PREFIX"query_execute", driver.second->query_execute );
			driver.first->resolve( SYMBOL_PREFIX"query_free", driver.second->query_free );
			driver.first->resolve( SYMBOL_PREFIX"rs_free_cursor", driver.second->rs_free_cursor );
			driver.first->resolve( SYMBOL_PREFIX"rs_get", driver.second->rs_get );
			driver.first->resolve( SYMBOL_PREFIX"rs_next", driver.second->rs_next );
			driver.first->resolve( SYMBOL_PREFIX"rs_get_field", driver.second->rs_get_field );
			driver.first->resolve( SYMBOL_PREFIX"rs_fields_count", driver.second->rs_fields_count );
			driver.first->resolve( SYMBOL_PREFIX"dbrs_records_count", driver.second->dbrs_records_count );
			driver.first->resolve( SYMBOL_PREFIX"dbrs_id", driver.second->dbrs_id );
			driver.first->resolve( SYMBOL_PREFIX"rs_column_name", driver.second->rs_column_name );
			driver.first->resolve( SYMBOL_PREFIX"db_connect", driver.second->db_connect );
			char const** tlq = NULL; /* msvcxx bug */
			driver.first->resolve( SYMBOL_PREFIX"TABLE_LIST_QUERY", tlq );
			driver.second->_tableListQuery = *tlq;
			char const** clq = NULL; /* msvcxx bug */
			driver.first->resolve( SYMBOL_PREFIX"COLUMN_LIST_QUERY", clq );
			driver.second->_columnListQuery = *clq;
			int* cni( NULL );
			driver.first->resolve( SYMBOL_PREFIX"COLUMN_NAME_INDEX", cni );
			driver.second->_columnNameIndex = *cni;
			it = _dBDrivers_.insert( make_pair( driverId_, driver ) ).first;
			driver.second->init();
		} catch ( HPluginException& e ) {
			log( LOG_LEVEL::NOTICE ) << "fail." << endl;
			HStringStream reason;
			reason << _( "cannot load database driver: " ) << e.what();
			M_THROW( reason.string(), _dataBaseDriver_ );
		}
		if ( driver.first->is_loaded() )
			log( LOG_LEVEL::NOTICE ) << "success." << endl;
		else {
			dbwrapper_error();
			dbwrapper_exit();
		}
	}
	return ( it->second.second.raw() );
	M_EPILOG
}

}

ODBConnector const* load_driver( ODBConnector::DRIVER::enum_t driverId_ ) {
	M_PROLOG
	errno = 0;
	cout << "Using dynamic database driver [" << _driver_[ driverId_ + 1 ] << "] ... " << flush;
	ODBConnector const* pConnector( NULL );
	if ( driverId_ != ODBConnector::DRIVER::NONE ) {
		if ( driverId_ == ODBConnector::DRIVER::DEFAULT ) {
			for ( int i = 1; i < ODBConnector::DRIVER::TERMINATOR; ++ i ) {
				if ( ( pConnector = try_load_driver( static_cast<ODBConnector::DRIVER::enum_t>( i ) ) ) )
					break;
			}
		} else
			pConnector = try_load_driver( static_cast<ODBConnector::DRIVER::enum_t>( driverId_ ) );
	}
	if ( ! pConnector || ( pConnector->db_connect != null_db_connect ) )
		cout << _done_ << flush;
	return ( pConnector );
	M_EPILOG
}

/*
 * ODBConnector implementation is in this file instead of db_driver.cxx because
 * build system discards implementation files with file name ending with _driver.cxx.
 */

ODBConnector::ODBConnector( void )
	: driver_init( NULL ),
	driver_cleanup( NULL ),
	db_connect( null_db_connect ),
	db_disconnect( null_db_disconnect ),
	dbrs_errno( null_dbrs_errno ),
	dbrs_error( null_dbrs_error ),
	db_fetch_query_result( null_db_fetch_query_result ),
	rs_free_query_result( null_rs_free_query_result ),
	db_query( null_db_query ),
	db_prepare_query( null_db_prepare_query ),
	query_bind( null_query_bind ),
	query_execute( null_query_execute ),
	query_free( null_query_free ),
	rs_free_cursor( null_rs_free_cursor ),
	rs_get( null_rs_get ),
	rs_next( null_rs_next ),
	rs_get_field( null_rs_get_field ),
	rs_fields_count( null_rs_fields_count ),
	dbrs_records_count( null_dbrs_records_count ),
	dbrs_id( null_dbrs_id ),
	rs_column_name( null_rs_column_name ),
	_tableListQuery( NULL ),
	_columnListQuery( NULL ),
	_columnNameIndex( 0 ) {
}

ODBConnector::~ODBConnector( void ) {
	M_PROLOG
	cleanup();
	return;
	M_DESTRUCTOR_EPILOG
}

void ODBConnector::cleanup( void ) {
	M_PROLOG
	if ( driver_cleanup )
		driver_cleanup();
	return;
	M_EPILOG
}

void ODBConnector::init( void ) {
	M_PROLOG
	if ( driver_init )
		driver_init();
	return;
	M_EPILOG
}

yaal::hcore::HString transform_sql( yaal::hcore::HString sql_, place_holder_generator_t gen_ ) {
	M_PROLOG
	int placeholerNo( 1 );
	int len( static_cast<int>( sql_.get_length() ) );
	bool inSingleQuote( false );
	bool inDoubleQuote( false );
	for ( int pos( 0 ), i( 0 ); i < len; ++ i, ++ pos ) {
		char currChar( sql_[pos] );
		if ( inSingleQuote ) {
			if ( currChar == '\'' ) {
				inSingleQuote = false;
			}
		} else if ( inDoubleQuote ) {
			if ( currChar == '"' ) {
				inDoubleQuote = false;
			}
		} else {
			if ( currChar == '\'' ) {
				inSingleQuote = true;
			} else if ( currChar == '"' ) {
				inDoubleQuote = true;
			} else if ( currChar == '?' ) {
				HString placeholer( gen_( placeholerNo ++ ) );
				M_ENSURE( placeholer.get_length() > 0 );
				if ( placeholer != "?" ) {
					sql_.replace( pos, 1, placeholer );
					pos += ( static_cast<int>( placeholer.get_length() ) - 1 );
				}
			}
		}
	}
	return ( sql_ );
	M_EPILOG
}

} /* namespace dbwrapper */

} /* namespace yaal */

