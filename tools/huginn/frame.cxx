/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "frame.hxx"
#include "runtime.hxx"
#include "statement.hxx"
#include "thread.hxx"
#include "objectfactory.hxx"
#include "keyword.hxx"
#include "objectreference.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace huginn {

HFrame::HFrame(
	HThread* thread_,
	HFrame* parent_
) : _thread( thread_ )
	, _parent( parent_ )
	, _object( nullptr )
	, _upCast( 0 )
	, _variables()
	, _variableIdentifiers()
	, _instructionPointers()
	, _values()
	, _valueCache()
	, _valueCacheSize( 0 )
	, _result()
	, _number( 0 )
	, _statement( nullptr )
	, _position( INVALID_POSITION ) {
	return;
}

void HFrame::init(
	HStatement const* statement_,
	HHuginn::value_t* object_,
	int upCast_
) {
	M_PROLOG
	_statement = statement_;
	_object = object_;
	_upCast = upCast_;
	_variables.reserve( _thread->runtime().max_local_variable_count() );
	reshape();
	_result = _thread->runtime().none_value();
	_number = _parent ? ( _parent->_number + 1 ) : 1;
	return;
	M_EPILOG
}

void HFrame::reshape( void ) {
	M_PROLOG
	int maxLocalVariableCount( _thread->runtime().max_local_variable_count() );
	for ( values_holder_t& v : _valueCache ) {
		v->reserve( maxLocalVariableCount );
	}
	return;
	M_EPILOG
}

void HFrame::set_thread( HThread* thread_ ) {
	M_PROLOG
	_thread = thread_;
	return;
	M_EPILOG
}

void HFrame::reset_expression_state( void ) {
	/*
	 * Order of those two calls matters.
	 * In case of uncaught exception from Huginn user defined destructor
	 * we set the state of whole frame stack as STATE::RUNTIME_EXCEPTION.
	 * Frame state is checked in ~HObject() with has_runtime_exception() call.
	 * Setting state first here ensures that no more user defined desturctors
	 * will be called.
	 */
	_values.clear();
	return;
}

void HFrame::set_result( HHuginn::value_t&& result_ ) {
	M_PROLOG
	_result = yaal::move( result_ );
	return;
	M_EPILOG
}

void HFrame::cleanup( void ) {
	M_PROLOG
	_values.clear();
	_valueCache.clear();
	_instructionPointers.clear();
	return;
	M_EPILOG
}

void HFrame::reset( void ) {
	M_PROLOG
	_position = INVALID_POSITION;
	_statement = nullptr;
	/*
	 * Huginn exception thrown from Huginn user defined destructor
	 * invoked on behave of `_result` could assign
	 * to `_result` and mess up ref-counter.
	 * Hence `move` to temporary.
	 */
	HHuginn::value_t r( yaal::move( _result ) );
	r.reset();
	_variableIdentifiers.clear();
	_variables.clear();
	return;
	M_EPILOG
}

void HFrame::note_variable( HHuginn::identifier_id_t identifier_ ) {
	M_PROLOG
	_variableIdentifiers.push_back( identifier_ );
	return;
	M_EPILOG
}

void HFrame::note_variable( HHuginn::identifier_id_t identifier_, int index_ ) {
	M_PROLOG
	if ( static_cast<int>( _variableIdentifiers.get_size() ) == index_ ) {
		_variableIdentifiers.push_back( identifier_ );
	}
	return;
	M_EPILOG
}

int HFrame::file_id( void ) const {
	return ( _statement->file_id() );
}

HHuginn::value_t HFrame::get_super( int position_ ) {
	M_PROLOG
	M_ASSERT( _object && !! *_object );
	HObjectFactory* of( _thread->runtime().object_factory() );
	return ( of->create<HObjectReference>( of->object_reference_class(), *_object, _upCast, file_id(), position_ ) );
	M_EPILOG
}

}

}

}

