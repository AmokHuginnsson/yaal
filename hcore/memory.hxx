/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

  hcore/memory.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/memory.hxx
 * \brief Declarations of basic memory functions.
 */

#ifndef YAAL_HCORE_MEMORY_HXX_INCLUDED
#define YAAL_HCORE_MEMORY_HXX_INCLUDED 1

#include <cstddef>
#include <new>

#include "hcore/hexception.hxx"

namespace yaal {

/*! \brief Memory handling routines.
 */
namespace memory {

struct YaalNew {};

static YaalNew yaal __attribute__((used));

struct MemoryAllocation {};
typedef yaal::hcore::HExceptionT<MemoryAllocation> HMemoryAllocationException;

enum class ON_ALLOC_FAILURE {
	ABORT,
	THROW
};

void* alloc( int long );
void* calloc( int long );
void* realloc( void*, int long );
void free0( void* ) throw();

template<typename tType>
inline tType* alloc( int long count_ ) {
	return ( static_cast<tType*>( alloc( count_ * static_cast<int>( sizeof ( tType ) ) ) ) );
}

template<typename tType>
inline tType* calloc( int long count_ ) {
	return ( static_cast<tType*>( calloc( count_ * static_cast<int>( sizeof ( tType ) ) ) ) );
}

template<typename tType>
inline tType* realloc( void* pointer_, int long count_ ) {
	return ( static_cast<tType*>( realloc( pointer_, count_ * static_cast<int>( sizeof ( tType ) ) ) ) );
}

template<typename tType>
inline void free( tType& pointer_ ) throw() {
	free0( pointer_ );
	pointer_ = NULL;
	return;
}

}

}

void* operator new ( std::size_t, yaal::memory::YaalNew const& ) throw ( yaal::memory::HMemoryAllocationException );
void operator delete ( void*, yaal::memory::YaalNew const& ) throw ();
inline void operator delete ( void* ptr_, yaal::memory::YaalNew const& ) throw () {
	::operator delete ( ptr_ );
}
void* operator new[] ( std::size_t, yaal::memory::YaalNew const& ) throw ( yaal::memory::HMemoryAllocationException );
void operator delete[] ( void*, yaal::memory::YaalNew const& ) throw ();
inline void operator delete[] ( void* ptr_, yaal::memory::YaalNew const& ) throw () {
	::operator delete[] ( ptr_ );
}

#endif /* #ifndef YAAL_HCORE_MEMORY_HXX_INCLUDED */

