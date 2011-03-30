/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hiodispatcher.cxx - this file is integral part of `yaal' project.

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
#include <libintl.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hiodispatcher.hxx"
#include "hcore/hprogramoptionshandler.hxx"
#include "hcore/system.hxx"
#include "hcore/hlog.hxx"
#include "hcore/algorithm.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal
{

namespace tools
{

static int const LOW_LATENCY_WARNING = 100; 

HIODispatcher::HIODispatcher( int noFileHandlers_, int long latency_ )
	: _initialised( false ), _loop( true ), _idleCycles( 0 ),
	_latency( latency_ ),
	_select( chunk_size<int>( noFileHandlers_ ) * 2 ), /* * 2 because 1 for readers and 1 for writters */
	_readers(), _writers(),
	_alert(), _idle(), _droppedFd( noFileHandlers_ ), _newIOHandlers( noFileHandlers_ ),
	_callbackContext( false ), _event(), _mutex()
	{
	M_PROLOG
	_droppedFd.clear();
	_newIOHandlers.clear();
	M_ASSERT( _droppedFd.is_empty() );
	M_ASSERT( _newIOHandlers.is_empty() );
	HSignalService& ss = HSignalService::get_instance();
	HSignalService::handler_t handler( call( &HIODispatcher::handler_interrupt, this, _1 ) );
	if ( _debugLevel_ < DEBUG_LEVEL::GDB )
		ss.register_handler( SIGINT, handler, this );
	ss.register_handler( SIGHUP, handler, this );
	register_file_descriptor_handler( _event.get_reader_fd(), call( &HIODispatcher::process_interrupt, this, _1 ) );
	if ( _latency < LOW_LATENCY_WARNING )
		log( LOG_TYPE::WARNING ) << "Low latency on IO Dispatcher!" << endl;
	return;
	M_EPILOG
	}

HIODispatcher::~HIODispatcher( void )
	{
	M_PROLOG
	HSignalService& ss = HSignalService::get_instance();
	ss.flush_handlers( this );
	return;
	M_DESTRUCTOR_EPILOG
	}

void HIODispatcher::register_file_descriptor_handler( int fileDescriptor_, call_fd_t HANDLER, FD_TYPE::fd_type_t const& fdType_ )
	{
	M_PROLOG
	if ( _callbackContext )
		_newIOHandlers.push_back( new_io_handler_t( fdType_, fileDescriptor_, HANDLER ) );
	else
		{
		io_handlers_t& fds( fdType_ == FD_TYPE::READER ? _readers : _writers );
		io_handlers_t::iterator it( find_if( fds.begin(), fds.end(),
					compose1( bind1st( equal_to<int>(), fileDescriptor_ ), select1st<io_handlers_t::value_type>() ) ) );
		M_ENSURE( it == fds.end() );
		fds.push_back( make_pair( fileDescriptor_, HANDLER ) );
		_select.realloc( chunk_size<int>( _readers.get_size() + _writers.get_size() ) );
		}
	return;
	M_EPILOG
	}

void HIODispatcher::unregister_file_descriptor_handler( int fileDescriptor_ )
	{
	M_PROLOG
	if ( _callbackContext )
		_droppedFd.push_back( fileDescriptor_ );
	else
		{
		io_handlers_t::iterator it( find_if( _readers.begin(), _readers.end(),
					compose1( bind1st( equal_to<int>(), fileDescriptor_ ), select1st<io_handlers_t::value_type>() ) ) );
		bool fail( false );
		if ( it != _readers.end() )
			_readers.erase( it );
		else
			{
			it = find_if( _writers.begin(), _writers.end(),
					compose1( bind1st( equal_to<int>(), fileDescriptor_ ), select1st<io_handlers_t::value_type>() ) );
			if ( it != _writers.end() )
				_writers.erase( it );
			else
				fail = true;
			}
		M_ASSERT( ! fail );
		}
	return;
	M_EPILOG
	}

int HIODispatcher::reconstruct_fdset( void )
	{
	M_PROLOG
	if ( _readers.is_empty() && _writers.is_empty() )
		return ( -1 );
	transform( _readers.begin(), _readers.end(), _select.get<int>(), select1st<io_handlers_t::value_type>() );
	transform( _writers.begin(), _writers.end(), _select.get<int>() + _readers.get_size(), select1st<io_handlers_t::value_type>() );
/* FD_SET is a macro and first argument is evaluated twice ! */
	return ( 0 );
	M_EPILOG
	}

void HIODispatcher::run( void )
	{
	M_PROLOG
	if ( _readers.is_empty() && _writers.is_empty() )
		M_THROW( _( "there is no file descriptor to check activity on" ), errno );
	while ( ! _isKilled_ && _loop )
		{
		_callbackContext = true;
		handle_alerts();
		reconstruct_fdset();
		int long wait( _latency );
		int nReaders( static_cast<int>( _readers.get_size() ) );
		int nWriters( static_cast<int>( _writers.get_size() ) );
		int* readers( _select.get<int>() );
		int* writers( _select.get<int>() + nReaders );
		int error( 0 );
		if ( ( error = system::wait_for_io( readers, nReaders, writers, nWriters, &wait ) ) )
			{
			M_ENSURE( error >= 0 );
			for ( int i( 0 ); i < nReaders; ++ i )
				{
				if ( readers[ i ] != -1 )
					{
					io_handler_t& h( _readers[ i ] );
					static_cast<void>( h.second( h.first ) );
					_idleCycles = 0;
					}
				}
			for ( int i( 0 ); i < nWriters; ++ i )
				{
				if ( writers[ i ] != -1 )
					{
					io_handler_t& h( _writers[ i ] );
					static_cast<void>( h.second( h.first ) );
					_idleCycles = 0;
					}
				}
			}
		else
			handle_idle();
		_callbackContext = false;
		if ( ! _droppedFd.is_empty() )
			{
			for_each( _droppedFd.begin(), _droppedFd.end(), call( &HIODispatcher::unregister_file_descriptor_handler, this, _1 ) );
			_droppedFd.clear();
			}
		if ( ! _newIOHandlers.is_empty() )
			{
			for ( new_io_handlers_t::const_iterator it( _newIOHandlers.begin() ),
					end( _newIOHandlers.end() ); it != end; ++ it )
				register_file_descriptor_handler( it->get<1>(), it->get<2>(), it->get<0>() );
			_newIOHandlers.clear();
			}
		}
	return;
	M_EPILOG
	}

int HIODispatcher::idle_cycles( void ) const
	{
	return ( _idleCycles );
	}

void HIODispatcher::stop( void )
	{
	M_PROLOG
	HLock l( _mutex );
	int sigNo = SIGINT;
	M_ENSURE( _event.write( &sigNo, sizeof ( sigNo ) ) == sizeof ( sigNo ) );
	return;
	M_EPILOG
	}

int HIODispatcher::handler_interrupt( int sigNo_ )
	{
	M_PROLOG
	HLock l( _mutex );
	M_ENSURE( _event.write( &sigNo_, sizeof ( sigNo_ ) ) == sizeof ( sigNo_ ) );
	return ( 1 );
	M_EPILOG
	}

void HIODispatcher::process_interrupt( int )
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

void HIODispatcher::add_idle_handle( delayed_call_t call_ )
	{
	M_PROLOG
	_idle.push_back( call_ );
	return;
	M_EPILOG
	}

void HIODispatcher::add_alert_handle( delayed_call_t call_ )
	{
	M_PROLOG
	_alert.push_back( call_ );
	return;
	M_EPILOG
	}

void HIODispatcher::handle_alerts( void )
	{
	M_PROLOG
	for ( delayed_calls_t::iterator it( _alert.begin() ), endIt( _alert.end() ); it != endIt; ++ it )
		(*it)();
	return;
	M_EPILOG
	}

void HIODispatcher::handle_idle( void )
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

