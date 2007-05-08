/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hprocess.cxx - this file is integral part of `yaal' project.

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

#include <cstring>
#include <unistd.h>
#include <libintl.h>

#include "hcore/hexception.h"
M_VCSID ( "$Id$" )
#include "hprocess.h"

namespace yaal
{

namespace hcore
{

HProcess::HProcess ( size_t a_uiFileHandlers )
	: f_bInitialised ( false ), f_bLoop ( true ), f_iIdleCycles ( 0 ),
	f_iLatencySeconds ( 0 ), f_iLatencyMicroseconds ( 0 ),
	f_sLatency(), f_xFileDescriptorSet(),
	f_oFileDescriptorHandlers ( a_uiFileHandlers )
	{
	M_PROLOG
	memset ( & f_sLatency, 0, sizeof ( f_sLatency ) );
	FD_ZERO ( & f_xFileDescriptorSet );
	return;
	M_EPILOG
	}

HProcess::~HProcess ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

int HProcess::init ( int a_iLatencySeconds, int a_iLatencyMicroseconds )
	{
	M_PROLOG
	if ( f_bInitialised )
		M_THROW ( "you can initialise your main process only once, dumbass",
				errno );
	f_iLatencySeconds = a_iLatencySeconds;
	f_iLatencyMicroseconds = a_iLatencyMicroseconds;
	f_bInitialised = true;
	return ( 1 );
	M_EPILOG
	}

int HProcess::register_file_descriptor_handler_internal ( int a_iFileDescriptor,
		process_handler_filedes_t HANDLER )
	{
	M_PROLOG
	f_oFileDescriptorHandlers [ a_iFileDescriptor ] = HANDLER;
	return ( 0 );
	M_EPILOG
	}

int HProcess::unregister_file_descriptor_handler ( int a_iFileDescriptor )
	{
	M_PROLOG
	return ( f_oFileDescriptorHandlers.remove ( a_iFileDescriptor ) );
	M_EPILOG
	}

int HProcess::reconstruct_fdset ( void )
	{
	M_PROLOG
	int l_iFileDes = 0;
	process_handler_filedes_t DUMMY = NULL;
	f_sLatency.tv_sec = f_iLatencySeconds;
	f_sLatency.tv_usec = f_iLatencyMicroseconds;
	FD_ZERO ( & f_xFileDescriptorSet );
	if ( ! f_oFileDescriptorHandlers.quantity() )
		return ( -1 );
	f_oFileDescriptorHandlers.rewind();
/* FD_SET is a macro and first argument is evaluated twice ! */
	while ( f_oFileDescriptorHandlers.iterate ( l_iFileDes, DUMMY ) )
		FD_SET ( l_iFileDes, & f_xFileDescriptorSet );
	return ( 0 );
	M_EPILOG
	}

int HProcess::run ( void )
	{
	M_PROLOG
	int l_iError = 0;
	int l_iFileDes = 0;
	process_handler_filedes_t HANDLER = NULL;
	if ( ! f_bInitialised )
		M_THROW ( _ ( "you have to call HProcess::init() first, dumbass" ), errno );
	if ( ! f_oFileDescriptorHandlers.quantity() )
		M_THROW ( _ ( "there is no file descriptor to check activity on" ), errno );
	while ( f_bLoop )
		{
		handler_alert ( 0 );
		reconstruct_fdset();
		if ( ( l_iError = select ( FD_SETSIZE, & f_xFileDescriptorSet,
						NULL, NULL, & f_sLatency ) ) )
			{
			if ( ( l_iError < 0 ) && ( errno == EINTR ) )
				{
				handler_interrupt ( 0 );
				continue;
				}
			if ( l_iError < 0 )
				M_THROW ( "select() returned", l_iError );
			f_oFileDescriptorHandlers.rewind();
			while ( f_oFileDescriptorHandlers.iterate ( l_iFileDes, HANDLER ) )
				{
				if ( FD_ISSET ( l_iFileDes, & f_xFileDescriptorSet ) )
					{
					static_cast < void > ( ( this->*HANDLER ) ( l_iFileDes ) );
					f_iIdleCycles = 0;
					}
				}
			}
		else
			handler_idle ( 0 );
		}
	return ( 0 );
	M_EPILOG
	}

int HProcess::handler_alert ( int, void * )
	{
	M_PROLOG
	return ( 0 );
	M_EPILOG
	}

int HProcess::handler_interrupt ( int, void * )
	{
	M_PROLOG
	return ( 0 );
	M_EPILOG
	}

int HProcess::handler_idle ( int a_iCode, void * )
	{
	M_PROLOG
	f_iIdleCycles ++;
	return ( a_iCode );
	M_EPILOG
	}

}

}

