/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	compat.h - this file is integral part of `yaal' project.

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

/* compatibility header for various platforms */

#ifndef __YAAL_HCORE_COMPAT_H
#define __YAAL_HCORE_COMPAT_H

#ifdef __YAAL_BUILD__
#include "config.h"
#endif /* __YAAL_BUILD__ */

#if not defined( HAVE_DECL_TEMP_FAILURE_RETRY ) || ( HAVE_DECL_TEMP_FAILURE_RETRY == 0 )
#define TEMP_FAILURE_RETRY(x) (x)
#endif /* not HAVE_DECL_TEMP_FAILURE_RETRY */

#if not defined( HAVE_MEMRCHR ) || ( HAVE_MEMRCHR == 0 )
#define memrchr( s, c, n ) strrchr( ( s ), ( c ) )
#endif /* not HAVE_MEMRCHR */

#endif /* not __YAAL_HCORE_COMPAT_H */
