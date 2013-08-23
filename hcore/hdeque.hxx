/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hcore/hdeque.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/hdeque.hxx
 * \brief Implementation of HDeque<> a Double-Ended-QUEue.
 */

#ifndef YAAL_HCORE_HDEQUE_HXX_INCLUDED
#define YAAL_HCORE_HDEQUE_HXX_INCLUDED 1

#include <new>

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
class HDeque {
private:
	static int long const DEFAULT_CHUNK_SIZE = ( 512 / sizeof ( type_t ) ) * sizeof ( type_t ) + ( ( 512 - ( ( 512 / sizeof ( type_t ) ) * sizeof ( type_t ) ) ) > ( sizeof ( type_t ) / 2 ) ? sizeof ( type_t ) : 0 );
	static int long const CHUNK_SIZE = meta::max<sizeof ( type_t ), DEFAULT_CHUNK_SIZE>::value;
	static int long const VALUES_PER_CHUNK = CHUNK_SIZE / sizeof ( type_t );
	static int long const MIN_CHUNKS_COUNT = 8;
protected:
	typedef HDeque<type_t> this_type;
public:
	typedef type_t value_type;
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
	static allocator_type _allocator;
	HChunk _chunks;
	int long _start;
	int long _size;
public:
	HDeque( void )
		: _chunks(), _start( 0 ), _size( 0 ) {
		return;
	}
	explicit HDeque( allocator_t const& )
		: _chunks(), _start( 0 ), _size( 0 ) {
		return;
	}
	explicit HDeque( int long size_ )
		: _chunks(), _start( 0 ), _size( 0 ) {
		M_PROLOG
		resize( size_ );
		return;
		M_EPILOG
	}

	HDeque( int long size_, allocator_t const& )
		: _chunks(), _start( 0 ), _size( 0 ) {
		M_PROLOG
		resize( size_ );
		return;
		M_EPILOG
	}

	HDeque( int long size_, type_t const& fillWith_, allocator_t const& = allocator_t() )
		: _chunks(), _start( 0 ), _size( 0 ) {
		M_PROLOG
		resize( size_, fillWith_ );
		return;
		M_EPILOG
	}

	template<typename iterator_t>
	HDeque( iterator_t first, iterator_t last, allocator_t const& = allocator_t() )
		: _chunks(), _start( 0 ), _size( 0 ) {
		M_PROLOG
		initialize( first, last, typename trait::add_pointer<typename is_integral<iterator_t>::type>::type() );
		return;
		M_EPILOG
	}

	virtual ~HDeque( void ) {
		M_PROLOG
		clear();
		return;
		M_DESTRUCTOR_EPILOG
	}
	HDeque( HDeque const& );
	HDeque( HDeque const&, allocator_t const& );

#if CXX_STANDARD >= 2011
	HDeque( HDeque&& deque_ )
		: _start( 0 ), _size( 0 ), _chunks() {
		M_PROLOG
		swap( deque_ );
		return;
		M_EPILOG
	}
#endif /* #if CXX_STANDARD >= 2011 */

	HDeque& operator = ( HDeque const& deque_ ) {
		M_PROLOG
		if ( &deque_ != this ) {
			HDeque tmp( deque_ );
			swap( tmp );
		}
		return ( *this );
		M_EPILOG
	}

#if CXX_STANDARD >= 2011
	HDeque& operator = ( HDeque&& deque_ ) {
		M_PROLOG
		if ( &deque_ != this ) {
			swap( deque_ );
			deque_.clear();
		}
		return ( *this );
		M_EPILOG
	}
#endif /* #if CXX_STANDARD >= 2011 */

	void swap( HDeque& deque_ ) {
		if ( &deque_ != this ) {
			using yaal::swap;
			swap( _start, deque_._start );
			swap( _size, deque_._size );
			swap( _chunks, deque_._chunks );
		}
		return;
	}

	allocator_type const& get_allocator( void ) const {
		return ( _allocator );
	}
	void resize( int long, type_t const& = value_type() );
	type_t const& operator[] ( int long index_ ) const {
		int long idx = ( index_ < 0 ) ? index_ + _size : index_;
		if ( ( idx >= _size ) || ( idx < 0 ) )
			M_THROW( _errMsgHDeque_[ ERROR::BAD_INDEX ], idx );
		int long itemIndex( _start + idx );
		return ( _chunks.get<value_type const*>()[ itemIndex / VALUES_PER_CHUNK][ itemIndex % VALUES_PER_CHUNK ] );
	}
	type_t& operator[] ( int long index_ ) {
		int long idx = ( index_ < 0 ) ? index_ + _size : index_;
		if ( ( idx >= _size ) || ( idx < 0 ) )
			M_THROW( _errMsgHDeque_[ ERROR::BAD_INDEX ], idx );
		int long itemIndex( _start + idx );
		return ( _chunks.get<value_type*>()[ itemIndex / VALUES_PER_CHUNK][ itemIndex % VALUES_PER_CHUNK ] );
	}
	void push_front( type_t const& );
	void pop_front( void );
	void push_back( type_t const& );
	void pop_back( void );
	type_t const& back( void ) const {
		M_PROLOG
		M_ASSERT( _size > 0 );
		int long itemIndex( _start + _size - 1 );
		return ( _chunks.get<value_type const*>()[ itemIndex / VALUES_PER_CHUNK][ itemIndex % VALUES_PER_CHUNK ] );
		M_EPILOG
	}
	type_t& back( void ) {
		M_PROLOG
		M_ASSERT( _size > 0 );
		int long itemIndex( _start + _size - 1 );
		return ( _chunks.get<value_type*>()[ itemIndex / VALUES_PER_CHUNK][ itemIndex % VALUES_PER_CHUNK ] );
		M_EPILOG
	}
	type_t const& front( void ) const {
		M_PROLOG
		M_ASSERT( _size > 0 );
		int long itemIndex( _start );
		return ( _chunks.get<value_type const*>()[ itemIndex / VALUES_PER_CHUNK][ itemIndex % VALUES_PER_CHUNK ] );
		M_EPILOG
	}
	type_t& front( void ) {
		M_PROLOG
		M_ASSERT( _size > 0 );
		int long itemIndex( _start );
		return ( _chunks.get<value_type*>()[ itemIndex / VALUES_PER_CHUNK][ itemIndex % VALUES_PER_CHUNK ] );
		M_EPILOG
	}
	bool is_empty( void ) const {
		return ( _size == 0 );
	}
	bool empty( void ) const {
		return ( _size == 0 );
	}
	int long get_size( void ) const {
		return ( _size );
	}
	int long size( void ) const {
		return ( _size );
	}
	void clear( void );
	bool operator ! ( void ) const {
		return ( is_empty() );
	}
	iterator insert( iterator, type_t const& );
	template<typename iterator_t>
	void assign( iterator_t, iterator_t );
	void assign( int long, type_t const& );
	template<typename iterator_t>
	void insert( iterator, iterator_t, iterator_t );
	void insert( iterator, int long, type_t const& );
	iterator erase( iterator it ) {
		M_PROLOG
		return ( erase( it, it + 1 ) );
		M_EPILOG
	}
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

	iterator find( int long );
	const_iterator find( int long ) const;
	bool operator == ( HDeque const& d_ ) const {
		M_PROLOG
		return ( ( &d_ == this ) || safe_equal( begin(), end(), d_.begin(), d_.end() ) );
		M_EPILOG
	}
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
	void initialize( int long size_, type_t const& fillWith_, trait::true_type const* ) {
		M_PROLOG
		resize( size_, fillWith_ );
		return;
		M_EPILOG
	}
	void insert_space( int long, int long );
	void accommodate_chunks( int long );
	int long minimal_cover_chunks_count( int long size_ ) const {
		return ( ( size_ / VALUES_PER_CHUNK ) + ( ( size_ % VALUES_PER_CHUNK ) ? 1 : 0 ) );
	}
	int long used_chunks( void ) const {
		return ( _size > 0 ? ( ( ( _start + _size - 1 ) / VALUES_PER_CHUNK ) + 1 ) - ( _start / VALUES_PER_CHUNK ) : 0 );
	}
};

template<typename type_t, typename allocator_t>
int long const HDeque<type_t, allocator_t>::CHUNK_SIZE;
template<typename type_t, typename allocator_t>
int long const HDeque<type_t, allocator_t>::VALUES_PER_CHUNK;
template<typename type_t, typename allocator_t>
int long const HDeque<type_t, allocator_t>::MIN_CHUNKS_COUNT;
template<typename type_t, typename allocator_t>
typename HDeque<type_t, allocator_t>::allocator_type HDeque<type_t, allocator_t>::_allocator;

}

template<typename type_t, typename allocator_t>
inline void swap( yaal::hcore::HDeque<type_t>& a, yaal::hcore::HDeque<type_t>& b )
	{ a.swap( b ); }

namespace hcore {

/*! \brief Iterator for HDeque<> data structure.
 */
template<typename type_t, typename allocator_t>
template<typename const_qual_t>
class HDeque<type_t, allocator_t>::HIterator : public iterator_interface<const_qual_t, iterator_category::random_access> {
	typedef HDeque<type_t, allocator_t> deque_t;
	deque_t const* _owner;
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
	const_qual_t* operator-> ( void ) {
		M_PROLOG
		M_ASSERT( _owner && ( _index >= 0 ) && ( _index < _owner->_size ) );
		const_qual_t* const* chunks = _owner->_chunks.template get<value_type*>();
		int long physical( _owner->_start + _index );
		return ( &chunks[ physical / HDeque<type_t>::VALUES_PER_CHUNK ][ physical % HDeque<type_t>::VALUES_PER_CHUNK ] );
		M_EPILOG
	}
	const_qual_t* operator-> ( void ) const {
		M_PROLOG
		M_ASSERT( _owner && ( _index >= 0 ) && ( _index < _owner->_size ) );
		const_qual_t* const* chunks = _owner->_chunks.template get<value_type const*>();
		int long physical( _owner->_start + _index );
		return ( &chunks[ physical / HDeque<type_t>::VALUES_PER_CHUNK ][ physical % HDeque<type_t>::VALUES_PER_CHUNK ] );
		M_EPILOG
	}
	const_qual_t& operator* ( void ) {
		M_PROLOG
		M_ASSERT( _owner && ( _index >= 0 ) && ( _index < _owner->_size ) );
		const_qual_t* const* chunks = _owner->_chunks.template get<const_qual_t*>();
		int long physical( _owner->_start + _index );
		return ( chunks[ physical / HDeque<type_t>::VALUES_PER_CHUNK ][ physical % HDeque<type_t>::VALUES_PER_CHUNK ] );
		M_EPILOG
	}
	const_qual_t& operator* ( void ) const {
		M_PROLOG
		M_ASSERT( _owner && ( _index >= 0 ) && ( _index < _owner->_size ) );
		const_qual_t* const* chunks = _owner->_chunks.template get<const_qual_t*>();
		int long physical( _owner->_start + _index );
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
private:
	friend class HDeque<type_t, allocator_t>;
	explicit HIterator( deque_t const* owner_, int long idx )
		: base_type(), _owner( owner_ ), _index( idx ) {};
};

template<typename type_t, typename allocator_t>
HDeque<type_t, allocator_t>::HDeque( HDeque const& deque )
	: _chunks(), _start( 0 ), _size( 0 ) {
	M_PROLOG
	if ( deque._size > 0 ) {
		_chunks.realloc( max( chunk_size<value_type*>( MIN_CHUNKS_COUNT ), deque._chunks.get_size() ), HChunk::STRATEGY::EXACT );
		M_ASSERT( _chunks.get_size() == deque._chunks.get_size() );
		int long firstUsedChunkIndex( deque._start / VALUES_PER_CHUNK );
		value_type const* const* srcChunks = deque._chunks.template get<value_type const*>();
		value_type** chunks = _chunks.get<value_type*>();
		for ( int long i( firstUsedChunkIndex ), chunksCount( deque._chunks.template count_of<value_type*>() ); ( i < chunksCount ) && srcChunks[ i ]; ++ i )
			chunks[ i ] = static_cast<value_type*>( ::operator new ( CHUNK_SIZE, memory::yaal ) );
		for ( int long i( deque._start ), endIdx( deque._start + deque._size ); i < endIdx; ++ i )
			new ( chunks[ i / VALUES_PER_CHUNK ] + ( i % VALUES_PER_CHUNK ) ) value_type( srcChunks[ i / VALUES_PER_CHUNK ][ i % VALUES_PER_CHUNK ] );
		_start = deque._start;
		_size = deque._size;
	}
	return;
	M_EPILOG
}

template<typename type_t, typename allocator_t>
HDeque<type_t, allocator_t>::HDeque( HDeque const& deque, allocator_t const& )
	: _chunks(), _start( 0 ), _size( 0 ) {
	M_PROLOG
	if ( deque._size > 0 ) {
		_chunks.realloc( max( chunk_size<value_type*>( MIN_CHUNKS_COUNT ), deque._chunks.get_size() ), HChunk::STRATEGY::EXACT );
		M_ASSERT( _chunks.get_size() == deque._chunks.get_size() );
		int long firstUsedChunkIndex( deque._start / VALUES_PER_CHUNK );
		value_type const* const* srcChunks = deque._chunks.template get<value_type const*>();
		value_type** chunks = _chunks.get<value_type*>();
		for ( int long i( firstUsedChunkIndex ), chunksCount( deque._chunks.template count_of<value_type*>() ); ( i < chunksCount ) && srcChunks[ i ]; ++ i )
			chunks[ i ] = static_cast<value_type*>( ::operator new ( CHUNK_SIZE, memory::yaal ) );
		for ( int long i( deque._start ), endIdx( deque._start + deque._size ); i < endIdx; ++ i )
			new ( chunks[ i / VALUES_PER_CHUNK ] + ( i % VALUES_PER_CHUNK ) ) value_type( srcChunks[ i / VALUES_PER_CHUNK ][ i % VALUES_PER_CHUNK ] );
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
	for ( iterator it( begin() ), endIt( end() ); it != endIt; ++ it )
		M_SAFE( (*it).~value_type() );
	for ( int long i( _start / VALUES_PER_CHUNK ),
			CAPACITY( _chunks.count_of<value_type*>() );
			( i < CAPACITY ) && chunks[ i ]; ++ i ) {
		operator delete ( static_cast<void*>( chunks[ i ] ) );
		chunks[ i ] = NULL;
	}
	_start = 0;
	_size = 0;
	return;
	M_EPILOG
}

template<typename type_t, typename allocator_t>
void HDeque<type_t, allocator_t>::accommodate_chunks( int long size_ ) {
	M_PROLOG
	int long firstUsedChunkIndex( _start / VALUES_PER_CHUNK );
	int long usedChunksCount( used_chunks() );
	int long availableChunksCount( _chunks.count_of<value_type*>() );
	int long newFirstChunkIndex( firstUsedChunkIndex );
	int long newUsedChunksCount( usedChunksCount );
	int long newAvailableChunksCount( availableChunksCount );
	if ( size_ > 0 ) /* Add chunks at back of HDeque<>. */ {
		int long newLastChunkIndex = ( _start + _size + size_ - 1 ) / VALUES_PER_CHUNK;
		newUsedChunksCount = newLastChunkIndex + 1 - firstUsedChunkIndex;
		if ( newLastChunkIndex >= availableChunksCount ) {
			if ( newUsedChunksCount > availableChunksCount ) {
				_chunks.realloc( chunk_size<value_type*>( max( newUsedChunksCount, MIN_CHUNKS_COUNT ) ) );
				newAvailableChunksCount = _chunks.count_of<value_type*>();
				M_ASSERT( newAvailableChunksCount >= newUsedChunksCount );
			}
			newFirstChunkIndex = ( newAvailableChunksCount - newUsedChunksCount ) / 2;
		}
	} else if ( size_ < 0 ) /* Add chunks in front of HDeque<>. */ {
		int long newStart( _start + size_ );
		if ( newStart < 0 ) {
			int long frontExtension( minimal_cover_chunks_count( - newStart ) + firstUsedChunkIndex );
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
		int long moveBy( firstUsedChunkIndex - newFirstChunkIndex );
		value_type** chunks = _chunks.get<value_type*>();
		if ( _size > 0 ) {
			if ( moveBy > 0 ) /* move to front */ {
				copy( chunks + firstUsedChunkIndex, chunks + firstUsedChunkIndex + usedChunksCount, chunks + newFirstChunkIndex );
				fill( chunks + firstUsedChunkIndex + usedChunksCount - min( moveBy, usedChunksCount ), chunks + firstUsedChunkIndex + usedChunksCount, static_cast<value_type*>( NULL ) );
				_start -= ( moveBy * VALUES_PER_CHUNK );
			} else if ( moveBy < 0 ) /* move to back */ {
				M_ASSERT( ( newFirstChunkIndex + usedChunksCount ) <= newAvailableChunksCount );
				copy_backward( chunks + firstUsedChunkIndex, chunks + firstUsedChunkIndex + usedChunksCount, chunks + newFirstChunkIndex + usedChunksCount );
				fill( chunks + firstUsedChunkIndex, chunks + firstUsedChunkIndex + min( - moveBy, usedChunksCount ), static_cast<value_type*>( NULL ) );
				_start -= ( moveBy * VALUES_PER_CHUNK );
				M_ASSERT( ( ( _start + _size + size_ - 1 ) / VALUES_PER_CHUNK ) < newAvailableChunksCount );
			}
			M_ASSERT( ( _start >= 0 ) && ( ( _start / VALUES_PER_CHUNK ) == newFirstChunkIndex ) );
		} else
			_start = newFirstChunkIndex * VALUES_PER_CHUNK;
		if ( size_ > 0 ) {
			for ( int long i( ( _start + _size + size_ - 1 ) / VALUES_PER_CHUNK ); ( i >= ( newFirstChunkIndex + usedChunksCount ) ) && ! chunks[ i ]; -- i )
				chunks[ i ] = static_cast<value_type*>( ::operator new ( CHUNK_SIZE, memory::yaal ) );
		} else if ( size_ < 0 ) {
			M_ASSERT( ( _start + size_ ) >= 0 );
			for ( int long i( ( _start + size_ ) / VALUES_PER_CHUNK ); ( i < newFirstChunkIndex ) && ! chunks[ i ]; ++ i )
				chunks[ i ] = static_cast<value_type*>( ::operator new (  CHUNK_SIZE, memory::yaal ) );
		}
	}
	return;
	M_EPILOG
}

template<typename type_t, typename allocator_t>
void HDeque<type_t, allocator_t>::resize( int long size_, type_t const& fillWith_ ) {
	M_PROLOG
	M_ASSERT( _size >= 0 );
	if ( size_ < 0 )
		M_THROW( _errMsgHDeque_[ ERROR::BAD_SIZE ], size_ );
	if ( size_ > _size ) {
		M_ASSERT( _start + size_ > 0 );
		accommodate_chunks( size_ - _size );
		value_type** chunks = _chunks.get<value_type*>();
		for ( int long i( _start + _size ), last( _start + size_ ); i < last; ++ i )
			new ( chunks[ i / VALUES_PER_CHUNK ] + ( i % VALUES_PER_CHUNK ) ) value_type( fillWith_ );
	} else if ( size_ < _size ) {
		value_type** chunks = _chunks.get<value_type*>();
		bool chunkStart( false );
		for ( int long i( _start + size_ ), last( _start + _size ); i < last; ++ i )
			M_SAFE( chunks[ i / VALUES_PER_CHUNK ][ i % VALUES_PER_CHUNK ].~value_type() );
		for ( int long i( ( ( ( _start + size_ - 1 ) >= _start ? ( _start + size_ - 1 ) : _start + size_ ) / VALUES_PER_CHUNK ) + ( size_ ? 1 : 0 ) ),
				lastChunkIndex( ( ( _start + _size - 1 ) / VALUES_PER_CHUNK ) + 1 ); i < lastChunkIndex; ++ i ) {
			::operator delete ( static_cast<void*>( chunks[ i ] ), memory::yaal );
			chunks[ i ] = NULL;
		}
	}
	_size = size_;
	return;
	M_EPILOG
}

template<typename type_t, typename allocator_t>
void HDeque<type_t, allocator_t>::insert_space( int long index_, int long size_ ) {
	M_PROLOG
	accommodate_chunks( ( index_ <= _size / 2 ) ? - size_ : size_ );
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
		if ( index_ <= ( _size / 2 ) ) /* Move from back to front. */ {
			for ( int long src( _start + size_ ), dst( _start ); dst < ( index_ + _start ); ++ src, ++ dst ) {
				new ( chunks[ dst / VALUES_PER_CHUNK ] + ( dst % VALUES_PER_CHUNK ) ) value_type( chunks[ src / VALUES_PER_CHUNK ][ src % VALUES_PER_CHUNK ] );
				M_SAFE( chunks[ src / VALUES_PER_CHUNK ][ src % VALUES_PER_CHUNK ].~value_type() );
			}
		} else /* Move from front to back. */ {
			for ( int long src( _start + _size - 1 ), dst( _start + _size + size_ - 1 ); src >= ( _start + index_ ); -- src, -- dst ) {
				new ( chunks[ dst / VALUES_PER_CHUNK ] + ( dst % VALUES_PER_CHUNK ) ) value_type( chunks[ src / VALUES_PER_CHUNK ][ src % VALUES_PER_CHUNK ] );
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
	if ( ( it_._index < 0 ) || ( it_._index > _size ) )
		M_THROW( _errMsgHDeque_[ ERROR::INVALID_ITERATOR ], it_._index );
	using yaal::distance;
	insert_space( it_._index, distance( first_, last_ ) );
	value_type** chunks = _chunks.get<value_type*>();
	for ( int long i( it_._index + _start ); first_ != last_; ++ first_, ++ i )
		new ( chunks[ i / VALUES_PER_CHUNK ] + ( i % VALUES_PER_CHUNK ) ) value_type( *first_ );
	return;
	M_EPILOG
}

template<typename type_t, typename allocator_t>
typename HDeque<type_t, allocator_t>::iterator HDeque<type_t, allocator_t>::erase( iterator first_, iterator last_ ) {
	M_PROLOG
	M_ASSERT( first_._owner == this );
	M_ASSERT( last_._owner == this );
	if ( ( first_._index < 0 ) && ( first_._index > _size ) )
		M_THROW( _errMsgHDeque_[ ERROR::INVALID_ITERATOR ], first_._index );
	if ( ( last_._index < 0 ) && ( last_._index > _size ) )
		M_THROW( _errMsgHDeque_[ ERROR::INVALID_ITERATOR ], last_._index );
	int long toRemove( last_._index - first_._index );
	if ( last_._index < first_._index )
		M_THROW( _errMsgHDeque_[ ERROR::INVALID_ITERATOR ], toRemove );
	if ( toRemove ) {
		value_type** chunks = _chunks.get<value_type*>();
		if ( first_._index < ( _size - last_._index ) ) /* Move front. */ {
			for ( int long src( _start ), dst( _start + first_._index ); dst < ( _start + last_._index ); ++ src, ++ dst )
				chunks[ dst / VALUES_PER_CHUNK ][ dst % VALUES_PER_CHUNK ] = chunks[ src / VALUES_PER_CHUNK ][ src % VALUES_PER_CHUNK ];
			for ( int long del( _start ); del < ( _start + toRemove ); ++ del )
				M_SAFE( chunks[ del / VALUES_PER_CHUNK ][ del % VALUES_PER_CHUNK ].~value_type() );
			for ( int long chunkIndex( _start / VALUES_PER_CHUNK ), newFirstChunkIndex( ( _start + toRemove ) / VALUES_PER_CHUNK );
					chunkIndex < newFirstChunkIndex; ++ chunkIndex ) {
				M_ASSERT( chunks[ chunkIndex ] );
				::operator delete ( static_cast<void*>( chunks[ chunkIndex ] ), memory::yaal );
				chunks[ chunkIndex ] = NULL;
			}
			_start += toRemove;
		} else /* Move back. */ {
			for ( int long src( _start + last_._index ), dst( _start + first_._index ); src < ( _start + _size ); ++ src, ++ dst )
				chunks[ dst / VALUES_PER_CHUNK ][ dst % VALUES_PER_CHUNK ] = chunks[ src / VALUES_PER_CHUNK ][ src % VALUES_PER_CHUNK ];
			for ( int long del( _start + _size - toRemove ); del < ( _start + _size ); ++ del )
				M_SAFE( chunks[ del / VALUES_PER_CHUNK ][ del % VALUES_PER_CHUNK ].~value_type() );
			int long chunksCount( _chunks.template count_of<value_type*>() );
			for ( int long chunkIndex( ( ( ( ( ( _start + _size - toRemove ) - 1 ) >= _start ) ? ( ( _start + _size - toRemove ) - 1 ) : ( _start + _size - toRemove ) ) / VALUES_PER_CHUNK ) + ( ( _size - toRemove ) > 0 ? 1 : 0 ) );
					( chunkIndex < ( ( ( _start + _size - 1 ) / VALUES_PER_CHUNK ) + 1 ) ) && ( chunkIndex < chunksCount );
					++ chunkIndex ) {
				M_ASSERT( chunks[ chunkIndex ] );
				::operator delete ( static_cast<void*>( chunks[ chunkIndex ] ), memory::yaal );
				chunks[ chunkIndex ] = NULL;
			}
		}
		_size -= toRemove;
	}
	if ( ! _size )
		_start = 0;
	return ( last_._index < _size ? last_ : end() );
	M_EPILOG
}

template<typename type_t, typename allocator_t>
void HDeque<type_t, allocator_t>::push_back( type_t const& value_ ) {
	M_PROLOG
	int long idx( _start + _size );
	int long chunk( idx / VALUES_PER_CHUNK );
	int long offset( idx % VALUES_PER_CHUNK );
	value_type** chunks = NULL;
	if ( ! offset ) /* We start new chunk. */ {
		int long availableChunksCount( _chunks.count_of<value_type*>() );
		if ( chunk >= availableChunksCount ) /* We need at least to move chunks. */ {
			int long usedChunksCount( used_chunks() );
			int long newUsedChunksCount( usedChunksCount + 1 );
			if ( newUsedChunksCount > availableChunksCount ) /* We need allocate more space for chunks. */ {
				_chunks.realloc( chunk_size<value_type*>( max( newUsedChunksCount, MIN_CHUNKS_COUNT ) ) );
				availableChunksCount = _chunks.count_of<value_type*>();
			}
			/* We have enough space for chunks, but we need to move existing chunks. */
			chunks = _chunks.get<value_type*>();
			int long firstUsedChunkIndex( _start / VALUES_PER_CHUNK );
			int long newFirstUsedChunk( ( availableChunksCount - newUsedChunksCount ) / 2 );
			int long moveBy( firstUsedChunkIndex - newFirstUsedChunk );
			if ( _size > 0 ) {
				if ( moveBy > 0 ) /* move to front */ {
					copy( chunks + firstUsedChunkIndex, chunks + firstUsedChunkIndex + usedChunksCount, chunks + newFirstUsedChunk );
					fill( chunks + firstUsedChunkIndex + usedChunksCount - min( moveBy, usedChunksCount ), chunks + firstUsedChunkIndex + usedChunksCount, static_cast<value_type*>( NULL ) );
					_start -= ( moveBy * VALUES_PER_CHUNK );
				} else if ( moveBy < 0 ) /* move to back */ {
					M_ASSERT( ( newFirstUsedChunk + usedChunksCount ) <= availableChunksCount );
					copy_backward( chunks + firstUsedChunkIndex, chunks + firstUsedChunkIndex + usedChunksCount, chunks + newFirstUsedChunk + usedChunksCount );
					fill( chunks + firstUsedChunkIndex, chunks + firstUsedChunkIndex + min( - moveBy, usedChunksCount ), static_cast<value_type*>( NULL ) );
					_start -= ( moveBy * VALUES_PER_CHUNK );
					M_ASSERT( ( ( _start + _size ) / VALUES_PER_CHUNK ) < availableChunksCount );
				}
				M_ASSERT( ( _start >= 0 ) && ( ( _start / VALUES_PER_CHUNK ) == newFirstUsedChunk ) );
			} else
				_start = newFirstUsedChunk * VALUES_PER_CHUNK;
			chunk = ( _start + _size ) / VALUES_PER_CHUNK;
		} else /* We have enough space and we do not need to move chunks. */
			chunks = _chunks.get<value_type*>();
		chunks[ chunk ] = static_cast<value_type*>( ::operator new ( CHUNK_SIZE, memory::yaal ) );
	} else /* We use old chunk. */
		chunks = _chunks.get<value_type*>();
	new ( _chunks.get<value_type*>()[ chunk ] + offset ) value_type( value_ );
	++ _size;
	return;
	M_EPILOG
}

template<typename type_t, typename allocator_t>
void HDeque<type_t, allocator_t>::push_front( type_t const& value_ ) {
	M_PROLOG
	int long chunk( 0 );
	int long offset( 0 );
	value_type** chunks = NULL;
	if ( ! ( _start % VALUES_PER_CHUNK ) ) /* New chunk in front required. */ {
		if ( ! _start ) /* We need at least to move chunks. */ {
			int long availableChunksCount( _chunks.count_of<value_type*>() );
			int long usedChunksCount( used_chunks() );
			int long newUsedChunksCount( usedChunksCount + 1 );
			if ( newUsedChunksCount > availableChunksCount ) /* We need allocate more space for chunks. */ {
				_chunks.realloc( chunk_size<value_type*>( max( newUsedChunksCount, MIN_CHUNKS_COUNT ) ) );
				availableChunksCount = _chunks.count_of<value_type*>();
			}
			/* We have enough space for chunks, but we need to move existing chunks. */
			chunks = _chunks.get<value_type*>();
			int long oldFirstUsedChunk( ( ( availableChunksCount - newUsedChunksCount ) / 2 ) + 1 );
			if ( _size > 0 ) {
				M_ASSERT( ( oldFirstUsedChunk + usedChunksCount ) <= availableChunksCount );
				copy_backward( chunks, chunks + usedChunksCount, chunks + usedChunksCount + oldFirstUsedChunk );
				fill( chunks, chunks + min( oldFirstUsedChunk, usedChunksCount ), static_cast<value_type*>( NULL ) );
				_start += ( oldFirstUsedChunk * VALUES_PER_CHUNK );
				M_ASSERT( ( ( ( _start - 1 ) + _size ) / VALUES_PER_CHUNK ) < availableChunksCount );
				M_ASSERT( ( _start > 0 ) && ( ( _start / VALUES_PER_CHUNK ) == oldFirstUsedChunk ) );
			} else
				_start = oldFirstUsedChunk * VALUES_PER_CHUNK;
		} else /* We have enough space and we do not need to move chunks. */
			chunks = _chunks.get<value_type*>();
		-- _start;
		chunk = _start / VALUES_PER_CHUNK;
		offset = VALUES_PER_CHUNK - 1;
		chunks[ chunk ] = static_cast<value_type*>( ::operator new ( CHUNK_SIZE, memory::yaal ) );
	} else /* We use old chunk. */ {
		chunks = _chunks.get<value_type*>();
		-- _start;
		chunk = _start / VALUES_PER_CHUNK;
		offset = _start % VALUES_PER_CHUNK;
	}
	new ( _chunks.get<value_type*>()[ chunk ] + offset ) value_type( value_ );
	++ _size;
	return;
	M_EPILOG
}

template<typename type_t, typename allocator_t>
void HDeque<type_t, allocator_t>::pop_back( void ) {
	M_PROLOG
	M_ASSERT( _size > 0 );
	int long idx( _start + _size - 1 );
	int long chunk( idx / VALUES_PER_CHUNK );
	int long offset( idx % VALUES_PER_CHUNK );
	value_type** chunks = _chunks.get<value_type*>();
	M_SAFE( ( chunks[ chunk ] + offset )->~value_type() );
	-- _size;
	if ( ! ( offset && _size ) ) {
		::operator delete ( static_cast<void*>( chunks[ chunk ] ), memory::yaal );
		chunks[ chunk ] = NULL;
	}
	if ( ! _size )
		_start = 0;
	return;
	M_EPILOG
}

template<typename type_t, typename allocator_t>
void HDeque<type_t, allocator_t>::pop_front( void ) {
	M_PROLOG
	M_ASSERT( _size > 0 );
	int long chunk( _start / VALUES_PER_CHUNK );
	int long offset( _start % VALUES_PER_CHUNK );
	value_type** chunks = _chunks.get<value_type*>();
	M_SAFE( ( chunks[ chunk ] + offset )->~value_type() );
	-- _size;
	++ _start;
	if ( ! ( ( _start % VALUES_PER_CHUNK ) && _size ) ) {
		::operator delete ( static_cast<void*>( chunks[ chunk ] ), memory::yaal );
		chunks[ chunk ] = NULL;
	}
	if ( ! _size )
		_start = 0;
	return;
	M_EPILOG
}

template<typename type_t, typename allocator_t>
template<typename iterator_t>
void HDeque<type_t, allocator_t>::assign( iterator_t first_, iterator_t last_ ) {
	M_PROLOG
	using yaal::distance;
	int long count( distance( first_, last_ ) );
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
void HDeque<type_t, allocator_t>::assign( int long count_, type_t const& value_ ) {
	M_PROLOG
	fill_n( begin(), min( count_, _size ), value_ );
	if ( count_ < _size )
		erase( begin() + count_, end() );
	else if ( count_ > _size )
		resize( count_, value_ );
	return;
	M_EPILOG
}

template<typename type_t, typename allocator_t>
typename HDeque<type_t, allocator_t>::iterator HDeque<type_t, allocator_t>::insert( iterator pos_, type_t const& value_ ) {
	M_PROLOG
	M_ASSERT( pos_._owner == this );
	if ( ( pos_._index < 0 ) && ( pos_._index > _size ) )
		M_THROW( _errMsgHDeque_[ ERROR::INVALID_ITERATOR ], pos_._index );
	insert_space( pos_._index, 1 );
	value_type** chunks = _chunks.get<value_type*>();
	int long idx( _start + pos_._index );
	new ( chunks[ idx / VALUES_PER_CHUNK ] + ( idx % VALUES_PER_CHUNK ) ) value_type( value_ );
	return ( pos_ );
	M_EPILOG
}

template<typename type_t, typename allocator_t>
void HDeque<type_t, allocator_t>::insert( iterator pos_, int long count_, type_t const& value_ ) {
	M_PROLOG
	M_ASSERT( pos_._owner == this );
	if ( ( pos_._index < 0 ) && ( pos_._index > _size ) )
		M_THROW( _errMsgHDeque_[ ERROR::INVALID_ITERATOR ], pos_._index );
	insert_space( pos_._index, count_ );
	value_type** chunks = _chunks.get<value_type*>();
	for ( int long i( _start + pos_._index ), last( _start + pos_._index + count_ ); i < last; ++ i )
		new ( chunks[ i / VALUES_PER_CHUNK ] + ( i % VALUES_PER_CHUNK ) ) value_type( value_ );
	return;
	M_EPILOG
}

}

}

#endif /* #ifndef YAAL_HCORE_HDEQUE_HXX_INCLUDED */

