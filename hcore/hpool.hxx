/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hpool.hxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#ifndef YAAL_HCORE_HPOOL_HXX_INCLUDED
#define YAAL_HCORE_HPOOL_HXX_INCLUDED

#include "hcore/base.hxx"
#include "hcore/xalloc.hxx"

namespace yaal
{

namespace hcore
{

extern char const * n_ppcErrMsgHPool [ ];

/*! \brief Raw memory (malloc/free) pools manager.
 */
template<typename value_type>
class HPool
	{
	typedef HPool<value_type> self_t;
public:
	typedef value_type value_t;
	/*! \brief Error codes for HPool<> operations.
	 */
	struct ERROR
		{
		/*! \brief Codes.
		 */
		typedef enum
			{
			OK = 0,        /*!< No error. */
			BAD_SIZE,      /*!< Bad size specified. */
			REALLOC_FIXED, /*!< Resizing of fixed size pool. */
			BAD_INDEX      /*!< Out of bounds dereferencing. */
			} error_t;
		};
	/*! \brief Types of pool memory allocation policies.
	 */
	typedef enum
		{
		FIXED_SIZE, /*!< Fixed pool size. */
		AUTO_GROW,  /*!< Grow size automatically (geometric grow). */
		DUMB        /*!< Set pool size to precize requwested value. */
		} pool_type_t;
	typedef value_t* iterator;
	typedef value_t const* const_iterator;
private:
	pool_type_t f_ePoolType; /*!< Type of memory allocation policy. */
	int long f_lCapacity;	/*!< size of allocated memory buffer */
	int long f_lSize; /*!< size of container */
	value_t* f_ptPool;	/*!< pointer to allocated memory pool */
public:
	HPool( void );
	HPool( int long const&, pool_type_t const& = FIXED_SIZE );
	HPool( HPool const& );
	HPool& operator = ( HPool const& );
	virtual ~HPool( void );
	void pool_realloc( int long const& );
	value_t const& operator[] ( int long const& ) const;
	value_t& operator[] ( int long const& );
	void push_back( value_t const& );
	void reset( void );
	void clear( void );
	bool empty( void ) const;
	bool is_empty( void ) const;
	int long size( void ) const;
	int long get_size( void ) const;
	int long capacity( void ) const;
	int long get_capacity( void ) const;
	void swap( HPool<value_t>& );
	value_t const* raw( void ) const;
	value_t* raw( void );
	const_iterator begin( void ) const;
	iterator begin( void );
	const_iterator end( void ) const;
	iterator end( void );
	const_iterator rbegin( void ) const;
	iterator rbegin( void );
	const_iterator rend( void ) const;
	iterator rend( void );
private:
	value_t& get( int long const& ) const;
	};

template<typename value_t>
HPool<value_t>::HPool( void )
	: f_ePoolType( DUMB ), f_lCapacity( 0 ),
	f_lSize( 0 ), f_ptPool( NULL )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

template<typename value_t>
HPool<value_t>::HPool( int long const& a_ulNewSize, pool_type_t const& a_ePoolType )
	: f_ePoolType( a_ePoolType ), f_lCapacity( 0 ), f_lSize( 0 ),
	f_ptPool( NULL )
	{
	M_PROLOG
	if ( a_ulNewSize )
		pool_realloc( a_ulNewSize );
	return;
	M_EPILOG
	}

template<typename value_t>
HPool<value_t>::HPool( HPool<value_t> const& source )
	: f_ePoolType( source.f_ePoolType ), f_lCapacity( 0 ), f_lSize( 0 ),
	f_ptPool( NULL )
	{
	M_PROLOG
	if ( source.f_lSize )
		{
		pool_realloc( source.f_lSize );
		memcpy( f_ptPool, source.f_ptPool, f_lSize * sizeof ( value_t ) );
		}
	return;
	M_EPILOG
	}

template<typename value_t>
HPool<value_t>& HPool<value_t>::operator = ( HPool<value_t> const& source )
	{
	M_PROLOG
	if ( &source != this )
		{
		f_ePoolType = source.f_ePoolType;
		if ( source.f_lSize )
			{
			pool_realloc( source.f_lSize );
			memcpy( f_ptPool, source.f_ptPool, f_lSize * sizeof ( value_t ) );
			}
		}
	return ( *this );
	M_EPILOG
	}

template<typename value_t>
HPool<value_t>::~HPool( void )
	{
	M_PROLOG
	if ( f_ptPool )
		xfree( f_ptPool );
	f_lCapacity = 0;
	f_lSize = 0;
	return;
	M_EPILOG
	}

template<typename value_t>
void HPool<value_t>::pool_realloc( int long const& a_ulNewSize )
	{
	M_PROLOG
	if ( a_ulNewSize < 1 )
		M_THROW( n_ppcErrMsgHPool[ ERROR::BAD_SIZE ], a_ulNewSize );
	if ( f_ePoolType == AUTO_GROW )
		{
		if ( a_ulNewSize > f_lCapacity )
			{
			f_lCapacity = 1;
			while ( f_lCapacity < a_ulNewSize )
				f_lCapacity <<= 1;
			f_ptPool = xrealloc<value_t>( f_ptPool, f_lCapacity );
			::memset( f_ptPool + f_lSize, 0,
					( f_lCapacity - f_lSize ) * sizeof ( value_t ) );
			}
		}
	else if ( f_lCapacity != a_ulNewSize )
		{
		if ( f_ptPool && ( f_ePoolType == FIXED_SIZE ) )
			M_THROW( n_ppcErrMsgHPool[ ERROR::REALLOC_FIXED ], f_lCapacity );
		f_lCapacity = a_ulNewSize;
		f_ptPool = xrealloc<value_t>( f_ptPool, f_lCapacity );
		if ( a_ulNewSize > f_lSize )
			::memset( f_ptPool + f_lSize, 0,
					( f_lCapacity - f_lSize ) * sizeof ( value_t ) );
		}
	f_lSize = a_ulNewSize;
	return;
	M_EPILOG
	}

template<typename value_t>
value_t const& HPool<value_t>::operator[]( int long const& a_lIndex ) const
	{
	M_PROLOG
	return ( get( a_lIndex ) );
	M_EPILOG
	}

template<typename value_t>
value_t& HPool<value_t>::operator[]( int long const& a_lIndex )
	{
	M_PROLOG
	return ( get( a_lIndex ) );
	M_EPILOG
	}

template<typename value_t>
value_t& HPool<value_t>::get( int long const& a_lIndex ) const
	{
	M_PROLOG
	if ( ( a_lIndex < 0 ) || ( a_lIndex >= f_lSize ) )
		M_THROW( n_ppcErrMsgHPool[ ERROR::BAD_INDEX ], a_lIndex );
	return ( f_ptPool[ a_lIndex ] );
	M_EPILOG
	}

template<typename value_t>
void HPool<value_t>::push_back( value_t const& a_tPod )
	{
	M_PROLOG
	int long l_iOldTop = f_lSize;
	pool_realloc( f_lSize + 1 );
	f_ptPool[ l_iOldTop ] = a_tPod;
	return;
	M_EPILOG
	}

template<typename value_t>
void HPool<value_t>::reset( void )
	{
	f_lSize = 0;
	}

template<typename value_t>
int long HPool<value_t>::size( void ) const
	{
	return ( get_size() );
	}

template<typename value_t>
int long HPool<value_t>::get_size( void ) const
	{
	return ( f_lSize );
	}

template<typename value_t>
int long HPool<value_t>::capacity( void ) const
	{
	return ( get_capacity() );
	}

template<typename value_t>
int long HPool<value_t>::get_capacity( void ) const
	{
	return ( f_lCapacity );
	}

template<typename value_t>
bool HPool<value_t>::is_empty( void ) const
	{
	return ( ! f_lSize );
	}

template<typename value_t>
bool HPool<value_t>::empty( void ) const
	{
	return ( is_empty() );
	}

template<typename value_t>
void HPool<value_t>::swap( HPool<value_t>& other )
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

template<typename value_t>
value_t const* HPool<value_t>::raw( void ) const
	{
	return ( f_ptPool );
	}

template<typename value_t>
value_t* HPool<value_t>::raw( void )
	{
	return ( f_ptPool );
	}

template<typename value_t>
typename HPool<value_t>::const_iterator HPool<value_t>::begin( void ) const
	{
	return ( f_ptPool );
	}

template<typename value_t>
typename HPool<value_t>::iterator HPool<value_t>::begin( void )
	{
	return ( f_ptPool );
	}

template<typename value_t>
typename HPool<value_t>::const_iterator HPool<value_t>::end( void ) const
	{
	return ( f_ptPool + f_lSize );
	}

template<typename value_t>
typename HPool<value_t>::iterator HPool<value_t>::end( void )
	{
	return ( f_ptPool + f_lSize );
	}

template<typename value_t>
typename HPool<value_t>::const_iterator HPool<value_t>::rbegin( void ) const
	{
	return ( f_ptPool + f_lSize - 1 );
	}

template<typename value_t>
typename HPool<value_t>::iterator HPool<value_t>::rbegin( void )
	{
	return ( f_ptPool + f_lSize - 1 );
	}

template<typename value_t>
typename HPool<value_t>::const_iterator HPool<value_t>::rend( void ) const
	{
	return ( f_ptPool - 1 );
	}

template<typename value_t>
typename HPool<value_t>::iterator HPool<value_t>::rend( void )
	{
	return ( f_ptPool - 1 );
	}

}

template<typename value_t>
inline void swap( yaal::hcore::HPool<value_t>& a, yaal::hcore::HPool<value_t>& b )
	{ a.swap( b ); }

}

#endif /* not YAAL_HCORE_HPOOL_HXX_INCLUDED */

