/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	xalloc.h - this file is integral part of `stdhapi' project.

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

#ifndef __HCORE_XALLOC_H
#define __HCORE_XALLOC_H

extern "C"
{

#define xmalloc( size, type ) static_cast < type * > ( xmalloc_internal ( ( size ) * static_cast < int > ( sizeof ( type ) ) ) )
void * xmalloc_internal ( const long int );
#define xcalloc( size, type ) static_cast < type * > ( xcalloc_internal ( ( size ) * static_cast < int > ( sizeof ( type ) ) ) )
void * xcalloc_internal ( const long int );
#define xrealloc( pointer, size, type ) static_cast < type * > ( xrealloc_internal ( pointer, ( size ) * static_cast < int > ( sizeof ( type ) ) ) )
void * xrealloc_internal ( void *, const long int );
#define xfree( ptr ) xfree_internal ( reinterpret_cast < void * * > ( & ptr ) )
void xfree_internal ( void * * );
char * xstrdup ( char const * );

}

#endif /* not __HCORE_XALLOC_H */
