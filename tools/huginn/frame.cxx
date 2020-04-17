/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "frame.hxx"
#include "runtime.hxx"
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
	, _type( TYPE::SCOPE )
	, _state( STATE::NORMAL )
	, _statement( nullptr )
	, _position( INVALID_POSITION ) {
	return;
}

void HFrame::init(
	TYPE type_,
	HStatement const* statement_,
	HHuginn::value_t* object_,
	int upCast_
) {
	M_PROLOG
	_type = type_;
	_statement = statement_;
	_object = object_;
	_upCast = upCast_;
	_variables.reserve( _thread->runtime().max_local_variable_count() );
	reshape();
	_result = _thread->runtime().none_value();
	_number = _parent ? ( _parent->_number + ( ( type_ == TYPE::FUNCTION ) ? 1 : 0 ) ) : 1;
	_state = STATE::NORMAL;
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

void HFrame::break_execution( STATE state_ ) {
	/*
	 * Order of those two calls matters.
	 * In case of uncaught exception from Huginn user defined destructor
	 * we set the state of whole frame stack as STATE::RUNTIME_EXCEPTION.
	 * Frame state is checked in ~HObject() with has_runtime_exception() call.
	 * Setting state first here ensures that no more user defined desturctors
	 * will be called.
	 */
	_state = state_;
	_values.clear();
	return;
}

void HFrame::continue_execution( void ) {
	if ( _state == STATE::CONTINUE ) {
		_state = STATE::NORMAL;
	}
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
	_state = STATE::NORMAL;
	return;
	M_EPILOG
}

void HFrame::note_variable( HHuginn::identifier_id_t identifier_ ) {
	M_PROLOG
	_variableIdentifiers.push_back( identifier_ );
	return;
	M_EPILOG
}

void HFrame::note_variable( HHuginn::identifier_id_t identifier_, HStatement::statement_id_t statementId_, int index_ ) {
	M_PROLOG
	if ( ( statementId_ == _statement->id() ) && ( static_cast<int>( _variableIdentifiers.get_size() ) == index_ ) ) {
		_variableIdentifiers.push_back( identifier_ );
	}
	return;
	M_EPILOG
}

void HFrame::commit_variable( HHuginn::value_t&& value_, int position_ ) {
	M_PROLOG
	M_ASSERT( _result->type_id() == HHuginn::TYPE::REFERENCE );
	static_cast<HReference*>( _result.raw() )->set( _thread, yaal::move( value_ ), position_ );
	return;
	M_EPILOG
}

HHuginn::value_t HFrame::get_field( ACCESS access_, int index_ ) {
	M_PROLOG
	HHuginn::value_t* obj( object() );
	M_ASSERT( obj && !! *obj );
	HHuginn::value_t v;
	if ( access_ == ACCESS::VALUE ) {
		v = static_cast<HObject*>( obj->raw() )->field( *obj, index_ );
	} else {
		HHuginn::value_t& ref( static_cast<HObject*>( obj->raw() )->field_ref( index_ ) );
	  v = _thread->runtime().object_factory()->create_reference( ref );
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t const& HFrame::get_variable_value( HStatement::statement_id_t statementId_, int index_ ) {
	M_PROLOG
	HFrame* f( this );
	while ( statementId_ != f->_statement->id() ) {
		f = f->_parent;
		M_ASSERT( f );
	}
	return ( f->_variables[index_] );
	M_EPILOG
}

HHuginn::value_t HFrame::make_variable( HStatement::statement_id_t M_DEBUG_CODE( statementId_ ), int index_ ) {
	M_PROLOG
	M_ASSERT( statementId_ == _statement->id() );
	/*
	 * It is very difficult to remove following if() statement due to
	 * loop statements in Huginn language.
	 * For `for' and `while' loop statement variables that are local to their scope ({...})
	 * shall be created during first iteration and only updated during subsequent iterations.
	 *
	 * Current implementation has interesting side effect that local variables
	 * in all but last of iterations are destroyed in order of their definition
	 * and not in reverse order of their definition as it would be expected.
	 */
	if ( static_cast<int>( _variables.get_size() ) == index_ ) {
		M_ASSERT( _variables.get_capacity() >= ( _variables.get_size() + 1 ) );
		_variables.push_back( HHuginn::value_t() );
	}
	return ( _thread->runtime().object_factory()->create_reference( _variables[index_] ) );
	M_EPILOG
}

HHuginn::value_t HFrame::get_variable_reference( HStatement::statement_id_t statementId_, int index_ ) {
	M_PROLOG
	HFrame* f( this );
	while ( statementId_ != f->_statement->id() ) {
		f = f->_parent;
		M_ASSERT( f );
	}
	return ( _thread->runtime().object_factory()->create_reference( f->_variables[index_] ) );
	M_EPILOG
}

HHuginn::value_t HFrame::get_this( void ) {
	M_PROLOG
	HHuginn::value_t* obj( object() );
	M_ASSERT( obj && !! *obj );
	return ( *obj );
	M_EPILOG
}

HHuginn::value_t HFrame::get_super( int position_ ) {
	M_PROLOG
	HHuginn::value_t* obj( object() );
	M_ASSERT( obj && !! *obj );
	HObjectFactory* of( _thread->runtime().object_factory() );
	return ( of->create<HObjectReference>( of->object_reference_class(), *obj, _upCast, file_id(), position_ ) );
	M_EPILOG
}

HHuginn::value_t* HFrame::object( void ) const {
	HFrame const* f( this );
	while ( ! f->_object ) {
		M_ASSERT( f->_parent && ( f->_parent->_number == _number ) );
		f = f->_parent;
	}
	return ( f->_object );
}

}

}

}

