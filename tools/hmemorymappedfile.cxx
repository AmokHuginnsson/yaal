/*
---           yaal 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  hmemorymappedfile.cxx - this file is integral part of `yaal' project.

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

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hmemorymappedfile.hxx"
#include "hcore/hexception.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
static void const* const FWD_MAP_FAILED = MAP_FAILED;
#pragma GCC diagnostic pop
}

HMemoryMappedFile::HMemoryMappedFile( yaal::hcore::HString const& path_, int long size_ )
	: _fd( -1 ), _map( nullptr ), _size( 0 ) {
	M_PROLOG
	HUTF8String utf8( path_ );
	M_ENSURE( ( _fd = ::open( utf8.c_str(), O_RDWR ) ) >= 0, path_ );
	try {
		if ( ! size_ ) {
			M_ENSURE( ( _size = static_cast<int long>( ::lseek( _fd, 0, SEEK_END ) ) ) >= 0 );
		}
		M_ENSURE( ( _map = ::mmap( nullptr, static_cast<size_t>( _size ), PROT_READ | PROT_WRITE, MAP_PRIVATE, _fd, 0 ) ) != FWD_MAP_FAILED );
	} catch ( ... ) {
		M_SAFE( M_ENSURE( ::close( _fd ) != 0 ) );
		_fd = -1;
		_map = nullptr;
		throw;
	}
	return;
	M_EPILOG
}

HMemoryMappedFile::~HMemoryMappedFile( void ) {
	M_PROLOG
	if ( _fd >= 0 ) {
		M_ASSERT( _map );
		M_ENSURE( ::munmap( static_cast<caddr_t>( _map ), static_cast<size_t>( _size ) ) == 0 );
		M_ENSURE( ::close( _fd ) == 0 );
	} else {
		M_ASSERT( ! _map );
	}
	return;
	M_DESTRUCTOR_EPILOG
}

void* HMemoryMappedFile::map( void ) {
	return ( _map );
}

void const* HMemoryMappedFile::map( void ) const {
	return ( _map );
}

int long HMemoryMappedFile::get_size( void ) const {
	return ( _size );
}

int long HMemoryMappedFile::size( void ) const {
	return ( get_size() );
}

}

}

