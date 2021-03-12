/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstring>
#include <cstdio>
#include <csignal>
#include <poll.h>
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
using namespace yaal::hcore::system;

namespace yaal {

namespace tools {

static int const LOW_LATENCY_WARNING = 100;

HIODispatcher::HIOHandler::HIOHandler( stream_t const& stream_, callback_t const& callback_, yaal::hcore::system::IO_EVENT_TYPE ioEventType_ )
	: _stream( stream_ )
	, _callback( callback_ )
	, _reqIOEventType( ioEventType_ ) {
	return;
}

void HIODispatcher::HIOHandler::call( IO_EVENT_TYPE ioEventType_ ) {
	M_PROLOG
	_callback( _stream, ioEventType_ );
	return;
	M_EPILOG
}

HIODispatcher::HIOHandler::stream_id_t HIODispatcher::HIOHandler::id( void ) const {
	return ( _stream.raw() );
}

yaal::hcore::system::IO_EVENT_TYPE HIODispatcher::HIOHandler::req_io_event_type( void ) const {
	return ( _reqIOEventType );
}

bool HIODispatcher::HIOHandler::can_read_now( void ) const {
	return ( _stream->immediate_read_size() > 0 );
}

HIODispatcher::HIODispatcher( int noFileHandlers_, int latency_ )
	: _initialized( false )
	, _loop( true )
	, _idleCycles( 0 )
	, _latency( latency_ )
	, _handlers()
	, _refs()
	, _pollfds()
	, _valid( false )
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
		call( &HIODispatcher::process_interrupt, this, _1, _2 ),
		IO_EVENT_TYPE::READ
	);
	if ( _latency < LOW_LATENCY_WARNING ) {
		hcore::log( LOG_LEVEL::WARNING ) << "Low latency on IO Dispatcher!" << endl;
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
	HIOHandler::callback_t callback_,
	yaal::hcore::system::IO_EVENT_TYPE ioEventType_
) {
	M_PROLOG
	if ( stream_->data() == memory::INVALID_HANDLE ) {
		throw HIODispatcherException( "The stream is invalid." );
	}
	if ( _callbackContext ) {
		_newIOHandlers.push_back( new_io_handler_t( ioEventType_, stream_, callback_ ) );
	} else {
		M_ENSURE( _handlers.find( stream_.raw() ) == _handlers.end(), "Stream is already registered in the HIODispatcher." );
		_handlers.insert( make_pair( stream_.raw(), HIOHandler( stream_, callback_, ioEventType_ ) ) );
		_valid = false;
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
	io_handlers_t::iterator it( _handlers.find( id_ ) );
	M_ENSURE( it != _handlers.end(), "Stream is not registered in the HIODispatcher." );
	_handlers.erase( it );
	_valid = false;
	return;
	M_EPILOG
}

void HIODispatcher::reconstruct( void ) {
	M_PROLOG
	int handlersCount( static_cast<int>( _handlers.get_size() ) );
	if ( ! _valid ) {
		if ( ! _handlers.is_empty() ) {
			_pollfds.realloc( chunk_size<pollfd>( handlersCount ) );
			_refs.resize( handlersCount );
		}
		int i( 0 );
		pollfd* pfd( _pollfds.get<pollfd>() );
		for ( io_handlers_t::value_type& ioHandler : _handlers ) {
			_refs[i] = &ioHandler.second;
			pfd[i].fd = static_cast<int>( reinterpret_cast<int_native_t>( ioHandler.first->data() ) );
			IO_EVENT_TYPE reqIOEventType( ioHandler.second.req_io_event_type() );
			pfd[i].events = static_cast<int short>(
				( !! ( reqIOEventType & IO_EVENT_TYPE::READ ) ? ( POLLIN | POLLPRI ) : 0 )
				| ( !! ( reqIOEventType & IO_EVENT_TYPE::WRITE ) ? POLLOUT : 0 )
			);
			pfd[i].revents = 0;
			++ i;
		}
		_valid = true;
	} else {
		pollfd* pfd( _pollfds.get<pollfd>() );
		for ( int i( 0 ); i < handlersCount; ++ i ) {
			pfd[i].revents = 0;
		}
	}
	return;
	M_EPILOG
}

bool HIODispatcher::dispatch( dropped_io_handlers_t const& droppedIOHandlers_ ) {
	M_PROLOG
	bool active( false );
	pollfd* pfd( _pollfds.get<pollfd>() );
	for ( int i( 0 ), handlersCount( static_cast<int>( _handlers.get_size() ) ); i < handlersCount; ++ i ) {
		HIOHandler& ioHandler( *_refs[i] );
		yaal::hcore::system::IO_EVENT_TYPE res( IO_EVENT_TYPE::NONE );
		res |= ( pfd[i].revents & ( POLLIN | POLLPRI | POLLHUP ) ? IO_EVENT_TYPE::READ : IO_EVENT_TYPE::NONE );
		res |= ( pfd[i].revents & POLLOUT ? IO_EVENT_TYPE::WRITE : IO_EVENT_TYPE::NONE );
		if ( ( res != IO_EVENT_TYPE::NONE ) && ( droppedIOHandlers_.find( ioHandler.id() ) == droppedIOHandlers_.end() ) ) {
			ioHandler.call( res );
			active = true;
		}
	}
	return active;
	M_EPILOG
}

bool HIODispatcher::dispatch_pending_reads( dropped_io_handlers_t const& droppedIOHandlers_ ) {
	M_PROLOG
	bool active( false );
	for ( int i( 0 ), handlersCount( static_cast<int>( _handlers.get_size() ) ); i < handlersCount; ++ i ) {
		HIOHandler& ioHandler( *_refs[i] );
		if ( ioHandler.can_read_now() && ( droppedIOHandlers_.find( ioHandler.id() ) == droppedIOHandlers_.end() ) ) {
			ioHandler.call( IO_EVENT_TYPE::READ );
			active = true;
		}
	}
	return active;
	M_EPILOG
}

void HIODispatcher::run( void ) {
	M_PROLOG
	if ( _handlers.is_empty() ) {
		M_THROW( _( "there is no file descriptor to check activity on" ), errno );
	}
	while ( ! _isKilled_ && _loop ) {
		_callbackContext = true;
		handle_alerts();
		reconstruct();
		int pollResult( 0 );
		int latency( _latency );
		while ( dispatch_pending_reads( _droppedIOHandlers ) ) {
			_idleCycles = 0;
		}
		do {
			pollResult = ::poll( _pollfds.get<pollfd>(), static_cast<nfds_t>( _handlers.get_size() ), latency );
			if ( latency > 0 ) {
				-- latency;
			}
		} while ( ( pollResult == -1 ) && ( errno == EINTR ) );
		if ( pollResult > 0 ) {
			M_ENSURE( pollResult >= 0 );
			if ( dispatch( _droppedIOHandlers ) ) {
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
	_event.flush();
	return;
	M_EPILOG
}

int HIODispatcher::handler_interrupt( int sigNo_ ) {
	M_PROLOG
	HLock l( _mutex );
	M_ENSURE( _event.write( &sigNo_, static_cast<int>( sizeof ( sigNo_ ) ) ) == static_cast<int>( sizeof ( sigNo_ ) ) );
	_event.flush();
	return ( 1 );
	M_EPILOG
}

void HIODispatcher::process_interrupt( HIOHandler::stream_t& M_DEBUG_CODE( stream_ ), IO_EVENT_TYPE ) {
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

