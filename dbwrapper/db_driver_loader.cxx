/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

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
#include "db_driver_loader.h"
#include "dbwrapper.h"

using namespace yaal::hcore;

namespace yaal
{

namespace dbwrapper
{

namespace
{

char const * const etag = "Error: Data base request (";
char const * const eend = ") while no driver loaded.";

typedef void ( * simple_function_ptr_t ) ( void );
typedef union
	{
	void * f_pvObjectPointer;
	simple_function_ptr_t FUNCTION_POINTER;
	} caster_t;

}

template < typename tType >
tType dlsym_wrapper ( void * a_pvSpace, char const * a_pcName )
	{
	caster_t l_xCaster;
	l_xCaster.f_pvObjectPointer = dlsym ( a_pvSpace, a_pcName );
	return ( reinterpret_cast < tType > ( l_xCaster.FUNCTION_POINTER ) );
	}

char const x_tag_g_pcDone [ ] = "done.\r\n", * g_pcDone = x_tag_g_pcDone;

static char const * g_ppcDriver [ 7 ] =
	{
	NULL,
	"libsqlite3_driver.so",
	"libsqlite_driver.so",
	"libmysql_driver.so",
	"libpostgresql_driver.so",
	"liboracle_driver.so",
	NULL
	};

	
void * n_pvDlHandle = NULL;

typedef void * ( * t0 ) ( char const *, char const *, char const * );
void * ( * db_connect ) ( char const *, char const *, char const * );

typedef void ( * t1 ) ( void * );
void ( * db_disconnect ) ( void * );

typedef int ( * t2 ) ( void * );
int ( * db_errno ) ( void * );

typedef char const * ( * t3 ) ( void * );
char const * ( * db_error ) ( void * );

typedef void * ( * t4 ) ( void *, char const * );
void * ( * db_query ) ( void *, char const * );

typedef void ( * t5 ) ( void * );
void ( * db_unquery ) ( void * );

typedef char * ( * t6 ) ( void *, int, int );
char * ( * rs_get ) ( void *, int, int );

typedef int ( * t7 ) ( void * );
int ( * rs_fields_count ) ( void * );

typedef int long ( * t8 ) ( void *, void * );
int long ( * dbrs_records_count ) ( void *, void * );

typedef int long ( * t9 ) ( void *, void * );
int long ( * dbrs_id ) ( void *, void * );

typedef char * ( * tA ) ( void *, int );
char * ( * rs_column_name ) ( void *, int );

/* Null driver */

void * null_db_connect ( char const *, char const *, char const * )
	{
	log ( LOG_TYPE::D_ERROR ) << etag << "(db_connect)" << eend << endl;
	return ( NULL );
	}

void null_db_disconnect ( void * )
	{
	log ( LOG_TYPE::D_ERROR ) << etag << "(db_disconnect)" << eend << endl;
	return;
	}

int null_db_errno ( void * )
	{
	log ( LOG_TYPE::D_ERROR ) << etag << "(db_errno)" << eend << endl;
	return ( 0 );
	}

char const * null_db_error ( void * )
	{
	log ( LOG_TYPE::D_ERROR ) << etag << "db_error)" << eend << endl;
	return ( _ ( "null database driver loaded" ) );
	}

void * null_db_query ( void *, char const * )
	{
	log ( LOG_TYPE::D_ERROR ) << etag << "(db_query)" << eend << endl;
	return ( NULL );
	}

void null_db_unquery ( void * )
	{
	log ( LOG_TYPE::D_ERROR ) << etag << "(db_unquery)" << eend << endl;
	return;
	}

char * null_rs_get ( void *, int, int )
	{
	log ( LOG_TYPE::D_ERROR ) << etag << "(rs_get)" << eend << endl;
	return ( NULL );
	}

int null_rs_fields_count ( void * )
	{
	log ( LOG_TYPE::D_ERROR ) << etag << "(rs_fields_count)" << eend << endl;
	return ( 0 );
	}

int long null_dbrs_records_count ( void *, void * )
	{
	log ( LOG_TYPE::D_ERROR ) << etag << "(dbrs_records_count)" << eend << endl;
	return ( 0 );
	}

int long null_dbrs_id ( void *, void * )
	{
	log ( LOG_TYPE::D_ERROR ) << etag << "(dbrs_id)" << eend << endl;
	return ( 0 );
	}

char * null_rs_column_name ( void *, int )
	{
	log ( LOG_TYPE::D_ERROR ) << etag << "(rs_column_name)" << eend << endl;
	return ( NULL );
	}

/* end of null-dummy driver */

void dbwrapper_error ( void )
	{
	HString l_oMessage;
	l_oMessage = dlerror();
	log ( LOG_TYPE::D_ERROR ) << l_oMessage << endl;
	fprintf ( stderr, "(%s), ", static_cast < char const * const > ( l_oMessage ) );
	return;
	}

void dbwrapper_exit ( void ) __attribute__  ((noreturn));
void dbwrapper_exit ( void )
	{
	fprintf ( stderr, "failed.\r\n" );
	exit ( 1 );
	}

void load_driver ( void )
	{
	M_PROLOG
	int l_iCtr = 0;
	errno = 0;
	fprintf ( stderr, "Loading dynamic database driver ... " );
	if ( dbwrapper::n_iDataBaseDriver )
		{
		l_iCtr = dbwrapper::n_iDataBaseDriver;
		n_pvDlHandle = dlopen ( g_ppcDriver [ l_iCtr ++ ], RTLD_NOW | RTLD_GLOBAL );
		if ( ! n_pvDlHandle )
			l_iCtr = 1;
		while ( ! n_pvDlHandle && g_ppcDriver [ l_iCtr ] )
			{
			if ( ( l_iCtr == dbwrapper::n_iDataBaseDriver ) && l_iCtr ++ )
				continue;
			dbwrapper_error();
			n_pvDlHandle = dlopen ( g_ppcDriver [ l_iCtr ++ ],
					RTLD_NOW | RTLD_GLOBAL );
			}
		if ( ! n_pvDlHandle )
			dbwrapper_exit();
		else
			{
			log ( LOG_TYPE::D_NOTICE ) << "Loading [" << g_ppcDriver [ l_iCtr - 1 ];
			log << "] driver." << endl;
			}
		fprintf ( stderr, x_tag_g_pcDone );
		fprintf ( stderr, "Linking symbols ... " );
		if ( ! ( db_disconnect = dlsym_wrapper < t1 > ( n_pvDlHandle,
						"db_disconnect" ) ) )
			dbwrapper_error();
		else if ( ! ( db_errno = dlsym_wrapper < t2 > ( n_pvDlHandle,
						"db_errno" ) ) )
			dbwrapper_error();
		else if ( ! ( db_error = dlsym_wrapper < t3 > ( n_pvDlHandle,
						"db_error" ) ) )
			dbwrapper_error();
		else if ( ! ( db_query = dlsym_wrapper < t4 > ( n_pvDlHandle,
						"db_query" ) ) )
			dbwrapper_error();
		else if ( ! ( db_unquery = dlsym_wrapper < t5 > ( n_pvDlHandle,
						"db_unquery" ) ) )
			dbwrapper_error();
		else if ( ! ( rs_get = dlsym_wrapper < t6 > ( n_pvDlHandle, "rs_get" ) ) )
			dbwrapper_error();
		else if ( ! ( rs_fields_count = dlsym_wrapper < t7 > ( n_pvDlHandle,
						"rs_fields_count" ) ) )
			dbwrapper_error();
		else if ( ! ( dbrs_records_count = dlsym_wrapper < t8 > ( n_pvDlHandle,
						"dbrs_records_count" ) ) )
			dbwrapper_error();
		else if ( ! ( dbrs_id = dlsym_wrapper < t9 > ( n_pvDlHandle, "dbrs_id" ) ) )
			dbwrapper_error();
		else if ( ! ( rs_column_name = dlsym_wrapper < tA > ( n_pvDlHandle,
						"rs_column_name" ) ) )
			dbwrapper_error();
		else if ( ! ( db_connect = dlsym_wrapper < t0 > ( n_pvDlHandle,
						"db_connect" ) ) )
			dbwrapper_error();
		if ( db_connect != autoloader_db_connect )
			fprintf ( stderr, x_tag_g_pcDone );
		else
			M_THROW ( _ ( "can not load database driver" ), n_iDataBaseDriver );
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

void * autoloader_db_connect ( char const * a_pcDataBase,
		char const * a_pcLogin, char const * a_pcPassword )
	{
	load_driver();
	return ( db_connect ( a_pcDataBase, a_pcLogin, a_pcPassword ) );
	}

void autoloader_db_disconnect ( void * a_pvDataBase )
	{
	load_driver();
	db_disconnect ( a_pvDataBase );
	return;
	}

int autoloader_db_errno ( void * a_pvDataBase )
	{
	load_driver();
	return ( db_errno ( a_pvDataBase ) );
	}

char const * autoloader_db_error ( void * a_pvDataBase )
	{
	load_driver();
	return ( db_error ( a_pvDataBase ) );
	}

void * autoloader_db_query ( void * a_pvDataBase, char const * a_pcQuery )
	{
	load_driver();
	return ( db_query ( a_pvDataBase, a_pcQuery ) );
	}

void autoloader_db_unquery ( void * a_pvDataBase )
	{
	load_driver();
	db_unquery ( a_pvDataBase );
	return;
	}

char * autoloader_rs_get ( void * a_pvResult, int a_iRow, int a_iCol )
	{
	load_driver();
	return ( rs_get ( a_pvResult, a_iRow, a_iCol ) );
	}

int autoloader_rs_fields_count ( void * a_pvResult )
	{
	load_driver();
	return ( rs_fields_count ( a_pvResult ) );
	}

int long autoloader_dbrs_records_count ( void * a_pvDataBase,
		void * a_pvResult )
	{
	load_driver();
	return ( dbrs_records_count ( a_pvDataBase, a_pvResult ) );
	}

int long autoloader_dbrs_id ( void * a_pvDataBase, void * a_pvResult )
	{
	load_driver();
	return ( dbrs_id ( a_pvDataBase, a_pvResult ) );
	}

char * autoloader_rs_column_name ( void * a_pvResult, int a_iColumn )
	{
	load_driver();
	return ( rs_column_name ( a_pvResult, a_iColumn ) );
	}

/* end of driver autoloader section */

} /* namespace dbwrapper */

} /* namespace yaal */

