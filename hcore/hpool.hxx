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
		typedef int long long padding_t;
		static int const OBJECTS_PER_BLOCK = 256;
		static int const PADDING_ELEMENTS_PER_BLOCK = sizeof ( T ) * ( OBJECTS_PER_BLOCK / sizeof ( padding_t ) );
		int long long _mem[ PADDING_ELEMENTS_PER_BLOCK ];
		int _free; /*!< index of first free object */
	public:
		HPoolBlock( void )
			: _mem(), _free( 0 ) {
			/* Create linked list of free object memory places. */
			for ( int i( 0 ); i < ( OBJECTS_PER_BLOCK - 1 ); ++ i ) {
			}
		}
		T* alloc( void ) {
			return ( NULL );
		}
		void free( T* ) {
		}
	};
public:
	T* alloc( void ) {
		return ( NULL );
	}
	void free( T* ) {
		/* Find HPoolBlock<> that holds this object memory. */
	}
};

}

}

#endif /* #ifndef YAAL_HCORE_HPOOL_HXX_INCLUDED */

