/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	db_driver_loader.h - this file is integral part of `yaal' project.

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

#ifndef __YAAL_DBWRAPPER_DB_DRIVER_LOADER_H
#define __YAAL_DBWRAPPER_DB_DRIVER_LOADER_H

namespace yaal
{

namespace hcore
{
namespace LOG_TYPE
	{
	static int const D_SQL = 1073741824;
	}
}

namespace dbwrapper
	{

extern void * ( * db_connect ) ( char const *, char const *, char const * );
extern void ( * db_disconnect ) ( void * );
extern int ( * db_errno ) ( void * );
extern char const * ( * db_error ) ( void * );
extern void * ( * db_query ) ( void *, char const * );
extern void ( * db_unquery ) ( void * );
extern char * ( * rs_get ) ( void *, int, int );
extern int ( * rs_fields_count ) ( void * );
extern int long ( * dbrs_records_count ) ( void *, void * );
extern int long ( * dbrs_id ) ( void *, void * );
extern char * ( * rs_column_name ) ( void *, int );

void * autoloader_db_connect ( char const *, char const *, char const * );
void autoloader_db_disconnect ( void * );
int autoloader_db_errno ( void * );
char const * autoloader_db_error ( void * );
void * autoloader_db_query ( void *, char const * );
void autoloader_db_unquery ( void * );
char * autoloader_rs_get ( void *, int, int );
int autoloader_rs_fields_count ( void * );
int long autoloader_dbrs_records_count ( void *, void * );
int long autoloader_dbrs_id ( void *, void * );
char * autoloader_rs_column_name ( void *, int );

	}

}

#endif /* not __YAAL_DBWRAPPER_DB_DRIVER_LOADER_H */

