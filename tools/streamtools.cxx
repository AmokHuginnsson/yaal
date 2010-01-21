/*
---           yaal 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	streamtools.cxx - this file is integral part of yaal project.

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

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "streamtools.hxx"

using namespace yaal::hcore;

namespace yaal
{

namespace tools
{

HBinaryFormatter::HBinaryFormatter( HStreamInterface* stream_ )
	: _stream( stream_ )
	{
	}

HStreamInterface& HBinaryFormatter::operator << ( HStreamInterface::manipulator_t const& M )
	{
	M_PROLOG
	return ( M( *_stream) );
	M_EPILOG
	}

HBinaryFormatter& HBinaryFormatter::binary( void const* buf_, int const& size_ )
	{
	M_PROLOG
	int const mask[] = { 128, 64, 32, 16, 8, 4, 2, 1 };
	for ( int i = 0; i < size_; ++ i )
		{
		for ( int b = 0; b < 8; ++ b )
			(*_stream) << ( static_cast<char const*>( buf_ )[ i ] & mask[ b ] ? '1' : '0' );
		}
	return ( *this );
	M_EPILOG
	}

HBinaryFormatter HBinaryFormatterSeed::create( HStreamInterface* stream_ ) const
	{
	M_PROLOG
	return ( HBinaryFormatter( stream_ ) );
	M_EPILOG
	}

HBinaryFormatter operator << ( HStreamInterface& stream_, HBinaryFormatterSeed const& seed_ )
	{
	M_PROLOG
	return ( seed_.create( &stream_ ) );
	M_EPILOG
	}

}

}

