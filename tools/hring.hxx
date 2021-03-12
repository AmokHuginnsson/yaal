/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file tools/hring.hxx
 * \brief Implementation of HRing<>.
 */

#ifndef YAAL_TOOLS_HRING_HXX_INCLUDED
#define YAAL_TOOLS_HRING_HXX_INCLUDED 1

#include <new>

#include "hcore/hchunk.hxx"
#include "hcore/algorithm.hxx"
#include "hcore/pod.hxx"
#include "hcore/htaggedpod.hxx"

namespace yaal {

namespace tools {

extern M_YAAL_TOOLS_PUBLIC_API char const* const _errMsgHRing_[];

namespace ring {

struct capacity_tag;
struct size_tag;

typedef yaal::hcore::HTaggedPOD<int long, capacity_tag> capacity_type;
typedef yaal::hcore::HTaggedPOD<int long, size_tag> size_type;

}

/*! \brief HRing<> - a cyclic (aka circular) container.
 */
template<typename type_t>
class HRing final {
public:
	typedef HRing<type_t> this_type;
	typedef type_t value_type;
	typedef ring::capacity_type capacity_type;
	typedef ring::size_type size_type;
	/*! \brief Error codes for HRing<> operations.
	 */
	struct ERROR {
		/*! \brief Error codes for HRing<> operations.
		 */
		typedef enum {
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
	HRing( void )
		: _buf(), _start( 0 ), _size( 0 ) {
		return;
	}
	explicit HRing( capacity_type capacity_ )
		: _buf(), _start( 0 ), _size( 0 ) {
		M_PROLOG
		reserve( capacity_ );
		return;
		M_EPILOG
	}
	HRing( capacity_type capacity_, size_type size_, type_t const& fillWith_ )
		: _buf(), _start( 0 ), _size( 0 ) {
		M_PROLOG
		resize( capacity_, size_, fillWith_ );
		return;
		M_EPILOG
	}
	template<typename iterator_t>
	HRing( capacity_type capacity_, iterator_t first, iterator_t last )
		: _buf(), _start( 0 ), _size( 0 ) {
		M_PROLOG
		initialize( capacity_, first, last, typename trait::add_pointer<typename is_integral<iterator_t>::type>::type() );
		return;
		M_EPILOG
	}
	~HRing( void ) {
		M_PROLOG
		clear();
		return;
		M_DESTRUCTOR_EPILOG
	}
	HRing( HRing const& );
	HRing& operator = ( HRing const& );
	void resize( capacity_type, size_type, type_t const& = value_type() );
	void reserve( capacity_type );
	type_t& operator [] ( int long );
	type_t const& operator [] ( int long ) const;
	void push_back( type_t const& value_ ) {
		M_PROLOG
		new ( space_back() ) value_type( value_ );
		++ _size;
		return;
		M_EPILOG
	}
	template<typename... arg_t>
	void push_back( arg_t&&... arg_ ) {
		M_PROLOG
		new ( space_back() ) value_type( yaal::forward<arg_t>( arg_ )... );
		++ _size;
		return;
		M_EPILOG
	}
	void pop_back( void );
	void push_front( type_t const& value_ ) {
		M_PROLOG
		new ( space_front() ) value_type( value_ );
		++ _size;
		return;
		M_EPILOG
	}
	template<typename... arg_t>
	void push_front( arg_t&&... arg_ ) {
		M_PROLOG
		new ( space_front() ) value_type( yaal::forward<arg_t>( arg_ )... );
		++ _size;
		return;
		M_EPILOG
	}
	void pop_front( void );
	type_t const& front( void ) const {
		M_PROLOG
		M_ASSERT( _size > 0 );
		return ( _buf.get<value_type const>()[ _start ] );
		M_EPILOG
	}
	type_t& front( void ) {
		M_PROLOG
		M_ASSERT( _size > 0 );
		return ( _buf.get<value_type>()[ _start ] );
		M_EPILOG
	}
	type_t& back( void );
	type_t const& back( void ) const;
	size_type get_size( void ) const {
		M_PROLOG
		return ( size_type( _size ) );
		M_EPILOG
	}
	size_type size( void ) const {
		M_PROLOG
		return ( size_type( _size ) );
		M_EPILOG
	}
	capacity_type capacity( void ) const {
		M_PROLOG
		return ( get_capacity() );
		M_EPILOG
	}
	capacity_type get_capacity( void ) const {
		M_PROLOG
		return ( capacity_type( _buf.count_of<value_type>() ) );
		M_EPILOG
	}
	bool is_empty( void ) const {
		M_PROLOG
		return ( _size ? false : true );
		M_EPILOG
	}
	bool empty( void ) const {
		M_PROLOG
		return ( _size ? false : true );
		M_EPILOG
	}
	bool is_full( void ) const {
		M_PROLOG
		return ( _size == get_capacity() );
		M_EPILOG
	}
	bool full( void ) const {
		M_PROLOG
		return ( is_full() );
		M_EPILOG
	}
	void clear( void ) {
		M_PROLOG
		erase( begin(), end() );
		return;
		M_EPILOG
	}
	explicit operator bool ( void ) const {
		M_PROLOG
		return ( ! is_empty() );
		M_EPILOG
	}
	iterator insert( iterator, type_t const& );
	template<typename iterator_t>
	void assign( iterator_t first, iterator_t last ) {
		M_PROLOG
		clear();
		insert( end(), first, last );
		return;
		M_EPILOG
	}
	void assign( size_type size_, type_t const& fillWith_ ) {
		M_PROLOG
		int long oldSize( _size );
		resize( get_capacity(), size_, fillWith_ );
		if ( oldSize > 0 )
			fill_n( begin(), oldSize, fillWith_ );
		return;
		M_EPILOG
	}
	template<typename iterator_t>
	void insert( iterator, iterator_t, iterator_t );
	void insert( iterator, int long, type_t const& );
	iterator erase( iterator it ) {
		M_PROLOG
		iterator next( it );
		++ next;
		return ( erase( it, next ) );
		M_EPILOG
	}
	iterator erase( iterator, iterator );
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
	void swap( HRing& );
	bool operator == ( HRing const& a_ ) const {
		M_PROLOG
		return ( ( &a_ == this ) || safe_equal( begin(), end(), a_.begin(), a_.end() ) );
		M_EPILOG
	}
	bool operator < ( HRing const& a_ ) const {
		M_PROLOG
		return ( ( &a_ != this ) && lexicographical_compare( begin(), end(), a_.begin(), a_.end() ) );
		M_EPILOG
	}
private:
	template<typename iterator_t>
	void initialize( capacity_type capacity_, iterator_t first, iterator_t last, trait::false_type const* ) {
		M_PROLOG
		reserve( capacity_ );
		insert( end(), first, last );
		return;
		M_EPILOG
	}
	void initialize( capacity_type capacity_, size_type size_, type_t const& fillWith_, trait::true_type const* ) {
		M_PROLOG
		resize( capacity_, size_, fillWith_ );
		return;
		M_EPILOG
	}
	value_type* space_front( void );
	value_type* space_back( void );
	void insert_space( int long, int long );
};

template<typename type_t>
inline void swap( yaal::tools::HRing<type_t>& a, yaal::tools::HRing<type_t>& b )
	{ a.swap( b ); }

/*! \brief Iterator for HRing<> data structure.
 */
template<typename type_t>
template<typename const_qual_t>
class HRing<type_t>::HIterator : public yaal::hcore::iterator_interface<const_qual_t, yaal::hcore::iterator_category::random_access> {
	typedef HRing<type_t> array_t;
	array_t const* _owner;
	int long _index;
public:
	typedef yaal::hcore::iterator_interface<const_qual_t, yaal::hcore::iterator_category::random_access> base_type;
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
	HIterator operator ++ ( int ) {
		HIterator it( *this );
		++ _index;
		return it;
	}
	HIterator& operator -- ( void ) {
		-- _index;
		return ( *this );
	}
	HIterator operator -- ( int ) {
		HIterator it( *this );
		-- _index;
		return it;
	}
	HIterator operator + ( int long off_ ) const {
		HIterator it( _owner, _index + off_ );
		return it;
	}
	HIterator& operator += ( int long off_ ) {
		_index += off_;
		return ( *this );
	}
	HIterator operator - ( int long off_ ) const {
		HIterator it( _owner, _index - off_ );
		return it;
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
		int long idx( _index + _owner->_start );
		int long capacity( _owner->get_capacity().get() );
		if ( idx >= capacity )
			idx -= capacity;
		return ( const_cast<const_qual_t&>( _owner->_buf.template get<const_qual_t>()[ idx ] ) );
	}
	const_qual_t& operator* ( void ) const {
		M_ASSERT( _owner && ( _index >= 0 ) && ( _index < _owner->_size ) );
		int long idx( _index + _owner->_start );
		int long capacity( _owner->get_capacity().get() );
		if ( idx >= capacity )
			idx -= capacity;
		return ( _owner->_buf.template get<const_qual_t>()[ idx ] );
	}
	const_qual_t* operator-> ( void ) {
		M_ASSERT( _owner && ( _index >= 0 ) && ( _index < _owner->_size ) );
		int long idx( _index + _owner->_start );
		int long capacity( _owner->get_capacity().get() );
		if ( idx >= capacity )
			idx -= capacity;
		return ( const_cast<const_qual_t*>( &_owner->_buf.template get<const_qual_t>()[ idx ] ) );
	}
	const_qual_t* operator-> ( void ) const {
		M_ASSERT( _owner && ( _index >= 0 ) && ( _index < _owner->_size ) );
		int long idx( _index + _owner->_start );
		int long capacity( _owner->get_capacity().get() );
		if ( idx >= capacity )
			idx -= capacity;
		return ( &_owner->_buf.template get<const_qual_t>()[ idx ] );
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
	friend class HRing<type_t>;
	explicit HIterator( array_t const* owner_, int long idx )
		: base_type(), _owner( owner_ ), _index( idx ) {};
};

template<typename type_t>
HRing<type_t>::HRing( HRing const& arr_ )
	: _buf(), _start( 0 ), _size( 0 ) {
	M_PROLOG
	if ( arr_._size > 0 )
		{
		_buf.realloc( yaal::hcore::chunk_size<value_type>( arr_._size ) );
		_size = arr_._size;
		int long curCapacity( arr_.get_capacity().get() );
		value_type* dst( _buf.get<value_type>() );
		value_type const* src( arr_._buf.template get<value_type>() );
		for ( int long i( 0 ); i < _size; ++ i ) {
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
HRing<type_t>& HRing<type_t>::operator = ( HRing const& arr_ ) {
	M_PROLOG
	if ( &arr_ != this ) {
		if ( arr_._size > get_capacity() ) {
			HRing<value_type> tmp( arr_ );
			swap( tmp );
		} else {
			clear();
			value_type* dst( _buf.get<value_type>() );
			value_type const* src( arr_._buf.template get<value_type>() );
			int long curCapacity( arr_.get_capacity().get() );
			for ( int long i( 0 ); i < arr_._size; ++ i ) {
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
void HRing<type_t>::resize( capacity_type capacity_, size_type size_, type_t const& fillWith_ ) {
	M_PROLOG
	if ( size_ < 0 )
		M_THROW( _errMsgHRing_[ ERROR::BAD_SIZE ], size_.get() );
	if ( size_.get() > capacity_ )
		M_THROW( _errMsgHRing_[ ERROR::BAD_SIZE ], capacity_.get() - size_.get() );
	if ( size_ > _size ) {
		reserve( capacity_ );
		int long curCapacity( get_capacity().get() );
		value_type* arr( _buf.get<value_type>() );
		for ( int long i( _size ); i < size_; ++ i ) {
			int long idx( i + _start );
			if ( idx >= curCapacity )
				idx -= curCapacity;
			new ( arr + idx ) value_type( fillWith_ );
		}
	} else if ( size_ < _size ) {
		int long curCapacity( get_capacity().get() );
		value_type* arr( _buf.get<value_type>() );
		for ( int long i( size_.get() ); i < _size; ++ i ) {
			int long idx( i + _start );
			if ( idx >= curCapacity )
				idx -= curCapacity;
			M_SAFE( arr[ idx ].~value_type() );
		}
	}
	_size = size_.get();
	return;
	M_EPILOG
}

template<typename type_t>
void HRing<type_t>::reserve( capacity_type capacity_ ) {
	M_PROLOG
	if ( capacity_ < 0 )
		M_THROW( _errMsgHRing_[ ERROR::BAD_SIZE ], capacity_.get() );
	capacity_type curCapacity( get_capacity() );
	if ( capacity_ > curCapacity ) {
		yaal::hcore::HChunk newBuf( yaal::hcore::chunk_size<value_type>( max( capacity_, curCapacity * 2 ).get() ) );
		value_type* dst( newBuf.get<value_type>() );
		value_type* src( _buf.get<value_type>() );
		for ( int long i( 0 ); i < _size; ++ i ) {
			int long idx( i + _start );
			if ( idx >= curCapacity )
				idx -= curCapacity.get();
			new ( dst + i ) value_type( src[ idx ] );
		}
		for ( int long i( 0 ); i < _size; ++ i ) {
			int long idx( i + _start );
			if ( idx >= curCapacity )
				idx -= curCapacity.get();
			M_SAFE( src[ idx ].~value_type() );
		}
		_buf.swap( newBuf );
		_start = 0;
	}
	return;
	M_EPILOG
}

template<typename type_t>
template<typename iterator_t>
void HRing<type_t>::insert( iterator pos_, iterator_t first_, iterator_t last_ ) {
	M_PROLOG
	M_ASSERT( pos_._owner == this );
	if ( ( pos_._index < 0 ) && ( pos_._index > _size ) )
		M_THROW( _errMsgHRing_[ ERROR::INVALID_ITERATOR ], pos_._index );
	int long curCapacity( get_capacity().get() );
	using yaal::distance;
	int long elemCount( distance( first_, last_ ) );
	M_ENSURE( ( _size + elemCount ) <= curCapacity, _errMsgHRing_[ ERROR::BAD_SIZE ] );
	insert_space( pos_._index, elemCount );
	value_type* arr( _buf.get<value_type>() );
	for ( int long i( pos_._index + _start ); first_ != last_; ++ first_,  ++ i ) {
		int long idx( i >= curCapacity ? i - curCapacity : i );
		new ( arr + idx ) value_type( *first_ );
	}
	return;
	M_EPILOG
}

template<typename type_t>
void HRing<type_t>::insert( iterator pos_, int long count_, type_t const& value_ ) {
	M_PROLOG
	M_ASSERT( pos_._owner == this );
	if ( ( pos_._index < 0 ) && ( pos_._index > _size ) )
		M_THROW( _errMsgHRing_[ ERROR::INVALID_ITERATOR ], pos_._index );
	int long curCapacity( get_capacity().get() );
	M_ENSURE( ( _size + count_ ) <= curCapacity, _errMsgHRing_[ ERROR::BAD_SIZE ] );
	insert_space( pos_._index, count_ );
	value_type* arr( _buf.get<value_type>() );
	for ( int long i( pos_._index + _start ), last( pos_._index + count_ ); i < last; ++ i ) {
		new ( arr + i ) value_type( value_ );
	}
	return;
	M_EPILOG
}

template<typename type_t>
typename HRing<type_t>::iterator HRing<type_t>::insert( iterator pos_, type_t const& value_ ) {
	M_PROLOG
	M_ASSERT( pos_._owner == this );
	if ( ( pos_._index < 0 ) && ( pos_._index > _size ) )
		M_THROW( _errMsgHRing_[ ERROR::INVALID_ITERATOR ], pos_._index );
	int long curCapacity( get_capacity().get() );
	M_ENSURE( ( _size + 1 ) <= curCapacity, _errMsgHRing_[ ERROR::BAD_SIZE ] );
	insert_space( pos_._index, 1 );
	value_type* arr( _buf.get<value_type>() );
	int long idx( ( pos_._index + _start ) >= curCapacity ? pos_._index + _start - curCapacity : pos_._index + _start );
	new ( arr + idx ) value_type( value_ );
	return pos_;
	M_EPILOG
}

template<typename type_t>
void HRing<type_t>::insert_space( int long pos_, int long size_ ) {
	M_PROLOG
	int long curCapacity( get_capacity().get() );
	M_ASSERT( ( _size + size_ ) <= curCapacity );
	int long oldSize( _size );
	_size += size_;
	value_type* arr( _buf.get<value_type>() );
	if ( pos_ <= ( _size / 2 ) ) {
		_start -= size_;
		if ( _start < 0 )
			_start += curCapacity;
		for ( int long src( _start + size_ ), dst( _start ); src < ( _start + size_ + pos_ ); ++ src, ++ dst ) {
			int long srcIdx( src >= curCapacity ? src - curCapacity : src );
			int long dstIdx( dst >= curCapacity ? dst - curCapacity : dst );
			new ( arr + dstIdx ) value_type( arr[ srcIdx ] );
			M_SAFE( arr[ srcIdx ].~value_type() );
		}
	} else {
		for ( int long src( _start + oldSize - 1 ), dst( _start + _size - 1 ); src >= ( pos_ + _start ); -- src, -- dst ) {
			int long srcIdx( src >= curCapacity ? src - curCapacity : src );
			int long dstIdx( dst >= curCapacity ? dst - curCapacity : dst );
			new ( arr + dstIdx ) value_type( arr[ srcIdx ] );
			M_SAFE( arr[ srcIdx ].~value_type() );
		}
	}
	return;
	M_EPILOG
}

template<typename type_t>
typename HRing<type_t>::iterator HRing<type_t>::erase( iterator first_, iterator last_ ) {
	M_PROLOG
	M_ASSERT( first_._owner == this );
	M_ASSERT( last_._owner == this );
	if ( ( first_._index < 0 ) || ( first_._index > _size ) )
		M_THROW( _errMsgHRing_[ ERROR::INVALID_ITERATOR ], first_._index );
	if ( ( last_._index < 0 ) || ( last_._index > _size ) )
		M_THROW( _errMsgHRing_[ ERROR::INVALID_ITERATOR ], last_._index );
	if ( last_._index < first_._index )
		M_THROW( _errMsgHRing_[ ERROR::INVALID_ITERATOR ], last_._index - first_._index );
	if ( first_ != last_ ) {
		if ( first_._index < ( _size - last_._index ) ) {
			for ( iterator it( begin() ), endIt( copy_backward( begin(), first_, last_ ) ); it != endIt; ++ it )
				M_SAFE( (*it).~value_type() );
			_start += ( last_._index - first_._index );
		} else {
			for ( iterator it( copy( last_, end(), first_ ) ), endIt( end() ); ( it != endIt ); ++ it )
				M_SAFE( (*it).~value_type() );
		}
	}
	_size -= ( last_._index - first_._index );
	return first_;
	M_EPILOG
}

template<typename type_t>
type_t const& HRing<type_t>::back( void ) const {
	M_PROLOG
	M_ASSERT( _size > 0 );
	int long curCapacity( get_capacity().get() );
	M_ASSERT( _size <= curCapacity );
	int long idx( _start + _size - 1 );
	if ( idx >= curCapacity )
		idx -= curCapacity;
	value_type* arr( _buf.get<value_type>() );
	return ( arr[ idx ] );
	M_EPILOG
}

template<typename type_t>
type_t& HRing<type_t>::back( void ) {
	M_PROLOG
	M_ASSERT( _size > 0 );
	int long curCapacity( get_capacity().get() );
	M_ASSERT( _size <= curCapacity );
	int long idx( _start + _size - 1 );
	if ( idx >= curCapacity )
		idx -= curCapacity;
	value_type* arr( _buf.get<value_type>() );
	return ( arr[ idx ] );
	M_EPILOG
}

template<typename type_t>
type_t& HRing<type_t>::operator[] ( int long index_ ) {
	M_PROLOG
	int long idx = ( index_ < 0 ) ? index_ + _size : index_;
	if ( ( idx >= _size ) || ( idx < 0 ) )
		M_THROW( _errMsgHRing_[ ERROR::BAD_INDEX ], idx );
	int long cap( get_capacity().get() );
	idx += _start;
	if ( idx >= cap )
		idx -= cap;
	return ( _buf.get<value_type>()[ idx ] );
	M_EPILOG
}

template<typename type_t>
type_t const& HRing<type_t>::operator[] ( int long index_ ) const {
	M_PROLOG
	int long idx( ( index_ < 0 ) ? index_ + _size : index_ );
	if ( ( idx >= _size ) || ( idx < 0 ) )
		M_THROW( _errMsgHRing_[ ERROR::BAD_INDEX ], idx );
	int long cap( get_capacity().get() );
	idx += _start;
	if ( idx >= cap )
		idx -= cap;
	return ( _buf.get<value_type const>()[ idx ] );
	M_EPILOG
}

template<typename type_t>
void HRing<type_t>::swap( HRing& other ) {
	M_PROLOG
	if ( &other != this ) {
		using yaal::swap;
		swap( _buf, other._buf );
		swap( _start, other._start );
		swap( _size, other._size );
	}
	return;
	M_EPILOG
}

template<typename type_t>
typename HRing<type_t>::value_type* HRing<type_t>::space_back( void ) {
	M_PROLOG
	int long curCapacity( get_capacity().get() );
	M_ASSERT( _size <= curCapacity );
	M_ENSURE( _size < curCapacity, _errMsgHRing_[ ERROR::RING_IS_FULL ] );
	value_type* arr( _buf.get<value_type>() );
	int long idx( _start + _size );
	if ( idx >= curCapacity )
		idx -= curCapacity;
	return ( arr + idx );
	M_EPILOG
}

template<typename type_t>
typename HRing<type_t>::value_type* HRing<type_t>::space_front( void ) {
	M_PROLOG
	int long curCapacity( get_capacity().get() );
	M_ASSERT( _size <= curCapacity );
	M_ENSURE( _size < curCapacity, _errMsgHRing_[ ERROR::RING_IS_FULL ] );
	-- _start;
	if ( _start < 0 )
		_start = curCapacity - 1;
	value_type* arr( _buf.get<value_type>() );
	return ( arr + _start );
	M_EPILOG
}

template<typename type_t>
void HRing<type_t>::pop_back( void ) {
	M_PROLOG
	M_ENSURE( _size > 0, _errMsgHRing_[ ERROR::RING_IS_EMPTY ] );
	-- _size;
	int long cap( get_capacity().get() );
	int long idx( _start + _size );
	if ( idx >= cap )
		idx -= cap;
	value_type* arr( _buf.get<value_type>() );
	M_SAFE( arr[ idx ].~value_type() );
	return;
	M_EPILOG
}

template<typename type_t>
void HRing<type_t>::pop_front( void ) {
	M_PROLOG
	M_ENSURE( _size > 0, _errMsgHRing_[ ERROR::RING_IS_EMPTY ] );
	value_type* arr( _buf.get<value_type>() );
	M_SAFE( arr[ _start ++ ].~value_type() );
	if ( _start == get_capacity() )
		_start = 0;
	-- _size;
	return;
	M_EPILOG
}

}

}

#endif /* #ifndef YAAL_TOOLS_HRING_HXX_INCLUDED */

