/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hpool.h - this file is integral part of `yaal' project.

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
     as a free push_back-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#ifndef __YAAL_HCORE_HPOOL_H
#define __YAAL_HCORE_HPOOL_H

#line 31

#include <cstddef>
#include <cstring>

#define D_VCSID_HPOOL_H "$Id$"

#include "hcore/xalloc.h"

namespace yaal
{

namespace hcore
{

extern char const * g_ppcErrMsgHPool [ ];

template<typename tType>
class HPool
	{
public:
	struct ERROR
		{
		typedef enum
			{
			E_OK = 0,
			E_BADSIZE,
			E_NOMEM,
			E_REALLOC_FIXED,
			E_BADINDEX
			} error_t;
		};
	typedef enum
		{
		D_FIXED_SIZE,
		D_AUTO_GROW,
		D_DUMB
		} pool_type_t;
private:
	pool_type_t f_ePoolType;
	size_t f_ulPoolSize;	/* size of allocated memory buffer */
	int f_iTop;
	tType* f_ptPool;	/* pointer to allocated memory pool */
public:
	HPool( size_t, pool_type_t = D_FIXED_SIZE );
	virtual ~HPool( void );
	size_t pool_realloc( size_t );
	tType& operator[] ( int ) const;
	void push_back( tType const& );
	void reset( void );
	int size( void ) const;
	void swap( HPool<tType>& );
	tType* raw( void ) const;
	HPool( HPool const& );
	HPool& operator = ( HPool const& );
	};

template<typename tType>
HPool<tType>::HPool( size_t a_ulNewSize, pool_type_t a_ePoolType )
	: f_ePoolType( a_ePoolType ), f_ulPoolSize( 0 ), f_iTop( 0 ),
	f_ptPool( NULL )
	{
	M_PROLOG
	if ( a_ulNewSize )
		pool_realloc( a_ulNewSize );
	return;
	M_EPILOG
	}

template<typename tType>
HPool<tType>::HPool( HPool<tType> const& source )
	: f_ePoolType( source.f_ePoolType ), f_ulPoolSize( 0 ), f_iTop( 0 ),
	f_ptPool( NULL )
	{
	M_PROLOG
	if ( source.f_ulPoolSize )
		{
		pool_realloc( source.f_ulPoolSize );
		memcpy( f_ptPool, source.f_ptPool, f_ulPoolSize * sizeof ( tType ) );
		}
	return;
	M_EPILOG
	}

template<typename tType>
HPool<tType>& HPool<tType>::operator = ( HPool<tType> const& source )
	{
	M_PROLOG
	if ( &source != this )
		{
		f_ePoolType = source.f_ePoolType;
		if ( source.f_ulPoolSize )
			{
			pool_realloc( source.f_ulPoolSize );
			memcpy( f_ptPool, source.f_ptPool, f_ulPoolSize * sizeof ( tType ) );
			}
		}
	return ( *this );
	M_EPILOG
	}

template<typename tType>
HPool<tType>::~HPool( void )
	{
	M_PROLOG
	if ( f_ptPool )
		xfree( f_ptPool );
	f_ulPoolSize = 0;
	f_iTop = 0;
	return;
	M_EPILOG
	}

template<typename tType>
size_t HPool<tType>::pool_realloc( const size_t a_ulNewSize )
	{
	M_PROLOG
	size_t l_ulOldSize = f_ulPoolSize;
	if ( a_ulNewSize < 1 )
		M_THROW( g_ppcErrMsgHPool[ ERROR::E_BADSIZE ], a_ulNewSize );
	if ( f_ePoolType == D_AUTO_GROW )
		{
		if ( a_ulNewSize > f_ulPoolSize )
			{
			f_ulPoolSize = 1;
			while ( f_ulPoolSize < a_ulNewSize )
				f_ulPoolSize <<= 1;
			f_ptPool = xrealloc<tType>( f_ptPool, f_ulPoolSize );
			memset( f_ptPool + l_ulOldSize, 0,
					( f_ulPoolSize - l_ulOldSize ) * sizeof ( tType ) );
			}
		}
	else if ( f_ulPoolSize != a_ulNewSize )
		{
		if ( f_ptPool && ( f_ePoolType == D_FIXED_SIZE ) )
			M_THROW( g_ppcErrMsgHPool[ ERROR::E_REALLOC_FIXED ], f_ulPoolSize );
		f_ulPoolSize = a_ulNewSize;
		f_ptPool = xrealloc<tType>( f_ptPool, f_ulPoolSize );
		}
	f_iTop = a_ulNewSize;
	return ( f_ulPoolSize - l_ulOldSize );
	M_EPILOG
	}

template<typename tType>
tType& HPool<tType>::operator[]( int a_iIndex ) const
	{
	M_PROLOG
	if ( ( a_iIndex < 0 ) || ( a_iIndex >= f_iTop ) )
		M_THROW( g_ppcErrMsgHPool[ ERROR::E_BADINDEX ], a_iIndex );
	return ( f_ptPool[ a_iIndex ] );
	M_EPILOG
	}

template<typename tType>
void HPool<tType>::push_back( tType const& a_tPod )
	{
	M_PROLOG
	size_t l_iOldTop = f_iTop;
	pool_realloc( f_iTop + 1 );
	f_ptPool[ l_iOldTop ] = a_tPod;
	return;
	M_EPILOG
	}

template<typename tType>
void HPool<tType>::reset( void )
	{
	f_iTop = 0;
	}

template<typename tType>
int HPool<tType>::size( void ) const
	{
	return ( f_iTop );
	}

template<typename tType>
void HPool<tType>::swap( HPool<tType>& other )
	{
	if ( &other != this )
		{
		pool_type_t l_ePoolType = f_ePoolType;
		size_t l_ulPoolSize = f_ulPoolSize;
		int l_iTop = f_iTop;
		tType* l_ptPool = f_ptPool;
		f_ePoolType = other.f_ePoolType;
		f_ulPoolSize = other.f_ulPoolSize;
		f_iTop = other.f_iTop;
		f_ptPool = other.f_ptPool;
		other.f_ePoolType = l_ePoolType;
		other.f_ulPoolSize = l_ulPoolSize;
		other.f_iTop = l_iTop;
		other.f_ptPool = l_ptPool;
		}
	return;
	}

template<typename tType>
tType* HPool<tType>::raw( void ) const
	{
	return ( f_ptPool );
	}

}

}

#endif /* not __YAAL_HCORE_HPOOL_H */

