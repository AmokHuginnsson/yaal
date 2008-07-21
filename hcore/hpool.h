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
	typedef HPool<tType> self_t;
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
	typedef tType* iterator;
	typedef tType const* const_iterator;
private:
	pool_type_t f_ePoolType;
	int long f_lAllocedObjects;	/* size of allocated memory buffer */
	int long f_lSize; /*! size of container */
	tType* f_ptPool;	/* pointer to allocated memory pool */
public:
	HPool( int long, pool_type_t = D_FIXED_SIZE );
	virtual ~HPool( void );
	void pool_realloc( int long );
	tType& operator[] ( int long ) const;
	void push_back( tType const& );
	void reset( void );
	void clear( void );
	bool empty( void ) const;
	bool is_empty( void ) const;
	int long size( void ) const;
	int long get_size( void ) const;
	static void swap( HPool<tType>&, HPool<tType>& );
	tType const* raw( void ) const;
	tType* raw( void );
	const_iterator begin( void ) const;
	iterator begin( void );
	const_iterator end( void ) const;
	iterator end( void );
	const_iterator rbegin( void ) const;
	iterator rbegin( void );
	const_iterator rend( void ) const;
	iterator rend( void );
	HPool( HPool const& );
	HPool& operator = ( HPool const& );
	};

template<typename tType>
HPool<tType>::HPool( int long a_ulNewSize, pool_type_t a_ePoolType )
	: f_ePoolType( a_ePoolType ), f_lAllocedObjects( 0 ), f_lSize( 0 ),
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
	: f_ePoolType( source.f_ePoolType ), f_lAllocedObjects( 0 ), f_lSize( 0 ),
	f_ptPool( NULL )
	{
	M_PROLOG
	if ( source.f_lSize )
		{
		pool_realloc( source.f_lSize );
		memcpy( f_ptPool, source.f_ptPool, f_lSize * sizeof ( tType ) );
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
		if ( source.f_lSize )
			{
			pool_realloc( source.f_lSize );
			memcpy( f_ptPool, source.f_ptPool, f_lSize * sizeof ( tType ) );
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
	f_lAllocedObjects = 0;
	f_lSize = 0;
	return;
	M_EPILOG
	}

template<typename tType>
void HPool<tType>::pool_realloc( const int long a_ulNewSize )
	{
	M_PROLOG
	if ( a_ulNewSize < 1 )
		M_THROW( g_ppcErrMsgHPool[ ERROR::E_BADSIZE ], a_ulNewSize );
	if ( f_ePoolType == D_AUTO_GROW )
		{
		if ( a_ulNewSize > f_lAllocedObjects )
			{
			f_lAllocedObjects = 1;
			while ( f_lAllocedObjects < a_ulNewSize )
				f_lAllocedObjects <<= 1;
			f_ptPool = xrealloc<tType>( f_ptPool, f_lAllocedObjects );
			::memset( f_ptPool + f_lSize, 0,
					( f_lAllocedObjects - f_lSize ) * sizeof ( tType ) );
			}
		}
	else if ( f_lAllocedObjects != a_ulNewSize )
		{
		if ( f_ptPool && ( f_ePoolType == D_FIXED_SIZE ) )
			M_THROW( g_ppcErrMsgHPool[ ERROR::E_REALLOC_FIXED ], f_lAllocedObjects );
		f_lAllocedObjects = a_ulNewSize;
		f_ptPool = xrealloc<tType>( f_ptPool, f_lAllocedObjects );
		if ( a_ulNewSize > f_lSize )
			::memset( f_ptPool + f_lSize, 0,
					( f_lAllocedObjects - f_lSize ) * sizeof ( tType ) );
		}
	f_lSize = a_ulNewSize;
	return;
	M_EPILOG
	}

template<typename tType>
tType& HPool<tType>::operator[]( int long a_iIndex ) const
	{
	M_PROLOG
	if ( ( a_iIndex < 0 ) || ( a_iIndex >= f_lSize ) )
		M_THROW( g_ppcErrMsgHPool[ ERROR::E_BADINDEX ], a_iIndex );
	return ( f_ptPool[ a_iIndex ] );
	M_EPILOG
	}

template<typename tType>
void HPool<tType>::push_back( tType const& a_tPod )
	{
	M_PROLOG
	int long l_iOldTop = f_lSize;
	pool_realloc( f_lSize + 1 );
	f_ptPool[ l_iOldTop ] = a_tPod;
	return;
	M_EPILOG
	}

template<typename tType>
void HPool<tType>::reset( void )
	{
	f_lSize = 0;
	}

template<typename tType>
int long HPool<tType>::size( void ) const
	{
	return ( get_size() );
	}

template<typename tType>
int long HPool<tType>::get_size( void ) const
	{
	return ( f_lSize );
	}

template<typename tType>
bool HPool<tType>::is_empty( void ) const
	{
	return ( ! f_lSize );
	}

template<typename tType>
bool HPool<tType>::empty( void ) const
	{
	return ( is_empty() );
	}

template<typename tType>
void HPool<tType>::swap( HPool<tType>& left, HPool<tType>& right )
	{
	if ( &left != &right )
		{
		yaal::swap( left.f_ePoolType, right.f_ePoolType );
		yaal::swap( left.f_lAllocedObjects, right.f_lAllocedObjects );
		yaal::swap( left.f_lSize, right.f_lSize );
		yaal::swap( left.f_ptPool, right.f_ptPool );
		}
	return;
	}

template<typename tType>
tType const* HPool<tType>::raw( void ) const
	{
	return ( f_ptPool );
	}

template<typename tType>
tType* HPool<tType>::raw( void )
	{
	return ( f_ptPool );
	}

template<typename tType>
typename HPool<tType>::const_iterator HPool<tType>::begin( void ) const
	{
	return ( f_ptPool );
	}

template<typename tType>
typename HPool<tType>::iterator HPool<tType>::begin( void )
	{
	return ( f_ptPool );
	}

template<typename tType>
typename HPool<tType>::const_iterator HPool<tType>::end( void ) const
	{
	return ( f_ptPool + f_lSize );
	}

template<typename tType>
typename HPool<tType>::iterator HPool<tType>::end( void )
	{
	return ( f_ptPool + f_lSize );
	}

template<typename tType>
typename HPool<tType>::const_iterator HPool<tType>::rbegin( void ) const
	{
	return ( f_ptPool + f_lSize - 1 );
	}

template<typename tType>
typename HPool<tType>::iterator HPool<tType>::rbegin( void )
	{
	return ( f_ptPool + f_lSize - 1 );
	}

template<typename tType>
typename HPool<tType>::const_iterator HPool<tType>::rend( void ) const
	{
	return ( f_ptPool - 1 );
	}

template<typename tType>
typename HPool<tType>::iterator HPool<tType>::rend( void )
	{
	return ( f_ptPool - 1 );
	}

}

}

#endif /* not __YAAL_HCORE_HPOOL_H */

