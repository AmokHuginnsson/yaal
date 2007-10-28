/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hpipe.cxx - this file is integral part of `yaal' project.

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

#include <unistd.h>

#include "hexception.h"
M_VCSID ( "$Id$" )
#include "hpipe.h"

#include "hrawfile.h"

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
	::close( f_piPipe[ 1 ] );
	::close( f_piPipe[ 0 ] );
	return;
	M_EPILOG
	}

int HPipe::get_reader_fd( void ) const
	{
	return ( f_piPipe[ 0 ] );
	}

int HPipe::do_read( void* const a_pcBuffer, int const a_iSize )
	{
	M_PROLOG
	return ( ::read( f_piPipe[ 0 ], a_pcBuffer, a_iSize ) );
	M_EPILOG
	}

int HPipe::do_write( void const* const a_pcBuffer, int const a_iSize )
	{
	M_PROLOG
	if ( HRawFile::is_write_ready( f_piPipe[ 1 ] ) )
		return ( ::write( f_piPipe[ 1 ], a_pcBuffer, a_iSize ) ); 
	return ( -1 );
	M_EPILOG
	}

void HPipe::do_flush( void ) const
	{
	}

}

}

