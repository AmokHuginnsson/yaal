/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file tools/huginn/thread.hxx
 * \brief Declaration of huginn::HThread class.
 */

#ifndef YAAL_TOOLS_HUGINN_THREAD_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_THREAD_HXX_INCLUDED 1

#include "tools/huginn/frame.hxx"

namespace yaal {

namespace tools {

namespace huginn {

static int const INVALID_FILE_ID = -1;

class HObjectFactory;

class HThread final {
public:
	typedef HThread this_type;
	enum class STATE {
		NORMAL,
		RETURN,
		BREAK,
		CONTINUE,
		/*
		 * Order matters! EXCEPTION, RUNTIME_EXCEPTION and EXIT must be
		 * its own group at the end of enumeration
		 */
		EXCEPTION,
		RUNTIME_EXCEPTION,
		EXIT
	};
	typedef yaal::hcore::HPointer<huginn::HFrame> frame_t;
	typedef yaal::hcore::HArray<frame_t> frames_t;
private:
	frames_t _frames;
	HFrame* _currentFrame;
	int _frameCount;
	STATE _state;
	int _breakLevel;
	yaal::hcore::HThread::id_t _id;
	HRuntime* _runtime;
	HHuginn::value_t _result;
	HHuginn::values_t _valueCache;
	HObjectFactory& _objectFactory;
	struct OException {
		yaal::hcore::HString _message;
		int _fileId;
		int _position;
		HHuginn::call_stack_t _trace;
		OException( void )
			: _message()
			, _fileId( INVALID_FILE_ID )
			, _position( 0 )
			, _trace() {
			return;
		}
	} _exception;
public:
	HThread( HRuntime*, yaal::hcore::HThread::id_t );
	~HThread( void );
	void create_frame( HStatement const*, HHuginn::value_t*, int );
	void create_incremental_frame( HStatement const*, HHuginn::value_t*, int );
	void pop_frame( void );
	void pop_incremental_frame( void );
	HFrame* current_frame( void ) {
		return ( _currentFrame );
	}
	HFrame const* current_frame( void ) const {
		return ( _currentFrame );
	}
	void state_transition( STATE old_, STATE new_, HHuginn::value_t&& value_ ) {
		if ( _state == old_ ) {
			_currentFrame->set_result( yaal::move( value_ ) );
			_state = new_;
		}
	}
	void state_transition( STATE old_, STATE new_ ) {
		if ( ( old_ == STATE::BREAK ) && ( _breakLevel > 0 ) ) {
			-- _breakLevel;
		} else if ( _state == old_ ) {
			_state = new_;
		}
	}
	void state_unbreak( void ) {
		if ( _state != STATE::BREAK ) {
			return;
		}
		if ( _breakLevel > 0 ) {
			-- _breakLevel;
		} else {
			_state = STATE::NORMAL;
		}
		return;
	}
	void state_set( STATE new_, HHuginn::value_t&& value_ ) {
		_result = yaal::move( value_ );
		_state = new_;
	}
	void state_set( STATE new_ ) {
		_state = new_;
	}
	void state_throw( HHuginn::value_t&&, int, int );
	STATE state( void ) const {
		return ( _state );
	}
	void set_exception( yaal::hcore::HString const&, int, int );
	bool can_continue( void ) const {
		return ( _state == STATE::NORMAL );
	}
	yaal::hcore::HThread::id_t id( void ) const {
		return ( _id );
	}
	HHuginn::value_t const& exception( void ) const {
		return ( _result );
	}
	void clean_exception( void );
	void flush_uncaught_exception( char const* = "" );
	void flush_runtime_exception( void );
	bool has_runtime_exception( void ) const;
	bool has_exception( void ) const;
	void raise( huginn::HClass const*, yaal::hcore::HString const&, int );
	HRuntime& runtime( void ) {
		return ( *_runtime );
	}
	HObjectFactory& object_factory( void ) const {
		return ( _objectFactory );
	}
	int call_stack_size( void ) const {
		return ( _frameCount );
	}
	HHuginn::values_t& value_cache( void ) {
		return ( _valueCache );
	}
	HHuginn::call_stack_t const& trace( void ) const {
		return ( _exception._trace );
	}
	int file_id( void ) const {
		return ( _currentFrame ? _currentFrame->file_id() : 0 );
	}
private:
	void add_frame( void );
	HThread( HThread const& ) = delete;
	HThread& operator = ( HThread const& ) = delete;
};

class HArguments {
private:
	HFrame* _frame;
	HHuginn::values_t& _values;
public:
	template<typename... args_t>
	HArguments( HThread* thread_, args_t&&... args_ )
		: HArguments( thread_, yaal::forward<args_t>( args_ )... ) {
	}
	template<typename head_t, typename... args_t>
	HArguments( HThread* thread_, head_t&& head_, args_t&&... args_ )
		: HArguments( thread_, yaal::forward<args_t>( args_ )... ) {
		_values.push_back( yaal::forward<head_t>( head_ ) );
	}
	HArguments( HThread* thread_ )
		: _frame( thread_->current_frame() )
		, _values( _frame ? _frame->value_cache() : thread_->value_cache() ) {
	}
	HArguments( HFrame* frame_ )
		: _frame( frame_ )
		, _values( _frame->value_cache() ) {
	}
	~HArguments( void ) {
		if ( _frame ) {
			_frame->invalidate_value_cache();
		} else {
			_values.clear();
		}
	}
	operator HHuginn::values_t& ( void ) {
		return ( _values );
	}
private:
	HArguments( HArguments&& ) = delete;
	HArguments( HArguments const& ) = delete;
	HArguments& operator = ( HArguments const& ) = delete;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_THREAD_HXX_INCLUDED */

