/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	dbwrapper.cxx - this file is integral part of `yaal' project.

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

#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "config.hxx"

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hcore/hlog.hxx"
#include "hcore/hstring.hxx"
#include "hcore/hprogramoptionshandler.hxx"
#include "hcore/hcore.hxx"
#include "hconsole/hconsole.hxx"
#include "tools/tools.hxx"
#include "dbwrapper.hxx"
#include "db_driver_loader.hxx"

using namespace yaal::hcore;
using namespace yaal::hconsole;
using namespace yaal::tools;

namespace yaal {

namespace dbwrapper {

namespace {

ODBConnector::DRIVER::enum_t db_driver_from_string( HString const& dbType_ ) {
	M_PROLOG
	ODBConnector::DRIVER::enum_t driver( ODBConnector::DRIVER::NONE );
	if ( ! ( strcasecmp( dbType_, "none" )
				&& strcasecmp( dbType_, "null" )
				&& strcasecmp( dbType_, "dummy" ) ) )
		driver = ODBConnector::DRIVER::NONE;
	else if ( ! strcasecmp( dbType_, "SQLite3" ) )
		driver = ODBConnector::DRIVER::SQLITE3;
	else if ( ! strcasecmp( dbType_, "PostgreSQL" ) )
		driver = ODBConnector::DRIVER::POSTGRESQL;
	else if ( ! strcasecmp( dbType_, "MySQL" ) )
		driver = ODBConnector::DRIVER::MYSQL;
	else if ( ! strcasecmp( dbType_, "Firebird" ) )
		driver = ODBConnector::DRIVER::FIREBIRD;
	else if ( ! strcasecmp( dbType_, "Oracle" ) )
		driver = ODBConnector::DRIVER::ORACLE;
	else
		throw HDataBaseException( "unsupported database type: " + dbType_ );
	return ( driver );
	M_EPILOG
}

}

namespace util {

HDataBase::ptr_t connect( yaal::hcore::HString const& dsn_ ) {
	M_PROLOG
	char const SCHEMA_SEPARATOR[] = "://";
	int dbTypeEnd( static_cast<int>( dsn_.find( SCHEMA_SEPARATOR ) ) );
	if ( dbTypeEnd == HString::npos )
		throw HDataBaseException( "bad database uri: " + dsn_ );
	HString dbType( dsn_.raw(), dbTypeEnd );
	dbType.lower();
	ODBConnector::DRIVER::enum_t driver( db_driver_from_string( dbType ) );
	HString connectionString = dsn_.substr( dbTypeEnd + static_cast<int>( sizeof ( SCHEMA_SEPARATOR ) ) - 1 );
	int userPassEnd( static_cast<int>( connectionString.find( '@' ) ) );
	HString userName;
	HString password;
	HString dbHostSpec;
	if ( userPassEnd != HString::npos ) {
		HString userPass( connectionString.raw(), userPassEnd );
		int userEnd( static_cast<int>( userPass.find( ':' ) ) );
		userName = userPass.left( userEnd );
		if ( userEnd != HString::npos )
			password = userPass.substr( userEnd + 1 );
		dbHostSpec = connectionString.substr( userPassEnd + 1 );
	} else
		dbHostSpec = connectionString;
	int hostSpecEnd( static_cast<int>( dbHostSpec.find( '/' ) ) );
	HString hostSpec;
	HString database;
	if ( hostSpecEnd != HString::npos ) {
		hostSpec = dbHostSpec.left( hostSpecEnd );
		database = dbHostSpec.substr( hostSpecEnd + 1 );
	} else
		throw HDataBaseException( "bad database uri: " + dsn_ );
	HDataBase::ptr_t db( HDataBase::get_connector( driver ) );
	db->connect( database, userName, password, hostSpec );
	return ( db );
	M_EPILOG
}

}

void dbwrapper_error( void );
void dbwrapper_exit( void ) __attribute__(( __noreturn__ ));

ODBConnector::DRIVER::enum_t _dataBaseDriver_ = ODBConnector::DRIVER::NONE;

namespace {

bool set_dbwrapper_variables( HString& option_, HString& value_ ) {
	M_PROLOG
	if ( ! strcasecmp( option_, "set_env" ) )
		decode_set_env( value_ );
	else if ( ! strcasecmp( option_, "log_mask" ) ) {
		if ( ! strcasecmp( value_, "LOG_SQL" ) )
			HLog::_logMask |= LOG_TYPE::SQL;
		else
			return ( true );
	} else if ( ! strcasecmp( option_, "data_base_driver" ) ) {
		try {
			dbwrapper::_dataBaseDriver_ = db_driver_from_string( value_ );
		} catch ( HDataBaseException const& ) {
			log( LOG_TYPE::ERROR ) << "Error: `" << value_;
			log << "' is unknown driver." << endl;
			exit( 1 );
		}
	} else
		return ( true );
	return ( false );
	M_EPILOG
}

}

class HDBWrapperInitDeinit {
public:
	HDBWrapperInitDeinit( void );
} initDeinit;

HDBWrapperInitDeinit::HDBWrapperInitDeinit( void ) {
	M_PROLOG
	yaal_options().process_rc_file( "yaal", "dbwrapper", set_dbwrapper_variables );
	return;
	M_EPILOG
}

void banner( void ) {
	::printf( "\tdbwrapper\n" );
	return;
}

}

}

#ifdef __DYNAMIC_LINKER__
extern "C"
int yaal_dbwrapper_main( int, char** ) __attribute__((noreturn));
extern "C"
int yaal_dbwrapper_main( int, char** ) {
	static char const dynamicLinkerPath[]
#ifdef HAVE_INTERP_ALLOWED
		__attribute__(( __section__(".interp") ))
#endif /* #ifdef HAVE_INTERP_ALLOWED */
		= __DYNAMIC_LINKER__;
	if ( dynamicLinkerPath[ 0 ] ) {
		yaal::hcore::banner();
		yaal::tools::banner();
		yaal::dbwrapper::banner();
		::printf( "\n" );
		::exit( 0 );
	}
	::exit( 0 );
}
#else /* #ifdef __DYNAMIC_LINKER__ */
extern "C"
int yaal_dbwrapper_main( int, char** );
extern "C"
int yaal_dbwrapper_main( int, char** ) {
	return ( 0 );
}
#endif /* #else #ifdef __DYNAMIC_LINKER__ */

