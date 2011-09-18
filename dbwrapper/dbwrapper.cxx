/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	dbwrapper.cxx - this file is integral part of `yaal' project.

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

#include "config.hxx"

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
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

void dbwrapper_error( void );
void dbwrapper_exit( void ) __attribute__(( __noreturn__ ));

ODBConnector::DRIVER::enum_t _dataBaseDriver_ = ODBConnector::DRIVER::NONE;

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
		if ( ! ( strcasecmp( value_, "none" )
					&& strcasecmp( value_, "null" )
					&& strcasecmp( value_, "dummy" ) ) )
			dbwrapper::_dataBaseDriver_ = ODBConnector::DRIVER::NONE;
		else if ( value_ == "SQLite3" )
			dbwrapper::_dataBaseDriver_ = ODBConnector::DRIVER::SQLITE3;
		else if ( value_ == "PostgreSQL" )
			dbwrapper::_dataBaseDriver_ = ODBConnector::DRIVER::POSTGRESQL;
		else if ( value_ == "MySQL" )
			dbwrapper::_dataBaseDriver_ = ODBConnector::DRIVER::MYSQL;
		else if ( value_ == "Oracle" )
			dbwrapper::_dataBaseDriver_ = ODBConnector::DRIVER::ORACLE;
		else {
			log( LOG_TYPE::ERROR ) << "Error: `" << value_;
			log << "' is unknown driver." << endl;
			exit( 1 );
		}
	} else
		return ( true );
	return ( false );
	M_EPILOG
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

extern char const* _done_;

void banner( void ) {
	::printf( "\tdbwrapper\n" );
	return;
}

}

}

extern "C"
int yaal_dbwrapper_main( int, char** ) {
	static char const dynamicLinkerPath[]
		__attribute__(( __section__(".interp") )) = __DYNAMIC_LINKER__;
	if ( dynamicLinkerPath[ 0 ] ) {
		yaal::hcore::banner();
		yaal::tools::banner();
		yaal::dbwrapper::banner();
		::exit( 0 );
	}
	return ( 0 );
}

