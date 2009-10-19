/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	harray.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/harray.hxx
 * \brief Declaration of HArray<> class.
 */

#ifndef YAAL_HCORE_HARRAY_HXX_INCLUDED
#define YAAL_HCORE_HARRAY_HXX_INCLUDED

#include <new>

#include "hcore/base.hxx"
#include "hcore/algorithm.hxx"

namespace yaal
{

namespace hcore
{

extern char const* const n_ppcErrMsgHArray[];

/*! \brief Simplest compund data structure.
 */
template<typename value_type>
class HArray
	{
	typedef HArray<value_type> self_t;
public:
	typedef value_type value_t;
	/*! \brief Error codes for HArray<> operations.
	 */
	struct ERROR
		{
		/*! \brief Error codes for HArray<> operations.
		 */
		typedef enum
			{
			OK = 0,        /*!< No error. */
			BAD_SIZE,      /*!< Bad size. */
			OUT_OF_MEMORY, /*!< Cannot allocate new memory. */
			BAD_INDEX      /*!< Index of of bounds. */
			} error_t;
		};
protected:
	int long f_lSize;
	int long f_lCapacity;
	value_type* f_ptArray;
public:
	template<typename const_qual_t>
	class HIterator;
	typedef HIterator<value_type> iterator;
	typedef HIterator<value_type const> const_iterator;
	HArray( int long const& = 0 );
	HArray( int long const&, value_type const& );
	virtual ~HArray( void );
	HArray( HArray const& );
	HArray& operator = ( HArray const& );
	void resize( int long const& );
	void resize( int long const&, value_type const& );
	void reserve( int long const& );
	value_type& operator [] ( int long const& );
	value_type const& operator [] ( int long const& ) const;
	void push_back( value_type const& );
	int long get_size( void ) const;
	int long size( void ) const;
	int long get_capacity( void ) const;
	int long capacity( void ) const;
	bool empty( void ) const;
	bool is_empty( void ) const;
	void clear( void );
	bool operator ! ( void ) const;
	iterator begin( void );
	iterator find( int long const& );
	iterator end( void );
	const_iterator begin( void ) const;
	const_iterator find( int long const& ) const;
	const_iterator end( void ) const;
	void swap( HArray& );
private:
	value_type& get( int long const& ) const;
	};

}

template<typename value_type>
inline void swap( yaal::hcore::HArray<value_type>& a, yaal::hcore::HArray<value_type>& b )
	{ a.swap( b ); }

namespace hcore
{

/*! \brief Iterator for HArray<> data structure.
 */
template<typename value_type>
template<typename const_qual_t>
class HArray<value_type>::HIterator
	{
	typedef HArray<value_type> array_t;
	array_t const* f_poOwner;
	int long f_lIndex;
public:
	typedef value_type value_t;
	HIterator( void ) : f_poOwner( NULL ), f_lIndex( 0 ) {}
	template<typename other_const_qual_t>
	HIterator( HIterator<other_const_qual_t> const& a_oIt ) : f_poOwner( a_oIt.f_poOwner ), f_lIndex( a_oIt.f_lIndex )
		{
		STATIC_ASSERT(( trait::same_type<const_qual_t, other_const_qual_t>::value || trait::same_type<const_qual_t, other_const_qual_t const>::value ));
		}
	HIterator& operator= ( HIterator const& a_oIt )
		{
		if ( &a_oIt != this )
			{
			f_poOwner = a_oIt.f_poOwner;
			f_lIndex = a_oIt.f_lIndex;
			}
		return ( *this );
		}
	HIterator& operator ++ ( void )
		{
		++ f_lIndex;
		return ( *this );
		}
	HIterator const operator ++ ( int )
		{
		HIterator it( f_poOwner, f_lIndex );
		++ f_lIndex;
		return ( it );
		}
	HIterator& operator -- ( void )
		{
		-- f_lIndex;
		return ( *this );
		}
	HIterator const operator -- ( int )
		{
		HIterator it( f_poOwner, f_lIndex );
		-- f_lIndex;
		return ( it );
		}
	HIterator operator + ( int long const& off_ ) const
		{
		HIterator it( f_poOwner, f_lIndex + off_ );
		return ( it );
		}
	HIterator& operator += ( int long const& off_ )
		{
		f_lIndex += off_;
		return ( *this );
		}
	HIterator operator - ( int long const& off_ ) const
		{
		HIterator it( f_poOwner, f_lIndex - off_ );
		return ( it );
		}
	HIterator& operator -= ( int long const& off_ )
		{
		f_lIndex -= off_;
		return ( *this );
		}
	int long operator - ( HIterator const& it ) const
		{
		M_ASSERT( f_poOwner == it.f_poOwner );
		return ( f_lIndex - it.f_lIndex );
		}
	const_qual_t& operator* ( void )
		{ return ( f_poOwner->f_ptArray[ f_lIndex ] ); }
	const_qual_t& operator* ( void ) const
		{ return ( f_poOwner->f_ptArray[ f_lIndex ] ); }
	const_qual_t* operator-> ( void )
		{ return ( &f_poOwner->f_ptArray[ f_lIndex ] ); }
	const_qual_t* operator-> ( void ) const
		{ return ( &f_poOwner->f_ptArray[ f_lIndex ] ); }
	template<typename other_const_qual_t>
	bool operator == ( HIterator<other_const_qual_t> const& it ) const
		{ return ( f_lIndex == it.f_lIndex ); }
	template<typename other_const_qual_t>
	bool operator != ( HIterator<other_const_qual_t> const& it ) const
		{ return ( f_lIndex != it.f_lIndex ); }
private:
	friend class HArray<value_type>;
	explicit HIterator( array_t const* a_poOwner, int long const& idx )
		: f_poOwner( a_poOwner ), f_lIndex( idx ) {};
	};

template<typename value_type>
HArray<value_type>::HArray( int long const& a_lSize ) : f_lSize( 0 ), f_lCapacity( 0 ), f_ptArray( NULL )
	{
	M_PROLOG
	if ( a_lSize )
		resize( a_lSize );
	return;
	M_EPILOG
	}

template<typename value_type>
HArray<value_type>::HArray( int long const& a_lSize, value_type const& a_tFillWith )
	: f_lSize( 0 ), f_lCapacity( 0 ), f_ptArray( NULL )
	{
	M_PROLOG
	if ( a_lSize )
		{
		resize( a_lSize );
		fill( begin(), end(), a_tFillWith );
		}
	return;
	M_EPILOG
	}

template<typename value_type>
HArray<value_type>::~HArray( void )
	{
	M_PROLOG
	clear();
	return;
	M_EPILOG
	}

template<typename value_type>
void HArray<value_type>::clear( void )
	{
	M_PROLOG
	if ( f_ptArray )
		delete [] f_ptArray;
	f_lSize = 0;
	f_lCapacity = 0;
	f_ptArray = NULL;
	return;
	M_EPILOG
	}

template<typename value_type>
HArray<value_type>::HArray( HArray const& a_roArray ) : f_lSize( 0 ), f_lCapacity( 0 ), f_ptArray( NULL )
	{
	M_PROLOG
	if ( a_roArray.f_lSize )
		{
		resize( a_roArray.f_lSize );
		copy( a_roArray.begin(), a_roArray.end(), begin() );
		}
	return;
	M_EPILOG
	}

template<typename value_type>
HArray<value_type>& HArray<value_type>::operator = ( HArray const& a_roArray )
	{
	M_PROLOG
	if ( &a_roArray != this )
		{
		if ( a_roArray.f_lSize > f_lCapacity )
			{
			HArray<value_type> tmpArray( a_roArray );
			using yaal::swap;
			swap( tmpArray, *this );
			}
		else
			{
			f_lSize = a_roArray.f_lSize;
			copy( a_roArray.begin(), a_roArray.end(), begin() );
			}
		}
	return ( *this );
	M_EPILOG
	}

template<typename value_type>
void HArray<value_type>::resize( int long const& a_lSize )
	{
	M_PROLOG
	reserve( a_lSize );
	f_lSize = a_lSize;
	return;
	M_EPILOG
	}

template<typename value_type>
void HArray<value_type>::resize( int long const& a_lSize, value_type const& t )
	{
	M_PROLOG
	reserve( a_lSize );
	if ( a_lSize > f_lSize )
		yaal::fill( f_ptArray + f_lSize, f_ptArray + a_lSize, t );
	f_lSize = a_lSize;
	return;
	M_EPILOG
	}

template<typename value_type>
void HArray<value_type>::reserve( int long const& a_lSize )
	{
	M_PROLOG
	if ( a_lSize < 1 )
		M_THROW( n_ppcErrMsgHArray[ ERROR::BAD_SIZE ], a_lSize );
	if ( a_lSize > f_lCapacity )
		{
		f_lCapacity = 1;
		while ( f_lCapacity < a_lSize )
			f_lCapacity <<= 1;
		HArray<value_type> tmpArray( 0 );
		using yaal::swap;
		swap( tmpArray, *this );
		f_ptArray = new ( std::nothrow ) value_type[ a_lSize ];
		if ( ! f_ptArray )
			M_THROW( n_ppcErrMsgHArray[ ERROR::OUT_OF_MEMORY ], a_lSize );
		swap_ranges( tmpArray.begin(), tmpArray.end(), begin() );
		}
	return;
	M_EPILOG
	}

template<typename value_type>
value_type& HArray<value_type>::get( int long const& a_lIndex ) const
	{
	M_PROLOG
	int long idx = ( a_lIndex < 0 ) ? a_lIndex + f_lSize : a_lIndex;
	if ( ( idx >= f_lSize ) || ( idx < 0 ) )
		M_THROW( n_ppcErrMsgHArray[ ERROR::BAD_INDEX ], idx );
	return ( f_ptArray[ idx ] );
	M_EPILOG
	}

template<typename value_type>
value_type& HArray<value_type>::operator[] ( int long const& a_lIndex )
	{
	M_PROLOG
	return ( get( a_lIndex ) );
	M_EPILOG
	}

template<typename value_type>
value_type const& HArray<value_type>::operator[] ( int long const& a_lIndex ) const
	{
	M_PROLOG
	return ( get( a_lIndex ) );
	M_EPILOG
	}

template<typename value_type>
bool HArray<value_type>::operator ! ( void ) const
	{
	M_PROLOG
	return ( ! f_ptArray );
	M_EPILOG
	}

template<typename value_type>
int long HArray<value_type>::get_size( void ) const
	{
	M_PROLOG
	return ( f_lSize );
	M_EPILOG
	}

template<typename value_type>
int long HArray<value_type>::size( void ) const
	{
	M_PROLOG
	return ( f_lSize );
	M_EPILOG
	}

template<typename value_type>
bool HArray<value_type>::is_empty( void ) const
	{
	M_PROLOG
	return ( f_lSize ? false : true );
	M_EPILOG
	}

template<typename value_type>
bool HArray<value_type>::empty( void ) const
	{
	M_PROLOG
	return ( f_lSize ? false : true );
	M_EPILOG
	}

template<typename value_type>
int long HArray<value_type>::capacity( void ) const
	{
	return ( get_capacity() );
	}

template<typename value_type>
int long HArray<value_type>::get_capacity( void ) const
	{
	return ( f_lCapacity );
	}

template<typename value_type>
typename HArray<value_type>::iterator HArray<value_type>::begin( void )
	{
	return ( iterator( this, 0 ) );
	}

template<typename value_type>
typename HArray<value_type>::iterator HArray<value_type>::find( int long const& idx )
	{
	return ( iterator( this, min( idx, f_lSize ) ) );
	}

template<typename value_type>
typename HArray<value_type>::iterator HArray<value_type>::end( void )
	{
	return ( iterator( this, f_lSize ) );
	}

template<typename value_type>
typename HArray<value_type>::const_iterator HArray<value_type>::begin( void ) const
	{
	return ( const_iterator( this, 0 ) );
	}

template<typename value_type>
typename HArray<value_type>::const_iterator HArray<value_type>::find( int long const& idx ) const
	{
	return ( const_iterator( this, min( idx, f_lSize ) ) );
	}

template<typename value_type>
typename HArray<value_type>::const_iterator HArray<value_type>::end( void ) const
	{
	return ( const_iterator( this, f_lSize ) );
	}

template<typename value_type>
void HArray<value_type>::swap( HArray& other )
	{
	using yaal::swap;
	swap( f_ptArray, other.f_ptArray );
	swap( f_lSize, other.f_lSize );
	swap( f_lCapacity, other.f_lCapacity );
	return;
	}

template<typename value_type>
void HArray<value_type>::push_back( value_type const& a_tPod )
	{
	M_PROLOG
	int long l_iOldTop = f_lSize;
	resize( f_lSize + 1 );
	f_ptArray[ l_iOldTop ] = a_tPod;
	return;
	M_EPILOG
	}

}

}

#endif /* YAAL_HCORE_HARRAY_HXX_INCLUDED */

