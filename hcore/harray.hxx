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
#include "hcore/pod.hxx"

namespace yaal
{

namespace hcore
{

extern char const* const _errMsgHArray_[];

/*! \brief Simplest compund data structure.
 */
template<typename type_t>
class HArray
	{
	typedef HArray<type_t> self_t;
public:
	typedef type_t value_type;
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
private:
	HChunk _buf;
	int long _size;
public:
	template<typename const_qual_t>
	class HIterator;
	typedef HIterator<type_t> iterator;
	typedef HIterator<type_t const> const_iterator;
	HArray( void );
	explicit HArray( int long const& );
	HArray( int long const&, type_t const& );
	template<typename iterator_t>
	HArray( iterator_t, iterator_t );
	virtual ~HArray( void );
	HArray( HArray const& );
	HArray& operator = ( HArray const& );
	void resize( int long const&, type_t const& = value_type() );
	void reserve( int long const& );
	type_t& operator [] ( int long const& );
	type_t const& operator [] ( int long const& ) const;
	void push_back( type_t const& );
	void pop_back( void );
	int long get_size( void ) const;
	int long size( void ) const;
	int long get_capacity( void ) const;
	int long capacity( void ) const;
	bool empty( void ) const;
	bool is_empty( void ) const;
	void clear( void );
	bool operator ! ( void ) const;
	iterator insert( iterator, type_t const& );
	template<typename iterator_t>
	void insert( iterator, iterator_t, iterator_t );
	void insert( iterator, int long, type_t const& );
	iterator erase( iterator );
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
private:
	template<typename iterator_t>
	void initialize( iterator_t, iterator_t, trait::false_type const* );
	void initialize( int long const&, type_t const&, trait::true_type const* );
	};

}

template<typename type_t>
inline void swap( yaal::hcore::HArray<type_t>& a, yaal::hcore::HArray<type_t>& b )
	{ a.swap( b ); }

namespace hcore
{

/*! \brief Iterator for HArray<> data structure.
 */
template<typename type_t>
template<typename const_qual_t>
class HArray<type_t>::HIterator
	{
	typedef HArray<type_t> array_t;
	array_t const* _owner;
	int long _index;
public:
	typedef type_t value_type;
	HIterator( void ) : _owner( NULL ), _index( 0 ) {}
	template<typename other_const_qual_t>
	HIterator( HIterator<other_const_qual_t> const& it_ ) : _owner( it_._owner ), _index( it_._index )
		{
		STATIC_ASSERT(( trait::same_type<const_qual_t, other_const_qual_t>::value || trait::same_type<const_qual_t, other_const_qual_t const>::value ));
		}
	HIterator& operator= ( HIterator const& it_ )
		{
		if ( &it_ != this )
			{
			_owner = it_._owner;
			_index = it_._index;
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
	friend class HArray<type_t>;
	explicit HIterator( array_t const* owner_, int long const& idx )
		: _owner( owner_ ), _index( idx ) {};
	};

template<typename type_t>
HArray<type_t>::HArray( void )
	: _buf(), _size( 0 )
	{
	return;
	}

template<typename type_t>
HArray<type_t>::HArray( int long const& size_ )
	: _buf(), _size( 0 )
	{
	M_PROLOG
	resize( size_ );
	return;
	M_EPILOG
	}

template<typename type_t>
HArray<type_t>::HArray( int long const& size_, type_t const& fillWith_ )
	: _buf(), _size( 0 )
	{
	M_PROLOG
	resize( size_, fillWith_ );
	return;
	M_EPILOG
	}

template<typename type_t>
template<typename iterator_t>
HArray<type_t>::HArray( iterator_t first, iterator_t last )
	: _buf(), _size( 0 )
	{
	M_PROLOG
	initialize( first, last, typename trait::make_pointer<typename is_integral<iterator_t>::type>::type() );
	return;
	M_EPILOG
	}

template<typename type_t>
HArray<type_t>::~HArray( void )
	{
	M_PROLOG
	clear();
	return;
	M_EPILOG
	}

template<typename type_t>
template<typename iterator_t>
void HArray<type_t>::initialize( iterator_t first, iterator_t last, trait::false_type const* )
	{
	M_PROLOG
	insert( end(), first, last );
	return;
	M_EPILOG
	}

template<typename type_t>
void HArray<type_t>::initialize( int long const& size_, type_t const& fillWith_, trait::true_type const* )
	{
	M_PROLOG
	resize( size_, fillWith_ );
	return;
	M_EPILOG
	}

template<typename type_t>
void HArray<type_t>::clear( void )
	{
	M_PROLOG
	erase( begin(), end() );
	return;
	M_EPILOG
	}

template<typename type_t>
HArray<type_t>::HArray( HArray const& arr_ )
	: _buf(), _size( 0 )
	{
	M_PROLOG
	if ( arr_._size > 0 )
		{ 
		_buf.realloc( chunk_size<value_type>( arr_._size ) );
		_size = arr_._size;
		value_type* dst( _buf.get<value_type>() );
		value_type const* src( arr_._buf.template get<value_type>() );
		for ( int long i = 0; i < _size; ++ i )
			new ( dst + i ) value_type( src[ i ] );
		}
	return;
	M_EPILOG
	}

template<typename type_t>
HArray<type_t>& HArray<type_t>::operator = ( HArray const& arr_ )
	{
	M_PROLOG
	if ( &arr_ != this )
		{
		if ( arr_._size > get_capacity() )
			{
			HArray<value_type> tmp( arr_ );
			swap( tmp );
			}
		else
			{
			value_type* dst( _buf.get<value_type>() );
			value_type const* src( arr_._buf.template get<value_type>() );
			copy_n( src, _size, dst );
			if ( arr_._size > _size )
				{
				for ( int long i = _size; i < arr_._size; ++ i )
					new ( dst + i ) value_type( src[ i ] );
				}
			else if ( arr_._size < _size )
				{
				for ( int long i = arr_._size; i < _size; ++ i )
					dst[ i ].~value_type();
				}
			_size = arr_._size;
			}
		}
	return ( *this );
	M_EPILOG
	}

template<typename type_t>
void HArray<type_t>::resize( int long const& size_, type_t const& fillWith_ )
	{
	M_PROLOG
	if ( size_ < 0 )
		M_THROW( _errMsgHArray_[ ERROR::BAD_SIZE ], size_ );
	if ( size_ > _size )
		{
		reserve( size_ );
		value_type* arr( _buf.get<value_type>() );
		for ( int long i = _size; i < size_; ++ i )
			new ( arr + i ) value_type( fillWith_ );
		}
	else if ( size_ < _size )
		{
		value_type* arr( _buf.get<value_type>() );
		for ( int long i = size_; i < _size; ++ i )
			arr[ i ].~value_type();
		}
	_size = size_;
	return;
	M_EPILOG
	}

template<typename type_t>
void HArray<type_t>::reserve( int long const& capacity_ )
	{
	M_PROLOG
	if ( capacity_ < 0 )
		M_THROW( _errMsgHArray_[ ERROR::BAD_SIZE ], capacity_ );
	int long curCapacity( get_capacity() );
	if ( capacity_ > curCapacity )
		{
		HChunk newBuf( chunk_size<value_type>( max( capacity_, curCapacity * 2 ) ) );
		value_type* dst( newBuf.get<value_type>() );
		value_type* src( _buf.get<value_type>() );
		for ( int long i = 0; i < _size; ++ i )
			new ( dst + i ) value_type( src[ i ] );
		for ( int long i = 0; i < _size; ++ i )
			src[ i ].~value_type();
		_buf.swap( newBuf );
		}
	return;
	M_EPILOG
	}

template<typename type_t>
template<typename iterator_t>
void HArray<type_t>::insert( iterator pos, iterator_t first, iterator_t last )
	{
	M_PROLOG
	for ( ; first != last; ++ first )
		pos = insert( pos, *first );
	return;
	M_EPILOG
	}

template<typename type_t>
typename HArray<type_t>::iterator HArray<type_t>::erase( iterator first, iterator last )
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
		first->~value_type();
	_size -= removed;
	return ( last._index < _size ? last : end() );
	M_EPILOG
	}

template<typename type_t>
typename HArray<type_t>::iterator HArray<type_t>::erase( iterator it )
	{
	M_PROLOG
	iterator next( it );
	++ next;
	return ( erase( it, next ) );
	M_EPILOG
	}

template<typename type_t>
type_t& HArray<type_t>::operator[] ( int long const& index_ )
	{
	M_PROLOG
	int long idx = ( index_ < 0 ) ? index_ + _size : index_;
	if ( ( idx >= _size ) || ( idx < 0 ) )
		M_THROW( _errMsgHArray_[ ERROR::BAD_INDEX ], idx );
	return ( _buf.get<value_type>()[ idx ] );
	M_EPILOG
	}

template<typename type_t>
type_t const& HArray<type_t>::operator[] ( int long const& index_ ) const
	{
	M_PROLOG
	int long idx( ( index_ < 0 ) ? index_ + _size : index_ );
	if ( ( idx >= _size ) || ( idx < 0 ) )
		M_THROW( _errMsgHArray_[ ERROR::BAD_INDEX ], idx );
	return ( _buf.get<value_type const>()[ idx ] );
	M_EPILOG
	}

template<typename type_t>
bool HArray<type_t>::operator ! ( void ) const
	{
	M_PROLOG
	return ( ! _buf.get<type_t>() );
	M_EPILOG
	}

template<typename type_t>
int long HArray<type_t>::get_size( void ) const
	{
	M_PROLOG
	return ( _size );
	M_EPILOG
	}

template<typename type_t>
int long HArray<type_t>::size( void ) const
	{
	M_PROLOG
	return ( _size );
	M_EPILOG
	}

template<typename type_t>
bool HArray<type_t>::is_empty( void ) const
	{
	M_PROLOG
	return ( _size ? false : true );
	M_EPILOG
	}

template<typename type_t>
bool HArray<type_t>::empty( void ) const
	{
	M_PROLOG
	return ( _size ? false : true );
	M_EPILOG
	}

template<typename type_t>
int long HArray<type_t>::capacity( void ) const
	{
	return ( get_capacity() );
	}

template<typename type_t>
int long HArray<type_t>::get_capacity( void ) const
	{
	return ( _buf.size() / sizeof ( value_type ) );
	}

template<typename type_t>
typename HArray<type_t>::iterator HArray<type_t>::begin( void )
	{
	return ( iterator( this, 0 ) );
	}

template<typename type_t>
typename HArray<type_t>::iterator HArray<type_t>::find( int long const& idx )
	{
	return ( iterator( this, min( idx, _size ) ) );
	}

template<typename type_t>
typename HArray<type_t>::iterator HArray<type_t>::end( void )
	{
	return ( iterator( this, _size ) );
	}

template<typename type_t>
typename HArray<type_t>::const_iterator HArray<type_t>::begin( void ) const
	{
	return ( const_iterator( this, 0 ) );
	}

template<typename type_t>
typename HArray<type_t>::const_iterator HArray<type_t>::find( int long const& idx ) const
	{
	return ( const_iterator( this, min( idx, _size ) ) );
	}

template<typename type_t>
typename HArray<type_t>::const_iterator HArray<type_t>::end( void ) const
	{
	return ( const_iterator( this, _size ) );
	}

template<typename type_t>
void HArray<type_t>::swap( HArray& other )
	{
	M_PROLOG
	if ( &other != this )
		{
		using yaal::swap;
		swap( _buf, other._buf );
		swap( _size, other._size );
		}
	return;
	M_EPILOG
	}

template<typename type_t>
void HArray<type_t>::push_back( type_t const& val_ )
	{
	M_PROLOG
	int long curCapacity( get_capacity() );
	M_ASSERT( _size <= curCapacity );
	if ( _size == curCapacity )
		reserve( curCapacity + 1 );
	value_type* arr( _buf.get<value_type>() );
	new ( arr + _size ) value_type( val_ );
	++ _size;
	return;
	M_EPILOG
	}

template<typename type_t>
void HArray<type_t>::pop_back( void )
	{
	M_PROLOG
	M_ASSERT( _size > 0 );
	value_type* arr( _buf.get<value_type>() );
	arr[ -- _size ].~value_type();
	return;
	M_EPILOG
	}

}

}

#endif /* YAAL_HCORE_HARRAY_HXX_INCLUDED */

