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
	, _functionFrameCount( 0 )
	, _id( id_ )
	, _runtime( runtime_ )
	, _valueCache()
	, _objectFactory( *_runtime->object_factory() )
	, _exceptionMessage()
	, _exceptionFileId( INVALID_FILE_ID )
	, _exceptionPosition( 0 )
	, _trace() {
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

void HThread::create_function_frame( HStatement const* statement_, HHuginn::value_t* object_, int upCast_ ) {
	M_PROLOG
	if ( call_stack_size() >= _runtime->max_call_stack_size() ) {
		throw HHuginn::HHuginnRuntimeException( "Call stack size limit exceeded: "_ys.append( call_stack_size() + 1 ), _currentFrame->file_id(), _currentFrame->position() );
	}
	add_frame();
	++ _functionFrameCount;
	_currentFrame->init( HFrame::TYPE::FUNCTION, statement_, object_, upCast_ );
	return;
	M_EPILOG
}

void HThread::create_incremental_function_frame( HStatement const* statement_, HHuginn::value_t* object_, int upCast_ ) {
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
	_currentFrame->init( HFrame::TYPE::FUNCTION, statement_, object_, upCast_ );
	return;
	M_EPILOG
}

void HThread::create_loop_frame( HStatement const* statement_ ) {
	M_PROLOG
	add_frame();
	_currentFrame->init( HFrame::TYPE::LOOP, statement_ );
	return;
	M_EPILOG
}

void HThread::create_scope_frame( HStatement const* statement_ ) {
	M_PROLOG
	add_frame();
	_currentFrame->init( HFrame::TYPE::SCOPE, statement_ );
	return;
	M_EPILOG
}

void HThread::create_try_catch_frame( HStatement const* statement_ ) {
	M_PROLOG
	add_frame();
	_currentFrame->init( HFrame::TYPE::TRY_CATCH, statement_ );
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
	if ( _currentFrame->type() == HFrame::TYPE::FUNCTION ) {
		-- _functionFrameCount;
	}
	_currentFrame->reset();
	_currentFrame = _currentFrame->parent();
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

void HThread::break_execution( HFrame::STATE state_, HHuginn::value_t&& value_, int level_, int fileId_, int position_ ) {
	M_PROLOG
	M_ASSERT( state_ != HFrame::STATE::NORMAL );
	int level( 0 );
	HFrame* target( current_frame() );
	int no( target->number() );
	char const* exMsg = "";
	while ( target ) {
		if ( target->is_loop() ) {
			++ level;
		}
		target->break_execution( state_ );
		HFrame* parent( target->parent() );
		if ( ! parent ) {
			if ( ( state_ == HFrame::STATE::EXCEPTION ) || ( state_ == HFrame::STATE::RUNTIME_EXCEPTION ) ) {
				target = nullptr;
			} else {
				M_ASSERT( ( state_ == HFrame::STATE::RETURN ) || ( state_ == HFrame::STATE::EXIT ) );
			}
			break;
		} else if ( ( state_ == HFrame::STATE::RETURN ) && ( parent->number() != no ) ) {
			break;
		} else if ( ( state_ == HFrame::STATE::BREAK ) && ( level > level_ ) ) {
			break;
		} else if ( ( state_ == HFrame::STATE::CONTINUE ) && ( level > 0 ) ) {
			break;
		} else if ( state_ == HFrame::STATE::EXCEPTION ) {
			if ( target->has_catch() ) {
				break;
			} else if (
				( target->type() == HFrame::TYPE::FUNCTION )
				&& ( static_cast<HFunction const*>( target->statement() )->name() == IDENTIFIER::KEYWORD::DESTRUCTOR )
			) {
				exMsg = " from destructor";
				while ( target ) {
					target->break_execution( HFrame::STATE::RUNTIME_EXCEPTION ); /* ---> DTOR_FIX <--- */
					target = target->parent();
				}
				break;
			}
		} else if ( ( state_ == HFrame::STATE::EXIT ) && ! parent ) {
			break;
		}
		target = parent;
	}
	if ( target ) {
		target->set_result( yaal::move( value_ ) );
	} else if ( state_ == HFrame::STATE::EXCEPTION ) {
		/*
		 * Uncaught STATE::EXCEPTION becomes STATE::RUNTIME_EXCEPTION!
		 */
		huginn::HException const* e( dynamic_cast<huginn::HException const*>( value_.raw() ) );
		_exceptionFileId = fileId_;
		_exceptionPosition = position_;
		_exceptionMessage.assign( "Uncaught " );
		_trace = _runtime->get_call_stack( this );
		if ( e ) {
			_exceptionMessage
				.append( e->get_class()->name() )
				.append( exMsg )
				.append( ": " )
				.append( e->message() );
		} else {
			_exceptionMessage
				.append( "exception" )
				.append( exMsg )
				.append( " of type " )
				.append( value_->get_class()->name() )
				.append( "." );
		}
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
	break_execution( HFrame::STATE::EXCEPTION, yaal::move( e ), 0, fileId, position_ );
	return;
	M_EPILOG
}

void HThread::set_exception( yaal::hcore::HString const& message_, int fileId_, int position_ ) {
	M_PROLOG
	_exceptionMessage = message_;
	_exceptionFileId = fileId_;
	_exceptionPosition = position_;
	return;
	M_EPILOG
}

bool HThread::has_exception( void ) const {
	M_ASSERT( current_frame() );
	return ( current_frame()->state() == HFrame::STATE::EXCEPTION );
}

bool HThread::has_runtime_exception( void ) const {
	return (
		( _currentFrame && _currentFrame->state() == HFrame::STATE::RUNTIME_EXCEPTION ) /* Needed by ~HObject() from DTOR_FIX. */
		|| ! _exceptionMessage.is_empty()
		|| ( _exceptionPosition != 0 )
	);
}

void HThread::flush_runtime_exception( void ) {
	M_PROLOG
	if ( has_runtime_exception() ) {
		int fileId( _exceptionFileId );
		_exceptionFileId = INVALID_FILE_ID;
		int position( _exceptionPosition );
		_exceptionPosition = 0;
		hcore::HString message( yaal::move( _exceptionMessage ) );
		throw HHuginn::HHuginnRuntimeException( message, fileId, position );
	}
	M_EPILOG
}

}

}

}

