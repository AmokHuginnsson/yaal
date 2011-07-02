/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hcore/memory.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/memory.hxx
 * \brief Declarations of basic memory functions.
 */

#ifndef YAAL_HCORE_MEMORY_HXX_INCLUDED
#define YAAL_HCORE_MEMORY_HXX_INCLUDED 1

#include <cstddef>

#include "hcore/hexception.hxx"

namespace yaal
{

/*! \brief Memory handling routines.
 */
namespace memory
{

struct YaalNew {};

static YaalNew yaal __attribute__((used));

struct MemoryAllocation {};
typedef yaal::hcore::HExceptionT<MemoryAllocation> HMemoryAllocationException;

struct ON_ALLOC_FAILURE
	{
	typedef enum
		{
		ABORT,
		THROW
		} on_alloc_failure_t;
	};

void* alloc( int long );
void* calloc( int long );
void* realloc( void*, int long );
void free0( void* ) throw();

template<typename tType>
inline tType* alloc( int long count_ )
	{
	return ( static_cast<tType*>( alloc( count_ * sizeof ( tType ) ) ) );
	}

template<typename tType>
inline tType* calloc( int long count_ )
	{
	return ( static_cast<tType*>( calloc( count_ * sizeof ( tType ) ) ) );
	}

template<typename tType>
inline tType* realloc( void* pointer_, int long count_ )
	{
	return ( static_cast<tType*>( realloc( pointer_, count_ * sizeof ( tType ) ) ) );
	}

template<typename tType>
inline void free( tType& pointer_ ) throw()
	{
	free0( pointer_ );
	pointer_ = NULL;
	return;
	}

}

}

void* operator new ( std::size_t, yaal::memory::YaalNew const& ) throw ( yaal::memory::HMemoryAllocationException );
void operator delete ( void*, yaal::memory::YaalNew const& ) throw ();
void* operator new[] ( std::size_t, yaal::memory::YaalNew const& ) throw ( yaal::memory::HMemoryAllocationException );
void operator delete[] ( void*, yaal::memory::YaalNew const& ) throw ();

#endif /* #ifndef YAAL_HCORE_MEMORY_HXX_INCLUDED */

