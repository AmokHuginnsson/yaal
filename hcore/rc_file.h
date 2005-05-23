/*
---         `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski           ---

	rc_file.h - this file is integral part of `stdhapi' project.

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

#ifndef __HCORE_RC_FILE_H
#define __HCORE_RC_FILE_H

#include <stdio.h>

#include "hstring.h"

namespace stdhapi
{

namespace hcore
{

struct OVariable
	{
	int f_iType;
	char const * f_pcKey;
	void * f_pvValue;
	};

namespace rc_file
	{

#define process_rc_file( filename, section, data, callback ) \
	process_rc_file_internal ( filename, section, data, \
			( sizeof ( data ) / sizeof ( OVariable ) ) - 1, callback )
int process_rc_file_internal ( char const *, char const *,
		OVariable const *, const int, bool ( * ) ( HString &, HString & ) = NULL );
int read_rc_line ( HString &, HString &, FILE *, int & );
FILE * rc_open ( char const *, bool, FILE * = NULL );
void rc_close ( FILE * );
void rc_set_variable ( char const *, bool & );
void rc_set_variable ( char const *, char & );
void rc_set_variable ( char const *, char ** );
void rc_set_variable ( char const *, int & );

	}

}

}

#endif /* not __HCORE_RC_FILE_H */
