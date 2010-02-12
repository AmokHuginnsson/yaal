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

namespace yaal
{

namespace hcore
{

HPipe::HPipe( void )
	{
	M_PROLOG
	M_ENSURE( ::pipe( f_piPipe ) == 0 );
	return;
	M_EPILOG
	}

HPipe::~HPipe( void )
	{
	M_PROLOG
	TEMP_FAILURE_RETRY( ::close( f_piPipe[ 1 ] ) );
	TEMP_FAILURE_RETRY( ::close( f_piPipe[ 0 ] ) );
	return;
	M_EPILOG
	}

int HPipe::get_reader_fd( void ) const
	{
	return ( f_piPipe[ 0 ] );
	}

int long HPipe::do_read( void* const a_pcBuffer, int long const& a_lSize )
	{
	M_PROLOG
	return ( ::read( f_piPipe[ 0 ], a_pcBuffer, a_lSize ) );
	M_EPILOG
	}

int long HPipe::do_write( void const* const a_pcBuffer, int long const& a_lSize )
	{
	M_PROLOG
	int long iWritten = -1;
	if ( HRawFile::is_write_ready( f_piPipe[ 1 ] ) )
		{
		iWritten = 0;
		do
			{
			iWritten += TEMP_FAILURE_RETRY( ::write( f_piPipe[ 1 ],
						static_cast<char const* const>( a_pcBuffer ) + iWritten,
						a_lSize - iWritten ) );
			}
		while ( iWritten < a_lSize );
		}
	return ( iWritten );
	M_EPILOG
	}

void HPipe::do_flush( void ) const
	{
	}

bool HPipe::do_is_valid( void ) const
	{
	M_PROLOG
	return ( ( f_piPipe[ 0 ] >= 0 ) && ( f_piPipe[ 1 ] >= 0 ) );
	M_EPILOG
	}

}

}

