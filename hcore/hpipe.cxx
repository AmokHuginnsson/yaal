/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hpipe.cxx - this file is integral part of `yaal' project.

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

#include <unistd.h>

#include "base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hpipe.hxx"
#include "hrawfile.hxx"
#include "system.hxx"

namespace yaal
{

namespace hcore
{

HPipe::HPipe( void )
	: _pipe()
	{
	M_PROLOG
	M_ENSURE( ::pipe( _pipe ) == 0 );
	return;
	M_EPILOG
	}

HPipe::~HPipe( void )
	{
	M_PROLOG
	TEMP_FAILURE_RETRY( system::close( _pipe[ 1 ] ) );
	TEMP_FAILURE_RETRY( system::close( _pipe[ 0 ] ) );
	return;
	M_DESTRUCTOR_EPILOG
	}

int HPipe::get_reader_fd( void ) const
	{
	return ( _pipe[ 0 ] );
	}

int long HPipe::do_read( void* const buffer_, int long size_ )
	{
	M_PROLOG
	return ( ::read( _pipe[ 0 ], buffer_, size_ ) );
	M_EPILOG
	}

int long HPipe::do_write( void const* const buffer_, int long size_ )
	{
	M_PROLOG
	M_ASSERT( _pipe[1] >= 0 );
	int long nWritten( 0 );
	int long nWriteChunk( 0 );
	do
		{
		nWriteChunk = TEMP_FAILURE_RETRY( ::write( _pipe[ 1 ],
					static_cast<char const* const>( buffer_ ) + nWritten,
					size_ - nWritten ) );
		nWritten += nWriteChunk;
		}
	while ( ( nWriteChunk > 0 ) && ( nWritten < size_ ) );
	return ( nWritten );
	M_EPILOG
	}

void HPipe::do_flush( void ) const
	{
	}

bool HPipe::do_is_valid( void ) const
	{
	M_PROLOG
	return ( ( _pipe[ 0 ] >= 0 ) && ( _pipe[ 1 ] >= 0 ) );
	M_EPILOG
	}

}

}

