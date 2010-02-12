/*
---           yaal 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hstreamblockiterator.cxx - this file is integral part of yaal project.

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
M_VCSID( "$Id: "__TID__" $" )
#include "hcore/algorithm.hxx"
#include "hstreamblockiterator.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal
{

namespace tools
{

namespace
{

static int long const BUFFER_SIZE = yaal::meta::power<2,20>::value;

}

HStreamBlockIterator::HStreamBlockIterator( HStreamInterface& a_roStream, int long a_lSize )
	: f_oBuffer( BUFFER_SIZE ), f_lIndex( 0 ), f_lSize( a_lSize ),
	f_lBufferOffset( 0 ), f_lBufferSize( 0 ), f_roStream( a_roStream )
	{
	M_ASSERT( f_lSize > 0 );
	if ( f_lSize > BUFFER_SIZE )
		M_THROW( "so large stream blocks are not supported", f_lSize );
	}

HStreamBlockIterator::~HStreamBlockIterator( void )
	{
	}

HStreamBlockIterator& HStreamBlockIterator::operator ++ ( void )
	{
	M_PROLOG
	++ f_lIndex;
	return ( *this );
	M_EPILOG
	}

HStreamBlockIterator::HBlock HStreamBlockIterator::operator* ( void )
	{
	M_PROLOG
	int long firstOctet = f_lIndex * f_lSize;
	if ( ( firstOctet >= ( f_lBufferOffset + f_lBufferSize ) ) && ( ( f_lBufferSize == BUFFER_SIZE ) || ! f_lBufferSize ) )
		{
		f_lBufferOffset = firstOctet;
		f_lBufferSize = f_roStream.read( f_oBuffer.raw(), BUFFER_SIZE );
		}
	return ( HBlock( ( f_oBuffer.raw() + firstOctet ) - f_lBufferOffset, min( f_lBufferSize - ( firstOctet - f_lBufferOffset ), f_lSize ) ) );
	M_EPILOG
	}

}

}

