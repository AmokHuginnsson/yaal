/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstdio>
#include <unistd.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hlockedfile.hxx"

namespace yaal {

namespace tools {

HLockedFile::HLockedFile( void )
	: HFile() {
}

HLockedFile::HLockedFile( yaal::hcore::HString const& path, open_t mode )
	: HFile( path, mode ) {
	M_PROLOG
	if ( is_opened() ) {
		lock();
	}
	return;
	M_EPILOG
}

HLockedFile::~HLockedFile( void ) {
	M_PROLOG
	close();
	return;
	M_DESTRUCTOR_EPILOG
}

int HLockedFile::do_open( yaal::hcore::HString const& path_, open_t mode_ ) {
	M_PROLOG
	int ret( HFile::do_open( path_, mode_ ) );
	if ( ! ret ) {
		lock();
	}
	return ( ret );
	M_EPILOG
}

int HLockedFile::do_close( void ) {
	M_PROLOG
	if ( is_opened() ) {
		unlock();
	}
	return ( HFile::do_close() );
	M_EPILOG
}

void HLockedFile::lock( void ) {
	M_PROLOG
	M_ASSERT( is_opened() );
	int fd( get_file_descriptor() );
	M_ENSURE( fd >= 0 );
	M_ENSURE( lockf( fd, F_LOCK, 0 ) == 0 );
	return;
	M_EPILOG
}

void HLockedFile::unlock( void ) {
	M_PROLOG
	int fd( get_file_descriptor() );
	M_ENSURE( fd >= 0 );
	M_ENSURE( lockf( fd, F_ULOCK, 0 ) == 0 );
	return;
	M_EPILOG
}

}

}

