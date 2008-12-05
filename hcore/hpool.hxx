/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hpool.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCORE_HPOOL_HXX_INCLUDED
#define YAAL_HCORE_HPOOL_HXX_INCLUDED

#line 31

#include <cstring>

#define D_VCSID_HPOOL_H "$Id$"

#include "hcore/xalloc.hxx"

namespace yaal
{

namespace hcore
{

extern char const * n_ppcErrMsgHPool [ ];

/*! \brief Raw memory (malloc/free) pools manager.
 */
template<typename tType>
class HPool
	{
	typedef HPool<tType> self_t;
public:
	/*! \brief Error codes for HPool<> operations.
	 */
	struct ERROR
		{
		/*! \brief Codes.
		 */
		typedef enum
			{
			E_OK = 0,        /*!< No error. */
			E_BAD_SIZE,      /*!< Bad size specified. */
			E_REALLOC_FIXED, /*!< Resizing of fixed size pool. */
			E_BAD_INDEX      /*!< Out of bounds dereferencing. */
			} error_t;
		};
	/*! \brief Types of pool memory allocation policies.
	 */
	typedef enum
		{
		D_FIXED_SIZE, /*!< Fixed pool size. */
		D_AUTO_GROW,  /*!< Grow size automatically (geometric grow). */
		D_DUMB        /*!< Set pool size to precize requwested value. */
		} pool_type_t;
	typedef tType* iterator;
	typedef tType const* const_iterator;
private:
	pool_type_t f_ePoolType; /*!< Type of memory allocation policy. */
	int long f_lCapacity;	/*!< size of allocated memory buffer */
	int long f_lSize; /*!< size of container */
	tType* f_ptPool;	/*!< pointer to allocated memory pool */
public:
	HPool( void );
	HPool( int long const&, pool_type_t const& = D_FIXED_SIZE );
	HPool( HPool const& );
	HPool& operator = ( HPool const& );
	virtual ~HPool( void );
	void pool_realloc( int long const& );
	tType const& operator[] ( int long const& ) const;
	tType& operator[] ( int long const& );
	void push_back( tType const& );
	void reset( void );
	void clear( void );
	bool empty( void ) const;
	bool is_empty( void ) const;
	int long size( void ) const;
	int long get_size( void ) const;
	int long capacity( void ) const;
	int long get_capacity( void ) const;
	void swap( HPool<tType>& );
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
private:
	tType& get( int long const& ) const;
	};

template<typename tType>
HPool<tType>::HPool( void )
	: f_ePoolType( D_DUMB ), f_lCapacity( 0 ),
	f_lSize( 0 ), f_ptPool( NULL )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

template<typename tType>
HPool<tType>::HPool( int long const& a_ulNewSize, pool_type_t const& a_ePoolType )
	: f_ePoolType( a_ePoolType ), f_lCapacity( 0 ), f_lSize( 0 ),
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
	: f_ePoolType( source.f_ePoolType ), f_lCapacity( 0 ), f_lSize( 0 ),
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
	f_lCapacity = 0;
	f_lSize = 0;
	return;
	M_EPILOG
	}

template<typename tType>
void HPool<tType>::pool_realloc( int long const& a_ulNewSize )
	{
	M_PROLOG
	if ( a_ulNewSize < 1 )
		M_THROW( n_ppcErrMsgHPool[ ERROR::E_BAD_SIZE ], a_ulNewSize );
	if ( f_ePoolType == D_AUTO_GROW )
		{
		if ( a_ulNewSize > f_lCapacity )
			{
			f_lCapacity = 1;
			while ( f_lCapacity < a_ulNewSize )
				f_lCapacity <<= 1;
			f_ptPool = xrealloc<tType>( f_ptPool, f_lCapacity );
			::memset( f_ptPool + f_lSize, 0,
					( f_lCapacity - f_lSize ) * sizeof ( tType ) );
			}
		}
	else if ( f_lCapacity != a_ulNewSize )
		{
		if ( f_ptPool && ( f_ePoolType == D_FIXED_SIZE ) )
			M_THROW( n_ppcErrMsgHPool[ ERROR::E_REALLOC_FIXED ], f_lCapacity );
		f_lCapacity = a_ulNewSize;
		f_ptPool = xrealloc<tType>( f_ptPool, f_lCapacity );
		if ( a_ulNewSize > f_lSize )
			::memset( f_ptPool + f_lSize, 0,
					( f_lCapacity - f_lSize ) * sizeof ( tType ) );
		}
	f_lSize = a_ulNewSize;
	return;
	M_EPILOG
	}

template<typename tType>
tType const& HPool<tType>::operator[]( int long const& a_lIndex ) const
	{
	M_PROLOG
	return ( get( a_lIndex ) );
	M_EPILOG
	}

template<typename tType>
tType& HPool<tType>::operator[]( int long const& a_lIndex )
	{
	M_PROLOG
	return ( get( a_lIndex ) );
	M_EPILOG
	}

template<typename tType>
tType& HPool<tType>::get( int long const& a_lIndex ) const
	{
	M_PROLOG
	if ( ( a_lIndex < 0 ) || ( a_lIndex >= f_lSize ) )
		M_THROW( n_ppcErrMsgHPool[ ERROR::E_BAD_INDEX ], a_lIndex );
	return ( f_ptPool[ a_lIndex ] );
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
int long HPool<tType>::capacity( void ) const
	{
	return ( get_capacity() );
	}

template<typename tType>
int long HPool<tType>::get_capacity( void ) const
	{
	return ( f_lCapacity );
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
void HPool<tType>::swap( HPool<tType>& other )
	{
	if ( &other != this )
		{
		using yaal::swap;
		swap( f_ePoolType, other.f_ePoolType );
		swap( f_lCapacity, other.f_lCapacity );
		swap( f_lSize, other.f_lSize );
		swap( f_ptPool, other.f_ptPool );
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

template<typename tType>
inline void swap( yaal::hcore::HPool<tType>& a, yaal::hcore::HPool<tType>& b )
	{ a.swap( b ); }

}

#endif /* not YAAL_HCORE_HPOOL_HXX_INCLUDED */

