/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hrawfile.c - this file is integral part of `stdhapi' project.

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
M_CVSID ( "$CVSHeader$" );
#include "hrawfile.h"

namespace stdhapi
{

namespace hcore
{

HRawFile::HRawFile ( void )
	{
	M_PROLOG;
	f_iFileDescriptor = 0;
	M_EPILOG;
	}

HRawFile::~HRawFile ( void )
	{
	M_PROLOG
	if ( f_iFileDescriptor )
		close ( );
	M_EPILOG
	}

int HRawFile::close ( void )
	{
	M_PROLOG
	int l_iError = 0;
	if ( f_iFileDescriptor )
		l_iError = ::close ( f_iFileDescriptor );
	else
		M_THROW ( "file is not opened", g_iErrNo );
	f_iFileDescriptor = 0;
	return ( l_iError );
	M_EPILOG
	}

file_descriptor_t HRawFile::get_file_descriptor ( void )
	{
	M_PROLOG
	return ( f_iFileDescriptor );
	M_EPILOG
	}

}

}

