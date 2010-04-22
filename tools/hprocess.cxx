/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hprocess.cxx - this file is integral part of `yaal' project.

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

#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <signal.h>
#include <libintl.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hprocess.hxx"
#include "hcore/hprogramoptionshandler.hxx"

using namespace yaal::hcore;

namespace yaal
{

namespace tools
{

HProcess::HProcess( int noFileHandlers_, int a_iLatencySeconds, int a_iLatencyMicroseconds )
	: f_bInitialised( false ), f_bLoop( true ), f_iIdleCycles( 0 ),
	f_iLatencySeconds( a_iLatencySeconds ), f_iLatencyMicroseconds( a_iLatencyMicroseconds ),
	f_sLatency(), f_xFileDescriptorSet(),
	f_oFileDescriptorHandlers( noFileHandlers_ ),
	_alert(), _idle(), f_oDroppedFd( noFileHandlers_ ),
	f_bCallbackContext( false ), f_oEvent()
	{
	M_PROLOG
	f_oDroppedFd.clear();
	M_ASSERT( f_oDroppedFd.is_empty() );
	::memset( &f_sLatency, 0, sizeof ( f_sLatency ) );
	FD_ZERO( &f_xFileDescriptorSet );
	HSignalService& ss = HSignalServiceFactory::get_instance();
	HSignalService::HHandlerGeneric::ptr_t handler( new HSignalService::HHandlerExternal( this, &HProcess::handler_interrupt ) );
	if ( n_iDebugLevel < DEBUG_LEVEL::GDB )
		ss.register_handler( SIGINT, handler );
	ss.register_handler( SIGHUP, handler );
	register_file_descriptor_handler( f_oEvent.get_reader_fd(), bound_call( &HProcess::process_interrupt, this, _1 ) );
	return;
	M_EPILOG
	}

HProcess::~HProcess( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

int HProcess::register_file_descriptor_handler( int a_iFileDescriptor, process_filedes_handler_t HANDLER )
	{
	M_PROLOG
	HPair<process_filedes_map_t::iterator, bool> s( f_oFileDescriptorHandlers.insert( make_pair( a_iFileDescriptor, HANDLER ) ) );
	return ( s.second ? 0 : 1 );
	M_EPILOG
	}

void HProcess::unregister_file_descriptor_handler( int a_iFileDescriptor )
	{
	M_PROLOG
	if ( f_bCallbackContext )
		f_oDroppedFd.push_back( a_iFileDescriptor );
	else
		{
		bool fail( f_oFileDescriptorHandlers.erase( a_iFileDescriptor ) == 1 );
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
	FD_ZERO( &f_xFileDescriptorSet );
	if ( ! f_oFileDescriptorHandlers.size() )
		return ( -1 );
/* FD_SET is a macro and first argument is evaluated twice ! */
	for ( process_filedes_map_t::iterator it = f_oFileDescriptorHandlers.begin(); it != f_oFileDescriptorHandlers.end(); ++ it )
		FD_SET( it->first, &f_xFileDescriptorSet );
	return ( 0 );
	M_EPILOG
	}

int HProcess::run( void )
	{
	M_PROLOG
	int l_iError = 0;
	if ( ! f_oFileDescriptorHandlers.size() )
		M_THROW( _( "there is no file descriptor to check activity on" ), errno );
	while ( f_bLoop )
		{
		f_bCallbackContext = true;
		handle_alerts();
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
				if ( FD_ISSET( it->first, &f_xFileDescriptorSet ) )
					{
					static_cast<void>( ( it->second->invoke( it->first ) ) );
					f_iIdleCycles = 0;
					}
				}
			}
		else
			handle_idle();
		f_bCallbackContext = false;
		if ( ! f_oDroppedFd.is_empty() )
			{
			for ( dropped_fd_t::iterator it = f_oDroppedFd.begin(); it != f_oDroppedFd.end(); ++ it )
				unregister_file_descriptor_handler( *it );
			f_oDroppedFd.clear();
			}
		}
	return ( 0 );
	M_EPILOG
	}

void HProcess::stop( void )
	{
	f_bLoop = false;
	return;
	}

int HProcess::idle_cycles( void ) const
	{
	return ( f_iIdleCycles );
	}

void HProcess::process_interrupt( int )
	{
	M_PROLOG
	int l_iSigNo = 0;
	f_oEvent.read( &l_iSigNo, sizeof ( l_iSigNo ) );
	if ( l_iSigNo == SIGINT )
		f_bLoop = false;
	else if ( l_iSigNo == SIGHUP )
		program_options_helper::reload_configuration();
	return;
	M_EPILOG
	}

int HProcess::handler_interrupt( int a_iSigNo )
	{
	M_PROLOG
	f_oEvent.write( &a_iSigNo, sizeof ( a_iSigNo ) );
	return ( 1 );
	M_EPILOG
	}

void HProcess::add_idle_handle( delayed_call_t call_ )
	{
	M_PROLOG
	_idle.push_back( call_ );
	return;
	M_EPILOG
	}

void HProcess::add_alert_handle( delayed_call_t call_ )
	{
	M_PROLOG
	_alert.push_back( call_ );
	return;
	M_EPILOG
	}

void HProcess::handle_alerts( void )
	{
	M_PROLOG
	for ( delayed_calls_t::iterator it( _alert.begin() ), endIt( _alert.end() ); it != endIt; ++ it )
		(*it)->invoke();
	return;
	M_EPILOG
	}

void HProcess::handle_idle( void )
	{
	M_PROLOG
	++ f_iIdleCycles;
	for ( delayed_calls_t::iterator it( _idle.begin() ), endIt( _idle.end() ); it != endIt; ++ it )
		(*it)->invoke();
	return;
	M_EPILOG
	}

}

}

