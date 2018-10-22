/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstring>
#include <cstdio>
#include <csignal>
#include <libintl.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hiodispatcher.hxx"
#include "hcore/hprogramoptionshandler.hxx"
#include "hcore/system.hxx"
#include "hcore/hlog.hxx"
#include "hcore/algorithm.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

static int const LOW_LATENCY_WARNING = 100;

HIODispatcher::HIOHandler::HIOHandler( stream_t const& stream_, callback_t const& callback_ )
	: _stream( stream_ )
	, _callback( callback_ ) {
	return;
}

void HIODispatcher::HIOHandler::call( void ) {
	M_PROLOG
	_callback( _stream );
	return;
	M_EPILOG
}

HIODispatcher::HIODispatcher( int noFileHandlers_, int long latency_ )
	: _initialized( false )
	, _loop( true )
	, _idleCycles( 0 )
	, _latency( latency_ )
	, _select( chunk_size<int>( noFileHandlers_ ) * 2 ) /* * 2 because 1 for readers and 1 for writers */
	, _readers()
	, _writers()
	, _alert()
	, _idle()
	, _droppedIOHandlers( noFileHandlers_ )
	, _newIOHandlers( noFileHandlers_ )
	, _callbackContext( false )
	, _event()
	, _mutex() {
	M_PROLOG
	_droppedIOHandlers.clear();
	_newIOHandlers.clear();
	M_ASSERT( _droppedIOHandlers.is_empty() );
	M_ASSERT( _newIOHandlers.is_empty() );
	HSignalService& ss = HSignalService::get_instance();
	HSignalService::handler_t handler( call( &HIODispatcher::handler_interrupt, this, _1 ) );
	if ( _debugLevel_ < DEBUG_LEVEL::GDB ) {
		ss.register_handler( SIGINT, handler, this );
	}
	ss.register_handler( SIGHUP, handler, this );
	register_file_descriptor_handler(
		_event.out(),
		call( &HIODispatcher::process_interrupt, this, _1 )
	);
	if ( _latency < LOW_LATENCY_WARNING ) {
		log( LOG_LEVEL::WARNING ) << "Low latency on IO Dispatcher!" << endl;
	}
	return;
	M_EPILOG
}

HIODispatcher::~HIODispatcher( void ) {
	M_PROLOG
	HSignalService& ss = HSignalService::get_instance();
	ss.flush_handlers( this );
	return;
	M_DESTRUCTOR_EPILOG
}

void HIODispatcher::register_file_descriptor_handler(
	HIOHandler::stream_t stream_,
	HIOHandler::callback_t callback_, FD_TYPE fdType_
) {
	M_PROLOG
	if ( stream_->data() == memory::INVALID_HANDLE ) {
		throw HIODispatcherException( "The stream is invalid." );
	}
	if ( _callbackContext ) {
		_newIOHandlers.push_back( new_io_handler_t( fdType_, stream_, callback_ ) );
	} else {
		io_handlers_t& fds( fdType_ == FD_TYPE::READER ? _readers : _writers );
		M_ENSURE( fds.find( stream_->data() ) == fds.end() );
		fds.insert( make_pair( stream_->data(), HIOHandler( stream_, callback_ ) ) );
		_select.realloc( chunk_size<int>( _readers.get_size() + _writers.get_size() ) );
	}
	return;
	M_EPILOG
}

void HIODispatcher::unregister_file_descriptor_handler( yaal::hcore::HStreamInterface::ptr_t const& stream_ ) {
	M_PROLOG
	if ( _callbackContext ) {
		_droppedIOHandlers.insert( stream_->data() );
	} else {
		do_unregister_file_descriptor_handler( stream_->data() );
	}
	return;
	M_EPILOG
}

void HIODispatcher::do_unregister_file_descriptor_handler( HIOHandler::stream_id_t id_ ) {
	M_PROLOG
	io_handlers_t::iterator it( _readers.find( id_ ) );
	if ( it != _readers.end() ) {
		_readers.erase( it );
	} else {
		it = _writers.find( id_ );
		if ( it != _writers.end() ) {
			_writers.erase( it );
		} else {
			M_ASSERT( ! "Inconsistent internal HIODispatcher state."[0] );
		}
	}
	return;
	M_EPILOG
}

namespace {
int stream_to_fd( HIODispatcher::io_handlers_t::value_type& ioHandler_ ) {
	return ( static_cast<int>( reinterpret_cast<int_native_t>( ioHandler_.first ) ) );
}
}

void HIODispatcher::reconstruct_fdset( void ) {
	M_PROLOG
	if ( ! _readers.is_empty() ) {
		transform( _readers.begin(), _readers.end(), _select.get<int>(), stream_to_fd );
	}
	if ( ! _writers.is_empty() ) {
		transform( _writers.begin(), _writers.end(), _select.get<int>() + _readers.get_size(), stream_to_fd );
	}
/* FD_SET is a macro and first argument is evaluated twice ! */
	return;
	M_EPILOG
}

void HIODispatcher::run( void ) {
	M_PROLOG
	if ( _readers.is_empty() && _writers.is_empty() ) {
		M_THROW( _( "there is no file descriptor to check activity on" ), errno );
	}
	while ( ! _isKilled_ && _loop ) {
		_callbackContext = true;
		handle_alerts();
		reconstruct_fdset();
		int long wait( _latency );
		int nReaders( static_cast<int>( _readers.get_size() ) );
		int nWriters( static_cast<int>( _writers.get_size() ) );
		int* readers( _select.get<int>() );
		int* writers( _select.get<int>() + nReaders );
		int error( 0 );
		if ( ( error = system::wait_for_io( readers, nReaders, writers, nWriters, &wait ) ) ) {
			M_ENSURE( error >= 0 );
			for ( int i( 0 ); i < nReaders; ++ i ) {
				HIOHandler::stream_id_t id( reinterpret_cast<HIOHandler::stream_id_t>( readers[ i ] ) );
				if ( ( readers[ i ] != -1 ) && ( _droppedIOHandlers.find( id ) == _droppedIOHandlers.end() ) ) {
					_readers.at( id ).call();
					_idleCycles = 0;
				}
			}
			for ( int i( 0 ); i < nWriters; ++ i ) {
				HIOHandler::stream_id_t id( reinterpret_cast<HIOHandler::stream_id_t>( writers[ i ] ) );
				if ( ( writers[ i ] != -1 ) && ( _droppedIOHandlers.find( id ) == _droppedIOHandlers.end() ) ) {
					_writers.at( id ).call();
					_idleCycles = 0;
				}
			}
		} else {
			handle_idle();
		}
		_callbackContext = false;
		if ( ! _newIOHandlers.is_empty() ) {
			for (
				new_io_handlers_t::const_iterator it( _newIOHandlers.begin() ), end( _newIOHandlers.end() );
				it != end;
				++ it
			) {
				register_file_descriptor_handler( it->get<1>(), it->get<2>(), it->get<0>() );
			}
			_newIOHandlers.clear();
		}
		if ( ! _droppedIOHandlers.is_empty() ) {
			for_each( _droppedIOHandlers.begin(), _droppedIOHandlers.end(), call( &HIODispatcher::do_unregister_file_descriptor_handler, this, _1 ) );
			_droppedIOHandlers.clear();
		}
	}
	return;
	M_EPILOG
}

int HIODispatcher::idle_cycles( void ) const {
	return ( _idleCycles );
}

void HIODispatcher::stop( void ) {
	M_PROLOG
	HLock l( _mutex );
	int sigNo = SIGINT;
	M_ENSURE( _event.write( &sigNo, static_cast<int>( sizeof ( sigNo ) ) ) == static_cast<int>( sizeof ( sigNo ) ) );
	return;
	M_EPILOG
}

int HIODispatcher::handler_interrupt( int sigNo_ ) {
	M_PROLOG
	HLock l( _mutex );
	M_ENSURE( _event.write( &sigNo_, static_cast<int>( sizeof ( sigNo_ ) ) ) == static_cast<int>( sizeof ( sigNo_ ) ) );
	return ( 1 );
	M_EPILOG
}

void HIODispatcher::process_interrupt( HIOHandler::stream_t& M_DEBUG_CODE( stream_ ) ) {
	M_PROLOG
	HLock l( _mutex );
	M_ASSERT( stream_ == _event.out() );
	int sigNo = 0;
	M_ENSURE( _event.read( &sigNo, static_cast<int>( sizeof ( sigNo ) ) ) == static_cast<int>( sizeof ( sigNo ) ) );
	if ( sigNo == SIGINT ) {
		_loop = false;
	} else if ( sigNo == SIGHUP ) {
		program_options_helper::reload_configuration();
	}
	return;
	M_EPILOG
}

void HIODispatcher::add_idle_handle( delayed_call_t call_ ) {
	M_PROLOG
	_idle.push_back( call_ );
	return;
	M_EPILOG
}

void HIODispatcher::add_alert_handle( delayed_call_t call_ ) {
	M_PROLOG
	_alert.push_back( call_ );
	return;
	M_EPILOG
}

void HIODispatcher::handle_alerts( void ) {
	M_PROLOG
	for ( delayed_calls_t::iterator it( _alert.begin() ), endIt( _alert.end() ); it != endIt; ++ it ) {
		(*it)();
	}
	return;
	M_EPILOG
}

void HIODispatcher::handle_idle( void ) {
	M_PROLOG
	++ _idleCycles;
	for ( delayed_calls_t::iterator it( _idle.begin() ), endIt( _idle.end() ); it != endIt; ++ it ) {
		(*it)();
	}
	return;
	M_EPILOG
}

}

}

