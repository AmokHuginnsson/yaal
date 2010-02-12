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
M_VCSID( "$Id: "__TID__" $" )
#include "hmemory.hxx"
#include "hcore/algorithm.hxx"

namespace yaal
{

namespace tools
{

HMemory::HMemory( void* ptr, int long const& size_, INITIAL_STATE::enum_t const& initialState_ )
	: f_pvBlock( ptr ), f_lSize( size_ ),
	f_lValid( initialState_ == INITIAL_STATE::AUTO ? -1 : ( initialState_ == INITIAL_STATE::VALID ? size_ : 0 ) ),
	f_lCursorRead( 0 ), f_lCursorWrite( 0 )
	{
	M_ASSERT( size_ > 0 );
	M_ASSERT( ptr );
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

int long HMemory::do_write( void const* const src_, int long const& size_ )
	{
	M_PROLOG
	if ( f_lValid == -1 ) /* First data access. */
		f_lValid = 0;
	int long maxWrite( f_lSize - f_lValid );
	int long size( min( size_, maxWrite ) );
	if ( ( f_lCursorWrite + size ) > f_lSize )
		{
		int long part1( f_lSize - f_lCursorWrite );
		int long part2( size - part1 );
		::memcpy( static_cast<char*>( f_pvBlock ) + f_lCursorWrite, src_, part1 );
		::memcpy( static_cast<char*>( f_pvBlock ), static_cast<char const* const>( src_ ) + part1, part2 );
		f_lCursorWrite = part2;
		}
	else
		{
		::memcpy( static_cast<char*>( f_pvBlock ) + f_lCursorWrite, src_, size );
		f_lCursorWrite += size;
		}
	f_lValid += size;
	return ( size );
	M_EPILOG
	}

void HMemory::do_flush( void ) const
	{
	return;
	}

int long HMemory::do_read( void* const dest_, int long const& size_ )
	{
	M_PROLOG
	if ( f_lValid == -1 ) /* First data access. */
		f_lValid = f_lSize;
	int long size( min( size_, f_lValid ) );
	if ( ( f_lCursorRead + size ) > f_lSize )
		{
		int long part1( f_lSize - f_lCursorRead );
		int long part2( size - part1 );
		::memcpy( dest_, static_cast<char*>( f_pvBlock ) + f_lCursorRead, part1 );
		::memcpy( static_cast<char*>( dest_ ) + part1, static_cast<char*>( f_pvBlock ), part2 );
		f_lCursorWrite = part2;
		}
	else
		{
		::memcpy( dest_, static_cast<char const* const>( f_pvBlock ) + f_lCursorRead, size );
		f_lCursorRead += size;
		}
	f_lValid -= size;
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

