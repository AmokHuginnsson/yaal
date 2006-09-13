/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hrawfile.cxx - this file is integral part of `yaal' project.

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
#include <libintl.h>

#include "hexception.h"
M_VCSID ( "$Id$" )
#include "hrawfile.h"

namespace yaal
{

namespace hcore
{

char const * const n_pcError = _ ( "file is not opened" );

HRawFile::HRawFile ( void ) : f_iFileDescriptor ( - 1 )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HRawFile::~HRawFile ( void )
	{
	M_PROLOG
	if ( f_iFileDescriptor >= 0 )
		HRawFile::close ( );
	M_EPILOG
	}

int HRawFile::close ( void )
	{
	M_PROLOG
	int l_iError = 0;
	if ( f_iFileDescriptor < 0 )
		M_THROW ( "file is not opened", errno );
	l_iError = ::close ( f_iFileDescriptor );
	f_iFileDescriptor = - 1;
	return ( l_iError );
	M_EPILOG
	}

file_descriptor_t HRawFile::get_file_descriptor ( void ) const
	{
	M_PROLOG
	return ( f_iFileDescriptor );
	M_EPILOG
	}

int HRawFile::read ( void * const a_pcBuffer, int const a_iSize )
	{
	M_PROLOG
	int l_iCnt = 0;
	if ( f_iFileDescriptor < 0 )
		M_THROW ( n_pcError, errno );
	l_iCnt = ::read ( f_iFileDescriptor, a_pcBuffer, a_iSize );
	return ( l_iCnt );
	M_EPILOG
	}

int HRawFile::write ( void const * const a_pcBuffer, int const a_iSize )
	{
	M_PROLOG
	int l_iCnt = 0;
	if ( f_iFileDescriptor < 0 )
		M_THROW ( n_pcError, errno );
	l_iCnt = ::write ( f_iFileDescriptor, a_pcBuffer, a_iSize );
	return ( l_iCnt );
	M_EPILOG
	}

}

}

