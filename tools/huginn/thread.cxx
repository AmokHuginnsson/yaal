/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "thread.hxx"
#include "runtime.hxx"
#include "function.hxx"
#include "keyword.hxx"
#include "objectfactory.hxx"
#include "exception.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace huginn {

HThread::HThread( HRuntime* runtime_, yaal::hcore::HThread::id_t id_ )
	: _frames()
	, _currentFrame( nullptr )
	, _frameCount( 0 )
	, _state( STATE::NORMAL )
	, _breakLevel( 0 )
	, _id( id_ )
	, _runtime( runtime_ )
	, _result()
	, _valueCache()
	, _objectFactory( *_runtime->object_factory() )
	, _exception() {
	_valueCache.reserve( _runtime->max_local_variable_count() );
	return;
}

HThread::~HThread( void ) {
	M_PROLOG
	/*
	 * If we have any frames it means that execution was stopped by C++ exception.
	 * We need to manually unwind stack and while doing it we need to make sure
	 * frames are in consistent state.
	 */
	HFrame const* incrementalFrame( _runtime->incremental_frame().raw() );
	while ( _currentFrame ) {
		_currentFrame->cleanup();
		if ( _currentFrame != incrementalFrame ) {
			pop_frame();
		} else {
			pop_incremental_frame();
		}
	}
	return;
	M_DESTRUCTOR_EPILOG
}

void HThread::add_frame( void ) {
	if ( _frameCount == static_cast<int>( _frames.get_size() ) ) {
		_frames.emplace_back( make_pointer<HFrame>( this, _currentFrame ) );
	}
	_currentFrame = _frames[_frameCount].raw();
	++ _frameCount;
}

void HThread::create_frame( HStatement const* statement_, HHuginn::value_t* object_, int upCast_ ) {
	M_PROLOG
	if ( call_stack_size() >= _runtime->max_call_stack_size() ) {
		throw HHuginn::HHuginnRuntimeException( "Call stack size limit exceeded: "_ys.append( call_stack_size() + 1 ), _currentFrame->file_id(), _currentFrame->position() );
	}
	add_frame();
	_currentFrame->init( statement_, object_, upCast_ );
	return;
	M_EPILOG
}

void HThread::create_incremental_frame( HStatement const* statement_, HHuginn::value_t* object_, int upCast_ ) {
	M_PROLOG
	frame_t incrementalFrame( _runtime->incremental_frame() );
	M_ASSERT( _frameCount == 0 );
	M_ASSERT( _frames.get_size() == 0 );
	if ( !! incrementalFrame ) {
		_frames.emplace_back( incrementalFrame );
		_currentFrame = incrementalFrame.raw();
		++ _frameCount;
	} else {
		add_frame();
	}
	_runtime->set_incremental_frame( _frames.back() );
	_currentFrame->set_thread( this );
	_currentFrame->init( statement_, object_, upCast_ );
	return;
	M_EPILOG
}

void HThread::pop_frame( void ) {
	M_PROLOG
	/*
	 * Order of those lines matter and is extremely non-trivial!
	 *
	 * When removing a frame it is possible that
	 * non-trivial destructor of user class is invoked.
	 * This destructor body will create another frame on top of currently
	 * removed frame.
	 * So we need first to reset currently removed frame (invoking non-trivial
	 * destructors in the process) and only then we update _currentFrame marker.
	 */
	M_ASSERT( _currentFrame );
	HFrame* parent( _currentFrame->parent() );
	_currentFrame->reset();
	_currentFrame = parent;
	-- _frameCount;
	return;
	M_EPILOG
}

void HThread::pop_incremental_frame( void ) {
	M_PROLOG
	M_ASSERT( _currentFrame );
	_currentFrame = _currentFrame->parent();
	-- _frameCount;
	return;
	M_EPILOG
}

void HThread::state_throw( HHuginn::value_t&& value_, int fileId_, int position_ ) {
	M_PROLOG
	_state = STATE::EXCEPTION;
	_result = yaal::move( value_ );
	_exception._trace = _runtime->get_call_stack( this );
	_exception._fileId = fileId_;
	_exception._position = position_;
	return;
	M_EPILOG
}

void HThread::clean_exception( void ) {
	M_PROLOG
	M_ASSERT( _state == STATE::EXCEPTION );
	_result.reset();
	_exception._fileId = INVALID_FILE_ID;
	_exception._position = 0;
	_exception._message.clear();
	_state = STATE::NORMAL;
	return;
	M_EPILOG
}

void HThread::flush_uncaught_exception( char const* exMsg_ ) {
	M_PROLOG
	if ( _state != STATE::EXCEPTION ) {
		return;
	}
	/*
	 * Uncaught STATE::EXCEPTION becomes STATE::RUNTIME_EXCEPTION!
	 */
	huginn::HException const* e( dynamic_cast<huginn::HException const*>( _result.raw() ) );
	_exception._message.assign( "Uncaught " );
	_state = HThread::STATE::RUNTIME_EXCEPTION;
	if ( e ) {
		_exception._message
			.append( e->get_class()->name() )
			.append( exMsg_ )
			.append( ": " )
			.append( e->message() );
	} else {
		_exception._message
			.append( "exception" )
			.append( exMsg_ )
			.append( " of type " )
			.append( _result->get_class()->name() )
			.append( "." );
	}
	return;
	M_EPILOG
}

void HThread::raise( HClass const* class_, yaal::hcore::HString const& message_, int position_ ) {
	M_PROLOG
	HFrame* f( current_frame() );
	f->set_position( position_ );
	HHuginn::value_t e( _runtime->object_factory()->create<HException>( this, class_, message_ ) );
	int fileId( f->file_id() );
	state_throw( yaal::move( e ), fileId, position_ );
	return;
	M_EPILOG
}

void HThread::set_exception( yaal::hcore::HString const& message_, int fileId_, int position_ ) {
	M_PROLOG
	_exception._message = message_;
	_exception._fileId = fileId_;
	_exception._position = position_;
	return;
	M_EPILOG
}

bool HThread::has_exception( void ) const {
	return ( _state == HThread::STATE::EXCEPTION );
}

bool HThread::has_runtime_exception( void ) const {
	/* Needed by ~HObject() from DTOR_FIX. */
	return ( _state == HThread::STATE::RUNTIME_EXCEPTION );
}

void HThread::flush_runtime_exception( void ) {
	M_PROLOG
	if ( ! has_runtime_exception() ) {
		flush_uncaught_exception();
	}
	if ( has_runtime_exception() ) {
		int fileId( _exception._fileId );
		_exception._fileId = INVALID_FILE_ID;
		int position( _exception._position );
		_exception._position = 0;
		hcore::HString message( yaal::move( _exception._message ) );
		throw HHuginn::HHuginnRuntimeException( message, fileId, position );
	}
	M_EPILOG
}

}

}

}

