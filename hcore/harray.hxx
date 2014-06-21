/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	harray.hxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/
/*! \file hcore/harray.hxx
 * \brief Declaration of HArray<> class.
 */

#ifndef YAAL_HCORE_HARRAY_HXX_INCLUDED
#define YAAL_HCORE_HARRAY_HXX_INCLUDED 1

#include <new>

#include "hcore/base.hxx"
#include "hcore/allocator.hxx"
#include "hcore/algorithm.hxx"
#include "hcore/pod.hxx"

namespace yaal {

namespace hcore {

extern M_YAAL_HCORE_PUBLIC_API char const* const _errMsgHArray_[];

/*! \brief Simplest compund data structure.
 *
 * \tparam type_t - type of objects that will be stored in array.
 * \tparam allocator_t - type of internal allocator used for all array's allocations.
 */
template<typename type_t, typename allocator_t = allocator::system<type_t> >
class HArray {
public:
	typedef HArray<type_t, allocator_t> this_type;
	typedef type_t value_type;
	/*! \brief Error codes for HArray<> operations.
	 */
	struct ERROR {
		/*! \brief Error codes for HArray<> operations.
		 */
		typedef enum {
			OK = 0,          /*!< No error. */
			BAD_SIZE,        /*!< Bad size requested. */
			BAD_INDEX,       /*!< Index of of bounds. */
			INVALID_ITERATOR /*!< iterator used for operation is not valid */
		} error_t;
	};
	typedef allocator::system<type_t> allocator_type;
private:
	static allocator_type _allocator;
	value_type* _buf;
	int long _size;
	int long _capacity;
public:
	template<typename const_qual_t>
	class HIterator;
	typedef HIterator<type_t> iterator;
	typedef HIterator<type_t const> const_iterator;
	typedef HReverseIterator<iterator> reverse_iterator;
	typedef HReverseIterator<const_iterator> const_reverse_iterator;
	/*! \brief Construct empty array object.
	 */
	HArray( void )
		: _buf( NULL ), _size( 0 ), _capacity( 0 ) {
		return;
	}
	/*! \brief Construct empty array object using external allocator.
	 *
	 * \param allocator_ - external allocator that should be used for all array's allocations.
	 */
	explicit HArray( allocator_t const& )
		: _buf( NULL ), _size( 0 ), _capacity( 0 ) {
		return;
	}
	/*! \brief Construct array of given size.
	 *
	 * Array will be filled with default values of array's objects type.
	 *
	 * \param size_ - size for newly created array.
	 */
	explicit HArray( int long size_ )
		: _buf( NULL ), _size( 0 ), _capacity( 0 ) {
		M_PROLOG
		resize( size_ );
		return;
		M_EPILOG
	}
	/*! \brief Construct array of given size using external allocator.
	 *
	 * Array will be filled with default values of array's objects type.
	 *
	 * \param size_ - size for newly created array.
	 * \param allocator_ - external allocator that should be used for all array's allocations.
	 */
	HArray( int long size_, allocator_t const& )
		: _buf( NULL ), _size( 0 ), _capacity( 0 ) {
		M_PROLOG
		resize( size_ );
		return;
		M_EPILOG
	}
	/*! \brief Construct array of given size using explicit default value and external allocator.
	 *
	 * Array will be filled with objects of given value.
	 *
	 * \param size_ - size for newly created array.
	 * \param fillWith_ - value prototype that should be set for all objects in this newly created array.
	 * \param allocator_ - external allocator that should be used for all array's allocations.
	 */
	HArray( int long size_, type_t const& fillWith_, allocator_t const& = allocator_t() )
		: _buf( NULL ), _size( 0 ), _capacity( 0 ) {
		M_PROLOG
		resize( size_, fillWith_ );
		return;
		M_EPILOG
	}
	/*! \brief Construct array based on given range of objects and external allocator.
	 *
	 * \tparam iterator_t -  type of iterator used to specify input range.
	 * \param first - beginning of input range.
	 * \param last - one past the end of input range.
	 * \param allocator_ - external allocator that should be used for all array's allocations.
	 */
	template<typename iterator_t>
	HArray( iterator_t first, iterator_t last, allocator_t const& = allocator_t() )
		: _buf( NULL ), _size( 0 ), _capacity( 0 ) {
		M_PROLOG
		initialize( first, last, typename trait::add_pointer<typename is_integral<iterator_t>::type>::type() );
		return;
		M_EPILOG
	}
	/*! \brief Copy constructor.
	 *
	 * \param arr_ - array object that this new array should be a copy of.
	 */
	HArray( HArray const& arr_ )
		: _buf( NULL ), _size( 0 ), _capacity( 0 ) {
		M_PROLOG
		if ( arr_._size > 0 )
			{
			reserve( arr_._size );
			_size = arr_._size;
			for ( int long i = 0; i < _size; ++ i )
				new ( _buf + i ) value_type( arr_._buf[ i ] );
		}
		return;
		M_EPILOG
	}
#if CXX_STANDARD >= 2011
	/*! \brief Moving constructor.
	 *
	 * \param arr_ - array whose whole data should be transfered to this newly created array.
	 */
	HArray( HArray&& arr_ )
		: _buf( NULL ), _size( 0 ), _capacity( 0 ) {
		M_PROLOG
		swap( arr_ );
		return;
		M_EPILOG
	}
#endif /* #if CXX_STANDARD >= 2011 */
	/*! \brief Copy constructor with external allocator.
	 *
	 * \param arr_ - array object that this new array should be a copy of.
	 * \param allocator_ - external allocator that should be used for all array's allocations.
	 */
	HArray( HArray const& arr_, allocator_t const& )
		: _buf( NULL ), _size( 0 ), _capacity( 0 ) {
		M_PROLOG
		if ( arr_._size > 0 )
			{
			reserve( arr_._size );
			_size = arr_._size;
			for ( int long i = 0; i < _size; ++ i )
				new ( _buf + i ) value_type( arr_._buf[ i ] );
		}
		return;
		M_EPILOG
	}
	virtual ~HArray( void ) {
		M_PROLOG
		clear();
		:: operator delete ( _buf, memory::yaal );
		_capacity = 0;
		return;
		M_DESTRUCTOR_EPILOG
	}
	/*! \brief Remove all elemets from array.
	 */
	void clear( void ) {
		M_PROLOG
		erase( begin(), end() );
		return;
		M_EPILOG
	}
	/*! \brief Assing contents of another array to this array.
	 *
	 * \param arr_ - other array that's contents will be assigned to this array.
	 * \return Self.
	 */
	HArray& operator = ( HArray const& arr_ ) {
		M_PROLOG
		if ( &arr_ != this ) {
			if ( arr_._size > _capacity ) {
				HArray<value_type> tmp( arr_ );
				swap( tmp );
			} else {
				copy_n( arr_._buf, min( _size, arr_._size ), _buf );
				if ( arr_._size > _size ) {
					for ( int long i = _size; i < arr_._size; ++ i )
						new ( _buf + i ) value_type( arr_._buf[ i ] );
				} else if ( arr_._size < _size ) {
					for ( int long i = arr_._size; i < _size; ++ i )
						M_SAFE( _buf[ i ].~value_type() );
				}
				_size = arr_._size;
			}
		}
		return ( *this );
		M_EPILOG
	}
#if CXX_STANDARD >= 2011
	/*! \brief Replace contents of this array with contents moved from another array.
	 *
	 * \param arr_ - other array that's contents will replace current contents of this array.
	 * \return Self.
	 */
	HArray& operator = ( HArray&& arr_ ) {
		M_PROLOG
		if ( &arr_ != this ) {
			swap( arr_ );
			arr_.clear();
		}
		return ( *this );
		M_EPILOG
	}
#endif /* #if CXX_STANDARD >= 2011 */
	/*! \brief Swap contents of this array with another array.
	 *
	 * \param other - the other array that will swap contents with this array.
	 */
	void swap( HArray& other ) {
		M_PROLOG
		if ( &other != this ) {
			using yaal::swap;
			swap( _buf, other._buf );
			swap( _size, other._size );
			swap( _capacity, other._capacity );
		}
		return;
		M_EPILOG
	}
	allocator_type const& get_allocator( void ) const {
		return ( _allocator );
	}
	void resize( int long size_, type_t const& fillWith_ = value_type() ) {
		M_PROLOG
		if ( size_ < 0 )
			M_THROW( _errMsgHArray_[ ERROR::BAD_SIZE ], size_ );
		if ( size_ > _size ) {
			reserve( size_ );
			for ( int long i( _size ); i < size_; ++ i )
				new ( _buf + i ) value_type( fillWith_ );
		} else if ( size_ < _size ) {
			for ( int long i( size_ ); i < _size; ++ i )
				M_SAFE( _buf[ i ].~value_type() );
		}
		_size = size_;
		return;
		M_EPILOG
	}
	void reserve( int long capacity_ ) {
		M_PROLOG
		if ( capacity_ < 0 )
			M_THROW( _errMsgHArray_[ ERROR::BAD_SIZE ], capacity_ );
		if ( capacity_ > _capacity ) {
			int long newCapacity( _capacity ? max( capacity_, _capacity * 2 ) : capacity_ );
			value_type* newBuf( static_cast<value_type*>( ::operator new ( static_cast<size_t>( newCapacity * static_cast<int>( sizeof ( value_type ) ) ), memory::yaal ) ) );
			for ( int long i( 0 ); i < _size; ++ i )
				new ( newBuf + i ) value_type( _buf[ i ] );
			for ( int long i( 0 ); i < _size; ++ i )
				M_SAFE( _buf[ i ].~value_type() );
			using yaal::swap;
			swap( newBuf, _buf );
			_capacity = newCapacity;
			:: operator delete ( newBuf, memory::yaal );
		}
		return;
		M_EPILOG
	}
	type_t& operator[] ( int long index_ ) {
		M_PROLOG
		int long idx = ( index_ < 0 ) ? index_ + _size : index_;
		if ( ( idx >= _size ) || ( idx < 0 ) )
			M_THROW( _errMsgHArray_[ ERROR::BAD_INDEX ], idx );
		return ( _buf[ idx ] );
		M_EPILOG
	}
	type_t const& operator[] ( int long index_ ) const {
		M_PROLOG
		int long idx( ( index_ < 0 ) ? index_ + _size : index_ );
		if ( ( idx >= _size ) || ( idx < 0 ) )
			M_THROW( _errMsgHArray_[ ERROR::BAD_INDEX ], idx );
		return ( _buf[ idx ] );
		M_EPILOG
	}
	void push_back( type_t const& val_ ) {
		M_PROLOG
		M_ASSERT( _size <= _capacity );
		if ( _size == _capacity )
			reserve( _capacity + 1 );
		new ( _buf + _size ) value_type( val_ );
		++ _size;
		return;
		M_EPILOG
	}
	void pop_back( void ) {
		M_PROLOG
		M_ASSERT( _size > 0 );
		M_SAFE( _buf[ -- _size ].~value_type() );
		return;
		M_EPILOG
	}
	type_t const& front( void ) const {
		M_PROLOG
		M_ASSERT( _size > 0 );
		return ( *_buf );
		M_EPILOG
	}
	type_t& front( void ) {
		M_PROLOG
		M_ASSERT( _size > 0 );
		return ( *_buf );
		M_EPILOG
	}
	type_t const& back( void ) const {
		M_PROLOG
		M_ASSERT( _size > 0 );
		return ( _buf[ _size - 1 ] );
		M_EPILOG
	}
	type_t& back( void ) {
		M_PROLOG
		M_ASSERT( _size > 0 );
		return ( _buf[ _size - 1 ] );
		M_EPILOG
	}
	int long capacity( void ) const {
		return ( get_capacity() );
	}
	int long get_capacity( void ) const {
		return ( _capacity );
	}
	bool operator ! ( void ) const {
		M_PROLOG
		return ( is_empty() );
		M_EPILOG
	}
	int long get_size( void ) const {
		return ( _size );
	}
	int long size( void ) const {
		return ( _size );
	}
	bool is_empty( void ) const {
		return ( _size ? false : true );
	}
	bool empty( void ) const {
		return ( _size ? false : true );
	}
	template<typename iterator_t>
	void assign( iterator_t first, iterator_t last ) {
		M_PROLOG
		clear();
		insert( end(), first, last );
		return;
		M_EPILOG
	}
	void assign( int long size_, type_t const& fillWith_ ) {
		M_PROLOG
		int long oldSize( _size );
		resize( size_, fillWith_ );
		if ( oldSize > 0 )
			fill_n( begin(), oldSize, fillWith_ );
		return;
		M_EPILOG
	}
	template<typename iterator_t>
	void insert( iterator pos_, iterator_t first_, iterator_t last_ ) {
		M_PROLOG
		M_ASSERT( pos_._owner == this );
		if ( ( pos_._index < 0 ) && ( pos_._index > _size ) )
			M_THROW( _errMsgHArray_[ ERROR::INVALID_ITERATOR ], pos_._index );
		using yaal::distance;
		insert_space( pos_._index, distance( first_, last_ ) );
		for ( int long i( pos_._index ); first_ != last_; ++ first_,  ++ i )
			new ( _buf + i ) value_type( *first_ );
		return;
		M_EPILOG
	}
	void insert( iterator pos_, int long count_, type_t const& value_ ) {
		M_PROLOG
		M_ASSERT( pos_._owner == this );
		if ( ( pos_._index < 0 ) && ( pos_._index > _size ) )
			M_THROW( _errMsgHArray_[ ERROR::INVALID_ITERATOR ], pos_._index );
		insert_space( pos_._index, count_ );
		for ( int long i( pos_._index ), last( pos_._index + count_ ); i < last; ++ i )
			new ( _buf + i ) value_type( value_ );
		return;
		M_EPILOG
	}
	iterator insert( iterator pos_, type_t const& value_ ) {
		M_PROLOG
		M_ASSERT( pos_._owner == this );
		if ( ( pos_._index < 0 ) && ( pos_._index > _size ) )
			M_THROW( _errMsgHArray_[ ERROR::INVALID_ITERATOR ], pos_._index );
		insert_space( pos_._index, 1 );
		new ( _buf + pos_._index ) value_type( value_ );
		return ( pos_ );
		M_EPILOG
	}
	iterator erase( iterator first_, iterator last_ ) {
		M_PROLOG
		M_ASSERT( first_._owner == this );
		M_ASSERT( last_._owner == this );
		if ( ( first_._index < 0 ) && ( first_._index > _size ) )
			M_THROW( _errMsgHArray_[ ERROR::INVALID_ITERATOR ], first_._index );
		if ( ( last_._index < 0 ) && ( last_._index > _size ) )
			M_THROW( _errMsgHArray_[ ERROR::INVALID_ITERATOR ], last_._index );
		if ( last_._index < first_._index )
			M_THROW( _errMsgHArray_[ ERROR::INVALID_ITERATOR ], last_._index - first_._index );
		for ( iterator it( copy( last_, end(), first_ ) ), endIt( end() ); ( it != endIt ); ++ it )
			M_SAFE( (*it).~value_type() );
		_size -= ( last_._index - first_._index );
		return ( last_._index < _size ? last_ : end() );
		M_EPILOG
	}
	iterator erase( iterator it ) {
		M_PROLOG
		iterator next( it );
		++ next;
		return ( erase( it, next ) );
		M_EPILOG
	}
	iterator begin( void ) {
		return ( iterator( this, 0 ) );
	}
	iterator find( int long idx ) {
		return ( iterator( this, min( idx, _size ) ) );
	}
	iterator end( void ) {
		return ( iterator( this, _size ) );
	}
	const_iterator begin( void ) const {
		return ( const_iterator( this, 0 ) );
	}
	const_iterator cbegin( void ) const {
		return ( const_iterator( this, 0 ) );
	}
	const_iterator find( int long idx ) const {
		return ( const_iterator( this, min( idx, _size ) ) );
	}
	const_iterator end( void ) const {
		return ( const_iterator( this, _size ) );
	}
	const_iterator cend( void ) const {
		return ( const_iterator( this, _size ) );
	}
	reverse_iterator rbegin( void ) {
		return ( end() );
	}
	reverse_iterator rend( void ) {
		return ( begin() );
	}
	const_reverse_iterator rbegin( void ) const {
		return ( end() );
	}
	const_reverse_iterator rend( void ) const {
		return ( begin() );
	}
	const_reverse_iterator crbegin( void ) const {
		return ( end() );
	}
	const_reverse_iterator crend( void ) const {
		return ( begin() );
	}
	bool operator == ( HArray const& a_ ) const {
		M_PROLOG
		return ( ( &a_ == this ) || safe_equal( begin(), end(), a_.begin(), a_.end() ) );
		M_EPILOG
	}
	bool operator < ( HArray const& a_ ) const {
		M_PROLOG
		return ( ( &a_ != this ) && lexicographical_compare( begin(), end(), a_.begin(), a_.end() ) );
		M_EPILOG
	}
private:
	template<typename iterator_t>
	void initialize( iterator_t first, iterator_t last, trait::false_type const* ) {
		M_PROLOG
		insert( end(), first, last );
		return;
		M_EPILOG
	}
	void initialize( int long size_, type_t const& fillWith_, trait::true_type const* ) {
		M_PROLOG
		resize( size_, fillWith_ );
		return;
		M_EPILOG
	}
	void insert_space( int long pos_, int long size_ ) {
		M_PROLOG
		int long oldSize( _size );
		reserve( _size + size_ );
		_size += size_;
		for ( int long src( oldSize - 1 ), dst( _size - 1 ); src >= pos_; -- src, -- dst ) {
			new ( _buf + dst ) value_type( _buf[ src ] );
			M_SAFE( _buf[ src ].~value_type() );
		}
		return;
		M_EPILOG
	}
};
template<typename type_t, typename allocator_t>
typename HArray<type_t, allocator_t>::allocator_type HArray<type_t, allocator_t>::_allocator;

}

template<typename type_t, typename allocator_t>
inline void swap( yaal::hcore::HArray<type_t, allocator_t>& a, yaal::hcore::HArray<type_t, allocator_t>& b )
	{ a.swap( b ); }

namespace hcore {

/*! \brief Iterator for HArray<> data structure.
 */
template<typename type_t, typename allocator_t>
template<typename const_qual_t>
class HArray<type_t, allocator_t>::HIterator : public iterator_interface<const_qual_t, iterator_category::random_access> {
	typedef HArray<type_t, allocator_t> array_t;
	array_t const* _owner;
	int long _index;
public:
	typedef type_t value_type;
	typedef iterator_interface<const_qual_t, iterator_category::random_access> base_type;
	HIterator( void ) : base_type(), _owner( NULL ), _index( 0 ) {}
	HIterator( HIterator const& it_ ) : base_type(), _owner( it_._owner ), _index( it_._index ) {}
	template<typename other_const_qual_t>
	HIterator( HIterator<other_const_qual_t> const& it_ ) : base_type(), _owner( it_._owner ), _index( it_._index ) {
		STATIC_ASSERT(( trait::same_type<const_qual_t, other_const_qual_t>::value || trait::same_type<const_qual_t, other_const_qual_t const>::value ));
	}
	HIterator& operator = ( HIterator const& it_ ) {
		if ( &it_ != this ) {
			_owner = it_._owner;
			_index = it_._index;
		}
		return ( *this );
	}
	HIterator& operator ++ ( void ) {
		++ _index;
		return ( *this );
	}
	HIterator const operator ++ ( int ) {
		HIterator it( *this );
		operator ++ ();
		return ( it );
	}
	HIterator& operator -- ( void ) {
		-- _index;
		return ( *this );
	}
	HIterator const operator -- ( int ) {
		HIterator it( *this );
		operator -- ();
		return ( it );
	}
	HIterator operator + ( int long off_ ) const {
		HIterator it( _owner, _index + off_ );
		return ( it );
	}
	HIterator& operator += ( int long off_ ) {
		_index += off_;
		return ( *this );
	}
	HIterator operator - ( int long off_ ) const {
		HIterator it( _owner, _index - off_ );
		return ( it );
	}
	HIterator& operator -= ( int long off_ ) {
		_index -= off_;
		return ( *this );
	}
	int long operator - ( HIterator const& it ) const {
		M_ASSERT( _owner == it._owner );
		return ( _index - it._index );
	}
	const_qual_t& operator* ( void ) {
		M_ASSERT( _owner && ( _index >= 0 ) && ( _index < _owner->_size ) );
		return ( const_cast<const_qual_t&>( _owner->_buf[ _index ] ) );
	}
	const_qual_t& operator* ( void ) const {
		M_ASSERT( _owner && ( _index >= 0 ) && ( _index < _owner->_size ) );
		return ( _owner->_buf[ _index ] );
	}
	const_qual_t* operator-> ( void ) {
		M_ASSERT( _owner && ( _index >= 0 ) && ( _index < _owner->_size ) );
		return ( const_cast<const_qual_t*>( &_owner->_buf[ _index ] ) );
	}
	const_qual_t* operator-> ( void ) const {
		M_ASSERT( _owner && ( _index >= 0 ) && ( _index < _owner->_size ) );
		return ( &_owner->_buf[ _index ] );
	}
	template<typename other_const_qual_t>
	bool operator == ( HIterator<other_const_qual_t> const& it ) const {
		M_ASSERT( _owner == it._owner );
		return ( _index == it._index );
	}
	template<typename other_const_qual_t>
	bool operator != ( HIterator<other_const_qual_t> const& it ) const {
		M_ASSERT( _owner == it._owner );
		return ( _index != it._index );
	}
	template<typename other_const_qual_t>
	bool operator < ( HIterator<other_const_qual_t> const& it ) const {
		M_ASSERT( _owner == it._owner );
		return ( _index < it._index );
	}
private:
	friend class HArray<type_t, allocator_t>;
	explicit HIterator( array_t const* owner_, int long idx )
		: base_type(), _owner( owner_ ), _index( idx ) {};
};

}

}

#endif /* #ifndef YAAL_HCORE_HARRAY_HXX_INCLUDED */

