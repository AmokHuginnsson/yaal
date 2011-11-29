/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	firebird_driver.cxx - this file is integral part of `yaal' project.

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

#include <ibase.h>

#include "dbwrapper/db_driver.hxx"

using namespace yaal::dbwrapper;

extern "C" {

M_EXPORT_SYMBOL bool db_connect( ODBLink&, char const* /*dataBase_*/,
		char const* /*login_*/, char const* /*password_*/ ) {
	return ( true );
}

M_EXPORT_SYMBOL void db_disconnect( ODBLink& /*data_*/ ) {
	return;
}

M_EXPORT_SYMBOL int dbrs_errno( ODBLink const& /*dataB_*/, void* /*dataR_*/ ) {
	return ( errno );
}

M_EXPORT_SYMBOL char const* dbrs_error( ODBLink const& /*dataB_*/, void* /*dataR_*/ ) {
	return ( "" );
}

M_EXPORT_SYMBOL void* db_query( ODBLink& /*data_*/, char const* /*query_*/ ) {
	return ( NULL );
}

M_EXPORT_SYMBOL void rs_unquery( void* /*data_*/ ) {
	return;
}

M_EXPORT_SYMBOL char const* rs_get( void* /*data_*/, int long /*row_*/, int /*column_*/ ) {
	return ( NULL );
}

M_EXPORT_SYMBOL int rs_fields_count( void* /*data_*/ ) {
	return ( 0 );
}

M_EXPORT_SYMBOL int long dbrs_records_count( ODBLink& /*dataB_*/, void* /*dataR_*/ ) {
	return ( 0 );
}

M_EXPORT_SYMBOL int long dbrs_id( ODBLink& /*dataB_*/, void* /*dataR_*/ ) {
	return ( 0 );
}

M_EXPORT_SYMBOL char const* rs_column_name( void* /*dataR_*/, int /*field_*/ ) {
	return ( NULL );
}

int yaal_firebird_driver_main( int, char** ) {
	return ( 0 );
}

}

