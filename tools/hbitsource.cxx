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

#include "hcore/base.h"
M_VCSID( "$Id: "__ID__" $" )
#include "hbitsource.h"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal
{

namespace tools
{

HBitmap	HBitSourceInterface::get_nth_block( int long const& block, int long const& size ) const
	{
	return ( do_get_nth_block( block, size ) );
	}

HBitSourceFile::HBitSourceFile( HString const& a_oPath )
	: HBitSourceInterface(), f_oPath( a_oPath ), f_oFile(), f_lLastBit( -1 )
	{
	}

HBitSourceFile::~HBitSourceFile( void )
	{
	}

HBitmap HBitSourceFile::do_get_nth_block( int long const& block, int long const& size ) const
	{
	M_PROLOG
	M_ASSERT( size > 0 );
	if ( size & 7 )
		M_THROW( "reading fractions of octet not supported", static_cast<int>( size & 7 ) );
	if ( ! f_oFile )
		M_ENSURE( f_oFile.open( f_oPath ) == 0 );
	int long startBit = block * size;
	if ( startBit != ( f_lLastBit + 1 ) )
		f_oFile.seek( startBit >> 3, HFile::SEEK::D_SET );
	HBitmap bmp( size );
	bmp.reserve( f_oFile.read( const_cast<void*>( bmp.raw() ), size >> 3 ) << 3 );
	f_lLastBit = ( startBit + size ) - 1;
	return ( bmp );
	M_EPILOG
	}

HBitSourceMemory::HBitSourceMemory( void const* a_pvMemory, int long a_lSize )
	: HBitSourceInterface(), f_pvMemory( a_pvMemory ), f_lSIze( a_lSize )
	{
	}

HBitSourceMemory::~HBitSourceMemory( void )
	{
	}

HBitmap HBitSourceMemory::do_get_nth_block( int long const& block, int long const& size ) const
	{
	M_PROLOG
	M_ASSERT( size > 0 );
	if ( size & 7 )
		M_THROW( "reading fractions of octet not supported", static_cast<int>( size & 7 ) );
	int long offset = ( block * size ) >> 3;
	HBitmap bmp;
	if ( offset < f_lSIze )
		{
		int long left = ( f_lSIze - offset ) << 3;
		int long toCopy = size < left ? size : left;
		bmp.reserve( toCopy );
		bmp.copy( static_cast<char const*>( f_pvMemory ) + offset, toCopy );
		}
	return ( bmp );
	M_EPILOG
	}

}

}

