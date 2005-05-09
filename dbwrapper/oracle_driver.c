/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	null_driver.c - this file is integral part of `stdhapi' project.

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

#define __STRICT_ANSI__
#include <oci.h>

#include "hcore/xalloc.h"

#ifndef NULL
#define NULL	0
#endif /* not NULL */

#define D_TEXT_BUFFER_SIZE	512

extern "C"
{

typedef struct
	{
	int f_iStatus;
	char * f_pcTextBuffer;
	OCIEnv * f_psEnvironment;
	OCIError * f_psError;
	OCISvcCtx * f_psServiceContext;
	} OOracle;

OOracle * g_psBrokenDB = NULL;

void db_disconnect ( void * );

void * db_connect ( char const * /* In Oracle user name is name of schema. */,
		char const * a_pcLogin, char const * a_pcPassword )
	{
	OOracle * l_psOracle = NULL;
	if ( g_psBrokenDB )
		{
		db_disconnect ( g_psBrokenDB );
		g_psBrokenDB = NULL;
		}
	l_psOracle = xcalloc ( 1, OOracle );
	l_psOracle->f_pcTextBuffer = xcalloc ( D_TEXT_BUFFER_SIZE, char );
	if ( ( l_psOracle->f_iStatus = OCIEnvCreate ( & l_psOracle->f_psEnvironment,
				OCI_DEFAULT | OCI_THREADED, NULL, NULL, NULL, NULL, 0,
				NULL ) ) != OCI_SUCCESS )
		{
		g_psBrokenDB = l_psOracle;
		return ( NULL );
		}
	if ( ( l_psOracle->f_iStatus = OCIHandleAlloc ( l_psOracle->f_psEnvironment,
				reinterpret_cast < void ** > ( & l_psOracle->f_psError ),
				OCI_HTYPE_ERROR, 0, NULL ) ) != OCI_SUCCESS )
		{
		g_psBrokenDB = l_psOracle;
		return ( NULL );
		}
	if ( ( l_psOracle->f_iStatus = OCILogon ( l_psOracle->f_psEnvironment,
				l_psOracle->f_psError, & l_psOracle->f_psServiceContext,
				reinterpret_cast < const OraText * > ( a_pcLogin ),
				strlen ( a_pcLogin ),
				reinterpret_cast < const OraText * > ( a_pcPassword ),
				strlen ( a_pcPassword ),
				reinterpret_cast < const OraText * > ( "SPOON" ),
				strlen ( "SPOON" ) ) ) != OCI_SUCCESS )
		{
		g_psBrokenDB = l_psOracle;
		return ( NULL );
		}
	return ( NULL );
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
		if ( l_psOracle->f_pcTextBuffer )
			xfree ( l_psOracle->f_pcTextBuffer );
		xfree ( l_psOracle );
		}
	return;
	}

int db_errno ( void * a_pvData )
	{
	int l_iError = 0;
	OOracle * l_psOracle = static_cast < OOracle * > ( a_pvData );
	if ( ( l_psOracle->f_iStatus != OCI_SUCCESS_WITH_INFO )
			&& ( l_psOracle->f_iStatus != OCI_ERROR ) )
		return ( l_psOracle->f_iStatus );
	OCIErrorGet ( l_psOracle->f_psError, 1, NULL, & l_iError, NULL, 0,
			OCI_HTYPE_ERROR );
	return ( l_iError );
	}

char const * db_error  ( void * a_pvData )
	{
	OOracle * l_psOracle = static_cast < OOracle * > ( a_pvData );
	switch ( l_psOracle->f_iStatus )
		{
		case ( OCI_SUCCESS_WITH_INFO ):
		case ( OCI_ERROR ):
			{
			OCIErrorGet ( l_psOracle->f_psError, 1, NULL, NULL,
					reinterpret_cast < OraText * > ( l_psOracle->f_pcTextBuffer ),
					D_TEXT_BUFFER_SIZE, OCI_HTYPE_ERROR );
			break;
			}
		case ( OCI_NEED_DATA ):
			{
			return ( "OCI_NEED_DATA" );
			break;
			}
		case ( OCI_NO_DATA ):
			{
			return ( "OCI_NO_DATA" );
			break;
			}
		case ( OCI_INVALID_HANDLE ):
			{
			return ( "OCI_INVALID_HANDLE" );
			break;
			}
		case ( OCI_STILL_EXECUTING ):
			{
			return ( "OCI_STILL_EXECUTING" );
			break;
			}
		case ( OCI_CONTINUE ):
			{
			return ( "OCI_CONTINUE" );
			break;
			}
		default :
			{
			snprintf ( l_psOracle->f_pcTextBuffer, D_TEXT_BUFFER_SIZE - 2,
					"Error - %d", l_psOracle->f_iStatus );
			break;
			}
		}
	return ( l_psOracle->f_pcTextBuffer );
	}

void * db_query ( void * /*a_pvData*/, char const * /*a_pcQuery*/ )
	{
	return ( NULL );
	}

void db_unquery ( void * /*a_pvData*/ )
	{
	return;
	}

char * rs_get ( void * /*a_pvData*/, int /*a_iRow*/, int /*a_iColumn*/ )
	{
	return ( NULL );
	}

int rs_fields_count ( void * /*a_pvData*/ )
	{
	return ( 0 );
	}

long int rsdb_records_count ( void * /*a_pvDataB*/, void * /*a_pvDataR*/ )
	{
	return ( 0 );
	}

long int rsdb_id ( void * /*a_pvDataB*/, void * )
	{
	return ( 0 );
	}

char * rs_column_name ( void * /*a_pvDataR*/, int /*a_iField*/ )
	{
	return ( NULL );
	}

}

