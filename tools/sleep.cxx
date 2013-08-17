/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	sleep.cxx - this file is integral part of `yaal' project.

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

#include <cerrno>
#include <cstdlib>
#include <sys/time.h> /* timeval (for FreeBSD) */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
#include "sleep.hxx"

namespace yaal {

namespace tools {

namespace sleep {

inline bool sleep_real( timeval& time_, bool ignoreInterrrupt_ ) {
	int err( 0 );
	while ( ( ( err = ::select( 0, NULL, NULL, NULL, &time_ ) ) == -1 ) && ( errno == EINTR ) && ignoreInterrrupt_ )
		;
	return ( ( err != 0 ) && ! ( ( errno == EINTR ) && ignoreInterrrupt_ ) );
}

bool milisecond( int quantity_, bool ignoreInterrrupt_ ) {
	timeval wait;
	wait.tv_sec = quantity_ / 1000;
	wait.tv_usec = ( quantity_ %  1000 ) * 1000;
	return ( sleep_real( wait, ignoreInterrrupt_ ) );
}

bool second( int quantity_, bool ignoreInterrrupt_ ) {
	timeval wait;
	wait.tv_sec = quantity_;
	wait.tv_usec = 0;
	return ( sleep_real( wait, ignoreInterrrupt_ ) );
}

}

}

}

