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

class HThread final {
public:
	typedef HThread this_type;
	typedef yaal::hcore::HPointer<huginn::HFrame> frame_t;
	typedef yaal::hcore::HArray<frame_t> frames_t;
private:
	frames_t _frames;
	HFrame* _currentFrame;
	int _frameCount;
	int _functionFrameCount;
	yaal::hcore::HThread::id_t _id;
	HRuntime* _runtime;
	HHuginn::values_t _valueCache;
	HObjectFactory& _objectFactory;
	yaal::hcore::HString _exceptionMessage;
	int _exceptionFileId;
	int _exceptionPosition;
public:
	HThread( HRuntime*, yaal::hcore::HThread::id_t );
	~HThread( void );
	void create_function_frame( HStatement const*, HHuginn::value_t*, int );
	void create_incremental_function_frame( HStatement const*, HHuginn::value_t*, int );
	void create_loop_frame( HStatement const* );
	void create_scope_frame( HStatement const* );
	void create_try_catch_frame( HStatement const* );
	void pop_frame( void );
	void pop_incremental_frame( void );
	HFrame* current_frame( void ) {
		return ( _currentFrame );
	}
	HFrame const* current_frame( void ) const {
		return ( _currentFrame );
	}
	void break_execution( HFrame::STATE, HHuginn::value_t&& = HHuginn::value_t(), int = 0, int = MAIN_FILE_ID, int = 0 );
	void set_exception( yaal::hcore::HString const&, int, int );
	bool can_continue( void ) const {
		M_PROLOG
		M_ASSERT( _currentFrame );
		return ( _currentFrame->can_continue() );
		M_EPILOG
	}
	yaal::hcore::HThread::id_t id( void ) const {
		return ( _id );
	}
	void flush_runtime_exception( void );
	bool has_runtime_exception( void ) const;
	bool has_exception( void ) const;
	void raise( HHuginn::HClass const*, yaal::hcore::HString const&, int );
	HRuntime& runtime( void ) {
		return ( *_runtime );
	}
	HObjectFactory& object_factory( void ) const {
		return ( _objectFactory );
	}
	int call_stack_size( void ) const {
		return ( _functionFrameCount );
	}
	HHuginn::values_t& value_cache( void ) {
		return ( _valueCache );
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

