/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  hcore/hpool.hxx - this file is integral part of `yaal' project.

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

/*! \file hcore/hpool.hxx
 * \brief Definition and implementation of object memory pool.
 */

#ifndef YAAL_HCORE_HPOOL_HXX_INCLUDED
#define YAAL_HCORE_HPOOL_HXX_INCLUDED 1

#include "config.hxx"
#include "hcore/memory.hxx"
#include "hcore/algorithm_low.hxx"

namespace yaal {

namespace hcore {

/*! \brief HPool<> - object memory pool.
 */
template<typename T>
class HPool final {
public:
	typedef int long long aligner_t;
	static int const OBJECTS_PER_BLOCK = 256;
	static int const OBJECT_SIZE = sizeof ( T );
#if TARGET_CPU_BITS == 64
	static int const OBJECT_SPACE = meta::ternary<(OBJECT_SIZE < 2), 2,
	                                meta::ternary<(OBJECT_SIZE < 4), 4,
	                                meta::ternary<(OBJECT_SIZE < 8), 8,
	                                (OBJECT_SIZE + 8) & ~7u>::value>::value>::value;
#elif TARGET_CPU_BITS == 32 /* #if TARGET_CPU_BITS == 64 */
	static int const OBJECT_SPACE = meta::ternary<(OBJECT_SIZE < 2), 2,
	                                meta::ternary<(OBJECT_SIZE < 4), 4,
	                                (OBJECT_SIZE + 4) & ~3u>::value>::value;
#else /* #elif TARGET_CPU_BITS == 32 #if TARGET_CPU_BITS == 64 */
#error Unsupported CPU bitness.
#endif /* #else #elif TARGET_CPU_BITS == 32 #if TARGET_CPU_BITS == 64 */
	static int const ALIGNER_ELEMENTS_PER_BLOCK = OBJECT_SPACE * ( OBJECTS_PER_BLOCK / sizeof ( aligner_t ) );
private:
	class HPoolBlock {
	private:
		aligner_t _mem[ HPool<T>::ALIGNER_ELEMENTS_PER_BLOCK ];
		int _free; /*!< index of first free object */
		int _used; /*!< number of allocated object in this block pool*/
		int _index; /*!< This block index in HPool<>. */
	public:
		HPoolBlock( int index_ )
			: _mem(), _free( 0 ), _used( 0 ), _index( index_ ) {
			/* Create linked list of free object memory places. */
			char unsigned* p( reinterpret_cast<char unsigned*>( _mem ) );
			for ( int i( 0 ); i < OBJECTS_PER_BLOCK; ++ i, p += OBJECT_SPACE ) {
				*p = static_cast<char unsigned>( i + 1 );
				*( p + OBJECT_SPACE - 1 ) = i;
			}
		}
		T* alloc( void ) {
			T* p( reinterpret_cast<T*>( reinterpret_cast<char*>( _mem ) + OBJECT_SPACE * _free ) );
			_free = *reinterpret_cast<char unsigned*>( p );
			++ _used;
			return ( p );
		}
		bool free( T* ptr_ ) {
			int freed( *( reinterpret_cast<char unsigned*>( ptr_ ) + OBJECT_SPACE - 1 ) );
			*reinterpret_cast<char unsigned*>( ptr_ ) = _free;
			_free = freed;
			-- _used;
			return ( _used == 0 );
		}
		bool is_full( void ) const {
			return ( _used == OBJECTS_PER_BLOCK );
		}
	private:
		friend class HPool<T>;
		HPoolBlock( HPoolBlock const& );
		HPoolBlock& operator = ( HPoolBlock const& );
	};
	HPoolBlock** _poolBlocks;
	int _poolBlockCount;
	int _poolBlockCapacity;
	int _free; /*!< index of first HPoolBlock with free space. */
public:
	HPool( void )
		: _poolBlocks( NULL ), _poolBlockCount( 0 ),
		_poolBlockCapacity( 0 ), _free( -1 )
		{}
	~HPool( void ) {
		for ( int i( 0 ); i < _poolBlockCount; ++ i )
			delete _poolBlocks[i];
		delete [] _poolBlocks;
	}
	T* alloc( void ) {
		if ( _free == -1 )
			get_free_block();
		T* p( _poolBlocks[_free]->alloc() );
		if ( _poolBlocks[_free]->is_full() ) {
			if ( _free < ( _poolBlockCount - 1 ) )
				++ _free;
			else
				_free = -1;
		}
		return ( p );
	}
	void free( T* p ) {
		/* Find HPoolBlock that holds this object memory. */
		int offset( *( reinterpret_cast<char unsigned*>( p ) + OBJECT_SPACE - 1 ) );
		HPoolBlock* pb( reinterpret_cast<HPoolBlock*>( reinterpret_cast<char unsigned*>( p ) - offset * OBJECT_SPACE ) );
		bool wasFull( pb->is_full() );
		if ( pb->free( p ) ) {
			/* HPoolBlock is no longer used so we can possibly remove it. */
			if ( ( _poolBlockCount - _free ) > 1 ) {
				/* We have at least one more block with free space so we can really remove this block. */
				-- _poolBlockCount;
				if ( pb != _poolBlocks[_poolBlockCount] ) {
					_poolBlocks[pb->_index] = _poolBlocks[_poolBlockCount];
					_poolBlocks[pb->_index]->_index = pb->_index;
				}
				if ( _free == _poolBlockCount )
					-- _free;
				delete pb;
			} /* else, this is the only block with free space so we keep it. */
		} else if ( wasFull ) {
			if ( _free > 0 )
				-- _free;
			else
				_free = _poolBlockCount - 1;
			if ( pb != _poolBlocks[_free] ) {
				using yaal::swap;
				/* Oreder of swaps matters. */
				swap( _poolBlocks[pb->_index], _poolBlocks[_free] );
				swap( pb->_index, _poolBlocks[pb->_index]->_index );
			}
		}
	}
	void swap( HPool& pool_ ) {
		if ( &pool_ != this ) {
			using yaal::swap;
			swap( _poolBlocks, pool_._poolBlocks );
			swap( _poolBlockCount, pool_._poolBlockCount );
			swap( _poolBlockCapacity, pool_._poolBlockCapacity );
			swap( _free, pool_._free );
		}
	}
private:
	void get_free_block( void ) {
		if ( _poolBlockCount == _poolBlockCapacity ) {
			int newCapacity( ( _poolBlockCapacity > 0 ? _poolBlockCapacity * 2 : 8 ) );
			/* msvcxx does not allow `T v( i );', it must be `T v = i;'. */
			HPoolBlock** poolBlocks = new ( memory::yaal ) HPoolBlock*[ newCapacity ];
			if ( _poolBlockCapacity )
				copy( _poolBlocks, _poolBlocks + _poolBlockCount, poolBlocks );
			using yaal::swap;
			swap( _poolBlocks, poolBlocks );
			delete [] poolBlocks;
			_poolBlockCapacity = newCapacity;
		}
		_poolBlocks[_poolBlockCount] = new ( memory::yaal ) HPoolBlock( _poolBlockCount );
		_free = _poolBlockCount;
		++ _poolBlockCount;
		return;
	}
private:
	HPool( HPool const& );
	HPool& operator = ( HPool const& );
};

template<typename T>
int const HPool<T>::OBJECTS_PER_BLOCK;
template<typename T>
int const HPool<T>::OBJECT_SPACE;

template<typename T>
inline void swap( yaal::hcore::HPool<T>& a, yaal::hcore::HPool<T>& b ) {
	a.swap( b );
}

}

}

#endif /* #ifndef YAAL_HCORE_HPOOL_HXX_INCLUDED */

