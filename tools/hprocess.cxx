/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

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
#include <cstdio>
#include <unistd.h>
#include <libintl.h>
#include <signal.h>

#include "hcore/hexception.h"
M_VCSID ( "$Id$" )
#include "hprocess.h"

#include "hcore/rc_file.h"

using namespace yaal::hcore;

namespace yaal
{

namespace tools
{

HProcess::HProcess( size_t a_uiFileHandlers )
	: f_bInitialised( false ), f_bLoop( true ), f_iIdleCycles( 0 ),
	f_iLatencySeconds( 0 ), f_iLatencyMicroseconds( 0 ),
	f_sLatency(), f_xFileDescriptorSet(),
	f_oFileDescriptorHandlers( a_uiFileHandlers ), f_oDroppedFd( a_uiFileHandlers, dropped_fd_t::D_AUTO_GROW ),
	f_bCallbackContext( false ), f_oEvent()
	{
	M_PROLOG
	f_oDroppedFd.reset();
	M_ASSERT( f_oDroppedFd.is_empty() );
	memset ( & f_sLatency, 0, sizeof ( f_sLatency ) );
	FD_ZERO ( & f_xFileDescriptorSet );
	return;
	M_EPILOG
	}

HProcess::~HProcess( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

int HProcess::do_init( void )
	{
	M_PROLOG
	if ( f_bInitialised )
		M_THROW ( "you can initialise your main process only once, dumbass",
				errno );
	HSignalService& ss = HSignalServiceFactory::get_instance();
	HSignalService::HHandlerGeneric::ptr_t handler( new HSignalService::HHandlerExternal( this, &HProcess::handler_interrupt ) );
	ss.register_handler( SIGINT, handler );
	ss.register_handler( SIGHUP, handler );
	register_file_descriptor_handler( f_oEvent.get_reader_fd(), &HProcess::process_interrupt );
	f_bInitialised = true;
	return ( 0 );
	M_EPILOG
	}

int HProcess::init( int a_iLatencySeconds, int a_iLatencyMicroseconds )
	{
	M_PROLOG
	f_iLatencySeconds = a_iLatencySeconds;
	f_iLatencyMicroseconds = a_iLatencyMicroseconds;
	return ( 1 );
	M_EPILOG
	}

int HProcess::do_cleanup( void )
	{
	return ( 0 );
	}

int HProcess::register_file_descriptor_handler_internal( int a_iFileDescriptor,
		process_handler_filedes_t HANDLER )
	{
	M_PROLOG
	f_oFileDescriptorHandlers[ a_iFileDescriptor ] = HANDLER;
	return ( 0 );
	M_EPILOG
	}

void HProcess::unregister_file_descriptor_handler( int a_iFileDescriptor )
	{
	M_PROLOG
	if ( f_bCallbackContext )
		f_oDroppedFd.push_back( a_iFileDescriptor );
	else
		{
		bool fail = f_oFileDescriptorHandlers.remove( a_iFileDescriptor );
		M_ASSERT( ! fail );
		}
	return;
	M_EPILOG
	}

int HProcess::reconstruct_fdset( void )
	{
	M_PROLOG
	f_sLatency.tv_sec = f_iLatencySeconds;
	f_sLatency.tv_usec = f_iLatencyMicroseconds;
	FD_ZERO ( & f_xFileDescriptorSet );
	if ( ! f_oFileDescriptorHandlers.size() )
		return ( -1 );
/* FD_SET is a macro and first argument is evaluated twice ! */
	for ( process_filedes_map_t::iterator it = f_oFileDescriptorHandlers.begin(); it != f_oFileDescriptorHandlers.end(); ++ it )
		FD_SET ( it->key, & f_xFileDescriptorSet );
	return ( 0 );
	M_EPILOG
	}

int HProcess::run( void )
	{
	M_PROLOG
	int l_iError = 0;
	do_init();
	if ( ! f_bInitialised )
		M_THROW( _ ( "you have to call HProcess::init() first, dumbass" ), errno );
	if ( ! f_oFileDescriptorHandlers.size() )
		M_THROW( _ ( "there is no file descriptor to check activity on" ), errno );
	while ( f_bLoop )
		{
		f_bCallbackContext = true;
		handler_alert( 0 );
		reconstruct_fdset();
		if ( ( l_iError = ::select( FD_SETSIZE, &f_xFileDescriptorSet,
						NULL, NULL, &f_sLatency ) ) )
			{
			if ( ( l_iError < 0 ) && ( errno == EINTR ) )
				continue;
			if ( l_iError < 0 )
				M_THROW ( "select() returned", l_iError );
			for ( process_filedes_map_t::iterator it = f_oFileDescriptorHandlers.begin();
					it != f_oFileDescriptorHandlers.end(); ++ it )
				{
				if ( FD_ISSET( it->key, &f_xFileDescriptorSet ) )
					{
					static_cast<void>( ( this->*(it->value) )( it->key ) );
					f_iIdleCycles = 0;
					}
				}
			}
		else
			handler_idle( 0 );
		f_bCallbackContext = false;
		if ( ! f_oDroppedFd.is_empty() )
			{
			for ( dropped_fd_t::iterator it = f_oDroppedFd.begin(); it != f_oDroppedFd.end(); ++ it )
				unregister_file_descriptor_handler( *it );
			f_oDroppedFd.reset();
			}
		}
	do_cleanup();
	return ( 0 );
	M_EPILOG
	}

int HProcess::process_interrupt( int )
	{
	M_PROLOG
	int l_iSigNo = 0;
	f_oEvent.read( &l_iSigNo, sizeof ( l_iSigNo ) );
	if ( l_iSigNo == SIGINT )
		f_bLoop = false;
	else if ( l_iSigNo == SIGHUP )
		rc_file::reload_configuration();
	return ( 0 );
	M_EPILOG
	}

int HProcess::handler_interrupt( int a_iSigNo )
	{
	M_PROLOG
	f_oEvent.write( &a_iSigNo, sizeof ( a_iSigNo ) );
	return ( 1 );
	M_EPILOG
	}

int HProcess::handler_alert( int, void const* )
	{
	M_PROLOG
	return ( 0 );
	M_EPILOG
	}

int HProcess::handler_idle( int a_iCode, void const* )
	{
	M_PROLOG
	f_iIdleCycles ++;
	return ( a_iCode );
	M_EPILOG
	}

}

}

