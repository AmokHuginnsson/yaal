/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hcore/hdeque.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/hdeque.hxx
 * \brief Implementation of HDeque<> a Double-Ended-QUEue.
 */

#ifndef YAAL_HCORE_HDEQUE_HXX_INCLUDED
#define YAAL_HCORE_HDEQUE_HXX_INCLUDED 1

#include <new>

#include "hcore/base.hxx"
#include "hcore/hchunk.hxx"
#include "hcore/algorithm.hxx"
#include "hcore/pod.hxx"

namespace yaal
{

namespace hcore
{

extern M_YAAL_HCORE_PUBLIC_API char const* const _errMsgHDeque_[];

/*! \brief HDeque class - a double-ended-queue.
 *
 * Implemented as array of arrays.
 */
template<typename type_t>
class HDeque
	{
private:
	static int long const DEFAULT_CHUNK_SIZE = 512;
	static int long const CHUNK_SIZE = meta::max<sizeof ( type_t ), DEFAULT_CHUNK_SIZE>::value;
	static int long const VALUES_PER_CHUNK = CHUNK_SIZE / sizeof ( type_t );
protected:
	typedef HDeque<type_t> this_type;
public:
	typedef type_t value_type;
	/*! \brief Error codes for HDeque<> operations.
	 */
	struct ERROR
		{
		/*! \brief Error codes for HDeque<> operations.
		 */
		typedef enum
			{
			OK = 0,          /*!< No error. */
			BAD_SIZE,        /*!< Index of of bounds. */
			BAD_INDEX,       /*!< Index of of bounds. */
			INVALID_ITERATOR /*!< iterator used for operation is not valid */
			} error_t;
		};
	template<typename const_qual_t>
	class HIterator;
	typedef HIterator<type_t> iterator;
	typedef HIterator<type_t const> const_iterator;
	typedef HReverseIterator<iterator> reverse_iterator;
	typedef HReverseIterator<const_iterator> const_reverse_iterator;
private:
	HChunk _chunks;
	int long _start;
	int long _size;
public:
	HDeque( void );
	explicit HDeque( int long );
	HDeque( int long, type_t const& );
	template<typename iterator_t>
	HDeque( iterator_t, iterator_t );
	virtual ~HDeque( void );
	HDeque( HDeque const& );
	HDeque& operator = ( HDeque const& );
	void resize( int long, type_t const& = value_type() );
	type_t& operator [] ( int long );
	type_t const& operator [] ( int long ) const;
	void push_front( type_t const& );
	void pop_front( void );
	void push_back( type_t const& );
	void pop_back( void );
	type_t& front( void );
	type_t const& front( void ) const;
	type_t& back( void );
	type_t const& back( void ) const;
	int long get_size( void ) const;
	int long size( void ) const;
	bool empty( void ) const;
	bool is_empty( void ) const;
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
	void swap( HDeque& );
	bool operator == ( HDeque const& ) const;
	bool operator < ( HDeque const& ) const;
private:
	template<typename iterator_t>
	void initialize( iterator_t, iterator_t, trait::false_type const* );
	void initialize( int long, type_t const&, trait::true_type const* );
	void insert_space( int long, int long );
	void center_chunks( int long );
	int long accommodate_chunks( int long );
	int long used_chunks( void ) const;
	int long minimal_cover_chunks_count( int long ) const;
	};

}

template<typename type_t>
inline void swap( yaal::hcore::HDeque<type_t>& a, yaal::hcore::HDeque<type_t>& b )
	{ a.swap( b ); }

namespace hcore
{

/*! \brief Iterator for HDeque<> data structure.
 */
template<typename type_t>
template<typename const_qual_t>
class HDeque<type_t>::HIterator : public iterator_interface<const_qual_t>
	{
	typedef HDeque<type_t> deque_t;
	deque_t const* _owner;
	int long _index;
public:
	typedef type_t value_type;
	HIterator( void ) : iterator_interface<const_qual_t>(), _owner( NULL ), _index( 0 ) {}
	HIterator( HIterator const& it_ ) : iterator_interface<const_qual_t>(), _owner( it_._owner ), _index( it_._index ) {}
	template<typename other_const_qual_t>
	HIterator( HIterator<other_const_qual_t> const& it_ ) : iterator_interface<const_qual_t>(), _owner( it_._owner ), _index( it_._index )
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
	const_qual_t& operator* ( void );
	const_qual_t& operator* ( void ) const;
	const_qual_t* operator-> ( void );
	const_qual_t* operator-> ( void ) const;
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
	friend class HDeque<type_t>;
	explicit HIterator( deque_t const* owner_, int long idx )
		: iterator_interface<const_qual_t>(), _owner( owner_ ), _index( idx ) {};
	};

template<typename type_t>
HDeque<type_t>::HDeque( void )
	: _chunks(), _start( 0 ), _size( 0 )
	{
	return;
	}

template<typename type_t>
HDeque<type_t>::~HDeque( void )
	{
	M_PROLOG
	clear();
	return;
	M_EPILOG
	}

template<typename type_t>
HDeque<type_t>::HDeque( HDeque const& deque )
	: _chunks(), _start( 0 ), _size( 0 )
	{
	M_PROLOG
	if ( deque._size > 0 )
		{
		_chunks.realloc( deque._chunks.get_size(), HChunk::STRATEGY::EXACT );
		M_ASSERT( _chunks.get_size() == deque._chunks.get_size() );
		int long firstUsedChunkIndex( deque._start / VALUES_PER_CHUNK );
		value_type const* const* srcChunks = deque._chunks.template get<value_type const*>();
		value_type** chunks = _chunks.get<value_type*>();
		for ( int long i( firstUsedChunkIndex ), chunksCount( deque._chunks.template count_of<value_type*>() ); ( i < chunksCount ) && srcChunks[ i ]; ++ i )
			chunks[ i ] = static_cast<value_type*>( static_cast<void*>( new char[ CHUNK_SIZE ] ) );
		for ( int long i( deque._start ), endIdx( deque._start + deque._size ); i < endIdx; ++ i )
			new ( chunks[ i / VALUES_PER_CHUNK ] + ( i % VALUES_PER_CHUNK ) ) value_type( srcChunks[ i / VALUES_PER_CHUNK ][ i % VALUES_PER_CHUNK ] );
		_start = deque._start;
		_size = deque._size;
		}
	return;
	M_EPILOG
	}

template<typename type_t>
HDeque<type_t>::HDeque( int long size_ ) 
	: _chunks(), _start( 0 ), _size( 0 )
	{
	M_PROLOG
	resize( size_ );
	return;
	M_EPILOG
	}

template<typename type_t>
HDeque<type_t>::HDeque( int long size_, type_t const& fillWith_ )
	: _chunks(), _start( 0 ), _size( 0 )
	{
	M_PROLOG
	resize( size_, fillWith_ );
	return;
	M_EPILOG
	}

template<typename type_t>
template<typename iterator_t>
HDeque<type_t>::HDeque( iterator_t first, iterator_t last )
	: _chunks(), _start( 0 ), _size( 0 )
	{
	M_PROLOG
	initialize( first, last, typename trait::make_pointer<typename is_integral<iterator_t>::type>::type() );
	return;
	M_EPILOG
	}

template<typename type_t>
HDeque<type_t>& HDeque<type_t>::operator = ( HDeque const& deque_ )
	{
	M_PROLOG
	if ( &deque_ != this )
		{
		HDeque tmp( deque_ );
		swap( tmp );
		}
	return ( *this );
	M_EPILOG
	}

template<typename type_t>
void HDeque<type_t>::swap( HDeque& deque_ )
	{
	if ( &deque_ != this )
		{
		using yaal::swap;
		swap( _start, deque_._start );
		swap( _size, deque_._size );
		swap( _chunks, deque_._chunks );
		}
	return;
	}

template<typename type_t>
void HDeque<type_t>::clear( void )
	{
	M_PROLOG
	value_type** chunks = _chunks.get<value_type*>();
	for ( iterator it( begin() ), endIt( end() ); it != endIt; ++ it )
		it->~value_type();
	for ( int long i( _start / VALUES_PER_CHUNK ),
			CAPACITY( _chunks.count_of<value_type*>() );
			( i < CAPACITY ) && chunks[ i ]; ++ i )
		{
		delete [] static_cast<char*>( static_cast<void*>( chunks[ i ] ) );
		chunks[ i ] = NULL;
		}
	_size = 0;
	return;
	M_EPILOG
	}

template<typename type_t>
int long HDeque<type_t>::minimal_cover_chunks_count( int long size_ ) const
	{
	return ( ( size_ / VALUES_PER_CHUNK ) + ( ( size_ % VALUES_PER_CHUNK ) ? 1 : 0 ) );
	}

template<typename type_t>
int long HDeque<type_t>::used_chunks( void ) const
	{
	return ( _size > 0 ? ( ( ( _start + _size - 1 ) / VALUES_PER_CHUNK ) + 1 ) - ( _start / VALUES_PER_CHUNK ) : 0 );
	}

template<typename type_t>
int long HDeque<type_t>::accommodate_chunks( int long size_ )
	{
	M_PROLOG
	int long firstUsedChunkIndex( _start / VALUES_PER_CHUNK );
	int long usedChunks( used_chunks() );
	int long availableChunks( _chunks.count_of<value_type*>() );
	if ( size_ < 0 ) /* We add space in front of HDeque<>. */
		{
		int long newStart( _start + size_ );
		if ( newStart < 0 )
			{
			int long chunkOffset( minimal_cover_chunks_count( - newStart ) );
			if ( ( usedChunks + chunkOffset ) > availableChunks )
				_chunks.realloc( chunk_size<value_type*>( usedChunks + chunkOffset ) );
			value_type** chunks = _chunks.get<value_type*>();
			copy_backward( chunks + firstUsedChunkIndex, chunks + firstUsedChunkIndex + usedChunks, chunks + firstUsedChunkIndex + usedChunks + chunkOffset );
			fill( chunks + firstUsedChunkIndex, chunks + firstUsedChunkIndex + chunkOffset, static_cast<value_type*>( NULL ) );
			_start += chunkOffset * VALUES_PER_CHUNK;
			usedChunks += chunkOffset;
			}
		}
	else if ( size_ > 0 ) /* We add space at HDeque<>'s back. */
		{
		int long newLastUsedChunk( ( _start + _size + size_ - 1 ) / VALUES_PER_CHUNK );
		usedChunks = newLastUsedChunk + 1 - firstUsedChunkIndex;
		if ( usedChunks > availableChunks )
			_chunks.realloc( chunk_size<value_type*>( usedChunks ) );
		}
	return ( usedChunks );
	M_EPILOG
	}

/* Ensure that used chunks are placed in the middle of _chunks array
 * modifies _start, if chunks are actually moved old-already-existing
 * chunks are moved to begining of used chunks space,
 * _start is modified to accomodate this change and is still valid
 * after this call.
 * No fresh chunks are created here so _size is also still valid.
 */
template<typename type_t>
void HDeque<type_t>::center_chunks( int long chunksCount_ )
	{
	M_PROLOG
	int long firstUsedChunkIndex( _start / VALUES_PER_CHUNK );
	int long chunksToMove( min( chunksCount_, _size > 0 ? used_chunks() : 0 ) );
	if ( chunksToMove > 0 )
		{
		int long nextFirstUsedChunkIndex( ( _chunks.count_of<value_type*>() - chunksCount_ ) / 2 );
		value_type** chunks = _chunks.get<value_type*>();
		if ( nextFirstUsedChunkIndex < firstUsedChunkIndex )
			{
			copy( chunks + firstUsedChunkIndex, chunks + firstUsedChunkIndex + chunksToMove, chunks + nextFirstUsedChunkIndex );
			fill( chunks + nextFirstUsedChunkIndex + chunksToMove, chunks + firstUsedChunkIndex + chunksToMove, static_cast<value_type*>( NULL ) );
			_start -= ( ( firstUsedChunkIndex - nextFirstUsedChunkIndex ) * VALUES_PER_CHUNK );
			M_ASSERT( _start >= 0 );
			}
		else if ( nextFirstUsedChunkIndex > firstUsedChunkIndex )
			{
			copy_backward( chunks + firstUsedChunkIndex, chunks + firstUsedChunkIndex + chunksToMove, chunks + nextFirstUsedChunkIndex + chunksToMove );
			fill( chunks + firstUsedChunkIndex, chunks + nextFirstUsedChunkIndex, static_cast<value_type*>( NULL ) );
			_start += ( ( nextFirstUsedChunkIndex - firstUsedChunkIndex ) * VALUES_PER_CHUNK );
			}
		}
	return;
	M_EPILOG
	}

template<typename type_t>
void HDeque<type_t>::resize( int long size_, type_t const& fillWith_ )
	{
	M_PROLOG
	M_ASSERT( _size >= 0 );
	if ( size_ < 0 )
		M_THROW( _errMsgHDeque_[ ERROR::BAD_SIZE ], size_ );
	if ( size_ > _size )
		{
		M_ASSERT( _start + size_ > 0 );
		int long nextUsedChunksCount( accommodate_chunks( size_ - _size ) );
		center_chunks( nextUsedChunksCount );
		value_type** chunks = _chunks.get<value_type*>();
		for ( int long i( _start + _size ), last( _start + size_ ); i < last; ++ i )
			{
			int long chunkIndex( i / VALUES_PER_CHUNK );
			if ( ! chunks[ chunkIndex ] )
				chunks[ chunkIndex ] = static_cast<value_type*>( static_cast<void*>( new char[ CHUNK_SIZE ] ) );
			new ( chunks[ chunkIndex ] + ( i % VALUES_PER_CHUNK ) ) value_type( fillWith_ );
			}
		}
	else if ( size_ < _size )
		{
		value_type** chunks = _chunks.get<value_type*>();
		bool chunkStart( false );
		for ( int long i( _start + size_ ), last( _start + _size ); i < last; ++ i )
			{
			int long chunkIndex( i / VALUES_PER_CHUNK );
			int long itemIndex( i % VALUES_PER_CHUNK );
			( chunks[ chunkIndex ] + itemIndex )->~value_type();
			if ( ! itemIndex )
				chunkStart = true;
			if ( chunkStart && ( ( itemIndex + 1 ) == VALUES_PER_CHUNK ) )
				{
				delete [] static_cast<char*>( static_cast<void*>( chunks[ chunkIndex ] ) );
				chunks[ chunkIndex ] = NULL;
				}
			}
		int long lastItem( ( _start + _size - 1 ) % VALUES_PER_CHUNK );
		if ( chunkStart && ( ( lastItem + 1 ) != VALUES_PER_CHUNK ) )
			{
			int long lastChunkIndex( ( _start + _size - 1 ) / VALUES_PER_CHUNK );
			delete [] static_cast<char*>( static_cast<void*>( chunks[ lastChunkIndex ] ) );
			chunks[ lastChunkIndex ] = NULL;
			}
		center_chunks( size_ > 0 ? ( ( _start + size_ - 1 ) / VALUES_PER_CHUNK ) + 1 - ( _start / VALUES_PER_CHUNK ) : 0 );
		}
	_size = size_;
	return;
	M_EPILOG
	}

template<typename type_t>
void HDeque<type_t>::insert_space( int long index_, int long size_ )
	{
	M_PROLOG
	int long nextUsedChunksCount( accommodate_chunks( ( index_ <= _size / 2 ) ? - size_ : size_ ) );
	value_type** chunks = _chunks.get<value_type*>();
	/* When we insert space we should ensure that we will move as little of
	 * existing objects as possible. We have something like "used chunks" in
	 * _chunks array, if we want to insert space before middle of used chunks range
	 * than we shall move front part of existing HDeque<>, if we insert space
	 * after the middle of used chunks range we shall move back part of existing HDeque<>.
	 */
	if ( _size > 0 )
		{
		/* We cannot use center_chunks() to move front part of existing HDeque<> because
		 * center_chunks() moves old-already-existing chunks to beginning of used chunks range.
		 * We work like this:
		 * If we push out front we have to move all existing chunks to the end of used chunks range,
		 * create fresh chunks in front of it and move only smaller part of the HDeque<> elements
		 * into the front, object by object.
		 * If we push out back of HDeque<>, we need to move all existing chunks into the begging
		 * of used chunks range, create fresh chunks after them move only smaller part of HDeque<>
		 * to the end, object by object.
		 */
		if ( index_ <= ( _size / 2 ) ) /* we push out front */
			{
			int long newFirstUsedChunkIndex( ( _start - size_ ) / VALUES_PER_CHUNK );
			int long newLastUsedChunkIndex( ( _start + _size - 1 ) / VALUES_PER_CHUNK );
			int long moveBy( abs( newFirstUsedChunkIndex - ( ( _chunks.count_of<value_type*>() - newLastUsedChunkIndex ) - 1 ) ) / 2 );
			if ( moveBy > 0 )
				{
				int long firstUsedChunkIndex( _start / VALUES_PER_CHUNK );
				int long chunksToMove( used_chunks() );
				copy_backward( chunks + firstUsedChunkIndex, chunks + firstUsedChunkIndex + chunksToMove, chunks + firstUsedChunkIndex + chunksToMove + moveBy );
				fill( chunks + firstUsedChunkIndex, chunks + firstUsedChunkIndex + min( chunksToMove, moveBy ),
						static_cast<value_type*>( NULL ) );
				_start += moveBy * VALUES_PER_CHUNK;
				}
			_start -= size_;
			}
		else /* we push out back */
			center_chunks( nextUsedChunksCount );
		}
	else
		_start = ( ( _chunks.count_of<value_type*>() - nextUsedChunksCount ) / 2 ) * VALUES_PER_CHUNK;
	int long firstUsedChunkIndex( _start / VALUES_PER_CHUNK );
	for ( int long i( firstUsedChunkIndex ); ( i < ( firstUsedChunkIndex + nextUsedChunksCount ) ) && ! chunks[ i ]; ++ i )
		chunks[ i ] = static_cast<value_type*>( static_cast<void*>( new char[ CHUNK_SIZE ] ) );
	for ( int long i( firstUsedChunkIndex + nextUsedChunksCount - 1 ); ( i >= ( firstUsedChunkIndex ) ) && ! chunks[ i ]; -- i )
		chunks[ i ] = static_cast<value_type*>( static_cast<void*>( new char[ CHUNK_SIZE ] ) );
	/* Here we perform actual move of smaller part.
	 */
	if ( _size > 0 )
		{
		if ( index_ <= ( _size / 2 ) ) /* Move from back to front. */
			{
			for ( int long src( _start + size_ ), dst( _start ); dst < ( index_ + _start ); ++ src, ++ dst )
				{
				new ( chunks[ dst / VALUES_PER_CHUNK ] + ( dst % VALUES_PER_CHUNK ) ) value_type( chunks[ src / VALUES_PER_CHUNK ][ src % VALUES_PER_CHUNK ] );
				chunks[ src / VALUES_PER_CHUNK ][ src % VALUES_PER_CHUNK ].~value_type();
				}
			}
		else /* Move from front to back. */
			{
			for ( int long src( _start + _size - 1 ), dst( _start + _size + size_ - 1 ); src >= ( _start + index_ ); -- src, -- dst )
				{
				new ( chunks[ dst / VALUES_PER_CHUNK ] + ( dst % VALUES_PER_CHUNK ) ) value_type( chunks[ src / VALUES_PER_CHUNK ][ src % VALUES_PER_CHUNK ] );
				chunks[ src / VALUES_PER_CHUNK ][ src % VALUES_PER_CHUNK ].~value_type();
				}
			}
		}
	_size += size_;
	return;
	M_EPILOG
	}

template<typename type_t>
template<typename iterator_t>
void HDeque<type_t>::insert( iterator it_, iterator_t first_, iterator_t last_ )
	{
	M_PROLOG
	M_ASSERT( it_._owner == this );
	if ( ( it_._index < 0 ) || ( it_._index > _size ) )
		M_THROW( _errMsgHDeque_[ ERROR::INVALID_ITERATOR ], it_._index );
	insert_space( it_._index, distance( first_, last_ ) );
	value_type** chunks = _chunks.get<value_type*>();
	for ( int long i( it_._index + _start ); first_ != last_; ++ first_, ++ i )
		new ( chunks[ i / VALUES_PER_CHUNK ] + ( i % VALUES_PER_CHUNK ) ) value_type( *first_ );
	return;
	M_EPILOG
	}

template<typename type_t>
template<typename iterator_t>
void HDeque<type_t>::initialize( iterator_t first, iterator_t last, trait::false_type const* )
	{
	M_PROLOG
	insert( end(), first, last );
	return;
	M_EPILOG
	}

template<typename type_t>
void HDeque<type_t>::initialize( int long size_, type_t const& fillWith_, trait::true_type const* )
	{
	M_PROLOG
	resize( size_, fillWith_ );
	return;
	M_EPILOG
	}

template<typename type_t>
typename HDeque<type_t>::iterator HDeque<type_t>::erase( iterator first_, iterator last_ )
	{
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
	if ( toRemove )
		{
		value_type** chunks = _chunks.get<value_type*>();
		if ( first_._index < ( _size - last_._index ) ) /* Move front. */
			{
			for ( int long src( _start ), dst( _start + first_._index ); dst < ( _start + last_._index ); ++ src, ++ dst )
				chunks[ dst / VALUES_PER_CHUNK ][ dst % VALUES_PER_CHUNK ] = chunks[ src / VALUES_PER_CHUNK ][ src % VALUES_PER_CHUNK ];
			for ( int long del( _start ); del < ( _start + toRemove ); ++ del )
				chunks[ del / VALUES_PER_CHUNK ][ del % VALUES_PER_CHUNK ].~value_type();
			for ( int long chunkIndex( _start / VALUES_PER_CHUNK ), newFirstChunk( ( _start + toRemove ) / VALUES_PER_CHUNK );
					chunkIndex < newFirstChunk; ++ chunkIndex )
				{
				M_ASSERT( chunks[ chunkIndex ] );
				delete [] static_cast<char*>( static_cast<void*>( chunks[ chunkIndex ] ) );
				chunks[ chunkIndex ] = NULL;
				}
			_start += toRemove;
			}
		else /* Move back. */
			{
			for ( int long src( _start + last_._index ), dst( _start + first_._index ); src < ( _start + _size ); ++ src, ++ dst )
				chunks[ dst / VALUES_PER_CHUNK ][ dst % VALUES_PER_CHUNK ] = chunks[ src / VALUES_PER_CHUNK ][ src % VALUES_PER_CHUNK ];
			for ( int long del( _start + _size - toRemove ); del < ( _start + _size ); ++ del )
				chunks[ del / VALUES_PER_CHUNK ][ del % VALUES_PER_CHUNK ].~value_type();
			int long chunksCount( _chunks.template count_of<value_type*>() );
			for ( int long chunkIndex( ( ( ( _start + _size - toRemove ) - 1 ) / VALUES_PER_CHUNK ) + 1 );
					( chunkIndex < ( ( ( _start + _size - 1 ) / VALUES_PER_CHUNK ) + 1 ) ) && ( chunkIndex < chunksCount );
					++ chunkIndex )
				{
				M_ASSERT( chunks[ chunkIndex ] );
				delete [] static_cast<char*>( static_cast<void*>( chunks[ chunkIndex ] ) );
				chunks[ chunkIndex ] = NULL;
				}
			}
		_size -= toRemove;
		center_chunks( used_chunks() );
		}
	return ( last_._index < _size ? last_ : end() );
	M_EPILOG
	}

template<typename type_t>
bool HDeque<type_t>::is_empty( void ) const
	{
	return ( _size == 0 );
	}

template<typename type_t>
bool HDeque<type_t>::empty( void ) const
	{
	return ( _size == 0 );
	}

template<typename type_t>
int long HDeque<type_t>::get_size( void ) const
	{
	return ( _size );
	}

template<typename type_t>
int long HDeque<type_t>::size( void ) const
	{
	return ( _size );
	}

template<typename type_t>
void HDeque<type_t>::push_back( type_t const& value_ )
	{
	M_PROLOG
	insert_space( _size, 1 );
	int long idx( _start + _size - 1 );
	new ( _chunks.get<value_type*>()[ idx / VALUES_PER_CHUNK ] + ( idx % VALUES_PER_CHUNK ) ) value_type( value_ );
	return;
	M_EPILOG
	}

template<typename type_t>
void HDeque<type_t>::push_front( type_t const& value_ )
	{
	M_PROLOG
	insert_space( 0, 1 );
	new ( _chunks.get<value_type*>()[ _start / VALUES_PER_CHUNK ] + ( _start % VALUES_PER_CHUNK ) ) value_type( value_ );
	return;
	M_EPILOG
	}

template<typename type_t>
void HDeque<type_t>::pop_back( void )
	{
	M_PROLOG
	M_ASSERT( _size > 0 );
	erase( end() - 1, end() );
	return;
	M_EPILOG
	}

template<typename type_t>
void HDeque<type_t>::pop_front( void )
	{
	M_PROLOG
	M_ASSERT( _size > 0 );
	erase( begin() );
	return;
	M_EPILOG
	}

template<typename type_t>
type_t const& HDeque<type_t>::back( void ) const
	{
	M_PROLOG
	M_ASSERT( _size > 0 );
	int long itemIndex( _start + _size - 1 );
	return ( _chunks.get<value_type const*>()[ itemIndex / VALUES_PER_CHUNK][ itemIndex % VALUES_PER_CHUNK ] );
	M_EPILOG
	}

template<typename type_t>
type_t& HDeque<type_t>::back( void )
	{
	M_PROLOG
	M_ASSERT( _size > 0 );
	int long itemIndex( _start + _size - 1 );
	return ( _chunks.get<value_type const*>()[ itemIndex / VALUES_PER_CHUNK][ itemIndex % VALUES_PER_CHUNK ] );
	M_EPILOG
	}

template<typename type_t>
type_t const& HDeque<type_t>::front( void ) const
	{
	M_PROLOG
	M_ASSERT( _size > 0 );
	int long itemIndex( _start );
	return ( _chunks.get<value_type const*>()[ itemIndex / VALUES_PER_CHUNK][ itemIndex % VALUES_PER_CHUNK ] );
	M_EPILOG
	}

template<typename type_t>
type_t& HDeque<type_t>::front( void )
	{
	M_PROLOG
	M_ASSERT( _size > 0 );
	int long itemIndex( _start );
	return ( _chunks.get<value_type const*>()[ itemIndex / VALUES_PER_CHUNK][ itemIndex % VALUES_PER_CHUNK ] );
	M_EPILOG
	}

template<typename type_t>
template<typename iterator_t>
void HDeque<type_t>::assign( iterator_t first_, iterator_t last_ )
	{
	M_PROLOG
	int long count( distance( first_, last_ ) );
	if ( count <= _size )
		{
		iterator last( copy( first_, last_, begin() ) );
		erase( last, end() );
		}
	else
		{
		copy_n( first_, _size, begin() );
		insert( end(), advance( first_, _size ), last_ );
		}
	return;
	M_EPILOG
	}

template<typename type_t>
void HDeque<type_t>::assign( int long count_, type_t const& value_ )
	{
	M_PROLOG
	fill_n( begin(), min( count_, _size ), value_ );
	if ( count_ < _size )
		erase( begin() + count_, end() );
	else if ( count_ > _size )
		resize( count_, value_ );
	return;
	M_EPILOG
	}

template<typename type_t>
typename HDeque<type_t>::iterator HDeque<type_t>::insert( iterator pos_, type_t const& value_ )
	{
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

template<typename type_t>
void HDeque<type_t>::insert( iterator pos_, int long count_, type_t const& value_ )
	{
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

template<typename type_t>
typename HDeque<type_t>::iterator HDeque<type_t>::erase( iterator it )
	{
	M_PROLOG
	return ( erase( it, it + 1 ) );
	M_EPILOG
	}

template<typename type_t>
bool HDeque<type_t>::operator!( void ) const
	{
	return ( is_empty() );
	}

template<typename type_t>
type_t const& HDeque<type_t>::operator[] ( int long index_ ) const
	{
	int long idx = ( index_ < 0 ) ? index_ + _size : index_;
	if ( ( idx >= _size ) || ( idx < 0 ) )
		M_THROW( _errMsgHDeque_[ ERROR::BAD_INDEX ], idx );
	int long itemIndex( _start + idx );
	return ( _chunks.get<value_type const*>()[ itemIndex / VALUES_PER_CHUNK][ itemIndex % VALUES_PER_CHUNK ] );
	}

template<typename type_t>
type_t& HDeque<type_t>::operator[] ( int long index_ )
	{
	int long idx = ( index_ < 0 ) ? index_ + _size : index_;
	if ( ( idx >= _size ) || ( idx < 0 ) )
		M_THROW( _errMsgHDeque_[ ERROR::BAD_INDEX ], idx );
	int long itemIndex( _start + idx );
	return ( _chunks.get<value_type*>()[ itemIndex / VALUES_PER_CHUNK][ itemIndex % VALUES_PER_CHUNK ] );
	}

template<typename type_t>
typename HDeque<type_t>::const_iterator HDeque<type_t>::begin( void ) const
	{
	return ( const_iterator( this, 0 ) );
	}

template<typename type_t>
typename HDeque<type_t>::iterator HDeque<type_t>::begin( void )
	{
	return ( iterator( this, 0 ) );
	}

template<typename type_t>
typename HDeque<type_t>::const_iterator HDeque<type_t>::end( void ) const
	{
	return ( const_iterator( this, _size ) );
	}

template<typename type_t>
typename HDeque<type_t>::iterator HDeque<type_t>::end( void )
	{
	return ( iterator( this, _size ) );
	}

template<typename type_t>
typename HDeque<type_t>::const_reverse_iterator HDeque<type_t>::rbegin( void ) const
	{
	return ( const_iterator( this, _size ) );
	}

template<typename type_t>
typename HDeque<type_t>::reverse_iterator HDeque<type_t>::rbegin( void )
	{
	return ( iterator( this, _size ) );
	}

template<typename type_t>
typename HDeque<type_t>::const_reverse_iterator HDeque<type_t>::rend( void ) const
	{
	return ( const_iterator( this, 0 ) );
	}

template<typename type_t>
typename HDeque<type_t>::reverse_iterator HDeque<type_t>::rend( void )
	{
	return ( iterator( this, 0 ) );
	}

template<typename type_t>
template<typename const_qual_t>
const_qual_t* HDeque<type_t>::HIterator<const_qual_t>::operator->( void )
	{
	M_PROLOG
	M_ASSERT( _owner && ( _index >= 0 ) && ( _index < _owner->_size ) );
	const_qual_t* const* chunks = _owner->_chunks.template get<value_type*>();
	int long physical( _owner->_start + _index );
	return ( &chunks[ physical / HDeque<type_t>::VALUES_PER_CHUNK ][ physical % HDeque<type_t>::VALUES_PER_CHUNK ] );
	M_EPILOG
	}

template<typename type_t>
template<typename const_qual_t>
const_qual_t* HDeque<type_t>::HIterator<const_qual_t>::operator->( void ) const
	{
	M_PROLOG
	M_ASSERT( _owner && ( _index >= 0 ) && ( _index < _owner->_size ) );
	const_qual_t* const* chunks = _owner->_chunks.template get<value_type const*>();
	int long physical( _owner->_start + _index );
	return ( &chunks[ physical / HDeque<type_t>::VALUES_PER_CHUNK ][ physical % HDeque<type_t>::VALUES_PER_CHUNK ] );
	M_EPILOG
	}

template<typename type_t>
template<typename const_qual_t>
const_qual_t& HDeque<type_t>::HIterator<const_qual_t>::operator*( void )
	{
	M_PROLOG
	M_ASSERT( _owner && ( _index >= 0 ) && ( _index < _owner->_size ) );
	const_qual_t* const* chunks = _owner->_chunks.template get<const_qual_t*>();
	int long physical( _owner->_start + _index );
	return ( chunks[ physical / HDeque<type_t>::VALUES_PER_CHUNK ][ physical % HDeque<type_t>::VALUES_PER_CHUNK ] );
	M_EPILOG
	}

template<typename type_t>
template<typename const_qual_t>
const_qual_t& HDeque<type_t>::HIterator<const_qual_t>::operator*( void ) const
	{
	M_PROLOG
	M_ASSERT( _owner && ( _index >= 0 ) && ( _index < _owner->_size ) );
	const_qual_t* const* chunks = _owner->_chunks.template get<const_qual_t*>();
	int long physical( _owner->_start + _index );
	return ( chunks[ physical / HDeque<type_t>::VALUES_PER_CHUNK ][ physical % HDeque<type_t>::VALUES_PER_CHUNK ] );
	M_EPILOG
	}

template<typename type_t>
bool HDeque<type_t>::operator == ( HDeque const& d_ ) const
	{
	M_PROLOG
	return ( ( &d_ == this ) || safe_equal( begin(), end(), d_.begin(), d_.end() ) );
	M_EPILOG
	}

template<typename type_t>
bool HDeque<type_t>::operator < ( HDeque const& d_ ) const
	{
	M_PROLOG
	return ( ( &d_ != this ) && lexicographical_compare( begin(), end(), d_.begin(), d_.end() ) );
	M_EPILOG
	}

}

}

#endif /* #ifndef YAAL_HCORE_HDEQUE_HXX_INCLUDED */

