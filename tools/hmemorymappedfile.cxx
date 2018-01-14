/* Read yaal/LICENSE.md file for copyright and licensing information. */

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

