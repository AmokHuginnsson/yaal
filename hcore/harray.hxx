/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hcore/harray.hxx
 * \brief Declaration of HArray<> class.
 */

#ifndef YAAL_HCORE_HARRAY_HXX_INCLUDED
#define YAAL_HCORE_HARRAY_HXX_INCLUDED 1

#include <new>
#include <initializer_list>

#include "hcore/base.hxx"
#include "hcore/allocator.hxx"
#include "hcore/algorithm.hxx"
#include "hcore/pod.hxx"
#include "hcore/safe_int.hxx"

namespace yaal {

namespace hcore {

extern M_YAAL_HCORE_PUBLIC_API char const* const _errMsgHArray_[];

/*! \brief Simplest compound data structure.
 *
 * \tparam type_t - type of objects that will be stored in array.
 * \tparam allocator_t - type of internal allocator used for all array's allocations.
 */
template<typename type_t, typename allocator_t = allocator::system<type_t>>
class HArray final {
public:
	typedef HArray<type_t, allocator_t> this_type;
	typedef type_t value_type;
	typedef int long size_type;
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
	size_type _size;
	size_type _capacity;
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
		: _buf( nullptr )
		, _size( 0 )
		, _capacity( 0 ) {
		return;
	}
	/*! \brief Construct empty array object using external allocator.
	 *
	 * \param allocator_ - external allocator that should be used for all array's allocations.
	 */
	explicit HArray( allocator_t const& )
		: _buf( nullptr )
		, _size( 0 )
		, _capacity( 0 ) {
		return;
	}
	/*! \brief Construct array of given size.
	 *
	 * Array will be filled with default values of array's objects type.
	 *
	 * \param size_ - size for newly created array.
	 */
	explicit HArray( size_type size_ )
		: _buf( nullptr )
		, _size( 0 )
		, _capacity( 0 ) {
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
	HArray( size_type size_, allocator_t const& )
		: _buf( nullptr )
		, _size( 0 )
		, _capacity( 0 ) {
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
	HArray( size_type size_, type_t const& fillWith_, allocator_t const& = allocator_t() )
		: _buf( nullptr )
		, _size( 0 )
		, _capacity( 0 ) {
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
		: _buf( nullptr )
		, _size( 0 )
		, _capacity( 0 ) {
		M_PROLOG
		initialize( first, last, typename trait::add_pointer<typename is_integral<iterator_t>::type>::type() );
		return;
		M_EPILOG
	}
	/*! \brief Construct array based on compile time constant data set.
	 *
	 * \param constants_ - set of compile time constants to into into this array.
	 */
	HArray( std::initializer_list<value_type> constants_ )
		: _buf( nullptr )
		, _size( 0 )
		, _capacity( 0 ) {
		M_PROLOG
		initialize( constants_.begin(), constants_.end(), static_cast<trait::false_type*>( nullptr ) );
		return;
		M_EPILOG
	}
	/*! \brief Copy constructor.
	 *
	 * \param arr_ - array object that this new array should be a copy of.
	 */
	HArray( HArray const& arr_ )
		: _buf( nullptr )
		, _size( 0 )
		, _capacity( 0 ) {
		M_PROLOG
		if ( arr_._size > 0 ) {
			reserve( arr_._size );
			_size = arr_._size;
			for ( size_type i = 0; i < _size; ++ i ) {
				new ( _buf + i ) value_type( arr_._buf[ i ] );
			}
		}
		return;
		M_EPILOG
	}
	/*! \brief Moving constructor.
	 *
	 * \param arr_ - array whose whole data should be transferred to this newly created array.
	 */
	HArray( HArray&& arr_ )
		: _buf( nullptr )
		, _size( 0 )
		, _capacity( 0 ) {
		M_PROLOG
		swap( arr_ );
		return;
		M_EPILOG
	}
	/*! \brief Copy constructor with external allocator.
	 *
	 * \param arr_ - array object that this new array should be a copy of.
	 * \param allocator_ - external allocator that should be used for all array's allocations.
	 */
	HArray( HArray const& arr_, allocator_t const& )
		: _buf( nullptr )
		, _size( 0 )
		, _capacity( 0 ) {
		M_PROLOG
		if ( arr_._size > 0 ) {
			reserve( arr_._size );
			_size = arr_._size;
			for ( size_type i = 0; i < _size; ++ i ) {
				new ( _buf + i ) value_type( arr_._buf[ i ] );
			}
		}
		return;
		M_EPILOG
	}
	~HArray( void ) {
		M_PROLOG
		clear();
		::operator delete ( _buf, memory::yaal );
		_capacity = 0;
		return;
		M_DESTRUCTOR_EPILOG
	}
	/*! \brief Remove all elements from array.
	 */
	void clear( void ) {
		M_PROLOG
#	pragma GCC diagnostic push
#if __GCC_VERSION__ > 0
#	pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif /* #if __GCC_VERSION__ > 0 */
		while ( _size > 0 ) {
#	pragma GCC diagnostic pop
			M_SAFE( _buf[ _size - 1 ].~value_type() );
			-- _size;
		}
		return;
		M_EPILOG
	}
	/*! \brief Assign contents of another array to this array.
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
					for ( size_type i = _size; i < arr_._size; ++ i ) {
						new ( _buf + i ) value_type( arr_._buf[ i ] );
					}
				} else if ( arr_._size < _size ) {
					for ( size_type i = arr_._size; i < _size; ++ i ) {
						M_SAFE( _buf[ i ].~value_type() );
					}
				}
				_size = arr_._size;
			}
		}
		return ( *this );
		M_EPILOG
	}
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
	/*! \brief Get internal allocator instance.
	 *
	 * \return Internal allocator instance.
	 */
	allocator_type const& get_allocator( void ) const {
		return ( _allocator );
	}
	/*! \brief Resize this array.
	 *
	 * Change size of this array to accommodate requested number of elements.
	 * If necessary remove elements, or add new elements with default value.
	 *
	 * \param size_ - requested new number of elements in this array.
	 * \param fillWith_ - default value for new values if this array will be enlarged.
	 */
	void resize( size_type size_, type_t const& fillWith_ = value_type() ) {
		M_PROLOG
		if ( size_ < 0 ) {
			M_THROW( _errMsgHArray_[ ERROR::BAD_SIZE ], size_ );
		}
		if ( size_ > _size ) {
			reserve( size_ );
			for ( size_type i( _size ); i < size_; ++ i ) {
				new ( _buf + i ) value_type( fillWith_ );
			}
		} else if ( size_ < _size ) {
			for ( size_type i( size_ ); i < _size; ++ i ) {
				M_SAFE( _buf[ i ].~value_type() );
			}
		}
		_size = size_;
		return;
		M_EPILOG
	}
	/*! \brief Reserve memory in internal buffer for given number of elements.
	 *
	 * If necessary reallocate internal memory buffer so additional elements could be
	 * inserted into this array without any further reallocation.
	 *
	 * \param capacity_ - number of elements that could be stored in array without next reallocation.
	 */
	void reserve( size_type capacity_ ) {
		M_PROLOG
		if ( capacity_ < 0 ) {
			M_THROW( _errMsgHArray_[ ERROR::BAD_SIZE ], capacity_ );
		}
		if ( capacity_ > _capacity ) {
			size_type newCapacity( _capacity ? max( capacity_, _capacity * 2 ) : capacity_ );
			value_type* newBuf( static_cast<value_type*>( ::operator new ( static_cast<size_t>( yaal::safe_int::mul<size_type>( newCapacity, static_cast<int>( sizeof ( value_type ) ) ) ), memory::yaal ) ) );
			for ( size_type i( 0 ); i < _size; ++ i ) {
				new ( newBuf + i ) value_type( yaal::move( _buf[ i ] ) );
			}
			for ( size_type i( 0 ); i < _size; ++ i ) {
				M_SAFE( _buf[ i ].~value_type() );
			}
			using yaal::swap;
			swap( newBuf, _buf );
			_capacity = newCapacity;
			::operator delete ( newBuf, memory::yaal );
		}
		return;
		M_EPILOG
	}
	/*! \brief Access element at given position in this array.
	 *
	 * Element is retrieved without bounds checking.
	 *
	 * \param index_ - index of requested element in this array.
	 * \return Reference to element at requested index.
	 */
	type_t& operator[] ( size_type index_ ) {
		return ( _buf[ index_ ] );
	}
	type_t const& operator[] ( size_type index_ ) const {
		return ( _buf[ index_ ] );
	}
	/*! \brief Access element at given position in this array.
	 *
	 * Element is retrieved with bounds checking.
	 *
	 * \param index_ - index of requested element in this array.
	 * \return Reference to element at requested index.
	 */
	type_t& at( size_type index_ ) {
		M_PROLOG
		size_type idx = ( index_ < 0 ) ? index_ + _size : index_;
		if ( ( idx >= _size ) || ( idx < 0 ) ) {
			throw HOutOfRangeException( yaal::hcore::to_string( _errMsgHArray_[ ERROR::BAD_INDEX ] ).append( idx ) );
		}
		return ( _buf[ idx ] );
		M_EPILOG
	}
	type_t const& at( size_type index_ ) const {
		M_PROLOG
		size_type idx( ( index_ < 0 ) ? index_ + _size : index_ );
		if ( ( idx >= _size ) || ( idx < 0 ) ) {
			throw HOutOfRangeException( yaal::hcore::to_string( _errMsgHArray_[ ERROR::BAD_INDEX ] ).append( idx ) );
		}
		return ( _buf[ idx ] );
		M_EPILOG
	}
	/*! \brief Add element at end of this array.
	 *
	 * \param val_ - element to be added at end of this array.
	 */
	void push_back( type_t const& val_ ) {
		M_PROLOG
		M_ASSERT( _size <= _capacity );
		if ( _size == _capacity ) {
			reserve( _capacity + 1 );
		}
		new ( _buf + _size ) value_type( val_ );
		++ _size;
		return;
		M_EPILOG
	}
	void push_back( type_t&& val_ ) {
		M_PROLOG
		M_ASSERT( _size <= _capacity );
		if ( _size == _capacity ) {
			reserve( _capacity + 1 );
		}
		new ( _buf + _size ) value_type( yaal::move( val_ ) );
		++ _size;
		return;
		M_EPILOG
	}
	/*! \brief Add element at end of this array constructing it directly in place.
	 *
	 * \param arg_ - arguments passed to value_type constructor.
	 */
	template<typename... arg_t>
	void emplace_back( arg_t&&... arg_ ) {
		M_ASSERT( _size <= _capacity );
		if ( _size == _capacity ) {
			reserve( _capacity + 1 );
		}
		new ( _buf + _size ) value_type( yaal::forward<arg_t>( arg_ )... );
		++ _size;
	}
	/*! \brief Remove last element of this array.
	 */
	void pop_back( void ) {
		M_PROLOG
		M_ASSERT( _size > 0 );
		M_SAFE( _buf[ -- _size ].~value_type() );
		return;
		M_EPILOG
	}
	/*! \brief Get underlying buffer.
	 *
	 * \return Pointer C-style array containing this HArray data.
	 */
	type_t const* data( void ) const {
		M_PROLOG
		M_ASSERT( _buf != nullptr );
		return ( _buf );
		M_EPILOG
	}
	type_t* data( void ) {
		M_PROLOG
		M_ASSERT( _buf != nullptr );
		return ( _buf );
		M_EPILOG
	}
	/*! \brief Get reference to first element in this array.
	 *
	 * \return Reference to first element in this array.
	 */
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
	/*! \brief Get reference to last element in this array.
	 *
	 * \return Reference to last element in this array.
	 */
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
	/*! \brief Alias for HArray::get_capacity().
	 */
	size_type capacity( void ) const {
		return ( get_capacity() );
	}
	/*! \brief Retrieve information about how many elements could be potentially stored in this array without reallocation.
	 *
	 * \return Maximum number of elements that could be stored in this array without reallocation.
	 */
	size_type get_capacity( void ) const {
		return ( _capacity );
	}
	/*! \brief Alias for HArray::is_empty().
	 */
	explicit operator  bool ( void ) const {
		M_PROLOG
		return ( ! is_empty() );
		M_EPILOG
	}
	/*! \brief Get number of elements in array.
	 *
	 * \return Number of elements in this array.
	 */
	size_type get_size( void ) const {
		return ( _size );
	}
	/*! \brief Alias for HArray::get_size().
	 */
	size_type size( void ) const {
		return ( _size );
	}
	/*! \brief Tell if this array is empty.
	 *
	 * \return True iff this array contains no elements - is empty.
	 */
	bool is_empty( void ) const {
		return ( _size ? false : true );
	}
	/*! \brief Alias for HArray::is_empty().
	 */
	bool empty( void ) const {
		return ( _size ? false : true );
	}
	/*! \brief Replace contents of this array with contents of given range.
	 *
	 * \tparam iterator_t - Type of iterator pair defining an input range.
	 *
	 * \param first - beginning of the range of values to copy into this array.
	 * \param last - one past the end of the range of values to copy into this array.
	 */
	template<typename iterator_t>
	void assign( iterator_t first, iterator_t last ) {
		M_PROLOG
		clear();
		insert( end(), first, last );
		return;
		M_EPILOG
	}
	/*! \brief Replace contents of this array with given number of given value.
	 *
	 * \param size_ - number of elements to fill this array with.
	 * \param fillWith_ - value that shall be set for all elements in this array.
	 */
	void assign( size_type size_, type_t const& fillWith_ ) {
		M_PROLOG
		size_type oldSize( _size );
		resize( size_, fillWith_ );
		if ( oldSize > 0 ) {
			fill_n( begin(), oldSize, fillWith_ );
		}
		return;
		M_EPILOG
	}
	/*! \brief Insert range of values at given position.
	 *
	 * \tparam iterator_t - Type of iterator pair defining an input range.
	 *
	 * \param pos_ - insert values from given range at this position in array.
	 * \param first_ - beginning of the range of values to insert into this array.
	 * \param last_ - one past the end of the range of values to insert into this array.
	 */
	template<typename iterator_t>
	void insert( iterator pos_, iterator_t first_, iterator_t last_ ) {
		M_PROLOG
		M_ASSERT( pos_._owner == this );
		if ( ( pos_._index < 0 ) && ( pos_._index > _size ) ) {
			M_THROW( _errMsgHArray_[ ERROR::INVALID_ITERATOR ], pos_._index );
		}
		using yaal::distance;
		insert_space( pos_._index, distance( first_, last_ ) );
		for ( size_type i( pos_._index ); first_ != last_; ++ first_,  ++ i ) {
			new ( _buf + i ) value_type( *first_ );
		}
		return;
		M_EPILOG
	}
	/*! \brief Insert copies of value at given position.
	 *
	 * \param pos_ - insert copies of given value at this position.
	 * \param count_ - insert that many copies of given value.
	 * \param value_ - insert copies of this value.
	 */
	void insert( iterator pos_, size_type count_, type_t const& value_ ) {
		M_PROLOG
		M_ASSERT( pos_._owner == this );
		if ( ( pos_._index < 0 ) && ( pos_._index > _size ) ) {
			M_THROW( _errMsgHArray_[ ERROR::INVALID_ITERATOR ], pos_._index );
		}
		insert_space( pos_._index, count_ );
		for ( size_type i( pos_._index ), last( pos_._index + count_ ); i < last; ++ i ) {
			new ( _buf + i ) value_type( value_ );
		}
		return;
		M_EPILOG
	}
	/*! \brief Insert value at given position.
	 *
	 * \param pos_ - insert given value at this position.
	 * \param value_ - value to insert to this array.
	 * \return Iterator pointing to newly inserted element.
	 */
	iterator insert( iterator pos_, type_t const& value_ ) {
		M_PROLOG
		M_ASSERT( pos_._owner == this );
		if ( ( pos_._index < 0 ) && ( pos_._index > _size ) ) {
			M_THROW( _errMsgHArray_[ ERROR::INVALID_ITERATOR ], pos_._index );
		}
		insert_space( pos_._index, 1 );
		new ( _buf + pos_._index ) value_type( value_ );
		return ( pos_ );
		M_EPILOG
	}
	iterator insert( iterator pos_, type_t&& value_ ) {
		M_PROLOG
		M_ASSERT( pos_._owner == this );
		if ( ( pos_._index < 0 ) && ( pos_._index > _size ) ) {
			M_THROW( _errMsgHArray_[ ERROR::INVALID_ITERATOR ], pos_._index );
		}
		insert_space( pos_._index, 1 );
		new ( _buf + pos_._index ) value_type( yaal::move( value_ ) );
		return ( pos_ );
		M_EPILOG
	}
	/*! \brief Insert value at given position, constructing it directly in place.
	 *
	 * \param pos_ - insert given value at this position.
	 * \param arg_ - arguments passed to value_type constructor.
	 * \return Iterator pointing to newly inserted element.
	 */
	template<typename... arg_t>
	iterator emplace( iterator pos_, arg_t&&... arg_ ) {
		M_PROLOG
		M_ASSERT( pos_._owner == this );
		if ( ( pos_._index < 0 ) && ( pos_._index > _size ) ) {
			M_THROW( _errMsgHArray_[ ERROR::INVALID_ITERATOR ], pos_._index );
		}
		insert_space( pos_._index, 1 );
		new ( _buf + pos_._index ) value_type( yaal::forward<arg_t>( arg_ )... );
		return ( pos_ );
		M_EPILOG
	}
	/*! \brief Remove part of array.
	 *
	 * Part of array to be removed is defined by pair of valid iterators
	 * forming a range in this array.
	 *
	 * \param first_ - beginning of range of elements to be removed from array.
	 * \param last_ - one past the end of range of elements to be removed from array.
	 * \return Iterator pointing to first element after removed range.
	 */
	iterator erase( iterator first_, iterator last_ ) {
		M_PROLOG
		M_ASSERT( first_._owner == this );
		M_ASSERT( last_._owner == this );
		if ( ( first_._index < 0 ) || ( first_._index > _size ) ) {
			M_THROW( _errMsgHArray_[ ERROR::INVALID_ITERATOR ], first_._index );
		}
		if ( ( last_._index < 0 ) || ( last_._index > _size ) ) {
			M_THROW( _errMsgHArray_[ ERROR::INVALID_ITERATOR ], last_._index );
		}
		if ( last_._index < first_._index ) {
			M_THROW( _errMsgHArray_[ ERROR::INVALID_ITERATOR ], last_._index - first_._index );
		}
		for ( iterator it( first_ ); it != last_; ++ it ) {
			M_SAFE( (*it).~value_type() );
		}
		for ( iterator it( last_ ), to( first_ ), endIt( end() ); it != endIt; ++ it, ++ to ) {
			new ( _buf + to._index ) value_type( yaal::move( *it ) );
			M_SAFE( (*it).~value_type() );
		}
		_size -= ( last_._index - first_._index );
		return ( first_ );
		M_EPILOG
	}
	/*! \brief Remove element at given position.
	 *
	 * \param it - position of the element to be removed from array.
	 * \return Iterator pointing to element directly after the removed one.
	 */
	iterator erase( iterator it ) {
		M_PROLOG
		iterator next( it );
		++ next;
		return ( erase( it, next ) );
		M_EPILOG
	}
	/*! \brief Get iterator pointing to the beginning of the collection.
	 *
	 * \return Iterator pointing to the beginning of the collection.
	 */
	iterator begin( void ) {
		return ( iterator( this, 0 ) );
	}
	iterator find( size_type idx ) {
		return ( iterator( this, min( idx, _size ) ) );
	}
	iterator end( void ) {
		return ( iterator( this, _size ) );
	}
	/*! \brief Get iterator pointing to the beginning of the collection.
	 *
	 * \return Iterator pointing to the beginning of the collection.
	 */
	const_iterator begin( void ) const {
		return ( const_iterator( this, 0 ) );
	}
	/*! \brief Get const iterator pointing to the beginning of the collection.
	 *
	 * \return Const iterator pointing to the beginning of the collection.
	 */
	const_iterator cbegin( void ) const {
		return ( const_iterator( this, 0 ) );
	}
	const_iterator find( size_type idx ) const {
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
	/*! \brief Test if another array is equal to this array.
	 *
	 * \param a_ - the other array to check for equality with.
	 * \return True iff other array is equal to this array.
	 */
	bool operator == ( HArray const& a_ ) const {
		M_PROLOG
		return ( ( &a_ == this ) || safe_equal( begin(), end(), a_.begin(), a_.end() ) );
		M_EPILOG
	}
	/*! \brief Test if this array is lexicographically lesser then given array.
	 *
	 * \param a_ - other array for lexicographical comparison.
	 * \return True iff this array is lexicographically lesser then given array.
	 */
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
	void initialize( size_type size_, type_t const& fillWith_, trait::true_type const* ) {
		M_PROLOG
		resize( size_, fillWith_ );
		return;
		M_EPILOG
	}
	void insert_space( size_type pos_, size_type size_ ) {
		M_PROLOG
		size_type oldSize( _size );
		reserve( _size + size_ );
		_size += size_;
		for ( size_type src( oldSize - 1 ), dst( _size - 1 ); src >= pos_; -- src, -- dst ) {
			new ( _buf + dst ) value_type( yaal::move( _buf[ src ] ) );
			M_SAFE( _buf[ src ].~value_type() );
		}
		return;
		M_EPILOG
	}
};
template<typename type_t, typename allocator_t>
typename HArray<type_t, allocator_t>::allocator_type HArray<type_t, allocator_t>::_allocator;

template<typename type_t, typename allocator_t>
inline void swap( yaal::hcore::HArray<type_t, allocator_t>& a, yaal::hcore::HArray<type_t, allocator_t>& b ) {
	a.swap( b );
}

/*! \brief Iterator for HArray<> data structure.
 */
template<typename type_t, typename allocator_t>
template<typename const_qual_t>
class HArray<type_t, allocator_t>::HIterator : public iterator_interface<const_qual_t, iterator_category::random_access> {
	typedef HArray<type_t, allocator_t> array_t;
	array_t const* _owner;
	size_type _index;
public:
	typedef iterator_interface<const_qual_t, iterator_category::random_access> base_type;
	HIterator( void )
		: base_type()
		, _owner( nullptr )
		, _index( 0 ) {
		return;
	}
	HIterator( HIterator const& it_ )
		: base_type()
		, _owner( it_._owner )
		, _index( it_._index ) {
		return;
	}
	template<typename other_const_qual_t>
	HIterator( HIterator<other_const_qual_t> const& it_ )
		: base_type()
		, _owner( it_._owner )
		, _index( it_._index ) {
		static_assert(
			( trait::same_type<const_qual_t, other_const_qual_t>::value || trait::same_type<const_qual_t, other_const_qual_t const>::value ),
			"creating non-const iterator instance discards qualifiers"
		);
		return;
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
	HIterator operator + ( size_type off_ ) const {
		HIterator it( _owner, _index + off_ );
		return ( it );
	}
	HIterator& operator += ( size_type off_ ) {
		_index += off_;
		return ( *this );
	}
	HIterator operator - ( size_type off_ ) const {
		HIterator it( _owner, _index - off_ );
		return ( it );
	}
	HIterator& operator -= ( size_type off_ ) {
		_index -= off_;
		return ( *this );
	}
	size_type operator - ( HIterator const& it ) const {
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
	template<typename other_const_qual_t>
	bool operator <= ( HIterator<other_const_qual_t> const& it ) const {
		M_ASSERT( _owner == it._owner );
		return ( _index <= it._index );
	}
	template<typename other_const_qual_t>
	bool operator > ( HIterator<other_const_qual_t> const& it ) const {
		M_ASSERT( _owner == it._owner );
		return ( _index > it._index );
	}
	template<typename other_const_qual_t>
	bool operator >= ( HIterator<other_const_qual_t> const& it ) const {
		M_ASSERT( _owner == it._owner );
		return ( _index >= it._index );
	}
private:
	friend class HArray<type_t, allocator_t>;
	explicit HIterator( array_t const* owner_, size_type idx )
		: base_type()
		, _owner( owner_ )
		, _index( idx ) {
	}
};

}

}

#endif /* #ifndef YAAL_HCORE_HARRAY_HXX_INCLUDED */

