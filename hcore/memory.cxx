/*
---          `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	memory.cxx - this file is integral part of `yaal' project.

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

#include <new>
#include <cstdlib>
#include <cstring>
#include <cstdio>

#include "base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "memory.hxx"

namespace yaal
{

namespace memory
{

ON_ALLOC_FAILURE::on_alloc_failure_t _onAllocFailure_ = ON_ALLOC_FAILURE::ABORT;

void* alloc( int long size_ )
	{
	M_ASSERT( ( size_ > 0 ) && "memory::malloc: requested size lower than 0" );
	register void* newPtr( ::malloc( size_ ) );
	if ( newPtr == 0 )
		{
		char const msg[] = "memory::malloc: malloc returned NULL";
		if ( _onAllocFailure_ == ON_ALLOC_FAILURE::ABORT )
			{
			::perror( msg );
			::abort();
			}
		else
			throw HMemoryAllocationException( msg );
		}
	return ( newPtr );
	}

void* calloc( int long size_ )
	{
	register void* newPtr( alloc( size_ ) );
	::memset( newPtr, 0, size_ );
	return ( newPtr );
	}

void* realloc( void* ptr_, int long size_ )
	{
	M_ASSERT( ( size_ >= 0 ) && "memory::realloc: requested size lower than 0" );
	register void* newPtr( ::realloc( ptr_, size_ ) );
	if ( newPtr == 0 )
		{
		char const msg[] = "memory::realloc: realloc returned NULL";
		if ( _onAllocFailure_ == ON_ALLOC_FAILURE::ABORT )
			{
			::perror( msg );
			::abort();
			}
		else
			throw HMemoryAllocationException( msg );
		}
	return ( newPtr );
	}

void free0( void* ptr_ ) throw()
	{
	M_ASSERT( ( ptr_ != NULL ) && "memory::free0: request to free NULL pointer" );
	::free( ptr_ );
	return;
	}

}

}

void* operator new ( int long unsigned size_, yaal::memory::YaalNew const& ) throw ( yaal::memory::HMemoryAllocationException )
	{
	M_ASSERT( ( size_ > 0 ) && "yaal::memory::new: requested size lower than 0" );
	void* newPtr( ::operator new ( size_, std::nothrow ) );
	if ( newPtr == 0 )
		{
		char const msg[] = "yaal::memory::new: new returned NULL";
		if ( yaal::memory::_onAllocFailure_ == yaal::memory::ON_ALLOC_FAILURE::ABORT )
			{
			::perror( msg );
			::abort();
			}
		else
			throw yaal::memory::HMemoryAllocationException( msg );
		}
	return ( newPtr );
	}

