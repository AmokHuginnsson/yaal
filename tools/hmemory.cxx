/*
---           yaal 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hmemory.cxx - this file is integral part of yaal project.

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
#include "hmemory.hxx"

namespace yaal
{

namespace tools
{

HMemory::HMemory( void* ptr, int long const& size ) : f_pvBlock( ptr ), f_lSize( size ), f_lCursorRead( 0 ), f_lCursorWrite( 0 )
	{
	M_ASSERT( size >= 0 );
	return;
	}

HMemory::~HMemory( void )
	{
	return;
	}

bool HMemory::operator == ( HMemory const& other ) const
	{
	M_PROLOG
	return ( ( other.f_lSize == f_lSize ) && ( ! memcmp( other.f_pvBlock, f_pvBlock, f_lSize ) ) );
	M_EPILOG
	}

int long HMemory::do_write( void const* const src, int long const& size )
	{
	M_PROLOG
	M_ENSURE( f_lCursorWrite + size < f_lSize );
	::memcpy( static_cast<char*>( f_pvBlock ) + f_lCursorWrite, src, size );
	f_lCursorWrite += size;
	return ( size );
	M_EPILOG
	}

void HMemory::do_flush( void ) const
	{
	return;
	}

int long HMemory::do_read( void* const dest, int long const& size )
	{
	M_PROLOG
	M_ENSURE( f_lCursorRead + size < f_lSize );
	::memcpy( dest, static_cast<char const* const>( f_pvBlock ) + f_lCursorRead, size );
	f_lCursorRead += size;
	return ( size );
	M_EPILOG
	}

bool HMemory::do_is_valid( void ) const
	{
	M_PROLOG
	return ( ( f_lCursorRead < f_lSize ) && ( f_lCursorWrite < f_lSize ) );
	M_EPILOG
	}

}

}

