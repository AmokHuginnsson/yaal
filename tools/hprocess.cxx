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
#include <csignal>
#include <unistd.h>
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

HProcess::HProcess( int noFileHandlers_, int latencySeconds_, int latencyMicroseconds_ )
	: _initialised( false ), _loop( true ), _idleCycles( 0 ),
	_latencySeconds( latencySeconds_ ), _latencyMicroseconds( latencyMicroseconds_ ),
	_latency(), _fileDescriptorSet(),
	_fileDescriptorHandlers( noFileHandlers_ ),
	_alert(), _idle(), _droppedFd( noFileHandlers_ ),
	_callbackContext( false ), _event(), _mutex()
	{
	M_PROLOG
	_droppedFd.clear();
	M_ASSERT( _droppedFd.is_empty() );
	::memset( &_latency, 0, sizeof ( _latency ) );
	FD_ZERO( &_fileDescriptorSet );
	HSignalService& ss = HSignalServiceFactory::get_instance();
	HSignalService::handler_t handler( call( &HProcess::handler_interrupt, this, _1 ) );
	if ( _debugLevel_ < DEBUG_LEVEL::GDB )
		ss.register_handler( SIGINT, handler );
	ss.register_handler( SIGHUP, handler );
	register_file_descriptor_handler( _event.get_reader_fd(), call( &HProcess::process_interrupt, this, _1 ) );
	return;
	M_EPILOG
	}

HProcess::~HProcess( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

int HProcess::register_file_descriptor_handler( int fileDescriptor_, process_filedes_handler_t HANDLER )
	{
	M_PROLOG
	HPair<process_filedes_map_t::iterator, bool> s( _fileDescriptorHandlers.insert( make_pair( fileDescriptor_, HANDLER ) ) );
	return ( s.second ? 0 : 1 );
	M_EPILOG
	}

void HProcess::unregister_file_descriptor_handler( int fileDescriptor_ )
	{
	M_PROLOG
	if ( _callbackContext )
		_droppedFd.push_back( fileDescriptor_ );
	else
		{
		bool fail( _fileDescriptorHandlers.erase( fileDescriptor_ ) != 1 );
		M_ASSERT( ! fail );
		}
	return;
	M_EPILOG
	}

int HProcess::reconstruct_fdset( void )
	{
	M_PROLOG
	_latency.tv_sec = _latencySeconds;
	_latency.tv_usec = _latencyMicroseconds;
	FD_ZERO( &_fileDescriptorSet );
	if ( ! _fileDescriptorHandlers.size() )
		return ( -1 );
/* FD_SET is a macro and first argument is evaluated twice ! */
	for ( process_filedes_map_t::iterator it = _fileDescriptorHandlers.begin(); it != _fileDescriptorHandlers.end(); ++ it )
		FD_SET( it->first, &_fileDescriptorSet );
	return ( 0 );
	M_EPILOG
	}

int HProcess::run( void )
	{
	M_PROLOG
	int error = 0;
	if ( ! _fileDescriptorHandlers.size() )
		M_THROW( _( "there is no file descriptor to check activity on" ), errno );
	while ( _loop )
		{
		_callbackContext = true;
		handle_alerts();
		reconstruct_fdset();
		if ( ( error = ::select( FD_SETSIZE, &_fileDescriptorSet,
						NULL, NULL, &_latency ) ) )
			{
			if ( ( error < 0 ) && ( errno == EINTR ) )
				continue;
			if ( error < 0 )
				M_THROW ( "select() returned", error );
			for ( process_filedes_map_t::iterator it = _fileDescriptorHandlers.begin();
					it != _fileDescriptorHandlers.end(); ++ it )
				{
				if ( FD_ISSET( it->first, &_fileDescriptorSet ) )
					{
					static_cast<void>( ( it->second( it->first ) ) );
					_idleCycles = 0;
					}
				}
			}
		else
			handle_idle();
		_callbackContext = false;
		if ( ! _droppedFd.is_empty() )
			{
			for ( dropped_fd_t::iterator it = _droppedFd.begin(); it != _droppedFd.end(); ++ it )
				unregister_file_descriptor_handler( *it );
			_droppedFd.clear();
			}
		}
	return ( 0 );
	M_EPILOG
	}

int HProcess::idle_cycles( void ) const
	{
	return ( _idleCycles );
	}

void HProcess::stop( void )
	{
	M_PROLOG
	HLock l( _mutex );
	int sigNo = SIGINT;
	M_ENSURE( _event.write( &sigNo, sizeof ( sigNo ) ) == sizeof ( sigNo ) );
	return;
	M_EPILOG
	}

int HProcess::handler_interrupt( int sigNo_ )
	{
	M_PROLOG
	HLock l( _mutex );
	M_ENSURE( _event.write( &sigNo_, sizeof ( sigNo_ ) ) == sizeof ( sigNo_ ) );
	return ( 1 );
	M_EPILOG
	}

void HProcess::process_interrupt( int )
	{
	M_PROLOG
	HLock l( _mutex );
	int sigNo = 0;
	M_ENSURE( _event.read( &sigNo, sizeof ( sigNo ) ) == sizeof ( sigNo ) );
	if ( sigNo == SIGINT )
		_loop = false;
	else if ( sigNo == SIGHUP )
		program_options_helper::reload_configuration();
	return;
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
		(*it)();
	return;
	M_EPILOG
	}

void HProcess::handle_idle( void )
	{
	M_PROLOG
	++ _idleCycles;
	for ( delayed_calls_t::iterator it( _idle.begin() ), endIt( _idle.end() ); it != endIt; ++ it )
		(*it)();
	return;
	M_EPILOG
	}

}

}

