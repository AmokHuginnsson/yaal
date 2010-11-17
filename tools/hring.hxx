/*
---           yaal 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	tools/hring.hxx - this file is integral part of yaal project.

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
/*! \file tools/hring.hxx
 * \brief Implementation of HRing<>.
 */

#ifndef YAAL_TOOLS_HRING_HXX_INCLUDED
#define YAAL_TOOLS_HRING_HXX_INCLUDED 1

#include <new>

#include "hcore/base.hxx"
#include "hcore/hchunk.hxx"
#include "hcore/algorithm.hxx"
#include "hcore/pod.hxx"

namespace yaal
{

namespace tools 
{

extern M_YAAL_TOOLS_PUBLIC_API char const* const _errMsgHRing_[];

namespace ring
{

struct capacity_type {};
struct size_type {};

typedef explicit_type<capacity_type, int long> capacity;
typedef explicit_type<size_type, int long> size;

}

/*! \brief HRing<> - a cyclic (aka circular) container.
 */
template<typename type_t>
class HRing
	{
public:
	typedef HRing<type_t> this_type;
	typedef type_t value_type;
	typedef ring::capacity capacity_type;
	typedef ring::size size_type;
	/*! \brief Error codes for HRing<> operations.
	 */
	struct ERROR
		{
		/*! \brief Error codes for HRing<> operations.
		 */
		typedef enum
			{
			OK = 0,           /*!< No error. */
			BAD_SIZE,         /*!< Bad size requested. */
			BAD_INDEX,        /*!< Index of of bounds. */
			INVALID_ITERATOR, /*!< iterator used for operation is not valid */
			RING_IS_FULL,     /*!< cannot expand ring any further as it is already full */
			RING_IS_EMPTY     /*!< cannot get data from ring as it is empty now */
			} error_t;
		};
private:
	yaal::hcore::HChunk _buf;
	int long _start;
	int long _size;
public:
	template<typename const_qual_t>
	class HIterator;
	typedef HIterator<type_t> iterator;
	typedef HIterator<type_t const> const_iterator;
	typedef yaal::hcore::HReverseIterator<iterator> reverse_iterator;
	typedef yaal::hcore::HReverseIterator<const_iterator> const_reverse_iterator;
	HRing( void );
	explicit HRing( capacity_type );
	HRing( capacity_type, size_type, type_t const& );
	template<typename iterator_t>
	HRing( capacity_type, iterator_t, iterator_t );
	virtual ~HRing( void );
	HRing( HRing const& );
	HRing& operator = ( HRing const& );
	void resize( capacity_type, size_type, type_t const& = value_type() );
	void reserve( int long );
	type_t& operator [] ( int long );
	type_t const& operator [] ( int long ) const;
	void push_back( type_t const& );
	void pop_back( void );
	void push_front( type_t const& );
	void pop_front( void );
	type_t& front( void );
	type_t const& front( void ) const;
	type_t& back( void );
	type_t const& back( void ) const;
	int long get_size( void ) const;
	int long size( void ) const;
	int long get_capacity( void ) const;
	int long capacity( void ) const;
	bool is_empty( void ) const;
	bool empty( void ) const;
	bool is_full( void ) const;
	bool full( void ) const;
	void clear( void );
	bool operator ! ( void ) const;
	iterator insert( iterator, type_t const& );
	template<typename iterator_t>
	void assign( iterator_t, iterator_t );
	void assign( int long, type_t const& );
	template<typename iterator_t>
	void insert( iterator, iterator_t, iterator_t );
	void insert( iterator, int long, type_t const& );
	iterator erase( iterator );
	iterator erase( iterator, iterator );
	iterator begin( void );
	iterator find( int long );
	iterator end( void );
	const_iterator begin( void ) const;
	const_iterator find( int long ) const;
	const_iterator end( void ) const;
	reverse_iterator rbegin( void );
	reverse_iterator rend( void );
	const_reverse_iterator rbegin( void ) const;
	const_reverse_iterator rend( void ) const;
	void swap( HRing& );
	bool operator == ( HRing const& ) const;
	bool operator < ( HRing const& ) const;
private:
	template<typename iterator_t>
	void initialize( capacity_type, iterator_t, iterator_t, trait::false_type const* );
	void initialize( capacity_type, size_type, type_t const&, trait::true_type const* );
	void insert_space( int long, int long );
	};

}

template<typename type_t>
inline void swap( yaal::tools::HRing<type_t>& a, yaal::tools::HRing<type_t>& b )
	{ a.swap( b ); }

namespace tools 
{

/*! \brief Iterator for HRing<> data structure.
 */
template<typename type_t>
template<typename const_qual_t>
class HRing<type_t>::HIterator : public yaal::hcore::iterator_interface<const_qual_t>
	{
	typedef HRing<type_t> array_t;
	array_t const* _owner;
	int long _index;
public:
	typedef type_t value_type;
	HIterator( void ) : yaal::hcore::iterator_interface<const_qual_t>(), _owner( NULL ), _index( 0 ) {}
	HIterator( HIterator const& it_ ) : yaal::hcore::iterator_interface<const_qual_t>(), _owner( it_._owner ), _index( it_._index ) {}
	template<typename other_const_qual_t>
	HIterator( HIterator<other_const_qual_t> const& it_ ) : yaal::hcore::iterator_interface<const_qual_t>(), _owner( it_._owner ), _index( it_._index )
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
	HIterator operator + ( int long off_ ) const
		{
		HIterator it( _owner, _index + off_ );
		return ( it );
		}
	HIterator& operator += ( int long off_ )
		{
		_index += off_;
		return ( *this );
		}
	HIterator operator - ( int long off_ ) const
		{
		HIterator it( _owner, _index - off_ );
		return ( it );
		}
	HIterator& operator -= ( int long off_ )
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
		{
		M_ASSERT( _owner && ( _index >= 0 ) && ( _index < _owner->_size ) );
		int long idx( _index + _owner->_start );
		int long capacity( _owner->capacity() );
		if ( idx >= capacity )
			idx -= capacity;
		return ( const_cast<const_qual_t&>( _owner->_buf.template get<const_qual_t>()[ idx ] ) );
		}
	const_qual_t& operator* ( void ) const
		{
		M_ASSERT( _owner && ( _index >= 0 ) && ( _index < _owner->_size ) );
		int long idx( _index + _owner->_start );
		int long capacity( _owner->capacity() );
		if ( idx >= capacity )
			idx -= capacity;
		return ( _owner->_buf.template get<const_qual_t>()[ idx ] );
		}
	const_qual_t* operator-> ( void )
		{
		M_ASSERT( _owner && ( _index >= 0 ) && ( _index < _owner->_size ) );
		int long idx( _index + _owner->_start );
		int long capacity( _owner->capacity() );
		if ( idx >= capacity )
			idx -= capacity;
		return ( const_cast<const_qual_t*>( &_owner->_buf.template get<const_qual_t>()[ idx ] ) );
		}
	const_qual_t* operator-> ( void ) const
		{
		M_ASSERT( _owner && ( _index >= 0 ) && ( _index < _owner->_size ) );
		int long idx( _index + _owner->_start );
		int long capacity( _owner->capacity() );
		if ( idx >= capacity )
			idx -= capacity;
		return ( &_owner->_buf.template get<const_qual_t>()[ idx ] );
		}
	template<typename other_const_qual_t>
	bool operator == ( HIterator<other_const_qual_t> const& it ) const
		{
		M_ASSERT( _owner == it._owner );
		return ( _index == it._index );
		}
	template<typename other_const_qual_t>
	bool operator != ( HIterator<other_const_qual_t> const& it ) const
		{
		M_ASSERT( _owner == it._owner );
		return ( _index != it._index );
		}
private:
	friend class HRing<type_t>;
	explicit HIterator( array_t const* owner_, int long idx )
		: yaal::hcore::iterator_interface<const_qual_t>(), _owner( owner_ ), _index( idx ) {};
	};

template<typename type_t>
HRing<type_t>::HRing( void )
	: _buf(), _start( 0 ), _size( 0 )
	{
	return;
	}

template<typename type_t>
HRing<type_t>::HRing( capacity_type capacity_ )
	: _buf(), _start( 0 ), _size( 0 )
	{
	M_PROLOG
	reserve( capacity_._value );
	return;
	M_EPILOG
	}

template<typename type_t>
HRing<type_t>::HRing( capacity_type capacity_, size_type size_, type_t const& fillWith_ )
	: _buf(), _start( 0 ), _size( 0 )
	{
	M_PROLOG
	resize( capacity_, size_, fillWith_ );
	return;
	M_EPILOG
	}

template<typename type_t>
template<typename iterator_t>
HRing<type_t>::HRing( capacity_type capacity_, iterator_t first, iterator_t last )
	: _buf(), _start( 0 ), _size( 0 )
	{
	M_PROLOG
	initialize( capacity_, first, last, typename trait::make_pointer<typename is_integral<iterator_t>::type>::type() );
	return;
	M_EPILOG
	}

template<typename type_t>
HRing<type_t>::~HRing( void )
	{
	M_PROLOG
	clear();
	return;
	M_EPILOG
	}

template<typename type_t>
template<typename iterator_t>
void HRing<type_t>::initialize( capacity_type capacity_, iterator_t first, iterator_t last, trait::false_type const* )
	{
	M_PROLOG
	reserve( capacity_._value );
	insert( end(), first, last );
	return;
	M_EPILOG
	}

template<typename type_t>
void HRing<type_t>::initialize( capacity_type capacity_, size_type size_, type_t const& fillWith_, trait::true_type const* )
	{
	M_PROLOG
	resize( capacity_, size_, fillWith_ );
	return;
	M_EPILOG
	}

template<typename type_t>
template<typename iterator_t>
void HRing<type_t>::assign( iterator_t first, iterator_t last )
	{
	M_PROLOG
	clear();
	insert( end(), first, last );
	return;
	M_EPILOG
	}

template<typename type_t>
void HRing<type_t>::assign( int long size_, type_t const& fillWith_ )
	{
	M_PROLOG
	int long oldSize( _size );
	resize( ring::capacity( get_capacity() ), ring::size( size_ ), fillWith_ );
	if ( oldSize > 0 )
		fill_n( begin(), oldSize, fillWith_ );
	return;
	M_EPILOG
	}

template<typename type_t>
void HRing<type_t>::clear( void )
	{
	M_PROLOG
	erase( begin(), end() );
	return;
	M_EPILOG
	}

template<typename type_t>
HRing<type_t>::HRing( HRing const& arr_ )
	: _buf(), _start( 0 ), _size( 0 )
	{
	M_PROLOG
	if ( arr_._size > 0 )
		{ 
		_buf.realloc( yaal::hcore::chunk_size<value_type>( arr_._size ) );
		_size = arr_._size;
		int long curCapacity( arr_.get_capacity() );
		value_type* dst( _buf.get<value_type>() );
		value_type const* src( arr_._buf.template get<value_type>() );
		for ( int long i( 0 ); i < _size; ++ i )
			{
			int long idx( i + arr_._start );
			if ( idx >= curCapacity )
				idx -= curCapacity;
			new ( dst + i ) value_type( src[ idx ] );
			}
		}
	return;
	M_EPILOG
	}

template<typename type_t>
HRing<type_t>& HRing<type_t>::operator = ( HRing const& arr_ )
	{
	M_PROLOG
	if ( &arr_ != this )
		{
		if ( arr_._size > get_capacity() )
			{
			HRing<value_type> tmp( arr_ );
			swap( tmp );
			}
		else
			{
			clear();
			value_type* dst( _buf.get<value_type>() );
			value_type const* src( arr_._buf.template get<value_type>() );
			int long curCapacity( arr_.get_capacity() );
			for ( int long i( 0 ); i < arr_._size; ++ i )
				{
				int long idx( i + arr_._start );
				if ( idx >= curCapacity )
					idx -= curCapacity;
				new ( dst + i ) value_type( src[ idx ] );
				}
			_size = arr_._size;
			_start = 0;
			}
		}
	return ( *this );
	M_EPILOG
	}

template<typename type_t>
void HRing<type_t>::resize( capacity_type capacity_, size_type size_, type_t const& fillWith_ )
	{
	M_PROLOG
	if ( size_._value < 0 )
		M_THROW( _errMsgHRing_[ ERROR::BAD_SIZE ], size_._value );
	if ( size_._value > capacity_._value )
		M_THROW( _errMsgHRing_[ ERROR::BAD_SIZE ], capacity_._value - size_._value );
	if ( size_._value > _size )
		{
		reserve( capacity_._value );
		int long curCapacity( get_capacity() );
		value_type* arr( _buf.get<value_type>() );
		for ( int long i( _size ); i < size_._value; ++ i )
			{
			int long idx( i + _start );
			if ( idx >= curCapacity )
				idx -= curCapacity;
			new ( arr + idx ) value_type( fillWith_ );
			}
		}
	else if ( size_._value < _size )
		{
		int long curCapacity( get_capacity() );
		value_type* arr( _buf.get<value_type>() );
		for ( int long i( size_._value ); i < _size; ++ i )
			{
			int long idx( i + _start );
			if ( idx >= curCapacity )
				idx -= curCapacity;
			arr[ idx ].~value_type();
			}
		}
	_size = size_._value;
	return;
	M_EPILOG
	}

template<typename type_t>
void HRing<type_t>::reserve( int long capacity_ )
	{
	M_PROLOG
	if ( capacity_ < 0 )
		M_THROW( _errMsgHRing_[ ERROR::BAD_SIZE ], capacity_ );
	int long curCapacity( get_capacity() );
	if ( capacity_ > curCapacity )
		{
		yaal::hcore::HChunk newBuf( yaal::hcore::chunk_size<value_type>( max( capacity_, curCapacity * 2 ) ) );
		value_type* dst( newBuf.get<value_type>() );
		value_type* src( _buf.get<value_type>() );
		for ( int long i( 0 ); i < _size; ++ i )
			{
			int long idx( i + _start );
			if ( idx >= curCapacity )
				idx -= curCapacity;
			new ( dst + i ) value_type( src[ idx ] );
			}
		for ( int long i( 0 ); i < _size; ++ i )
			{
			int long idx( i + _start );
			if ( idx >= curCapacity )
				idx -= curCapacity;
			src[ idx ].~value_type();
			}
		_buf.swap( newBuf );
		_start = 0;
		}
	return;
	M_EPILOG
	}

template<typename type_t>
template<typename iterator_t>
void HRing<type_t>::insert( iterator pos_, iterator_t first_, iterator_t last_ )
	{
	M_PROLOG
	M_ASSERT( pos_._owner == this );
	if ( ( pos_._index < 0 ) && ( pos_._index > _size ) )
		M_THROW( _errMsgHRing_[ ERROR::INVALID_ITERATOR ], pos_._index );
	int long curCapacity( get_capacity() );
	int long elemCount( distance( first_, last_ ) );
	M_ENSURE_EX( ( _size + elemCount ) <= curCapacity, _errMsgHRing_[ ERROR::BAD_SIZE ] );
	insert_space( pos_._index, elemCount );
	value_type* arr( _buf.get<value_type>() );
	for ( int long i( pos_._index + _start ); first_ != last_; ++ first_,  ++ i )
		{
		int long idx( i >= curCapacity ? i - curCapacity : i );
		new ( arr + idx ) value_type( *first_ );
		}
	return;
	M_EPILOG
	}

template<typename type_t>
void HRing<type_t>::insert( iterator pos_, int long count_, type_t const& value_ )
	{
	M_PROLOG
	M_ASSERT( pos_._owner == this );
	if ( ( pos_._index < 0 ) && ( pos_._index > _size ) )
		M_THROW( _errMsgHRing_[ ERROR::INVALID_ITERATOR ], pos_._index );
	int long curCapacity( get_capacity() );
	M_ENSURE_EX( ( _size + count_ ) <= curCapacity, _errMsgHRing_[ ERROR::BAD_SIZE ] );
	insert_space( pos_._index, count_ );
	value_type* arr( _buf.get<value_type>() );
	for ( int long i( pos_._index + _start ), last( pos_._index + count_ ); i < last; ++ i )
		{
		int long idx( i >= curCapacity ? i - curCapacity : i );
		new ( arr + i ) value_type( value_ );
		}
	return;
	M_EPILOG
	}

template<typename type_t>
typename HRing<type_t>::iterator HRing<type_t>::insert( iterator pos_, type_t const& value_ )
	{
	M_PROLOG
	M_ASSERT( pos_._owner == this );
	if ( ( pos_._index < 0 ) && ( pos_._index > _size ) )
		M_THROW( _errMsgHRing_[ ERROR::INVALID_ITERATOR ], pos_._index );
	int long curCapacity( get_capacity() );
	M_ENSURE_EX( ( _size + 1 ) <= curCapacity, _errMsgHRing_[ ERROR::BAD_SIZE ] );
	insert_space( pos_._index, 1 );
	value_type* arr( _buf.get<value_type>() );
	int long idx( ( pos_._index + _start ) >= curCapacity ? pos_._index + _start - curCapacity : pos_._index + _start );
	new ( arr + idx ) value_type( value_ );
	return ( pos_ );
	M_EPILOG
	}

template<typename type_t>
void HRing<type_t>::insert_space( int long pos_, int long size_ )
	{
	M_PROLOG
	int long curCapacity( get_capacity() );
	M_ASSERT( ( _size + size_ ) <= curCapacity );
	int long oldSize( _size );
	_size += size_;
	value_type* arr( _buf.get<value_type>() );
	if ( pos_ <= ( _size / 2 ) )
		{
		_start -= size_;
		if ( _start < 0 )
			_start += curCapacity;
		for ( int long src( _start + size_ ), dst( _start ); src < ( _start + size_ + pos_ ); ++ src, ++ dst )
			{
			int long srcIdx( src >= curCapacity ? src - curCapacity : src );
			int long dstIdx( dst >= curCapacity ? dst - curCapacity : dst );
			new ( arr + dstIdx ) value_type( arr[ srcIdx ] );
			arr[ srcIdx ].~value_type();
			}
		}
	else
		{
		for ( int long src( _start + oldSize - 1 ), dst( _start + _size - 1 ); src >= ( pos_ + _start ); -- src, -- dst )
			{
			int long srcIdx( src >= curCapacity ? src - curCapacity : src );
			int long dstIdx( dst >= curCapacity ? dst - curCapacity : dst );
			new ( arr + dstIdx ) value_type( arr[ srcIdx ] );
			arr[ srcIdx ].~value_type();
			}
		}
	return;
	M_EPILOG
	}

template<typename type_t>
typename HRing<type_t>::iterator HRing<type_t>::erase( iterator first_, iterator last_ )
	{
	M_PROLOG
	M_ASSERT( first_._owner == this );
	M_ASSERT( last_._owner == this );
	if ( ( first_._index < 0 ) && ( first_._index > _size ) )
		M_THROW( _errMsgHRing_[ ERROR::INVALID_ITERATOR ], first_._index );
	if ( ( last_._index < 0 ) && ( last_._index > _size ) )
		M_THROW( _errMsgHRing_[ ERROR::INVALID_ITERATOR ], last_._index );
	if ( last_._index < first_._index )
		M_THROW( _errMsgHRing_[ ERROR::INVALID_ITERATOR ], last_._index - first_._index );
	if ( first_ != last_ )
		{
		if ( first_._index < ( _size - last_._index ) )
			{
			for ( iterator it( begin() ), endIt( copy_backward( begin(), first_, last_ ) ); it != endIt; ++ it )
				(*it).~value_type();
			_start += ( last_._index - first_._index );
			}
		else
			{
			for ( iterator it( copy( last_, end(), first_ ) ), endIt( end() ); ( it != endIt ); ++ it )
				(*it).~value_type();
			}
		}
	_size -= ( last_._index - first_._index );
	return ( last_._index < _size ? last_ : end() );
	M_EPILOG
	}

template<typename type_t>
typename HRing<type_t>::iterator HRing<type_t>::erase( iterator it )
	{
	M_PROLOG
	iterator next( it );
	++ next;
	return ( erase( it, next ) );
	M_EPILOG
	}

template<typename type_t>
type_t const& HRing<type_t>::front( void ) const
	{
	M_PROLOG
	M_ASSERT( _size > 0 );
	return ( _buf.get<value_type const>()[ _start ] );
	M_EPILOG
	}

template<typename type_t>
type_t& HRing<type_t>::front( void )
	{
	M_PROLOG
	M_ASSERT( _size > 0 );
	return ( _buf.get<value_type>()[ _start ] );
	M_EPILOG
	}

template<typename type_t>
type_t const& HRing<type_t>::back( void ) const
	{
	M_PROLOG
	M_ASSERT( _size > 0 );
	int long curCapacity( get_capacity() );
	M_ASSERT( _size <= curCapacity );
	int long idx( _start + _size - 1 );
	if ( idx >= curCapacity )
		idx -= curCapacity;
	value_type* arr( _buf.get<value_type>() );
	return ( arr[ idx ] );
	M_EPILOG
	}

template<typename type_t>
type_t& HRing<type_t>::back( void )
	{
	M_PROLOG
	M_ASSERT( _size > 0 );
	int long curCapacity( get_capacity() );
	M_ASSERT( _size <= curCapacity );
	int long idx( _start + _size - 1 );
	if ( idx >= curCapacity )
		idx -= curCapacity;
	value_type* arr( _buf.get<value_type>() );
	return ( arr[ idx ] );
	M_EPILOG
	}

template<typename type_t>
type_t& HRing<type_t>::operator[] ( int long index_ )
	{
	M_PROLOG
	int long idx = ( index_ < 0 ) ? index_ + _size : index_;
	if ( ( idx >= _size ) || ( idx < 0 ) )
		M_THROW( _errMsgHRing_[ ERROR::BAD_INDEX ], idx );
	int long cap( capacity() );
	idx += _start;
	if ( idx >= cap )
		idx -= cap;
	return ( _buf.get<value_type>()[ idx ] );
	M_EPILOG
	}

template<typename type_t>
type_t const& HRing<type_t>::operator[] ( int long index_ ) const
	{
	M_PROLOG
	int long idx( ( index_ < 0 ) ? index_ + _size : index_ );
	if ( ( idx >= _size ) || ( idx < 0 ) )
		M_THROW( _errMsgHRing_[ ERROR::BAD_INDEX ], idx );
	int long cap( capacity() );
	idx += _start;
	if ( idx >= cap )
		idx -= cap;
	return ( _buf.get<value_type const>()[ idx ] );
	M_EPILOG
	}

template<typename type_t>
bool HRing<type_t>::operator ! ( void ) const
	{
	M_PROLOG
	return ( ! _buf.get<type_t>() );
	M_EPILOG
	}

template<typename type_t>
int long HRing<type_t>::get_size( void ) const
	{
	M_PROLOG
	return ( _size );
	M_EPILOG
	}

template<typename type_t>
int long HRing<type_t>::size( void ) const
	{
	M_PROLOG
	return ( _size );
	M_EPILOG
	}

template<typename type_t>
bool HRing<type_t>::is_empty( void ) const
	{
	M_PROLOG
	return ( _size ? false : true );
	M_EPILOG
	}

template<typename type_t>
bool HRing<type_t>::empty( void ) const
	{
	M_PROLOG
	return ( _size ? false : true );
	M_EPILOG
	}

template<typename type_t>
bool HRing<type_t>::is_full( void ) const
	{
	M_PROLOG
	return ( _size == capacity() );
	M_EPILOG
	}

template<typename type_t>
bool HRing<type_t>::full( void ) const
	{
	M_PROLOG
	return ( is_full() );
	M_EPILOG
	}

template<typename type_t>
int long HRing<type_t>::capacity( void ) const
	{
	return ( get_capacity() );
	}

template<typename type_t>
int long HRing<type_t>::get_capacity( void ) const
	{
	return ( _buf.count_of<value_type>() );
	}

template<typename type_t>
typename HRing<type_t>::iterator HRing<type_t>::begin( void )
	{
	return ( iterator( this, 0 ) );
	}

template<typename type_t>
typename HRing<type_t>::iterator HRing<type_t>::find( int long idx )
	{
	return ( iterator( this, min( idx, _size ) ) );
	}

template<typename type_t>
typename HRing<type_t>::iterator HRing<type_t>::end( void )
	{
	return ( iterator( this, _size ) );
	}

template<typename type_t>
typename HRing<type_t>::const_iterator HRing<type_t>::begin( void ) const
	{
	return ( const_iterator( this, 0 ) );
	}

template<typename type_t>
typename HRing<type_t>::const_iterator HRing<type_t>::find( int long idx ) const
	{
	return ( const_iterator( this, min( idx, _size ) ) );
	}

template<typename type_t>
typename HRing<type_t>::const_iterator HRing<type_t>::end( void ) const
	{
	return ( const_iterator( this, _size ) );
	}

template<typename type_t>
typename HRing<type_t>::reverse_iterator HRing<type_t>::rbegin( void )
	{
	return ( end() );
	}

template<typename type_t>
typename HRing<type_t>::reverse_iterator HRing<type_t>::rend( void )
	{
	return ( begin() );
	}

template<typename type_t>
typename HRing<type_t>::const_reverse_iterator HRing<type_t>::rbegin( void ) const
	{
	return ( end() );
	}

template<typename type_t>
typename HRing<type_t>::const_reverse_iterator HRing<type_t>::rend( void ) const
	{
	return ( begin() );
	}

template<typename type_t>
void HRing<type_t>::swap( HRing& other )
	{
	M_PROLOG
	if ( &other != this )
		{
		using yaal::swap;
		swap( _buf, other._buf );
		swap( _start, other._start );
		swap( _size, other._size );
		}
	return;
	M_EPILOG
	}

template<typename type_t>
void HRing<type_t>::push_back( type_t const& val_ )
	{
	M_PROLOG
	int long curCapacity( get_capacity() );
	M_ASSERT( _size <= curCapacity );
	M_ENSURE_EX( _size < curCapacity, _errMsgHRing_[ ERROR::RING_IS_FULL ] );
	value_type* arr( _buf.get<value_type>() );
	int long idx( _start + _size );
	if ( idx >= curCapacity )
		idx -= curCapacity;
	new ( arr + idx ) value_type( val_ );
	++ _size;
	return;
	M_EPILOG
	}

template<typename type_t>
void HRing<type_t>::push_front( type_t const& val_ )
	{
	M_PROLOG
	int long curCapacity( get_capacity() );
	M_ASSERT( _size <= curCapacity );
	M_ENSURE_EX( _size < curCapacity, _errMsgHRing_[ ERROR::RING_IS_FULL ] );
	-- _start;
	if ( _start < 0 )
		_start = curCapacity - 1;
	value_type* arr( _buf.get<value_type>() );
	new ( arr + _start ) value_type( val_ );
	++ _size;
	return;
	M_EPILOG
	}

template<typename type_t>
void HRing<type_t>::pop_back( void )
	{
	M_PROLOG
	M_ENSURE_EX( _size > 0, _errMsgHRing_[ ERROR::RING_IS_EMPTY ] );
	-- _size;
	int long cap( capacity() );
	int long idx( _start + _size );
	if ( idx >= cap )
		idx -= cap;
	value_type* arr( _buf.get<value_type>() );
	arr[ idx ].~value_type();
	return;
	M_EPILOG
	}

template<typename type_t>
void HRing<type_t>::pop_front( void )
	{
	M_PROLOG
	M_ENSURE_EX( _size > 0, _errMsgHRing_[ ERROR::RING_IS_EMPTY ] );
	value_type* arr( _buf.get<value_type>() );
	arr[ _start ++ ].~value_type();
	if ( _start == capacity() )
		_start = 0;
	-- _size;
	return;
	M_EPILOG
	}

template<typename type_t>
bool HRing<type_t>::operator == ( HRing const& a_ ) const
	{
	M_PROLOG
	return ( ( &a_ == this ) || safe_equal( begin(), end(), a_.begin(), a_.end() ) );
	M_EPILOG
	}

template<typename type_t>
bool HRing<type_t>::operator < ( HRing const& a_ ) const
	{
	M_PROLOG
	return ( ( &a_ != this ) && lexicographical_compare( begin(), end(), a_.begin(), a_.end() ) );
	M_EPILOG
	}


}

}

#endif /* #ifndef YAAL_TOOLS_HRING_HXX_INCLUDED */

