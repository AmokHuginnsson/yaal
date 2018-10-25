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

HIODispatcher::HIOHandler::stream_id_t HIODispatcher::HIOHandler::id( void ) const {
	return ( _stream.raw() );
}

HIODispatcher::HIOHandlerSet::HIOHandlerSet( int noFileHandlers_ )
	: _handlers()
	, _refs()
	, _select( chunk_size<int>( noFileHandlers_ * 2 ) )
	, _valid( false ) {
	return;
}

void HIODispatcher::HIOHandlerSet::add( stream_t const& stream_, callback_t const& callback_ ) {
	M_PROLOG
	M_ENSURE( _handlers.find( stream_.raw() ) == _handlers.end() );
	_handlers.insert( make_pair( stream_.raw(), HIOHandler( stream_, callback_ ) ) );
	_valid = false;
	return;
	M_EPILOG
}

bool HIODispatcher::HIOHandlerSet::remove( HIOHandler::stream_id_t id_ ) {
	M_PROLOG
	io_handlers_t::iterator it( _handlers.find( id_ ) );
	bool found( it != _handlers.end() );
	if ( found ) {
		_handlers.erase( it );
		_valid = false;
	}
	return ( found );
	M_EPILOG
}

void HIODispatcher::HIOHandlerSet::reconstruct( void ) {
	M_PROLOG
	int handlersCount( static_cast<int>( _handlers.get_size() ) );
	if ( ! _valid ) {
		if ( ! _handlers.is_empty() ) {
			_select.realloc( chunk_size<int>( handlersCount * 2 ) );
			_refs.resize( _handlers.get_size() );
		}
		int i( 0 );
		int* fds( static_cast<int*>( _select.get<int>() ) );
		for ( io_handlers_t::value_type& ioHandler : _handlers ) {
			_refs[i] = &ioHandler.second;
			fds[i] = static_cast<int>( reinterpret_cast<int_native_t>( ioHandler.first->data() ) );
			++ i;
		}
		::memcpy( fds + handlersCount, fds, static_cast<size_t>( handlersCount ) * sizeof ( int ) );
		_valid = true;
	} else {
		int* fds( static_cast<int*>( _select.get<int>() ) );
		::memcpy( fds, fds + handlersCount, static_cast<size_t>( handlersCount ) * sizeof ( int ) );
	}
	return;
	M_EPILOG
}

bool HIODispatcher::HIOHandlerSet::dispatch( dropped_io_handlers_t const& droppedIOHandlers_ ) {
	M_PROLOG
	bool active( false );
	int* fds( static_cast<int*>( _select.get<int>() ) );
	for ( int i( 0 ), handlersCount( count() ); i < handlersCount; ++ i ) {
		HIOHandler& ioHandler( *_refs[i] );
		if ( ( fds[ i ] != -1 ) && ( droppedIOHandlers_.find( ioHandler.id() ) == droppedIOHandlers_.end() ) ) {
			ioHandler.call();
			active = true;
		}
	}
	return ( active );
	M_EPILOG
}

void* HIODispatcher::HIOHandlerSet::select( void ) {
	return ( _select.raw() );
}

int HIODispatcher::HIOHandlerSet::count( void ) const {
	return ( static_cast<int>( _handlers.get_size() ) );
}

HIODispatcher::HIODispatcher( int noFileHandlers_, int long latency_ )
	: _initialized( false )
	, _loop( true )
	, _idleCycles( 0 )
	, _latency( latency_ )
	, _readers( noFileHandlers_ )
	, _writers( noFileHandlers_ )
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
		if ( fdType_ == FD_TYPE::READER ) {
			_readers.add( stream_, callback_ );
		} else {
			_writers.add( stream_, callback_ );
		}
	}
	return;
	M_EPILOG
}

void HIODispatcher::unregister_file_descriptor_handler( yaal::hcore::HStreamInterface::ptr_t const& stream_ ) {
	M_PROLOG
	if ( _callbackContext ) {
		_droppedIOHandlers.insert( stream_.raw() );
	} else {
		do_unregister_file_descriptor_handler( stream_.raw() );
	}
	return;
	M_EPILOG
}

void HIODispatcher::do_unregister_file_descriptor_handler( HIOHandler::stream_id_t id_ ) {
	M_PROLOG
	if ( _readers.remove( id_ ) ) {
	} else if ( _writers.remove( id_ ) ) {
	} else {
		M_ASSERT( ! "Inconsistent internal HIODispatcher state."[0] );
	}
	return;
	M_EPILOG
}

void HIODispatcher::run( void ) {
	M_PROLOG
	if ( ( _readers.count() == 0 ) && ( _writers.count() == 0 ) ) {
		M_THROW( _( "there is no file descriptor to check activity on" ), errno );
	}
	while ( ! _isKilled_ && _loop ) {
		_callbackContext = true;
		handle_alerts();
		int long wait( _latency );
		_readers.reconstruct();
		_writers.reconstruct();
		int selectResult(
			system::wait_for_io(
				static_cast<int*>( _readers.select() ), _readers.count(),
				static_cast<int*>( _writers.select() ), _writers.count(),
				&wait
			)
		);
		if ( selectResult ) {
			M_ENSURE( selectResult >= 0 );
			if ( _readers.dispatch( _droppedIOHandlers ) ) {
				_idleCycles = 0;
			}
			if ( _writers.dispatch( _droppedIOHandlers ) ) {
				_idleCycles = 0;
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

