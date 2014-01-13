/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hlockedfile.cxx - this file is integral part of `yaal' project.

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

#include <cstdio>
#include <unistd.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hlockedfile.hxx"

namespace yaal {

namespace tools {

HLockedFile::HLockedFile( void )
	: HFile()
	{ }

HLockedFile::HLockedFile( yaal::hcore::HString const& path, open_t const& mode )
	: HFile( path, mode ) {
	M_PROLOG
	if ( is_opened() )
		lock();
	return;
	M_EPILOG
}

HLockedFile::~HLockedFile( void ) {
	M_PROLOG
	close();
	return;
	M_DESTRUCTOR_EPILOG
}

int HLockedFile::do_open( yaal::hcore::HString const& path_, open_t const& mode_ ) {
	M_PROLOG
	int ret( HFile::do_open( path_, mode_ ) );
	if ( ! ret )
		lock();
	return ( ret );
	M_EPILOG
}

int HLockedFile::do_close( void ) {
	M_PROLOG
	if ( is_opened() )
		unlock();
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
