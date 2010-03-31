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
#include "hcore/hchunk.hxx"
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
			OK = 0,    /*!< No error. */
			BAD_SIZE,  /*!< Index of of bounds. */
			BAD_INDEX  /*!< Index of of bounds. */
			} error_t;
		};
protected:
	HChunk _buf;
	int long _size;
public:
	template<typename const_qual_t>
	class HIterator;
	typedef HIterator<value_type> iterator;
	typedef HIterator<value_type const> const_iterator;
	HArray( void );
	HArray( int long const& );
	HArray( int long const&, value_type const& );
	virtual ~HArray( void );
	HArray( HArray const& );
	HArray& operator = ( HArray const& );
	void resize( int long const&, value_type const& = value_t() );
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
	iterator erase( iterator, iterator );
	iterator begin( void );
	iterator find( int long const& );
	iterator end( void );
	const_iterator begin( void ) const;
	const_iterator find( int long const& ) const;
	const_iterator end( void ) const;
	iterator rbegin( void );
	iterator rend( void );
	const_iterator rbegin( void ) const;
	const_iterator rend( void ) const;
	void swap( HArray& );
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
	array_t const* _owner;
	int long _index;
public:
	typedef value_type value_t;
	HIterator( void ) : _owner( NULL ), _index( 0 ) {}
	template<typename other_const_qual_t>
	HIterator( HIterator<other_const_qual_t> const& a_oIt ) : _owner( a_oIt._owner ), _index( a_oIt._index )
		{
		STATIC_ASSERT(( trait::same_type<const_qual_t, other_const_qual_t>::value || trait::same_type<const_qual_t, other_const_qual_t const>::value ));
		}
	HIterator& operator= ( HIterator const& a_oIt )
		{
		if ( &a_oIt != this )
			{
			_owner = a_oIt._owner;
			_index = a_oIt._index;
			}
		return ( *this );
		}
	HIterator& operator ++ ( void )
		{
		++ _index;
		return ( *this );
		}
	HIterator const operator ++ ( int )
		{
		HIterator it( _owner, _index );
		++ _index;
		return ( it );
		}
	HIterator& operator -- ( void )
		{
		-- _index;
		return ( *this );
		}
	HIterator const operator -- ( int )
		{
		HIterator it( _owner, _index );
		-- _index;
		return ( it );
		}
	HIterator operator + ( int long const& off_ ) const
		{
		HIterator it( _owner, _index + off_ );
		return ( it );
		}
	HIterator& operator += ( int long const& off_ )
		{
		_index += off_;
		return ( *this );
		}
	HIterator operator - ( int long const& off_ ) const
		{
		HIterator it( _owner, _index - off_ );
		return ( it );
		}
	HIterator& operator -= ( int long const& off_ )
		{
		_index -= off_;
		return ( *this );
		}
	int long operator - ( HIterator const& it ) const
		{
		M_ASSERT( _owner == it._owner );
		return ( _index - it._index );
		}
	const_qual_t& operator* ( void )
		{ return ( const_cast<const_qual_t&>( _owner->_buf.template get<const_qual_t>()[ _index ] ) ); }
	const_qual_t& operator* ( void ) const
		{ return ( _owner->_buf.template get<const_qual_t>()[ _index ] ); }
	const_qual_t* operator-> ( void )
		{ return ( const_cast<const_qual_t*>( &_owner->_buf.template get<const_qual_t>()[ _index ] ) ); }
	const_qual_t* operator-> ( void ) const
		{ return ( &_owner->_buf.template get<const_qual_t>()[ _index ] ); }
	template<typename other_const_qual_t>
	bool operator == ( HIterator<other_const_qual_t> const& it ) const
		{ return ( _index == it._index ); }
	template<typename other_const_qual_t>
	bool operator != ( HIterator<other_const_qual_t> const& it ) const
		{ return ( _index != it._index ); }
private:
	friend class HArray<value_type>;
	explicit HIterator( array_t const* a_poOwner, int long const& idx )
		: _owner( a_poOwner ), _index( idx ) {};
	};

template<typename value_type>
HArray<value_type>::HArray( void )
	: _buf(), _size( 0 )
	{
	return;
	}

template<typename value_type>
HArray<value_type>::HArray( int long const& size_ )
	: _buf(), _size( 0 )
	{
	M_PROLOG
	resize( size_ );
	return;
	M_EPILOG
	}

template<typename value_type>
HArray<value_type>::HArray( int long const& size_, value_type const& fillWith_ )
	: _buf(), _size( 0 )
	{
	M_PROLOG
	resize( size_, fillWith_ );
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
	erase( begin(), end() );
	return;
	M_EPILOG
	}

template<typename value_type>
HArray<value_type>::HArray( HArray const& arr_ )
	: _buf( chunk_size<value_t>( arr_._size ) ), _size( arr_._size )
	{
	M_PROLOG
	if ( _size > 0 )
		{
		value_t* dst( _buf.get<value_t>() );
		value_t const* src( arr_._buf.template get<value_t>() );
		for ( int long i = 0; i < _size; ++ i )
			new ( dst + i ) value_t( src[ i ] );
		}
	return;
	M_EPILOG
	}

template<typename value_type>
HArray<value_type>& HArray<value_type>::operator = ( HArray const& arr_ )
	{
	M_PROLOG
	if ( &arr_ != this )
		{
		if ( arr_._size > get_capacity() )
			{
			HArray<value_t> tmp( arr_ );
			swap( tmp );
			}
		else
			{
			value_t* dst( _buf.get<value_t>() );
			value_t const* src( arr_._buf.template get<value_t>() );
			copy_n( src, _size, dst );
			if ( arr_._size > _size )
				{
				for ( int long i = _size; i < arr_._size; ++ i )
					new ( dst + i ) value_t( src[ i ] );
				}
			else if ( arr_._size < _size )
				{
				for ( int long i = arr_._size; i < _size; ++ i )
					dst[ i ].~value_t();
				}
			_size = arr_._size;
			}
		}
	return ( *this );
	M_EPILOG
	}

template<typename value_type>
void HArray<value_type>::resize( int long const& size_, value_type const& fillWith_ )
	{
	M_PROLOG
	if ( size_ < 0 )
		M_THROW( n_ppcErrMsgHArray[ ERROR::BAD_SIZE ], size_ );
	if ( size_ > _size )
		{
		reserve( size_ );
		value_t* arr( _buf.get<value_t>() );
		for ( int long i = _size; i < size_; ++ i )
			new ( arr + i ) value_t( fillWith_ );
		}
	else if ( size_ < _size )
		{
		value_t* arr( _buf.get<value_t>() );
		for ( int long i = size_; i < _size; ++ i )
			arr[ i ].~value_t();
		}
	_size = size_;
	return;
	M_EPILOG
	}

template<typename value_type>
void HArray<value_type>::reserve( int long const& capacity_ )
	{
	M_PROLOG
	if ( capacity_ < 0 )
		M_THROW( n_ppcErrMsgHArray[ ERROR::BAD_SIZE ], capacity_ );
	if ( capacity_ > get_capacity() )
		{
		HChunk newBuf( chunk_size<value_t>( capacity_ ) );
		value_t* dst( newBuf.get<value_t>() );
		value_t* src( _buf.get<value_t>() );
		for ( int long i = 0; i < _size; ++ i )
			new ( dst + i ) value_t( src[ i ] );
		for ( int long i = 0; i < _size; ++ i )
			src[ i ].~value_t();
		_buf.swap( newBuf );
		}
	return;
	M_EPILOG
	}

template<typename value_type>
typename HArray<value_type>::iterator HArray<value_type>::erase( iterator first, iterator last )
	{
	M_PROLOG
	if ( last != end() )
		{
		for ( iterator second = last, endIt = end();
				( second != endIt );
				++ first, ++ second )
			*first = *second;
		}
	int long removed( 0 );
	for ( iterator endIt = end();
			( first != endIt );
			++ first, ++ removed )
		first->~value_t();
	_size -= removed;
	return ( last._index < _size ? last : end() );
	M_EPILOG
	}

template<typename value_type>
value_type& HArray<value_type>::operator[] ( int long const& a_lIndex )
	{
	M_PROLOG
	int long idx = ( a_lIndex < 0 ) ? a_lIndex + _size : a_lIndex;
	if ( ( idx >= _size ) || ( idx < 0 ) )
		M_THROW( n_ppcErrMsgHArray[ ERROR::BAD_INDEX ], idx );
	return ( _buf.get<value_t>()[ idx ] );
	M_EPILOG
	}

template<typename value_type>
value_type const& HArray<value_type>::operator[] ( int long const& a_lIndex ) const
	{
	M_PROLOG
	int long idx = ( a_lIndex < 0 ) ? a_lIndex + _size : a_lIndex;
	if ( ( idx >= _size ) || ( idx < 0 ) )
		M_THROW( n_ppcErrMsgHArray[ ERROR::BAD_INDEX ], idx );
	return ( _buf.get<value_t const>()[ idx ] );
	M_EPILOG
	}

template<typename value_type>
bool HArray<value_type>::operator ! ( void ) const
	{
	M_PROLOG
	return ( ! _buf.get<value_type>() );
	M_EPILOG
	}

template<typename value_type>
int long HArray<value_type>::get_size( void ) const
	{
	M_PROLOG
	return ( _size );
	M_EPILOG
	}

template<typename value_type>
int long HArray<value_type>::size( void ) const
	{
	M_PROLOG
	return ( _size );
	M_EPILOG
	}

template<typename value_type>
bool HArray<value_type>::is_empty( void ) const
	{
	M_PROLOG
	return ( _size ? false : true );
	M_EPILOG
	}

template<typename value_type>
bool HArray<value_type>::empty( void ) const
	{
	M_PROLOG
	return ( _size ? false : true );
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
	return ( _buf.size() / sizeof ( value_t ) );
	}

template<typename value_type>
typename HArray<value_type>::iterator HArray<value_type>::begin( void )
	{
	return ( iterator( this, 0 ) );
	}

template<typename value_type>
typename HArray<value_type>::iterator HArray<value_type>::find( int long const& idx )
	{
	return ( iterator( this, min( idx, _size ) ) );
	}

template<typename value_type>
typename HArray<value_type>::iterator HArray<value_type>::end( void )
	{
	return ( iterator( this, _size ) );
	}

template<typename value_type>
typename HArray<value_type>::const_iterator HArray<value_type>::begin( void ) const
	{
	return ( const_iterator( this, 0 ) );
	}

template<typename value_type>
typename HArray<value_type>::const_iterator HArray<value_type>::find( int long const& idx ) const
	{
	return ( const_iterator( this, min( idx, _size ) ) );
	}

template<typename value_type>
typename HArray<value_type>::const_iterator HArray<value_type>::end( void ) const
	{
	return ( const_iterator( this, _size ) );
	}

template<typename value_type>
void HArray<value_type>::swap( HArray& other )
	{
	if ( &other != this )
		{
		using yaal::swap;
		swap( _buf, other._buf );
		swap( _size, other._size );
		}
	return;
	}

template<typename value_type>
void HArray<value_type>::push_back( value_type const& val_ )
	{
	M_PROLOG
	M_ASSERT( _size <= get_capacity() );
	if ( _size == get_capacity() )
		reserve( _size ? _size * 2 : 1 );
	value_t* arr( _buf.get<value_t>() );
	new ( arr + _size ) value_t( val_ );
	++ _size;
	return;
	M_EPILOG
	}

}

}

#endif /* YAAL_HCORE_HARRAY_HXX_INCLUDED */

