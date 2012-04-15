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

#include "hcore/memory.hxx"

namespace yaal {

namespace hcore {

/*! \brief HPool<> - object memory pool.
 */
template<typename T>
class HPool {
	class HPoolBlock {
	private:
		typedef int long long padding_t;
		static int const OBJECTS_PER_BLOCK = 256;
		static int const PADDING_ELEMENTS_PER_BLOCK = sizeof ( T ) * ( OBJECTS_PER_BLOCK / sizeof ( padding_t ) );
		int long long _mem[ PADDING_ELEMENTS_PER_BLOCK ];
		int _free; /*!< index of first free object */
		int _used; /*!< number of allocated object in this block pool*/
	public:
		HPoolBlock( void )
			: _mem(), _free( 0 ), _used( 0 ) {
			/* Create linked list of free object memory places. */
			for ( int i( 0 ); i < ( OBJECTS_PER_BLOCK - 1 ); ++ i ) {
				*reinterpret_cast<char*>( reinterpret_cast<T*>( _mem ) + i ) = i + 1;
			}
			*reinterpret_cast<char*>( reinterpret_cast<T*>( _mem ) + OBJECTS_PER_BLOCK - 1 ) = -1;
		}
		T* alloc( void ) {
			T* p( reinterpret_cast<T*>( _mem ) + _free );
			_free = *reinterpret_cast<char*>( p );
			++ _used;
			return ( p );
		}
		bool free( T* ptr_ ) {
			int freed( ptr_ - reinterpret_cast<T*>( _mem ) );
			*reinterpret_cast<char*>( ptr_ ) = _free;
			_free = freed;
			-- _used;
			return ( _used == 0 );
		}
		bool is_full( void ) const {
			return ( _free == -1 );
		}
	};
	HPoolBlock** _poolBlocks;
	int _poolBlockCount;
	int _poolBlockCapacity;
	int _free; /*!< index of first HPoolBlock with free space */
	int _freeCount; /*!< Number of HPoolBlock(s) that still have some free space. */
public:
	HPool( void )
		: _poolBlocks( NULL ), _poolBlockCount( 0 ),
		_poolBlockCapacity( 0 ), _free( -1 ),
		_freeCount( 0 )
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
			_free = -1;
			-- _freeCount;
		}
		return ( p );
	}
	void free( T* p ) {
		/* Find HPoolBlock that holds this object memory. */
		HPoolBlock** pb( lower_bound( _poolBlocks, _poolBlocks + _poolBlockCount, p ) );
		if ( pb != p )
			-- pb;
		int freeing( pb - _poolBlocks );
		bool wasFull( (*pb)->is_full() );
		if ( (*pb)->free( p ) ) {
			/* HPoolBlock is no longer used and we can remove it. */
			delete *pb;
			copy( pb + 1, _poolBlocks + _poolBlockCount, pb );
			-- _poolBlockCount;
			-- _freeCount;
			if ( freeing == _free )
				_free = -1;
		} else { 
			if ( wasFull )
				++ _freeCount;
			if ( _free == -1 )
				_free = freeing;
		}
	}
private:
	void get_free_block( void ) {
		if ( _freeCount > 0 ) {
			for ( int i( 0 ); i < _poolBlockCount; ++ i ) {
				if ( ! _poolBlocks[i]->is_full() ) {
					_free = i;
					break;
				}
			}
		} else {
			if ( _poolBlockCount == _poolBlockCapacity ) {
				int newCapacity( ( _poolBlockCapacity > 0 ? _poolBlockCapacity * 2 : 8 ) );
				HPoolBlock** poolBlocks( new HPoolBlock*[ newCapacity ] );
				if ( _poolBlockCapacity )
					copy( _poolBlocks, _poolBlocks + _poolBlockCount, poolBlocks );
				using yaal::swap;
				swap( _poolBlocks, poolBlocks );
				delete [] poolBlocks;
				_poolBlockCapacity = newCapacity;
			}
			HPoolBlock* pb( new HPoolBlock );
			HPoolBlock** spot( lower_bound( _poolBlocks, _poolBlocks + _poolBlockCount, pb ) );
			copy_backward( spot, _poolBlocks + _poolBlockCount, _poolBlocks + _poolBlockCount + 1 );
			*spot = pb;
			++ _poolBlockCount;
			++ _freeCount;
			_free = spot - _poolBlocks;
		}
		return;
	}
private:
	HPool( HPool const& );
	HPool& operator = ( HPool const& );
};

}

template<typename T>
inline void swap( yaal::hcore::HPool<T>& a, yaal::hcore::HPool<T>& b ) {
	a.swap( b );
}

}

#endif /* #ifndef YAAL_HCORE_HPOOL_HXX_INCLUDED */

