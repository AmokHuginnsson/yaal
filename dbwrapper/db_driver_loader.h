
/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	db_driver_loader.h - this file is integral part of `stdhapi' project.

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

#ifndef __DB_DRIVER_LOADER_H
#define __DB_DRIVER_LOADER_H

#ifndef NULL
#define NULL	0
#endif /* not NULL */

#define D_MYSQL				1
#define D_POSTGRESQL	2

namespace dbwrapper
	{

extern bool	n_bLogSQL;
extern int	n_iDataBaseDriver;

extern void * ( * db_connect ) ( const char *, const char *, const char * );
extern void ( * db_disconnect ) ( void * );
extern int ( * db_errno ) ( void * );
extern const char * ( * db_error ) ( void * );
extern void * ( * db_query ) ( void *, const char * );
extern void ( * db_unquery ) ( void * );
extern char * ( * rs_get ) ( void *, int, int );
extern long int ( * rsdb_count ) ( void *, void * = NULL );
extern long int ( * rsdb_id ) ( void *, void * );
extern char * ( * rs_column_name ) ( void *, int );

	}

#endif /* __DB_DRIVER_LOADER_H */
