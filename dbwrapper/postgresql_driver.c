
/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	mysql_driver.c - this file is integral part of `stdhapi' project.

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

#include <stdlib.h>
#include <string.h>

#include "../config.h"

#ifdef __PLD_HOST__
#	include <libpq-fe.h>
#else /* __PLD_HOST__ */
#	include <postgresql/libpq-fe.h>
#endif /* not __PLD_HOST__*/

#ifndef NULL
#define NULL	0
#endif /* not NULL */

extern "C"
{

void * db_connect ( const char * a_pcDataBase,
		const char * a_pcLogin, const char * a_pcPassword )
	{
	char * l_pcSockPath = NULL;
	PGconn * l_psConnection = NULL;
	l_pcSockPath = strchr ( a_pcDataBase, ':' );
	if ( l_pcSockPath )* l_pcSockPath = 0;
	l_psConnection = PQsetdbLogin ( NULL,	/* host */
																	NULL,					/* port */
																	NULL,					/* options */
																	NULL,					/* debugging tty */
																	a_pcDataBase, 
																	a_pcLogin, 
																	a_pcPassword );
	return ( l_psConnection );
	}

void db_disconnect ( void * a_pvData )
	{
	if ( a_pvData )
		PQfinish ( ( PGconn * ) a_pvData );
	return;
	}

int db_errno ( void * )
	{
	return ( 0 );
	}

const char * db_error  ( void * a_pvData )
	{
	return ( PQerrorMessage ( ( PGconn * ) a_pvData ) );
	}

void * db_query ( void * a_pvData, const char * a_pcQuery )
	{
	return ( PQexec ( ( PGconn * ) a_pvData, a_pcQuery ) );
	}

void db_unquery ( void * a_pvData )
	{
	PQclear ( ( PGresult * ) a_pvData );
	return;
	}

char * rs_get ( void * a_pvData, int a_iRow,  int a_iColumn )
	{
	return ( PQgetvalue ( ( PGresult * ) a_pvData, a_iRow, a_iColumn ) );
	}

long int rsdb_count ( void * a_pvDataR, void * = NULL )
	{
	char * l_pcTmp = PQcmdTuples ( ( PGresult * ) a_pvDataR );
	if ( l_pcTmp && l_pcTmp [ 0 ] )return ( strtol ( l_pcTmp, NULL, 10 ) );
	else return ( PQntuples ( ( PGresult * ) a_pvDataR ) );
	}

long int rsdb_id ( void * a_pvDataR, void * )
	{
	return ( PQoidValue ( ( PGresult * ) a_pvDataR ) );
	}

char * rs_column_name ( void * a_pvDataR, int a_iField )
	{
	return ( PQfname ( ( PGresult * ) a_pvDataR, a_iField ) );
	}

}
