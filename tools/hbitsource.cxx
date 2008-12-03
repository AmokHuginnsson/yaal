/*
---           hanalyser.h 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hbitsource.cxx - this file is integral part of hanalyser.h project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "hbitsource.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal
{

namespace tools
{

HBitmap	const& HBitSourceInterface::get_nth_block( int long const& block, int long const& size ) const
	{
	return ( do_get_nth_block( block, size ) );
	}

namespace
{

static int long const D_BUFFER_SIZE = yaal::power<2,20>::value;

}

HBitSourceFile::HBitSourceFile( HString const& a_oPath )
	: HBitSourceInterface(), f_oPath( a_oPath ), f_oFile(),
	f_oBuffer( xcalloc<char>( D_BUFFER_SIZE ) ), f_lBufferOffset( 0 ),
	f_lBufferSize( 0 ), f_oBitmap()
	{
	}

HBitSourceFile::~HBitSourceFile( void )
	{
	}

HBitmap const& HBitSourceFile::do_get_nth_block( int long const& block, int long const& size ) const
	{
	M_PROLOG
	M_ASSERT( size > 0 );
	if ( size & 7 )
		M_THROW( "reading fractions of octet not supported", static_cast<int>( size & 7 ) );
	if ( ! f_oFile )
		M_ENSURE( f_oFile.open( f_oPath ) == 0 );
	int long firstOctet = ( block * size ) >> 3;
	int long lastOctet = firstOctet + ( size >> 3 );
	if ( ( size >> 3 ) > D_BUFFER_SIZE )
		M_THROW( "so large bitmaps are not supported", size >> 3 );
	if ( ( firstOctet < f_lBufferOffset ) || ( lastOctet >= ( f_lBufferOffset + f_lBufferSize ) ) )
		{
		f_oFile.seek( f_lBufferOffset = firstOctet, HFile::SEEK::D_SET );
		f_lBufferSize = f_oFile.read( f_oBuffer.get<char>(), D_BUFFER_SIZE );
		}
	int long availableOctets = min( f_lBufferSize, size >> 3 );
	if ( availableOctets )
		{
		f_oBitmap.reserve( availableOctets );
		f_oBitmap.copy( static_cast<char const*>( f_oBuffer.get<char>() ) + firstOctet - f_lBufferOffset, availableOctets << 3 );
		}
	else
		f_oBitmap = HBitmap();
	return ( f_oBitmap );
	M_EPILOG
	}

HBitSourceMemory::HBitSourceMemory( void const* a_pvMemory, int long a_lSize )
	: HBitSourceInterface(), f_pvMemory( a_pvMemory ), f_lSIze( a_lSize ), f_oBitmap()
	{
	}

HBitSourceMemory::~HBitSourceMemory( void )
	{
	}

HBitmap const& HBitSourceMemory::do_get_nth_block( int long const& block, int long const& size ) const
	{
	M_PROLOG
	M_ASSERT( size > 0 );
	if ( size & 7 )
		M_THROW( "reading fractions of octet not supported", static_cast<int>( size & 7 ) );
	int long offset = ( block * size ) >> 3;
	if ( offset < f_lSIze )
		{
		int long left = ( f_lSIze - offset ) << 3;
		M_ASSERT( left > 0 );
		int long toCopy = size < left ? size : left;
		f_oBitmap.reserve( toCopy );
		f_oBitmap.copy( static_cast<char const*>( f_pvMemory ) + offset, toCopy );
		}
	else
		f_oBitmap = HBitmap();
	return ( f_oBitmap );
	M_EPILOG
	}

}

}

