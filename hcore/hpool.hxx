/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hcore/hpool.hxx
 * \brief Definition and implementation of object memory pool.
 */

#ifndef YAAL_HCORE_HPOOL_HXX_INCLUDED
#define YAAL_HCORE_HPOOL_HXX_INCLUDED 1

#include "config.hxx"
#include "hcore/memory.hxx"
#include "hcore/algorithm.hxx"

namespace yaal {

namespace hcore {

/*! \brief HPool<> - object memory pool.
 */
template<int const size>
class HPool final {
public:
	typedef int long long aligner_t;
	static int const OBJECTS_PER_BLOCK = 256;
	static int const OBJECT_SIZE = size;
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
	static int const ALIGNER_ELEMENTS_PER_BLOCK = OBJECT_SPACE * ( OBJECTS_PER_BLOCK / static_cast<int>( sizeof ( aligner_t ) ) );
private:
	class HPoolBlock final {
	private:
		aligner_t _mem[ HPool<size>::ALIGNER_ELEMENTS_PER_BLOCK ];
		int _free; /*!< index of first free object */
		int _used; /*!< number of allocated object in this block pool*/
		int _index; /*!< This block index in HPool<>. */
	public:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wunknown-warning-option"
#pragma GCC diagnostic ignored "-Wlarger-than="
		HPoolBlock( int index_ )
			: _mem()
			, _free( 0 )
			, _used( 0 )
			, _index( index_ ) {
			/* Create linked list of free object memory places. */
			u8_t* p( reinterpret_cast<u8_t*>( _mem ) );
			for ( int i( 0 ); i < OBJECTS_PER_BLOCK; ++ i, p += OBJECT_SPACE ) {
				*p = static_cast<u8_t>( i + 1 );
				*( p + OBJECT_SPACE - 1 ) = static_cast<u8_t>( i );
			}
		}
#pragma GCC diagnostic pop
		void* alloc( void ) {
			void* p( reinterpret_cast<char*>( _mem ) + OBJECT_SPACE * _free );
			_free = *static_cast<u8_t*>( p );
			++ _used;
			return ( p );
		}
		bool free( void* ptr_ ) {
			int freed( *( static_cast<u8_t*>( ptr_ ) + OBJECT_SPACE - 1 ) );
			*static_cast<u8_t*>( ptr_ ) = static_cast<u8_t>( _free );
			_free = freed;
			-- _used;
			return ( _used == 0 );
		}
		bool is_full( void ) const {
			return ( _used == OBJECTS_PER_BLOCK );
		}
	private:
		friend class HPool<size>;
		HPoolBlock( HPoolBlock const& ) = delete;
		HPoolBlock& operator = ( HPoolBlock const& ) = delete;
	};
	HPoolBlock** _poolBlocks;
	int _poolBlockCount;
	int _poolBlockCapacity;
	int _free; /*!< index of first HPoolBlock with free space. */
public:
	HPool( void )
		: _poolBlocks( nullptr )
		, _poolBlockCount( 0 )
		, _poolBlockCapacity( 0 )
		, _free( -1 ) {
		return;
	}
	HPool( HPool&& ) noexcept = default;
	HPool& operator = ( HPool&& ) noexcept = default;
	~HPool( void ) {
		M_ASSERT( ( _poolBlockCount == 0 ) || ( _poolBlockCount == 1 ) );
		if ( _poolBlockCount == 1 ) {
			M_ASSERT( _free == 0 );
			M_ASSERT( _poolBlocks[_free]->_used == 0 );
			delete _poolBlocks[_free];

		}
		delete [] _poolBlocks;
	}
	void* alloc( void ) {
		if ( _free == -1 ) {
			get_free_block();
		}
		void* p( _poolBlocks[_free]->alloc() );
		if ( _poolBlocks[_free]->is_full() ) {
			if ( _free < ( _poolBlockCount - 1 ) ) {
				++ _free;
			} else {
				_free = -1;
			}
		}
		return ( p );
	}
	void free( void* p ) {
		/* Find HPoolBlock that holds this object memory. */
		int offset( *( static_cast<u8_t*>( p ) + OBJECT_SPACE - 1 ) );
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"
		HPoolBlock* pb( reinterpret_cast<HPoolBlock*>( static_cast<u8_t*>( p ) - offset * OBJECT_SPACE ) );
#pragma GCC diagnostic pop
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
				if ( _free == _poolBlockCount ) {
					-- _free;
				}
				delete pb;
			} /* else, this is the only block with free space so we keep it. */
		} else if ( wasFull ) {
			if ( _free > 0 ) {
				-- _free;
			} else {
				_free = _poolBlockCount - 1;
			}
			if ( pb != _poolBlocks[_free] ) {
				using yaal::swap;
				/* Order of swaps matters. */
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
			if ( _poolBlockCapacity ) {
				yaal::copy( _poolBlocks, _poolBlocks + _poolBlockCount, poolBlocks );
			}
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

template<int const size>
inline void swap( yaal::hcore::HPool<size>& a, yaal::hcore::HPool<size>& b ) {
	a.swap( b );
}

}

}

#endif /* #ifndef YAAL_HCORE_HPOOL_HXX_INCLUDED */

