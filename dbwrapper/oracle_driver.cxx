/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	null_driver.c - this file is integral part of `yaal' project.

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
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h> /* strtol */

#define __STRICT_ANSI__
#include <oci.h>

#include "hcore/xalloc.h"
#include "hcore/hexception.h"
#include "hcore/hstring.h"
#include "hcore/rc_file.h"
#include "hcore/hlog.h"

using namespace yaal;
using namespace yaal::hcore;

extern "C"
{

static char const * const g_pcLogTag = "Oracle: ";

HString g_oInstanceName;
OVariable n_psVariables [ ] =
	{
		{ D_HSTRING, "instance_name", & g_oInstanceName },
		{ D_VOID, NULL, NULL }
	};

struct OAllocator
	{
	OAllocator * f_psNext;
	char * f_pcBuffer;
	};

typedef struct
	{
	int f_iStatus;
	OCIEnv * f_psEnvironment;
	OCIError * f_psError;
	OCISvcCtx * f_psServiceContext;
	} OOracle;

typedef struct
	{
	int * f_piStatus;
	OCIError * f_psError;
	OCIStmt * f_psStatement;
	OAllocator * f_psAllocator;
	} OQuery;

OOracle * g_psBrokenDB = NULL;

void db_disconnect ( void * );
void db_unquery ( void * );

void * db_connect ( char const * /* In Oracle user name is name of schema. */,
		char const * a_pcLogin, char const * a_pcPassword )
	{
	OOracle * l_psOracle = NULL;
	if ( g_psBrokenDB )
		{
		db_disconnect ( g_psBrokenDB );
		g_psBrokenDB = NULL;
		}
	l_psOracle = xcalloc < OOracle > ( 1 );
	if ( ( l_psOracle->f_iStatus = OCIInitialize ( OCI_DEFAULT, NULL, NULL,
					NULL, NULL ) ) != OCI_SUCCESS )
		{
		g_psBrokenDB = l_psOracle;
		return ( NULL );
		}
	if ( ( l_psOracle->f_iStatus = OCIEnvCreate ( & l_psOracle->f_psEnvironment,
				OCI_DEFAULT | OCI_THREADED, NULL, NULL, NULL, NULL, 0,
				NULL ) ) != OCI_SUCCESS )
		{
		g_psBrokenDB = l_psOracle;
		return ( NULL );
		}
	if ( ( l_psOracle->f_iStatus = OCIHandleAlloc ( l_psOracle->f_psEnvironment,
				reinterpret_cast < void * * > ( & l_psOracle->f_psError ),
				OCI_HTYPE_ERROR, 0, NULL ) ) != OCI_SUCCESS )
		{
		g_psBrokenDB = l_psOracle;
		return ( NULL );
		}
	if ( ( l_psOracle->f_iStatus = OCILogon ( l_psOracle->f_psEnvironment,
				l_psOracle->f_psError, & l_psOracle->f_psServiceContext,
				reinterpret_cast < OraText const * > ( a_pcLogin ),
				strlen ( a_pcLogin ),
				reinterpret_cast < OraText const * > ( a_pcPassword ),
				strlen ( a_pcPassword ),
				reinterpret_cast < OraText const * > ( static_cast < char const * > ( g_oInstanceName ) ),
				g_oInstanceName.get_length ( ) ) ) != OCI_SUCCESS )
		{
		g_psBrokenDB = l_psOracle;
		return ( NULL );
		}
	return ( l_psOracle );
	}

void db_disconnect ( void * a_pvData )
	{
	OOracle * l_psOracle = static_cast < OOracle * > ( a_pvData );
	if ( ! l_psOracle )
		l_psOracle = g_psBrokenDB;
	if ( l_psOracle )
		{
		if ( l_psOracle->f_psServiceContext )
			OCILogoff ( l_psOracle->f_psServiceContext, l_psOracle->f_psError );
		l_psOracle->f_psServiceContext = NULL;
		if ( l_psOracle->f_psError )
			OCIHandleFree ( l_psOracle->f_psError, OCI_HTYPE_ERROR );
		l_psOracle->f_psError = NULL;
		if ( l_psOracle->f_psEnvironment )
			OCIHandleFree ( l_psOracle->f_psEnvironment, OCI_HTYPE_ENV );
		l_psOracle->f_psEnvironment = NULL;
		xfree ( l_psOracle );
		}
	return;
	}

int db_errno ( void * a_pvData )
	{
	int l_iError = 0;
	OOracle * l_psOracle = NULL;
	if ( ! a_pvData )
		a_pvData = g_psBrokenDB;
	if ( ! a_pvData )
		return ( 0 );
	l_psOracle = static_cast < OOracle * > ( a_pvData );
	if ( ( l_psOracle->f_iStatus != OCI_SUCCESS_WITH_INFO )
			&& ( l_psOracle->f_iStatus != OCI_ERROR ) )
		return ( l_psOracle->f_iStatus );
	OCIErrorGet ( l_psOracle->f_psError, 1, NULL, & l_iError, NULL, 0,
			OCI_HTYPE_ERROR );
	return ( l_iError );
	}

char const * db_error  ( void * a_pvData )
	{
	sb4 code = 0;
	static char l_pcTextBuffer [ OCI_ERROR_MAXMSG_SIZE ];
	OOracle * l_psOracle = NULL;
	if ( ! a_pvData )
		a_pvData = g_psBrokenDB;
	if ( ! a_pvData )
		return ( "fatal" );
	l_psOracle = static_cast < OOracle * > ( a_pvData );
	switch ( l_psOracle->f_iStatus )
		{
		case ( OCI_SUCCESS_WITH_INFO ):
		case ( OCI_ERROR ):
			OCIErrorGet ( l_psOracle->f_psError, 1, NULL, & code,
					reinterpret_cast < OraText * > ( l_pcTextBuffer ),
					OCI_ERROR_MAXMSG_SIZE - 2, OCI_HTYPE_ERROR );
		break;
		case ( OCI_NEED_DATA ):
			return ( "OCI_NEED_DATA" );
		case ( OCI_NO_DATA ):
			return ( "OCI_NO_DATA" );
		case ( OCI_INVALID_HANDLE ):
			return ( "OCI_INVALID_HANDLE" );
		case ( OCI_STILL_EXECUTING ):
			return ( "OCI_STILL_EXECUTING" );
		case ( OCI_CONTINUE ):
			return ( "OCI_CONTINUE" );
		default:
			snprintf ( l_pcTextBuffer, OCI_ERROR_MAXMSG_SIZE - 2,
					"Error - %d", l_psOracle->f_iStatus );
		}
	return ( l_pcTextBuffer );
	}

void * db_query ( void * a_pvData, char const * a_pcQuery )
	{
	OOracle * l_psOracle = static_cast < OOracle * > ( a_pvData );
	OQuery * l_psQuery = xcalloc < OQuery > ( 1 );
	HString l_oQuery = a_pcQuery;
	int l_iIters = 0;
	int l_iLength = strlen ( a_pcQuery );
	char * l_pcEnd = ( const_cast < char * > ( a_pcQuery ) + l_iLength ) - 1;
	
	if ( ( * l_pcEnd ) == ';' )
		( * l_pcEnd ) = 0;
	l_psOracle->f_iStatus = OCIStmtPrepare2 ( l_psOracle->f_psServiceContext,
			& l_psQuery->f_psStatement, l_psOracle->f_psError,
			reinterpret_cast < const OraText * > ( a_pcQuery ),
			strlen ( a_pcQuery ), NULL, 0, OCI_NTV_SYNTAX, OCI_DEFAULT );
	l_psQuery->f_piStatus = & l_psOracle->f_iStatus;
	l_psQuery->f_psError = l_psOracle->f_psError;
	if ( ( l_psOracle->f_iStatus != OCI_SUCCESS )
			&& ( l_psOracle->f_iStatus != OCI_SUCCESS_WITH_INFO ) )
		{
		log ( LOG_TYPE::D_ERROR ) << g_pcLogTag << __FUNCTION__ << ": failed to prepare statement." << endl;
		db_unquery ( l_psQuery );
		l_psQuery = NULL;
		}
	else
		{
		if ( l_psOracle->f_iStatus == OCI_SUCCESS_WITH_INFO )
			log ( LOG_TYPE::D_INFO ) << g_pcLogTag <<  __FUNCTION__ << ": " << db_error ( l_psOracle ) << endl;
		l_oQuery.upper ( );
		if ( l_oQuery.find ( "INSERT" ) == 0 )
			l_iIters = 1;
		else if ( l_oQuery.find ( "UPDATE" ) == 0 )
			l_iIters = 1;
		else if ( l_oQuery.find ( "DELETE" ) == 0 )
			l_iIters = 1;
		l_psOracle->f_iStatus = OCIStmtExecute ( l_psOracle->f_psServiceContext,
				l_psQuery->f_psStatement, l_psOracle->f_psError, l_iIters, 0,
				NULL, NULL,
				OCI_DEFAULT | OCI_COMMIT_ON_SUCCESS | OCI_STMT_SCROLLABLE_READONLY );
		if ( ( l_psOracle->f_iStatus != OCI_SUCCESS )
				&& ( l_psOracle->f_iStatus != OCI_SUCCESS_WITH_INFO ) )
			{
			log ( LOG_TYPE::D_ERROR ) << g_pcLogTag << __FUNCTION__ << ": failed to execute statement." << endl;
			db_unquery ( l_psQuery );
			l_psQuery = NULL;
			}
		else if ( l_psOracle->f_iStatus == OCI_SUCCESS_WITH_INFO )
			log ( LOG_TYPE::D_INFO ) << g_pcLogTag <<  __FUNCTION__ << ": " << db_error ( l_psOracle ) << endl;
		}
	return ( l_psQuery );
	}

void db_unquery ( void * a_pvData )
	{
	OAllocator * l_psAllocator = NULL;
	OQuery * l_psQuery = static_cast < OQuery * > ( a_pvData );
	if ( ( ( * l_psQuery->f_piStatus ) == OCI_SUCCESS )
			|| ( ( * l_psQuery->f_piStatus ) == OCI_SUCCESS_WITH_INFO ) )
		( * l_psQuery->f_piStatus ) = OCIStmtRelease ( l_psQuery->f_psStatement,
				l_psQuery->f_psError, NULL, 0, OCI_DEFAULT );
	else
		OCIStmtRelease ( l_psQuery->f_psStatement,
				NULL, NULL, 0, OCI_DEFAULT );
	l_psAllocator = l_psQuery->f_psAllocator;
	while ( l_psAllocator )
		{
		l_psAllocator = l_psQuery->f_psAllocator->f_psNext;
		xfree ( l_psQuery->f_psAllocator );
		l_psQuery->f_psAllocator = l_psAllocator;
		}
	xfree ( l_psQuery );
	return;
	}

char * rs_get ( void * a_pvData, int a_iRow, int a_iColumn )
	{
	int l_iSize = 0;
	char * l_pcData = NULL;
	OAllocator * l_psAllocator;
	OCIParam * l_psParameter = NULL;
	OCIDefine * l_psResult = NULL;
	OQuery * l_psQuery = static_cast < OQuery * > ( a_pvData );
	if ( ( ( * l_psQuery->f_piStatus ) = OCIParamGet ( l_psQuery->f_psStatement,
					OCI_HTYPE_STMT, l_psQuery->f_psError,
					reinterpret_cast < void * * > ( & l_psParameter ), a_iColumn + 1 ) ) == OCI_SUCCESS )
		{
		if ( ( ( * l_psQuery->f_piStatus ) = OCIAttrGet ( l_psParameter,
						OCI_DTYPE_PARAM, & l_iSize, 0, OCI_ATTR_DATA_SIZE,
						l_psQuery->f_psError ) ) == OCI_SUCCESS )
			{
			l_pcData = xcalloc < char > ( l_iSize + 1 );
			if ( ( ( * l_psQuery->f_piStatus ) = OCIDefineByPos ( l_psQuery->f_psStatement,
							& l_psResult, l_psQuery->f_psError, a_iColumn + 1, l_pcData, l_iSize + 1,
							SQLT_STR, NULL, NULL, NULL, OCI_DEFAULT ) ) == OCI_SUCCESS )
				{
				if ( ( ( * l_psQuery->f_piStatus ) = OCIStmtFetch2 ( l_psQuery->f_psStatement,
								l_psQuery->f_psError, 1, OCI_FETCH_ABSOLUTE, a_iRow,
								OCI_DEFAULT ) ) == OCI_SUCCESS )
					{
					l_psAllocator = xcalloc < OAllocator > ( 1 );
					l_psAllocator->f_pcBuffer = l_pcData;
					if ( l_psQuery->f_psAllocator )
						l_psQuery->f_psAllocator->f_psNext = l_psAllocator;
					else
						l_psQuery->f_psAllocator = l_psAllocator;
					}
				else
					xfree ( l_pcData );
				}
			}
		}
	if ( l_psParameter )
		OCIDescriptorFree ( l_psParameter, OCI_DTYPE_PARAM );
	return ( NULL );
	}

int rs_fields_count ( void * a_pvData )
	{
	int l_iFields = - 1;
	OQuery * l_psQuery = static_cast < OQuery * > ( a_pvData );
	if ( ( ( * l_psQuery->f_piStatus ) = OCIAttrGet ( l_psQuery->f_psStatement,
					OCI_HTYPE_STMT, & l_iFields, 0, OCI_ATTR_PARAM_COUNT,
					l_psQuery->f_psError ) ) != OCI_SUCCESS )
		l_iFields = - 1;
	return ( l_iFields );
	}

int long dbrs_records_count ( void *, void * a_pvDataR )
	{
	int l_iRows = 0;
	OQuery * l_psQuery = static_cast < OQuery * > ( a_pvDataR );
	( * l_psQuery->f_piStatus ) = OCIStmtFetch2 ( l_psQuery->f_psStatement,
																								 l_psQuery->f_psError, 1,
																								 OCI_FETCH_LAST, 0,
																								 OCI_DEFAULT );
	if ( ( ( * l_psQuery->f_piStatus ) != OCI_SUCCESS )
			&& ( ( * l_psQuery->f_piStatus ) != OCI_SUCCESS_WITH_INFO ) )
		{
		log ( LOG_TYPE::D_ERROR ) << g_pcLogTag << __FUNCTION__ << ": failed to fetch last row." << endl;
		return ( - 1 );
		}
	if ( ( ( * l_psQuery->f_piStatus ) = OCIAttrGet ( l_psQuery->f_psStatement,
					OCI_HTYPE_STMT, & l_iRows, 0, OCI_ATTR_CURRENT_POSITION,
					l_psQuery->f_psError ) ) != OCI_SUCCESS )
		{
		if ( ( ( * l_psQuery->f_piStatus ) = OCIAttrGet ( l_psQuery->f_psStatement,
						OCI_HTYPE_STMT, & l_iRows, 0, OCI_ATTR_ROW_COUNT,
						l_psQuery->f_psError ) ) != OCI_SUCCESS )
			l_iRows = - 1;
		}
	return ( l_iRows );
	}

int long dbrs_id ( void *, void * a_pvDataR )
	{
	int l_iNameLength = 0;
	int long l_lId = 0;
	HString l_oSQL;
	text * l_pcName = NULL;
	OQuery * l_psQuery = static_cast < OQuery * > ( a_pvDataR );
	OQuery * l_psAutonumber = NULL;
	if ( ( ( * l_psQuery->f_piStatus ) = OCIAttrGet ( l_psQuery->f_psStatement,
					OCI_HTYPE_STMT, & l_pcName,
					reinterpret_cast < ub4 * > ( & l_iNameLength ),
					OCI_ATTR_NAME, l_psQuery->f_psError ) ) == OCI_SUCCESS )
		{
		l_pcName [ l_iNameLength ] = 0;
		l_oSQL.format ( "SELECT %s_sequence.currval FROM dual;",
				reinterpret_cast < char * > ( l_pcName ) );
		l_psAutonumber = static_cast < OQuery * > ( db_query ( l_psAutonumber, l_oSQL ) );
		l_lId = strtol ( rs_get ( l_psAutonumber, 0, 0 ), NULL, 10 );
		db_unquery ( l_psAutonumber );
		}
	return ( l_lId );
	}

char * rs_column_name ( void * a_pvDataR, int a_iField )
	{
	int l_iNameLength = 0;
	text * l_pcName = NULL;
	OCIParam * l_psParameter = NULL;
	OQuery * l_psQuery = static_cast < OQuery * > ( a_pvDataR );
	if ( ( ( * l_psQuery->f_piStatus ) = OCIParamGet ( l_psQuery->f_psStatement,
					OCI_HTYPE_STMT, l_psQuery->f_psError,
					reinterpret_cast < void * * > ( & l_psParameter ), a_iField + 1 ) ) == OCI_SUCCESS )
		{
		if ( ( ( * l_psQuery->f_piStatus ) = OCIAttrGet ( l_psParameter,
						OCI_DTYPE_PARAM, & l_pcName,
						reinterpret_cast < ub4 * > ( & l_iNameLength ),
						OCI_ATTR_NAME, l_psQuery->f_psError ) ) == OCI_SUCCESS )
			{
			if ( l_iNameLength >= 0 )
				l_pcName [ l_iNameLength ] = 0;
			}
		}
	if ( l_psParameter )
		OCIDescriptorFree ( l_psParameter, OCI_DTYPE_PARAM );
	return ( reinterpret_cast < char * > ( l_pcName ) );
	}

void oracle_init ( void ) __attribute__((__constructor__));
void oracle_init ( void )
	{
	rc_file::process_rc_file ( "yaal", "oracle", n_psVariables, NULL );
	return;
	}

}

