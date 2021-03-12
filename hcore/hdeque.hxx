/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hcore/hdeque.hxx
 * \brief Implementation of HDeque<> a Double-Ended-QUEue.
 */

#ifndef YAAL_HCORE_HDEQUE_HXX_INCLUDED
#define YAAL_HCORE_HDEQUE_HXX_INCLUDED 1

#include <new>
#include <initializer_list>

#include "hcore/base.hxx"
#include "hcore/allocator.hxx"
#include "hcore/hchunk.hxx"
#include "hcore/algorithm.hxx"
#include "hcore/pod.hxx"

namespace yaal {

namespace hcore {

extern M_YAAL_HCORE_PUBLIC_API char const* const _errMsgHDeque_[];

/*! \brief HDeque class - a double-ended-queue.
 *
 * Implemented as array of arrays.
 */
template<typename type_t, typename allocator_t = allocator::system<type_t> >
class HDeque final {
public:
	typedef HDeque<type_t> this_type;
	typedef type_t value_type;
	typedef int long size_type;
	/*! \brief Error codes for HDeque<> operations.
	 */
	struct ERROR {
		/*! \brief Error codes for HDeque<> operations.
		 */
		typedef enum {
			OK = 0,          /*!< No error. */
			BAD_SIZE,        /*!< Bad size requested. */
			BAD_INDEX,       /*!< Index of of bounds. */
			INVALID_ITERATOR /*!< iterator used for operation is not valid */
		} error_t;
	};
	typedef allocator::system<type_t> allocator_type;
	template<typename const_qual_t>
	class HIterator;
	typedef HIterator<type_t> iterator;
	typedef HIterator<type_t const> const_iterator;
	typedef HReverseIterator<iterator> reverse_iterator;
	typedef HReverseIterator<const_iterator> const_reverse_iterator;
private:
	static int const VALUE_SIZE = static_cast<int>( sizeof ( type_t ) );
	static size_type const DEFAULT_CHUNK_SIZE = ( 512 / VALUE_SIZE ) * VALUE_SIZE + ( ( 512 - ( ( 512 / VALUE_SIZE ) * VALUE_SIZE ) ) > ( VALUE_SIZE / 2 ) ? VALUE_SIZE : 0 );
	static size_type const CHUNK_SIZE = meta::max<VALUE_SIZE, DEFAULT_CHUNK_SIZE>::value;
	static size_type const VALUES_PER_CHUNK = CHUNK_SIZE / VALUE_SIZE;
	static size_type const MIN_CHUNKS_COUNT = 8;
private:
	static allocator_type _allocator;
	HChunk _chunks;
	size_type _start;
	size_type _size;
public:
	/*! \brief Construct empty deque object.
	 */
	HDeque( void )
		: _chunks()
		, _start( 0 )
		, _size( 0 ) {
		return;
	}

	/*! \brief Construct empty deque object using external allocator.
	 *
	 * \param allocator_ - external allocator that should be used for all deque's allocations.
	 */
	explicit HDeque( allocator_t const& )
		: _chunks()
		, _start( 0 )
		, _size( 0 ) {
		return;
	}

	/*! \brief Construct deque of given size.
	 *
	 * Array will be filled with default values of deque's objects type.
	 *
	 * \param size_ - size for newly created deque.
	 */
	explicit HDeque( size_type size_ )
		: _chunks()
		, _start( 0 )
		, _size( 0 ) {
		M_PROLOG
		resize( size_ );
		return;
		M_EPILOG
	}

	/*! \brief Construct deque of given size using external allocator.
	 *
	 * Array will be filled with default values of deque's objects type.
	 *
	 * \param size_ - size for newly created deque.
	 * \param allocator_ - external allocator that should be used for all deque's allocations.
	 */
	HDeque( size_type size_, allocator_t const& )
		: _chunks()
		, _start( 0 )
		, _size( 0 ) {
		M_PROLOG
		resize( size_ );
		return;
		M_EPILOG
	}

	/*! \brief Construct deque of given size using explicit default value and external allocator.
	 *
	 * Array will be filled with objects of given value.
	 *
	 * \param size_ - size for newly created deque.
	 * \param fillWith_ - value prototype that should be set for all objects in this newly created deque.
	 * \param allocator_ - external allocator that should be used for all deque's allocations.
	 */
	HDeque( size_type size_, type_t const& fillWith_, allocator_t const& = allocator_t() )
		: _chunks()
		, _start( 0 )
		, _size( 0 ) {
		M_PROLOG
		resize( size_, fillWith_ );
		return;
		M_EPILOG
	}

	/*! \brief Construct deque based on given range of objects and external allocator.
	 *
	 * \tparam iterator_t -  type of iterator used to specify input range.
	 * \param first - beginning of input range.
	 * \param last - one past the end of input range.
	 * \param allocator_ - external allocator that should be used for all deque's allocations.
	 */
	template<typename iterator_t>
	HDeque( iterator_t first, iterator_t last, allocator_t const& = allocator_t() )
		: _chunks()
		, _start( 0 )
		, _size( 0 ) {
		M_PROLOG
		initialize( first, last, typename trait::add_pointer<typename is_integral<iterator_t>::type>::type() );
		return;
		M_EPILOG
	}

	/*! \brief Construct deque based on compile time constant data set.
	 *
	 * \param constants_ - set of compile time constants to into into this deque.
	 */
	HDeque( std::initializer_list<value_type> constants_ )
		: _chunks()
		, _start( 0 )
		, _size( 0 ) {
		M_PROLOG
		initialize( constants_.begin(), constants_.end(), static_cast<trait::false_type*>( nullptr ) );
		return;
		M_EPILOG
	}

	~HDeque( void ) {
		M_PROLOG
		clear();
		return;
		M_DESTRUCTOR_EPILOG
	}

	/*! \brief Copy constructor.
	 *
	 * \param arr_ - deque object that this new deque should be a copy of.
	 */
	HDeque( HDeque const& );

	/*! \brief Copy constructor with external allocator.
	 *
	 * \param arr_ - deque object that this new deque should be a copy of.
	 * \param allocator_ - external allocator that should be used for all deque's allocations.
	 */
	HDeque( HDeque const&, allocator_t const& );

	HDeque( HDeque&& deque_ )
		: _chunks()
		, _start( 0 )
		, _size( 0 ) {
		M_PROLOG
		swap( deque_ );
		return;
		M_EPILOG
	}

	/*! \brief Assign contents of another deque to this deque.
	 *
	 * \param arr_ - other deque that's contents will be assigned to this deque.
	 * \return Self.
	 */
	HDeque& operator = ( HDeque const& deque_ ) {
		M_PROLOG
		if ( &deque_ != this ) {
			HDeque tmp( deque_ );
			swap( tmp );
		}
		return ( *this );
		M_EPILOG
	}

	HDeque& operator = ( HDeque&& deque_ ) {
		M_PROLOG
		if ( &deque_ != this ) {
			swap( deque_ );
			deque_.clear();
		}
		return ( *this );
		M_EPILOG
	}

	/*! \brief Swap contents of this deque with another deque.
	 *
	 * \param other - the other deque that will swap contents with this deque.
	 */
	void swap( HDeque& deque_ ) {
		if ( &deque_ != this ) {
			using yaal::swap;
			swap( _start, deque_._start );
			swap( _size, deque_._size );
			swap( _chunks, deque_._chunks );
		}
		return;
	}

	/*! \brief Get internal allocator instance.
	 *
	 * \return Internal allocator instance.
	 */
	allocator_type const& get_allocator( void ) const {
		return ( _allocator );
	}

	/*! \brief Resize this deque.
	 *
	 * Change size of this deque to accommodate requested number of elements.
	 * If necessary remove elements, or add new elements with default value.
	 *
	 * \param size_ - requested new number of elements in this deque.
	 * \param fillWith_ - default value for new values if this deque will be enlarged.
	 */
	void resize( size_type, type_t const& = value_type() );

	/*! \brief Access element at given position in this deque.
	 *
	 * Element is retrieved without bounds checking.
	 *
	 * \param index_ - index of requested element in this deque.
	 * \return Reference to element at requested index.
	 */
	type_t const& operator[] ( size_type index_ ) const {
		M_ASSERT( ( index_ >= 0 ) && ( index_ < _size ) );
		size_type itemIndex( _start + index_ );
		return ( _chunks.get<value_type const*>()[ itemIndex / VALUES_PER_CHUNK][ itemIndex % VALUES_PER_CHUNK ] );
	}
	type_t& operator[] ( size_type index_ ) {
		M_ASSERT( ( index_ >= 0 ) && ( index_ < _size ) );
		size_type itemIndex( _start + index_ );
		return ( _chunks.get<value_type*>()[ itemIndex / VALUES_PER_CHUNK][ itemIndex % VALUES_PER_CHUNK ] );
	}

	/*! \brief Access element at given position in this deque.
	 *
	 * Element is retrieved with bounds checking.
	 *
	 * \param index_ - index of requested element in this deque.
	 * \return Reference to element at requested index.
	 */
	type_t const& at( size_type index_ ) const {
		size_type idx = ( index_ < 0 ) ? index_ + _size : index_;
		if ( ( idx >= _size ) || ( idx < 0 ) ) {
			throw HOutOfRangeException( yaal::hcore::to_string( _errMsgHDeque_[ ERROR::BAD_INDEX ] ).append( idx ) );
		}
		size_type itemIndex( _start + idx );
		return ( _chunks.get<value_type const*>()[ itemIndex / VALUES_PER_CHUNK][ itemIndex % VALUES_PER_CHUNK ] );
	}
	type_t& at( size_type index_ ) {
		size_type idx = ( index_ < 0 ) ? index_ + _size : index_;
		if ( ( idx >= _size ) || ( idx < 0 ) ) {
			throw HOutOfRangeException( yaal::hcore::to_string( _errMsgHDeque_[ ERROR::BAD_INDEX ] ).append( idx ) );
		}
		size_type itemIndex( _start + idx );
		return ( _chunks.get<value_type*>()[ itemIndex / VALUES_PER_CHUNK][ itemIndex % VALUES_PER_CHUNK ] );
	}

	/*! \brief Add element at in the beginning of this deque.
	 *
	 * \param value_ - element to be added in the beginning of this deque.
	 */
	void push_front( type_t const& value_ ) {
		M_PROLOG
		new ( space_front() ) value_type( value_ );
		++ _size;
		return;
		M_EPILOG
	}
	void push_front( type_t&& value_ ) {
		M_PROLOG
		new ( space_front() ) value_type( yaal::move( value_ ) );
		++ _size;
		return;
		M_EPILOG
	}
	template<typename... arg_t>
	void emplace_front( arg_t&&... arg_ ) {
		M_PROLOG
		new ( space_front() ) value_type( yaal::forward<arg_t>( arg_ )... );
		++ _size;
		return;
		M_EPILOG
	}

	/*! \brief Remove first element of this deque.
	 */
	void pop_front( void );

	/*! \brief Add element at end of this deque.
	 *
	 * \param value_ - element to be added at end of this deque.
	 */
	void push_back( type_t const& value_ ) {
		M_PROLOG
		new ( space_back() ) value_type( value_ );
		++ _size;
		return;
		M_EPILOG
	}
	void push_back( type_t&& value_ ) {
		M_PROLOG
		new ( space_back() ) value_type( yaal::move( value_ ) );
		++ _size;
		return;
		M_EPILOG
	}
	template<typename... arg_t>
	void emplace_back( arg_t&&... arg_ ) {
		M_PROLOG
		new ( space_back() ) value_type( yaal::forward<arg_t>( arg_ )... );
		++ _size;
		return;
		M_EPILOG
	}

	/*! \brief Remove last element of this deque.
	 */
	void pop_back( void );

	/*! \brief Get reference to last element in this deque.
	 *
	 * \return Reference to last element in this deque.
	 */
	type_t const& back( void ) const {
		M_PROLOG
		M_ASSERT( _size > 0 );
		size_type itemIndex( _start + _size - 1 );
		return ( _chunks.get<value_type const*>()[ itemIndex / VALUES_PER_CHUNK][ itemIndex % VALUES_PER_CHUNK ] );
		M_EPILOG
	}
	type_t& back( void ) {
		M_PROLOG
		M_ASSERT( _size > 0 );
		size_type itemIndex( _start + _size - 1 );
		return ( _chunks.get<value_type*>()[ itemIndex / VALUES_PER_CHUNK][ itemIndex % VALUES_PER_CHUNK ] );
		M_EPILOG
	}

	/*! \brief Get reference to first element in this deque.
	 *
	 * \return Reference to first element in this deque.
	 */
	type_t const& front( void ) const {
		M_PROLOG
		M_ASSERT( _size > 0 );
		size_type itemIndex( _start );
		return ( _chunks.get<value_type const*>()[ itemIndex / VALUES_PER_CHUNK][ itemIndex % VALUES_PER_CHUNK ] );
		M_EPILOG
	}
	type_t& front( void ) {
		M_PROLOG
		M_ASSERT( _size > 0 );
		size_type itemIndex( _start );
		return ( _chunks.get<value_type*>()[ itemIndex / VALUES_PER_CHUNK][ itemIndex % VALUES_PER_CHUNK ] );
		M_EPILOG
	}

	/*! \brief Tell if this deque is empty.
	 *
	 * \return True iff this deque contains no elements - is empty.
	 */
	bool is_empty( void ) const {
		return ( _size == 0 );
	}
	bool empty( void ) const {
		return ( _size == 0 );
	}
	size_type get_size( void ) const {
		return ( _size );
	}
	size_type size( void ) const {
		return ( _size );
	}
	void clear( void );
	explicit operator  bool ( void ) const {
		return ( ! is_empty() );
	}
	iterator insert( iterator pos_, type_t const& val_ ) {
		M_ASSERT( pos_._owner == this );
		new ( space_at( pos_._index ) ) value_type( val_ );
		return pos_;
	}
	iterator insert( iterator pos_, type_t&& val_ ) {
		M_ASSERT( pos_._owner == this );
		new ( space_at( pos_._index ) ) value_type( yaal::move( val_ ) );
		return pos_;
	}
	template<typename... arg_t>
	iterator emplace( iterator pos_, arg_t&&... arg_ ) {
		M_ASSERT( pos_._owner == this );
		new ( space_at( pos_._index ) ) value_type( yaal::forward<arg_t>( arg_ )... );
		return pos_;
	}

	/*! \brief Replace contents of this deque with contents of given range.
	 *
	 * \tparam iterator_t - Type of iterator pair defining an input range.
	 *
	 * \param first - beginning of the range of values to copy into this deque.
	 * \param last - one past the end of the range of values to copy into this deque.
	 */
	template<typename iterator_t>
	void assign( iterator_t, iterator_t );
	void assign( size_type, type_t const& );
	template<typename iterator_t>
	void insert( iterator, iterator_t, iterator_t );
	void insert( iterator, size_type, type_t const& );
	/*! \brief Remove element at given position.
	 *
	 * \param it - position of the element to be removed from deque.
	 * \return Iterator pointing to element directly after the removed one.
	 */
	iterator erase( iterator it ) {
		M_PROLOG
		iterator next( it );
		++ next;
		return ( erase( it, next ) );
		M_EPILOG
	}
	/*! \brief Remove part of deque.
	 *
	 * Part of deque to be removed is defined by pair of valid iterators
	 * forming a range in this deque.
	 *
	 * \param first_ - beginning of range of elements to be removed from deque.
	 * \param last_ - one past the end of range of elements to be removed from deque.
	 * \return Iterator pointing to first element after removed range.
	 */
	iterator erase( iterator, iterator );

	const_iterator begin( void ) const {
		return ( const_iterator( this, 0 ) );
	}
	const_iterator cbegin( void ) const {
		return ( const_iterator( this, 0 ) );
	}
	iterator begin( void ) {
		return ( iterator( this, 0 ) );
	}
	const_iterator end( void ) const {
		return ( const_iterator( this, _size ) );
	}
	const_iterator cend( void ) const {
		return ( const_iterator( this, _size ) );
	}
	iterator end( void ) {
		return ( iterator( this, _size ) );
	}
	const_reverse_iterator rbegin( void ) const {
		return ( const_iterator( this, _size ) );
	}
	const_reverse_iterator crbegin( void ) const {
		return ( const_iterator( this, _size ) );
	}
	reverse_iterator rbegin( void ) {
		return ( iterator( this, _size ) );
	}
	const_reverse_iterator rend( void ) const {
		return ( const_iterator( this, 0 ) );
	}
	const_reverse_iterator crend( void ) const {
		return ( const_iterator( this, 0 ) );
	}
	reverse_iterator rend( void ) {
		return ( iterator( this, 0 ) );
	}

	iterator find( size_type );
	const_iterator find( size_type ) const;

	/*! \brief Test if another deque is equal to this deque.
	 *
	 * \param a_ - the other deque to check for equality with.
	 * \return True iff other deque is equal to this deque.
	 */
	bool operator == ( HDeque const& d_ ) const {
		M_PROLOG
		return ( ( &d_ == this ) || safe_equal( begin(), end(), d_.begin(), d_.end() ) );
		M_EPILOG
	}

	/*! \brief Test if this deque is lexicographically lesser then given deque.
	 *
	 * \param a_ - other deque for lexicographical comparison.
	 * \return True iff this deque is lexicographically lesser then given deque.
	 */
	bool operator < ( HDeque const& d_ ) const {
		M_PROLOG
		return ( ( &d_ != this ) && lexicographical_compare( begin(), end(), d_.begin(), d_.end() ) );
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
	value_type* space_front( void );
	value_type* space_back( void );
	value_type* space_at( size_type );
	void insert_space( size_type, size_type );
	void accommodate_chunks( size_type );
	size_type minimal_cover_chunks_count( size_type size_ ) const {
		return ( ( size_ / VALUES_PER_CHUNK ) + ( ( size_ % VALUES_PER_CHUNK ) ? 1 : 0 ) );
	}
	size_type used_chunks( void ) const {
		return ( _size > 0 ? ( ( ( _start + _size - 1 ) / VALUES_PER_CHUNK ) + 1 ) - ( _start / VALUES_PER_CHUNK ) : 0 );
	}
};

template<typename type_t, typename allocator_t>
typename HDeque<type_t, allocator_t>::size_type const HDeque<type_t, allocator_t>::CHUNK_SIZE;
template<typename type_t, typename allocator_t>
typename HDeque<type_t, allocator_t>::size_type const HDeque<type_t, allocator_t>::VALUES_PER_CHUNK;
template<typename type_t, typename allocator_t>
typename HDeque<type_t, allocator_t>::size_type const HDeque<type_t, allocator_t>::MIN_CHUNKS_COUNT;
template<typename type_t, typename allocator_t>
typename HDeque<type_t, allocator_t>::allocator_type HDeque<type_t, allocator_t>::_allocator;

template<typename type_t, typename allocator_t>
inline void swap( yaal::hcore::HDeque<type_t>& a, yaal::hcore::HDeque<type_t>& b ) {
	a.swap( b );
}

/*! \brief Iterator for HDeque<> data structure.
 */
template<typename type_t, typename allocator_t>
template<typename const_qual_t>
class HDeque<type_t, allocator_t>::HIterator : public iterator_interface<const_qual_t, iterator_category::random_access> {
	typedef HDeque<type_t, allocator_t> deque_t;
	deque_t const* _owner;
	size_type _index;
public:
	typedef iterator_interface<const_qual_t, iterator_category::random_access> base_type;
	HIterator( void )
		: base_type()
		, _owner( nullptr )
		, _index( 0 ) {
	}
	HIterator( HIterator const& it_ )
		: base_type()
		, _owner( it_._owner )
		, _index( it_._index ) {
	}
	template<typename other_const_qual_t>
	HIterator( HIterator<other_const_qual_t> const& it_ )
		: base_type()
		, _owner( it_._owner )
		, _index( it_._index ) {
		static_assert(
			trait::same_type<const_qual_t, other_const_qual_t>::value || trait::same_type<const_qual_t, other_const_qual_t const>::value,
			"creating non-const iterator instance discards qualifiers"
		);
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
		return it;
	}
	HIterator& operator -- ( void ) {
		-- _index;
		return ( *this );
	}
	HIterator const operator -- ( int ) {
		HIterator it( *this );
		operator -- ();
		return it;
	}
	HIterator operator + ( size_type off_ ) const {
		HIterator it( _owner, _index + off_ );
		return it;
	}
	HIterator& operator += ( size_type off_ ) {
		_index += off_;
		return ( *this );
	}
	HIterator operator - ( size_type off_ ) const {
		HIterator it( _owner, _index - off_ );
		return it;
	}
	HIterator& operator -= ( size_type off_ ) {
		_index -= off_;
		return ( *this );
	}
	size_type operator - ( HIterator const& it ) const {
		M_ASSERT( _owner == it._owner );
		return ( _index - it._index );
	}
	const_qual_t* operator-> ( void ) {
		M_PROLOG
		M_ASSERT( _owner && ( _index >= 0 ) && ( _index < _owner->_size ) );
		const_qual_t* const* chunks = _owner->_chunks.template get<value_type*>();
		size_type physical( _owner->_start + _index );
		return ( &chunks[ physical / HDeque<type_t>::VALUES_PER_CHUNK ][ physical % HDeque<type_t>::VALUES_PER_CHUNK ] );
		M_EPILOG
	}
	const_qual_t* operator-> ( void ) const {
		M_PROLOG
		M_ASSERT( _owner && ( _index >= 0 ) && ( _index < _owner->_size ) );
		const_qual_t* const* chunks = _owner->_chunks.template get<value_type const*>();
		size_type physical( _owner->_start + _index );
		return ( &chunks[ physical / HDeque<type_t>::VALUES_PER_CHUNK ][ physical % HDeque<type_t>::VALUES_PER_CHUNK ] );
		M_EPILOG
	}
	const_qual_t& operator* ( void ) {
		M_PROLOG
		M_ASSERT( _owner && ( _index >= 0 ) && ( _index < _owner->_size ) );
		const_qual_t* const* chunks = _owner->_chunks.template get<const_qual_t*>();
		size_type physical( _owner->_start + _index );
		return ( chunks[ physical / HDeque<type_t>::VALUES_PER_CHUNK ][ physical % HDeque<type_t>::VALUES_PER_CHUNK ] );
		M_EPILOG
	}
	const_qual_t& operator* ( void ) const {
		M_PROLOG
		M_ASSERT( _owner && ( _index >= 0 ) && ( _index < _owner->_size ) );
		const_qual_t* const* chunks = _owner->_chunks.template get<const_qual_t*>();
		size_type physical( _owner->_start + _index );
		return ( chunks[ physical / HDeque<type_t>::VALUES_PER_CHUNK ][ physical % HDeque<type_t>::VALUES_PER_CHUNK ] );
		M_EPILOG
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
	friend class HDeque<type_t, allocator_t>;
	explicit HIterator( deque_t const* owner_, size_type idx )
		: base_type()
		, _owner( owner_ )
		, _index( idx ) {
	}
};

template<typename type_t, typename allocator_t>
HDeque<type_t, allocator_t>::HDeque( HDeque const& deque )
	: _chunks()
	, _start( 0 )
	, _size( 0 ) {
	M_PROLOG
	if ( deque._size > 0 ) {
		_chunks.realloc( max( chunk_size<value_type*>( MIN_CHUNKS_COUNT ), deque._chunks.get_size() ), HChunk::STRATEGY::EXACT );
		M_ASSERT( _chunks.get_size() == deque._chunks.get_size() );
		size_type firstUsedChunkIndex( deque._start / VALUES_PER_CHUNK );
		value_type const* const* srcChunks = deque._chunks.template get<value_type const*>();
		value_type** chunks = _chunks.get<value_type*>();
		for ( size_type i( firstUsedChunkIndex ), chunksCount( deque._chunks.template count_of<value_type*>() ); ( i < chunksCount ) && srcChunks[ i ]; ++ i ) {
			chunks[ i ] = static_cast<value_type*>( ::operator new ( CHUNK_SIZE, memory::yaal ) );
		}
		for ( size_type i( deque._start ), endIdx( deque._start + deque._size ); i < endIdx; ++ i ) {
			new ( chunks[ i / VALUES_PER_CHUNK ] + ( i % VALUES_PER_CHUNK ) ) value_type( srcChunks[ i / VALUES_PER_CHUNK ][ i % VALUES_PER_CHUNK ] );
		}
		_start = deque._start;
		_size = deque._size;
	}
	return;
	M_EPILOG
}

template<typename type_t, typename allocator_t>
HDeque<type_t, allocator_t>::HDeque( HDeque const& deque, allocator_t const& )
	: _chunks()
	, _start( 0 )
	, _size( 0 ) {
	M_PROLOG
	if ( deque._size > 0 ) {
		_chunks.realloc( max( chunk_size<value_type*>( MIN_CHUNKS_COUNT ), deque._chunks.get_size() ), HChunk::STRATEGY::EXACT );
		M_ASSERT( _chunks.get_size() == deque._chunks.get_size() );
		size_type firstUsedChunkIndex( deque._start / VALUES_PER_CHUNK );
		value_type const* const* srcChunks = deque._chunks.template get<value_type const*>();
		value_type** chunks = _chunks.get<value_type*>();
		for ( size_type i( firstUsedChunkIndex ), chunksCount( deque._chunks.template count_of<value_type*>() ); ( i < chunksCount ) && srcChunks[ i ]; ++ i ) {
			chunks[ i ] = static_cast<value_type*>( ::operator new ( CHUNK_SIZE, memory::yaal ) );
		}
		for ( size_type i( deque._start ), endIdx( deque._start + deque._size ); i < endIdx; ++ i ) {
			new ( chunks[ i / VALUES_PER_CHUNK ] + ( i % VALUES_PER_CHUNK ) ) value_type( srcChunks[ i / VALUES_PER_CHUNK ][ i % VALUES_PER_CHUNK ] );
		}
		_start = deque._start;
		_size = deque._size;
	}
	return;
	M_EPILOG
}

template<typename type_t, typename allocator_t>
void HDeque<type_t, allocator_t>::clear( void ) {
	M_PROLOG
	value_type** chunks = _chunks.get<value_type*>();
	for ( iterator it( begin() ), endIt( end() ); it != endIt; ++ it ) {
		M_SAFE( (*it).~value_type() );
	}
	for ( size_type i( _start / VALUES_PER_CHUNK ),
			CAPACITY( _chunks.count_of<value_type*>() );
			( i < CAPACITY ) && chunks[ i ]; ++ i ) {
		operator delete ( static_cast<void*>( chunks[ i ] ) );
		chunks[ i ] = nullptr;
	}
	_start = 0;
	_size = 0;
	return;
	M_EPILOG
}

template<typename type_t, typename allocator_t>
void HDeque<type_t, allocator_t>::accommodate_chunks( size_type size_ ) {
	M_PROLOG
	size_type firstUsedChunkIndex( _start / VALUES_PER_CHUNK );
	size_type usedChunksCount( used_chunks() );
	size_type availableChunksCount( _chunks.count_of<value_type*>() );
	size_type newFirstChunkIndex( firstUsedChunkIndex );
	size_type newUsedChunksCount( usedChunksCount );
	size_type newAvailableChunksCount( availableChunksCount );
	if ( size_ > 0 ) { /* Add chunks at back of HDeque<>. */
		size_type newLastChunkIndex = ( _start + _size + size_ - 1 ) / VALUES_PER_CHUNK;
		newUsedChunksCount = newLastChunkIndex + 1 - firstUsedChunkIndex;
		if ( newLastChunkIndex >= availableChunksCount ) {
			if ( newUsedChunksCount > availableChunksCount ) {
				_chunks.realloc( chunk_size<value_type*>( max( newUsedChunksCount, MIN_CHUNKS_COUNT ) ) );
				newAvailableChunksCount = _chunks.count_of<value_type*>();
				M_ASSERT( newAvailableChunksCount >= newUsedChunksCount );
			}
			newFirstChunkIndex = ( newAvailableChunksCount - newUsedChunksCount ) / 2;
		}
	} else if ( size_ < 0 ) { /* Add chunks in front of HDeque<>. */
		size_type newStart( _start + size_ );
		if ( newStart < 0 ) {
			size_type frontExtension( minimal_cover_chunks_count( - newStart ) + firstUsedChunkIndex );
			newUsedChunksCount = usedChunksCount + frontExtension;
			if ( newUsedChunksCount > availableChunksCount ) {
				_chunks.realloc( chunk_size<value_type*>( max( newUsedChunksCount, MIN_CHUNKS_COUNT ) ) );
				newAvailableChunksCount = _chunks.count_of<value_type*>();
				M_ASSERT( newAvailableChunksCount >= newUsedChunksCount );
			}
			newFirstChunkIndex = ( ( newAvailableChunksCount - newUsedChunksCount ) / 2 ) + frontExtension;
		}
	}
	if ( size_ ) {
		size_type moveBy( firstUsedChunkIndex - newFirstChunkIndex );
		value_type** chunks = _chunks.get<value_type*>();
		if ( _size > 0 ) {
			if ( moveBy > 0 ) { /* move to front */
				copy( chunks + firstUsedChunkIndex, chunks + firstUsedChunkIndex + usedChunksCount, chunks + newFirstChunkIndex );
				fill( chunks + firstUsedChunkIndex + usedChunksCount - min( moveBy, usedChunksCount ), chunks + firstUsedChunkIndex + usedChunksCount, static_cast<value_type*>( nullptr ) );
				_start -= ( moveBy * VALUES_PER_CHUNK );
			} else if ( moveBy < 0 ) { /* move to back */
				M_ASSERT( ( newFirstChunkIndex + usedChunksCount ) <= newAvailableChunksCount );
				copy_backward( chunks + firstUsedChunkIndex, chunks + firstUsedChunkIndex + usedChunksCount, chunks + newFirstChunkIndex + usedChunksCount );
				fill( chunks + firstUsedChunkIndex, chunks + firstUsedChunkIndex + min( - moveBy, usedChunksCount ), static_cast<value_type*>( nullptr ) );
				_start -= ( moveBy * VALUES_PER_CHUNK );
				M_ASSERT( ( ( _start + _size + size_ - 1 ) / VALUES_PER_CHUNK ) < newAvailableChunksCount );
			}
			M_ASSERT( ( _start >= 0 ) && ( ( _start / VALUES_PER_CHUNK ) == newFirstChunkIndex ) );
		} else
			_start = newFirstChunkIndex * VALUES_PER_CHUNK;
		if ( size_ > 0 ) {
			for ( size_type i( ( _start + _size + size_ - 1 ) / VALUES_PER_CHUNK ); ( i >= ( newFirstChunkIndex + usedChunksCount ) ) && ! chunks[ i ]; -- i ) {
				chunks[ i ] = static_cast<value_type*>( ::operator new ( CHUNK_SIZE, memory::yaal ) );
			}
		} else if ( size_ < 0 ) {
			M_ASSERT( ( _start + size_ ) >= 0 );
			for ( size_type i( ( _start + size_ ) / VALUES_PER_CHUNK ); ( i < newFirstChunkIndex ) && ! chunks[ i ]; ++ i ) {
				chunks[ i ] = static_cast<value_type*>( ::operator new (  CHUNK_SIZE, memory::yaal ) );
			}
		}
	}
	return;
	M_EPILOG
}

template<typename type_t, typename allocator_t>
void HDeque<type_t, allocator_t>::resize( size_type size_, type_t const& fillWith_ ) {
	M_PROLOG
	M_ASSERT( _size >= 0 );
	if ( size_ < 0 ) {
		M_THROW( _errMsgHDeque_[ ERROR::BAD_SIZE ], size_ );
	}
	if ( size_ > _size ) {
		M_ASSERT( _start + size_ > 0 );
		accommodate_chunks( size_ - _size );
		value_type** chunks = _chunks.get<value_type*>();
		for ( size_type i( _start + _size ), last( _start + size_ ); i < last; ++ i ) {
			new ( chunks[ i / VALUES_PER_CHUNK ] + ( i % VALUES_PER_CHUNK ) ) value_type( fillWith_ );
		}
	} else if ( size_ < _size ) {
		value_type** chunks = _chunks.get<value_type*>();
		for ( size_type i( _start + size_ ), last( _start + _size ); i < last; ++ i ) {
			M_SAFE( chunks[ i / VALUES_PER_CHUNK ][ i % VALUES_PER_CHUNK ].~value_type() );
		}
		for ( size_type i( ( ( ( _start + size_ - 1 ) >= _start ? ( _start + size_ - 1 ) : _start + size_ ) / VALUES_PER_CHUNK ) + ( size_ ? 1 : 0 ) ),
				lastChunkIndex( ( ( _start + _size - 1 ) / VALUES_PER_CHUNK ) + 1 ); i < lastChunkIndex; ++ i ) {
			::operator delete ( static_cast<void*>( chunks[ i ] ), memory::yaal );
			chunks[ i ] = nullptr;
		}
	}
	_size = size_;
	return;
	M_EPILOG
}

template<typename type_t, typename allocator_t>
void HDeque<type_t, allocator_t>::insert_space( size_type index_, size_type size_ ) {
	M_PROLOG
	accommodate_chunks( ( index_ <= _size / 2 ) ? -size_ : size_ );
	value_type** chunks = _chunks.get<value_type*>();
	/* When we insert space we should ensure that we will move as little of
	 * existing objects as possible. We have something like "used chunks" in
	 * _chunks array, if we want to insert space before middle of used chunks range
	 * than we shall move front part of existing HDeque<>, if we insert space
	 * after the middle of used chunks range we shall move back part of existing HDeque<>.
	 */
	/* Here we perform actual move of smaller part.
	 */
	if ( index_ <= ( _size / 2 ) )
		_start -= size_;
	if ( _size > 0 ) {
		if ( index_ <= ( _size / 2 ) ) { /* Move from back to front. */
			for ( size_type src( _start + size_ ), dst( _start ); dst < ( index_ + _start ); ++ src, ++ dst ) {
				new ( chunks[ dst / VALUES_PER_CHUNK ] + ( dst % VALUES_PER_CHUNK ) ) value_type( yaal::move( chunks[ src / VALUES_PER_CHUNK ][ src % VALUES_PER_CHUNK ] ) );
				M_SAFE( chunks[ src / VALUES_PER_CHUNK ][ src % VALUES_PER_CHUNK ].~value_type() );
			}
		} else { /* Move from front to back. */
			for ( size_type src( _start + _size - 1 ), dst( _start + _size + size_ - 1 ); src >= ( _start + index_ ); -- src, -- dst ) {
				new ( chunks[ dst / VALUES_PER_CHUNK ] + ( dst % VALUES_PER_CHUNK ) ) value_type( yaal::move( chunks[ src / VALUES_PER_CHUNK ][ src % VALUES_PER_CHUNK ] ) );
				M_SAFE( chunks[ src / VALUES_PER_CHUNK ][ src % VALUES_PER_CHUNK ].~value_type() );
			}
		}
	}
	_size += size_;
	return;
	M_EPILOG
}

template<typename type_t, typename allocator_t>
template<typename iterator_t>
void HDeque<type_t, allocator_t>::insert( iterator it_, iterator_t first_, iterator_t last_ ) {
	M_PROLOG
	M_ASSERT( it_._owner == this );
	if ( ( it_._index < 0 ) || ( it_._index > _size ) ) {
		M_THROW( _errMsgHDeque_[ ERROR::INVALID_ITERATOR ], it_._index );
	}
	using yaal::distance;
	insert_space( it_._index, distance( first_, last_ ) );
	value_type** chunks = _chunks.get<value_type*>();
	for ( size_type i( it_._index + _start ); first_ != last_; ++ first_, ++ i ) {
		new ( chunks[ i / VALUES_PER_CHUNK ] + ( i % VALUES_PER_CHUNK ) ) value_type( *first_ );
	}
	return;
	M_EPILOG
}

template<typename type_t, typename allocator_t>
typename HDeque<type_t, allocator_t>::iterator HDeque<type_t, allocator_t>::erase( iterator first_, iterator last_ ) {
	M_PROLOG
	M_ASSERT( first_._owner == this );
	M_ASSERT( last_._owner == this );
	if ( ( first_._index < 0 ) || ( first_._index > _size ) )
		M_THROW( _errMsgHDeque_[ ERROR::INVALID_ITERATOR ], first_._index );
	if ( ( last_._index < 0 ) || ( last_._index > _size ) )
		M_THROW( _errMsgHDeque_[ ERROR::INVALID_ITERATOR ], last_._index );
	size_type toRemove( last_._index - first_._index );
	if ( last_._index < first_._index ) {
		M_THROW( _errMsgHDeque_[ ERROR::INVALID_ITERATOR ], toRemove );
	}
	if ( toRemove ) {
		value_type** chunks = _chunks.get<value_type*>();
		if ( first_._index < ( _size - last_._index ) ) /* Move front. */ {
			for ( size_type src( _start ), dst( _start + first_._index ); dst < ( _start + last_._index ); ++ src, ++ dst ) {
				chunks[ dst / VALUES_PER_CHUNK ][ dst % VALUES_PER_CHUNK ] = yaal::move( chunks[ src / VALUES_PER_CHUNK ][ src % VALUES_PER_CHUNK ] );
			}
			for ( size_type del( _start ); del < ( _start + toRemove ); ++ del ) {
				M_SAFE( chunks[ del / VALUES_PER_CHUNK ][ del % VALUES_PER_CHUNK ].~value_type() );
			}
			for (
				size_type chunkIndex( _start / VALUES_PER_CHUNK ), newFirstChunkIndex( ( _start + toRemove ) / VALUES_PER_CHUNK );
				chunkIndex < newFirstChunkIndex;
				++ chunkIndex
			) {
				M_ASSERT( chunks[ chunkIndex ] );
				::operator delete ( static_cast<void*>( chunks[ chunkIndex ] ), memory::yaal );
				chunks[ chunkIndex ] = nullptr;
			}
			_start += toRemove;
		} else { /* Move back. */
			for ( size_type src( _start + last_._index ), dst( _start + first_._index ); src < ( _start + _size ); ++ src, ++ dst ) {
				chunks[ dst / VALUES_PER_CHUNK ][ dst % VALUES_PER_CHUNK ] = yaal::move( chunks[ src / VALUES_PER_CHUNK ][ src % VALUES_PER_CHUNK ] );
			}
			for ( size_type del( _start + _size - toRemove ); del < ( _start + _size ); ++ del ) {
				M_SAFE( chunks[ del / VALUES_PER_CHUNK ][ del % VALUES_PER_CHUNK ].~value_type() );
			}
			size_type chunksCount( _chunks.template count_of<value_type*>() );
			size_type trimAt( ( _start + _size - toRemove ) - 1 );
			trimAt = ( ( trimAt >= _start ) ? trimAt : ( _start + _size - toRemove ) );
			for (
				size_type chunkIndex( ( trimAt / VALUES_PER_CHUNK ) + ( ( _size - toRemove ) > 0 ? 1 : 0 ) );
				( chunkIndex < ( ( ( _start + _size - 1 ) / VALUES_PER_CHUNK ) + 1 ) ) && ( chunkIndex < chunksCount );
				++ chunkIndex
			) {
				M_ASSERT( chunks[ chunkIndex ] );
				::operator delete ( static_cast<void*>( chunks[ chunkIndex ] ), memory::yaal );
				chunks[ chunkIndex ] = nullptr;
			}
		}
		_size -= toRemove;
	}
	if ( ! _size ) {
		_start = 0;
	}
	return first_;
	M_EPILOG
}

template<typename type_t, typename allocator_t>
typename HDeque<type_t, allocator_t>::value_type* HDeque<type_t, allocator_t>::space_back( void ) {
	M_PROLOG
	size_type idx( _start + _size );
	size_type chunk( idx / VALUES_PER_CHUNK );
	size_type offset( idx % VALUES_PER_CHUNK );
	value_type** chunks = nullptr;
	if ( ! offset ) { /* We start new chunk. */
		size_type availableChunksCount( _chunks.count_of<value_type*>() );
		if ( chunk >= availableChunksCount ) { /* We need at least to move chunks. */
			size_type usedChunksCount( used_chunks() );
			size_type newUsedChunksCount( usedChunksCount + 1 );
			if ( newUsedChunksCount > availableChunksCount ) { /* We need allocate more space for chunks. */
				_chunks.realloc( chunk_size<value_type*>( max( newUsedChunksCount, MIN_CHUNKS_COUNT ) ) );
				availableChunksCount = _chunks.count_of<value_type*>();
			}
			/* We have enough space for chunks, but we need to move existing chunks. */
			chunks = _chunks.get<value_type*>();
			size_type firstUsedChunkIndex( _start / VALUES_PER_CHUNK );
			size_type newFirstUsedChunk( ( availableChunksCount - newUsedChunksCount ) / 2 );
			size_type moveBy( firstUsedChunkIndex - newFirstUsedChunk );
			if ( _size > 0 ) {
				if ( moveBy > 0 ) { /* move to front */
					copy( chunks + firstUsedChunkIndex, chunks + firstUsedChunkIndex + usedChunksCount, chunks + newFirstUsedChunk );
					fill( chunks + firstUsedChunkIndex + usedChunksCount - min( moveBy, usedChunksCount ), chunks + firstUsedChunkIndex + usedChunksCount, static_cast<value_type*>( nullptr ) );
					_start -= ( moveBy * VALUES_PER_CHUNK );
				} else if ( moveBy < 0 ) { /* move to back */
					M_ASSERT( ( newFirstUsedChunk + usedChunksCount ) <= availableChunksCount );
					copy_backward( chunks + firstUsedChunkIndex, chunks + firstUsedChunkIndex + usedChunksCount, chunks + newFirstUsedChunk + usedChunksCount );
					fill( chunks + firstUsedChunkIndex, chunks + firstUsedChunkIndex + min( - moveBy, usedChunksCount ), static_cast<value_type*>( nullptr ) );
					_start -= ( moveBy * VALUES_PER_CHUNK );
					M_ASSERT( ( ( _start + _size ) / VALUES_PER_CHUNK ) < availableChunksCount );
				}
				M_ASSERT( ( _start >= 0 ) && ( ( _start / VALUES_PER_CHUNK ) == newFirstUsedChunk ) );
			} else {
				_start = newFirstUsedChunk * VALUES_PER_CHUNK;
			}
			chunk = ( _start + _size ) / VALUES_PER_CHUNK;
		} else { /* We have enough space and we do not need to move chunks. */
			chunks = _chunks.get<value_type*>();
		}
		chunks[ chunk ] = static_cast<value_type*>( ::operator new ( CHUNK_SIZE, memory::yaal ) );
	} else { /* We use old chunk. */
		chunks = _chunks.get<value_type*>();
	}
	return ( _chunks.get<value_type*>()[ chunk ] + offset );
	M_EPILOG
}

template<typename type_t, typename allocator_t>
typename HDeque<type_t, allocator_t>::value_type* HDeque<type_t, allocator_t>::space_front( void ) {
	M_PROLOG
	size_type chunk( 0 );
	size_type offset( 0 );
	value_type** chunks = nullptr;
	if ( ! ( _start % VALUES_PER_CHUNK ) ) { /* New chunk in front required. */
		if ( ! _start ) { /* We need at least to move chunks. */
			size_type availableChunksCount( _chunks.count_of<value_type*>() );
			size_type usedChunksCount( used_chunks() );
			size_type newUsedChunksCount( usedChunksCount + 1 );
			if ( newUsedChunksCount > availableChunksCount ) { /* We need allocate more space for chunks. */
				_chunks.realloc( chunk_size<value_type*>( max( newUsedChunksCount, MIN_CHUNKS_COUNT ) ) );
				availableChunksCount = _chunks.count_of<value_type*>();
			}
			/* We have enough space for chunks, but we need to move existing chunks. */
			chunks = _chunks.get<value_type*>();
			size_type oldFirstUsedChunk( ( ( availableChunksCount - newUsedChunksCount ) / 2 ) + 1 );
			if ( _size > 0 ) {
				M_ASSERT( ( oldFirstUsedChunk + usedChunksCount ) <= availableChunksCount );
				copy_backward( chunks, chunks + usedChunksCount, chunks + usedChunksCount + oldFirstUsedChunk );
				fill( chunks, chunks + min( oldFirstUsedChunk, usedChunksCount ), static_cast<value_type*>( nullptr ) );
				_start += ( oldFirstUsedChunk * VALUES_PER_CHUNK );
				M_ASSERT( ( ( ( _start - 1 ) + _size ) / VALUES_PER_CHUNK ) < availableChunksCount );
				M_ASSERT( ( _start > 0 ) && ( ( _start / VALUES_PER_CHUNK ) == oldFirstUsedChunk ) );
			} else
				_start = oldFirstUsedChunk * VALUES_PER_CHUNK;
		} else { /* We have enough space and we do not need to move chunks. */
			chunks = _chunks.get<value_type*>();
		}
		-- _start;
		chunk = _start / VALUES_PER_CHUNK;
		offset = VALUES_PER_CHUNK - 1;
		chunks[ chunk ] = static_cast<value_type*>( ::operator new ( CHUNK_SIZE, memory::yaal ) );
	} else { /* We use old chunk. */
		chunks = _chunks.get<value_type*>();
		-- _start;
		chunk = _start / VALUES_PER_CHUNK;
		offset = _start % VALUES_PER_CHUNK;
	}
	return ( _chunks.get<value_type*>()[ chunk ] + offset );
	M_EPILOG
}

template<typename type_t, typename allocator_t>
void HDeque<type_t, allocator_t>::pop_back( void ) {
	M_PROLOG
	M_ASSERT( _size > 0 );
	size_type idx( _start + _size - 1 );
	size_type chunk( idx / VALUES_PER_CHUNK );
	size_type offset( idx % VALUES_PER_CHUNK );
	value_type** chunks = _chunks.get<value_type*>();
	M_SAFE( ( chunks[ chunk ] + offset )->~value_type() );
	-- _size;
	if ( ! ( offset && _size ) ) {
		::operator delete ( static_cast<void*>( chunks[ chunk ] ), memory::yaal );
		chunks[ chunk ] = nullptr;
	}
	if ( ! _size ) {
		_start = 0;
	}
	return;
	M_EPILOG
}

template<typename type_t, typename allocator_t>
void HDeque<type_t, allocator_t>::pop_front( void ) {
	M_PROLOG
	M_ASSERT( _size > 0 );
	size_type chunk( _start / VALUES_PER_CHUNK );
	size_type offset( _start % VALUES_PER_CHUNK );
	value_type** chunks = _chunks.get<value_type*>();
	M_SAFE( ( chunks[ chunk ] + offset )->~value_type() );
	-- _size;
	++ _start;
	if ( ! ( ( _start % VALUES_PER_CHUNK ) && _size ) ) {
		::operator delete ( static_cast<void*>( chunks[ chunk ] ), memory::yaal );
		chunks[ chunk ] = nullptr;
	}
	if ( ! _size ) {
		_start = 0;
	}
	return;
	M_EPILOG
}

template<typename type_t, typename allocator_t>
template<typename iterator_t>
void HDeque<type_t, allocator_t>::assign( iterator_t first_, iterator_t last_ ) {
	M_PROLOG
	using yaal::distance;
	size_type count( distance( first_, last_ ) );
	if ( count <= _size ) {
		iterator last( copy( first_, last_, begin() ) );
		erase( last, end() );
	} else {
		copy_n( first_, _size, begin() );
		using yaal::advance;
		advance( first_, _size );
		insert( end(), first_, last_ );
	}
	return;
	M_EPILOG
}

template<typename type_t, typename allocator_t>
void HDeque<type_t, allocator_t>::assign( size_type count_, type_t const& value_ ) {
	M_PROLOG
	fill_n( begin(), min( count_, _size ), value_ );
	if ( count_ < _size ) {
		erase( begin() + count_, end() );
	} else if ( count_ > _size ) {
		resize( count_, value_ );
	}
	return;
	M_EPILOG
}

template<typename type_t, typename allocator_t>
typename HDeque<type_t, allocator_t>::value_type* HDeque<type_t, allocator_t>::space_at( size_type pos_ ) {
	M_PROLOG
	if ( ( pos_ < 0 ) && ( pos_ > _size ) ) {
		M_THROW( _errMsgHDeque_[ ERROR::INVALID_ITERATOR ], pos_ );
	}
	insert_space( pos_, 1 );
	value_type** chunks = _chunks.get<value_type*>();
	size_type idx( _start + pos_ );
	return ( chunks[ idx / VALUES_PER_CHUNK ] + ( idx % VALUES_PER_CHUNK ) );
	M_EPILOG
}

template<typename type_t, typename allocator_t>
void HDeque<type_t, allocator_t>::insert( iterator pos_, size_type count_, type_t const& value_ ) {
	M_PROLOG
	M_ASSERT( pos_._owner == this );
	if ( ( pos_._index < 0 ) && ( pos_._index > _size ) ) {
		M_THROW( _errMsgHDeque_[ ERROR::INVALID_ITERATOR ], pos_._index );
	}
	insert_space( pos_._index, count_ );
	value_type** chunks = _chunks.get<value_type*>();
	for ( size_type i( _start + pos_._index ), last( _start + pos_._index + count_ ); i < last; ++ i ) {
		new ( chunks[ i / VALUES_PER_CHUNK ] + ( i % VALUES_PER_CHUNK ) ) value_type( value_ );
	}
	return;
	M_EPILOG
}

}

}

#endif /* #ifndef YAAL_HCORE_HDEQUE_HXX_INCLUDED */

