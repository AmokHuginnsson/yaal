/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hpool.h - this file is integral part of `stdhapi' project.

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

#ifndef __HPOOL_H
#define __HPOOL_H

#line 31

#include <stddef.h>
#include <string.h>

#define D_CVSID_HPOOL_H "$CVSHeader$"

#define E_HPOOL_BADSIZE				0
#define E_HPOOL_NOMEM					1
#define E_HPOOL_REALLOC_FIXED	2
#define E_HPOOL_BADINDEX			3

typedef enum
	{
	D_HPOOL_FIXED_SIZE,
	D_HPOOL_AUTO_GROW,
	D_HPOOL_DUMB
	} pool_type_t;

extern const char * g_ppcErrMsgHPool [ ];

template < class tType >
class HPool
	{
private:
	/*{*/
	pool_type_t f_ePoolType;
	size_t f_lPoolSize;	/* size of allocated memory buffer */
	int f_iTop;
	/*}*/
protected:
	/*{*/
	tType * f_ptPool;	/* pointer to allocated memory pool */
	/*}*/
public:
	/*{*/
	HPool ( size_t, pool_type_t = D_HPOOL_FIXED_SIZE );
	virtual ~HPool ( void );
	size_t pool_realloc ( size_t );
	tType & operator [ ] ( int );
	tType & add ( tType & );
	void reset ( void );
	/*}*/
	};

#include "xalloc.h"

template < class tType >
HPool < tType >::HPool ( size_t a_lNewSize, pool_type_t a_ePoolType )
	{
	M_PROLOG
	f_lPoolSize = 0;
	f_ptPool = NULL;
	f_ePoolType = a_ePoolType;
	f_iTop = 0;
	if ( a_lNewSize )
		pool_realloc ( a_lNewSize );
	return;
	M_EPILOG
	}

template < class tType >
HPool < tType >::~HPool ( void )
	{
	M_PROLOG
	if ( f_ptPool )
		xfree ( f_ptPool );
	f_lPoolSize = 0;
	return;
	M_EPILOG
	}

template < class tType >
size_t HPool < tType >::pool_realloc ( const size_t a_lNewSize )
	{
	M_PROLOG
	size_t l_lOldSize = f_lPoolSize;
	if ( a_lNewSize < 1 )
		M_THROW ( g_ppcErrMsgHPool [ E_HPOOL_BADSIZE ], a_lNewSize );
	if ( f_ePoolType == D_HPOOL_AUTO_GROW )
		{
		if ( a_lNewSize > f_lPoolSize )
			{
			f_lPoolSize = 1;
			while ( f_lPoolSize < a_lNewSize )
				f_lPoolSize <<= 1;
			f_ptPool = ( tType * ) xrealloc ( f_ptPool,
					f_lPoolSize * sizeof ( tType ) );
			memset ( f_ptPool + l_lOldSize, 0,
					( f_lPoolSize - l_lOldSize ) * sizeof ( tType ) );
			}
		}
	else if ( f_lPoolSize != a_lNewSize )
		{
		if ( f_ptPool && ( f_ePoolType == D_HPOOL_FIXED_SIZE ) )
			M_THROW ( g_ppcErrMsgHPool [ E_HPOOL_REALLOC_FIXED ], f_lPoolSize );
		f_lPoolSize = a_lNewSize;
		f_ptPool = ( tType * ) xrealloc ( f_ptPool,
				f_lPoolSize * sizeof ( tType ) );
		}
	return ( f_lPoolSize - l_lOldSize );
	M_EPILOG
	}

template < class tType >
tType & HPool < tType >::operator [ ] ( int a_iIndex )
	{
	M_PROLOG
	if ( ( a_iIndex < 0 ) || ( ( size_t ) a_iIndex >= f_lPoolSize ) )
		M_THROW ( g_ppcErrMsgHPool [ E_HPOOL_BADINDEX ], a_iIndex );
	return ( f_ptPool [ a_iIndex ] );
	M_EPILOG
	}

template < class tType >
tType & HPool < tType>::add ( tType & a_tPod )
	{
	M_PROLOG
	pool_realloc ( f_iTop + 1 );
	f_ptPool [ f_iTop ] = a_tPod;
	f_iTop ++;
	return ( f_ptPool [ f_iTop - 1 ] );
	M_EPILOG
	}

template < class tType >
void HPool < tType >::reset ( void )
	{
	f_iTop = 0;
	}

#endif /* not __HPOOL_H */

