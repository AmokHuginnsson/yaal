/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstdlib>
#include <cstring>
#include <cstdio>

#include "base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "memory.hxx"

namespace yaal {

namespace memory {

ON_ALLOC_FAILURE _onAllocFailure_ = ON_ALLOC_FAILURE::ABORT;

void* alloc( int long size_ ) {
	M_ASSERT( ( size_ > 0 ) && "memory::malloc: requested size lower than 0" );
	void* newPtr( ::malloc( static_cast<size_t>( size_ ) ) );
	if ( newPtr == nullptr ) {
		char const msg[] = "memory::malloc: malloc returned NULL";
		if ( _onAllocFailure_ == ON_ALLOC_FAILURE::ABORT ) {
			::perror( msg );
			::abort();
		} else
			throw HMemoryAllocationException( msg );
	}
	return ( newPtr );
}

void* calloc( int long size_ ) {
	void* newPtr( alloc( size_ ) );
	::memset( newPtr, 0, static_cast<size_t>( size_ ) );
	return ( newPtr );
}

void* realloc( void* ptr_, int long size_ ) {
	M_ASSERT( ( size_ >= 0 ) && "memory::realloc: requested size lower than 0" );
	void* newPtr( ::realloc( ptr_, static_cast<size_t>( size_ ) ) );
	if ( newPtr == nullptr ) {
		char const msg[] = "memory::realloc: realloc returned NULL";
		if ( _onAllocFailure_ == ON_ALLOC_FAILURE::ABORT ) {
			::perror( msg );
			::abort();
		} else
			throw HMemoryAllocationException( msg );
	}
	return ( newPtr );
}

void free0( void* ptr_ ) throw() {
	M_ASSERT( ( ptr_ != nullptr ) && "memory::free0: request to free NULL pointer" );
	::free( ptr_ );
	return;
}

}

}

void* operator new ( std::size_t size_, yaal::memory::YaalNew const& ) {
	M_ASSERT( ( size_ > 0 ) && "yaal::memory::new: requested size lower than 0" );
	void* newPtr( ::operator new ( size_, std::nothrow ) );
	if ( newPtr == nullptr ) {
		char const msg[] = "yaal::memory::new: new returned NULL";
		if ( yaal::memory::_onAllocFailure_ == yaal::memory::ON_ALLOC_FAILURE::ABORT ) {
			::perror( msg );
			::abort();
		} else
			throw yaal::memory::HMemoryAllocationException( msg );
	}
	return ( newPtr );
}

void* operator new[] ( std::size_t size_, yaal::memory::YaalNew const& ) {
	M_ASSERT( ( size_ > 0 ) && "yaal::memory::new[]: requested size lower than 0" );
	void* newPtr( ::operator new[] ( size_, std::nothrow ) );
	if ( newPtr == nullptr ) {
		char const msg[] = "yaal::memory::new: new[] returned NULL";
		if ( yaal::memory::_onAllocFailure_ == yaal::memory::ON_ALLOC_FAILURE::ABORT ) {
			::perror( msg );
			::abort();
		} else
			throw yaal::memory::HMemoryAllocationException( msg );
	}
	return ( newPtr );
}

