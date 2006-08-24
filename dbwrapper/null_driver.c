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

#include "hcore/xalloc.h"

extern "C"
{

void db_disconnect ( void * );

void * db_connect ( char const * /*a_pcDataBase*/
		char const * /*a_pcLogin*/ char const * /*a_pcPassword*/)
	{
	return ( NULL );
	}

void db_disconnect ( void * /*a_pvData*/ )
	{
	return;
	}

int db_errno ( void * /*a_pvData*/ )
	{
	return ( errno );
	}

char const * dbrs_error  ( void * /*a_pvDataB*/ void * /*a_pvDataR*/)
	{
	return ( "" );
	}

void * dbrs_query ( void * /*a_pvData*/ char const * /*a_pcQuery*/)
	{
	return ( NULL );
	}

void db_unquery ( void * /*a_pvData*/)
	{
	return;
	}

char * rs_get ( void * /*a_pvData*/ int /*a_iRow*/ int /*a_iColumn*/)
	{
	return ( NULL );
	}

int rs_fields_count ( void * /*a_pvData*/)
	{
	return ( 0 );
	}

int long dbrs_records_count ( void * /*a_pvDataB*/ void * /*a_pvDataR*/)
	{
	return ( 0 );
	}

int long dbrs_id ( void * /*a_pvDataB*/ void * /*a_pvDataR*/)
	{
	return ( 0 );
	}

char * rs_column_name ( void * /*a_pvDataR*/ int /*a_iField*/)
	{
	return ( NULL );
	}

}

