/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hrandomizer.cxx - this file is integral part of `yaal' project.

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

#include <sys/time.h>

#include "base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hrandomizer.hxx"
#include "hexception.hxx"

namespace yaal
{

namespace hcore
{

namespace
	{
	static long unsigned const MULTIPLIER = 0x015a4e35L;
	static long unsigned const INCREMENT  = 1;
	}

HRandomizer::HRandomizer( int long unsigned const seed_, int cap_ )
	: _seed( seed_ ), _range( cap_ )
	{
	M_PROLOG
	M_ENSURE( _range > 0 );
	return;
	M_EPILOG
	}
	
int HRandomizer::operator()( int range_ )
	{
	M_PROLOG
	_seed = MULTIPLIER * _seed + INCREMENT;
	M_ENSURE( range_ > 0 );
	return ( static_cast<int>( ( _seed >> 16 ) & 0x7fff ) % range_ );
	M_EPILOG
	}

int HRandomizer::operator()( void )
	{
	M_PROLOG
	_seed = MULTIPLIER * _seed + INCREMENT;
	return ( static_cast<int>( ( _seed >> 16 ) & 0x7fff ) % _range );
	M_EPILOG
	}

namespace randomizer_helper
{

HRandomizer make_randomizer( int cap_ )
	{
	M_PROLOG
	struct timeval tv;
	M_ENSURE( gettimeofday( &tv, NULL ) == 0 );
	return ( HRandomizer( tv.tv_sec + tv.tv_usec, cap_ ) );
	M_EPILOG
	}

}

}

}

