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
			OK = 0,    /*!< No error. */
			BAD_SIZE,  /*!< Index of of bounds. */
			BAD_INDEX  /*!< Index of of bounds. */
			} error_t;
		};
	template<typename const_qual_t>
	class HIterator : public iterator_interface<const_qual_t>
		{
		typedef HDeque<type_t> array_t;
		array_t const* _owner;
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
			{ return ( _index == it._index ); }
		template<typename other_const_qual_t>
		bool operator != ( HIterator<other_const_qual_t> const& it ) const
			{ return ( _index != it._index ); }
	private:
		friend class HDeque<type_t>;
		explicit HIterator( array_t const* owner_, int long idx )
			: iterator_interface<const_qual_t>(), _owner( owner_ ), _index( idx ) {};
		};
	typedef HIterator<type_t> iterator;
	typedef HIterator<type_t const> const_iterator;
	typedef HReverseIterator<iterator> reverse_iterator;
	typedef HReverseIterator<const_iterator> const_reverse_iterator;
private:
	HChunk _chunks;
	iterator _start;
	iterator _finish;
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
HDeque<type_t>::HDeque( void )
	: _chunks(), _start( this, 0 ), _finish( this, 0 ), _size( 0 )
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
HDeque<type_t>::HDeque( int long size_ ) 
	: _chunks(), _start( this, 0 ), _finish( this, 0 ), _size( 0 )
	{
	M_PROLOG
	resize( size_ );
	return;
	M_EPILOG
	}

template<typename type_t>
HDeque<type_t>::HDeque( int long size_, type_t const& fillWith_ )
	: _chunks(), _start( this, 0 ), _finish( this, 0 ), _size( 0 )
	{
	M_PROLOG
	resize( size_, fillWith_ );
	return;
	M_EPILOG
	}

template<typename type_t>
template<typename iterator_t>
HDeque<type_t>::HDeque( iterator_t first, iterator_t last )
	: _chunks(), _start( this, 0 ), _finish( this, 0 ), _size( 0 )
	{
	M_PROLOG
	initialize( first, last, typename trait::make_pointer<typename is_integral<iterator_t>::type>::type() );
	return;
	M_EPILOG
	}

template<typename type_t>
void HDeque<type_t>::clear( void )
	{
	M_PROLOG
	value_type** chunks( _chunks.get<value_type*>() );
	for ( iterator it( _start ); it != _finish; ++ it )
		it->~value_type();
	for ( int long i( _start._index / CHUNK_SIZE ),
			CAPACITY( _chunks.count_of<value_type*>() );
			( i < CAPACITY ) && chunks[ i ]; ++ i )
		delete [] chunks[ i ];
	_size = 0;
	return;
	M_EPILOG
	}

#if 0
template<typename type_t>
void HDeque<type_t>::center_chunks( int long chunksCount_ )
	{
	M_PROLOG
	int long firstUsedChunkIndex( _start._index / VALUES_PER_CHUNK );
	int long chunksToMove( min( chunksCount_, _finish._index > 0 ? ( ( ( ( _finish._index - 1 ) / VALUES_PER_CHUNK ) - firstUsedChunkIndex ) + 1 ) : 0 ) );
	int long nextFirstUsedChunkIndex( ( _chunks.count_of<value_type*>() - chunksCount_ ) / 2 );
	if ( chunksToMove > 0 )
		{
		value_type** chunks( _chunks.get<value_type*>() );
		if ( nextFirstUsedChunkIndex < firstUsedChunkIndex )
			{
			copy( chunks + firstUsedChunkIndex, chunks + firstUsedChunkIndex + chunksToMove, chunks + nextFirstUsedChunkIndex );
			_start._index -= ( chunksToMove * VALUES_PER_CHUNK );
			M_ASSERT( _start._index >= 0 );
			}
		else if ( nextFirstUsedChunkIndex > firstUsedChunkIndex )
			{
			copy_backward( chunks + firstUsedChunkIndex, chunks + firstUsedChunkIndex + chunksToMove, chunks + nextFirstUsedChunkIndex );
			_start._index += ( chunksToMove * VALUES_PER_CHUNK );
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
	M_ASSERT( size_ >= 0 );
	if ( size_ > _size )
		{
		int long firstUsedChunkIndex( _start._index / VALUES_PER_CHUNK );
		M_ASSERT( _start._index + size_ > 0 );
		/* Last chunk index after resize().
		 */
		int long nextLastUsedChunkIndex( ( ( _start._index + size_ ) - 1 ) / VALUES_PER_CHUNK );
		/* Do we have to resize _chunks?
		 * i.e.
		 * _chunks can hold 4 chunks,
		 * first used chunk index == 0
		 * last used chunk index before resize == 3 (max possible last chunk index for given _chunks size)
		 * last used chunk index after resize == 4
		 * so test need to have following semantics:
		 * ( ( next last used chunk index - first used chunk index ) + 1 > current _chunks size ) => resize _chunks
		 * + 1 apeared in expression due to the fact that used chunk indexes are always inclusive
		 */
		/* Dafault realloc policy is geometric
		 */
		int long nextUsedChunksCount( ( nextLastUsedChunkIndex - firstUsedChunkIndex ) + 1 );
		if ( nextUsedChunksCount > _chunks.count_of<value_type*>() )
			_chunks.realloc( nextUsedChunksCount * sizeof ( value_type* ) );
		center_chunks( nextUsedChunksCount );
		}
	else if ( size_ < _size )
		{
		}
	_size = size_;
	return;
	M_EPILOG
	}
#endif

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
typename HDeque<type_t>::const_iterator HDeque<type_t>::begin( void ) const
	{
	return ( _start );
	}

template<typename type_t>
typename HDeque<type_t>::iterator HDeque<type_t>::begin( void )
	{
	return ( _start );
	}

template<typename type_t>
typename HDeque<type_t>::const_iterator HDeque<type_t>::end( void ) const
	{
	return ( _finish );
	}

template<typename type_t>
typename HDeque<type_t>::iterator HDeque<type_t>::end( void )
	{
	return ( _finish );
	}

template<typename type_t>
typename HDeque<type_t>::const_reverse_iterator HDeque<type_t>::rbegin( void ) const
	{
	return ( _finish );
	}

template<typename type_t>
typename HDeque<type_t>::reverse_iterator HDeque<type_t>::rbegin( void )
	{
	return ( _finish );
	}

template<typename type_t>
typename HDeque<type_t>::const_reverse_iterator HDeque<type_t>::rend( void ) const
	{
	return ( _start );
	}

template<typename type_t>
typename HDeque<type_t>::reverse_iterator HDeque<type_t>::rend( void )
	{
	return ( _start );
	}

}

}

#endif /* #ifndef YAAL_HCORE_HDEQUE_HXX_INCLUDED */

