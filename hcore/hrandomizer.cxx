/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hrandomizer.cxx - this file is integral part of `yaal' project.

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

#include <sys/time.h>

#include "base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hrandomizer.hxx"
#include "hexception.hxx"

namespace yaal {

namespace hcore {

namespace {
	static long unsigned const MULTIPLIER = 0x015a4e35L;
	static long unsigned const INCREMENT  = 1;
}

HRandomizer::HRandomizer( int long unsigned const seed_, int cap_ )
	: _seed( seed_ ), _range( cap_ ) {
	M_PROLOG
	M_ENSURE( _range > 0 );
	return;
	M_EPILOG
}
	
int HRandomizer::operator()( int range_ ) {
	M_PROLOG
	_seed = MULTIPLIER * _seed + INCREMENT;
	M_ENSURE( range_ > 0 );
	return ( static_cast<int>( ( _seed >> 16 ) & 0x7fff ) % range_ );
	M_EPILOG
}

int HRandomizer::operator()( void ) {
	M_PROLOG
	_seed = MULTIPLIER * _seed + INCREMENT;
	return ( static_cast<int>( ( _seed >> 16 ) & 0x7fff ) % _range );
	M_EPILOG
}

namespace randomizer_helper {

HRandomizer make_randomizer( int cap_ ) {
	M_PROLOG
	struct timeval tv;
	M_ENSURE( gettimeofday( &tv, NULL ) == 0 );
	return ( HRandomizer( tv.tv_sec + tv.tv_usec, cap_ ) );
	M_EPILOG
}

}

}

}

