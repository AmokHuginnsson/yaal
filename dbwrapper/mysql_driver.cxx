
/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	mysql_driver.cxx - this file is integral part of `yaal' project.

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

#include "hcore/xalloc.hxx"

#include "config.hxx"

#ifdef __HOST_OS_TYPE_FREEBSD__
#	define TO_BE_INCLUDED_LATER 1
#endif /* __HOST_OS_TYPE_FREEBSD__ */

#include <mysql/mysql.h>

extern "C"
{

MYSQL* _brokenDB_ = NULL;

void db_disconnect( void* );

void* db_connect( char const* dataBase_,
		char const* login_, char const * password_ )
	{
	MYSQL* mySQL( NULL );
	if ( _brokenDB_ )
		{
		db_disconnect( _brokenDB_ );
		_brokenDB_ = NULL;
		}
	mySQL = mysql_init( NULL );
	if ( mySQL )
		{
		int unsigned protocol( MYSQL_PROTOCOL_SOCKET );
		if ( mysql_options( mySQL, MYSQL_OPT_PROTOCOL, &protocol ) )
			_brokenDB_ = mySQL, mySQL = NULL;
		else if ( ! mysql_real_connect( mySQL, NULL, login_, password_,
				dataBase_, 0, NULL, CLIENT_IGNORE_SPACE ) )
			_brokenDB_ = mySQL, mySQL = NULL;
		}
	return ( mySQL );
	}

void db_disconnect( void* data_ )
	{
	mysql_close( static_cast<MYSQL*>( data_ ) );
	return;
	}

int dbrs_errno( void* data_, void* )
	{
	if ( ! data_ )
		data_ = _brokenDB_;
	return ( ::mysql_errno( static_cast<MYSQL*>( data_ ) ) );
	}

char const* dbrs_error( void* data_, void* )
	{
	if ( ! data_ )
		data_ = _brokenDB_;
	return ( ::mysql_error( static_cast<MYSQL*>( data_ ) ) );
	}

void* db_query( void* data_, char const* query_ )
	{
	mysql_query( static_cast<MYSQL*>( data_ ), query_ );
	return ( mysql_store_result( static_cast<MYSQL*>( data_ ) ) );
	}

void rs_unquery( void* data_ )
	{
	mysql_free_result( static_cast<MYSQL_RES*>( data_ ) );
	return;
	}

char const* rs_get( void* data_, int long row_, int column_ )
	{
	MYSQL_ROW row;
	mysql_data_seek( static_cast<MYSQL_RES*>( data_ ), row_ );
	row = mysql_fetch_row( static_cast<MYSQL_RES*>( data_ ) );
	return ( row [ column_ ] );
	}

int rs_fields_count( void* data_ )
	{
	return ( ::mysql_num_fields( static_cast<MYSQL_RES*>( data_ ) ) );
	}

int long dbrs_records_count( void* dataB_, void* dataR_ )
	{
	if ( dataR_ )
		return ( static_cast<int long>( mysql_num_rows( static_cast<MYSQL_RES*>( dataR_ ) ) ) );
	else
		return ( static_cast<int long>( mysql_affected_rows( static_cast<MYSQL*>( dataB_ ) ) ) );
	}

int long dbrs_id( void* dataB_, void* )
	{
	return ( static_cast<int long>( mysql_insert_id( static_cast<MYSQL*>( dataB_ ) ) ) );
	}

char const* rs_column_name( void* dataR_, int field_ )
	{
	MYSQL_FIELD* field = NULL;
	field = mysql_fetch_field_direct( static_cast<MYSQL_RES*>( dataR_ ), field_ );
	return ( field->name );
	}

int yaal_mysql_driver_main( int, char** ) __attribute__(( __noreturn__ ));
int yaal_mysql_driver_main( int, char** )
	{
	::exit( 0 );
	}

}

namespace
{

class HMySQLInitDeinit
	{
public:
	HMySQLInitDeinit( void );
	~HMySQLInitDeinit( void );
	} mySQLInitDeinit;

HMySQLInitDeinit::HMySQLInitDeinit( void )
	{
	mysql_library_init( 0, NULL, NULL );
	return;
	}

HMySQLInitDeinit::~HMySQLInitDeinit( void )
	{
	mysql_library_end();
	return;
	}

}

