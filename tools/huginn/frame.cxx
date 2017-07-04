/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  frame.cxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "frame.hxx"
#include "runtime.hxx"
#include "thread.hxx"
#include "objectfactory.hxx"
#include "keyword.hxx"

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
	, _instructionPointers()
	, _values()
	, _valueCache()
	, _result()
	, _number( 0 )
	, _type( TYPE::SCOPE )
	, _state( STATE::NORMAL )
	, _fileId( INVALID_FILE_ID )
	, _statementId( INVALID_STATEMENT_IDENTIFIER ) {
	_variables.reserve( _thread->runtime().max_local_variable_count() );
	return;
}

void HFrame::init(
	TYPE type_,
	int fileId_,
	HStatement::statement_id_t statementId_,
	HHuginn::value_t* object_,
	int upCast_
) {
	M_PROLOG
	_type = type_;
	_fileId = fileId_;
	_statementId = statementId_;
	_object = object_;
	_upCast = upCast_;
	_result = _thread->runtime().none_value();
	_number = _parent ? ( _parent->_number + ( ( type_ == TYPE::FUNCTION ) ? 1 : 0 ) ) : 1;
	_state = STATE::NORMAL;
	return;
	M_EPILOG
}

void HFrame::reshape( HThread* thread_, int size_ ) {
	M_PROLOG
	_thread = thread_;
	_variables.reserve( size_ );
	return;
	M_EPILOG
}

void HFrame::break_execution( STATE state_ ) {
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
	_statementId = INVALID_STATEMENT_IDENTIFIER;
	_result.reset();
	_variables.clear();
	_state = STATE::NORMAL;
	return;
	M_EPILOG
}

void HFrame::add_variable( HHuginn::value_t const& value_ ) {
	M_PROLOG
	_variables.push_back( value_ );
	return;
	M_EPILOG
}

void HFrame::commit_variable( HHuginn::value_t const& value_, int position_ ) {
	M_PROLOG
	if ( _result->type_id() != HHuginn::TYPE::REFERENCE ) {
		M_ASSERT( _result->type_id() == HHuginn::TYPE::CHARACTER );
		throw HHuginn::HHuginnRuntimeException( "String does not support item assignment.", position_ );
	}
	static_cast<HHuginn::HReference*>( _result.raw() )->value() = value_;
	return;
	M_EPILOG
}

HHuginn::value_t HFrame::get_field( HExpression::ACCESS access_, int index_ ) {
	M_PROLOG
	HHuginn::value_t* obj( object() );
	M_ASSERT( obj && !! *obj );
	HHuginn::value_t v;
	if ( access_ == HExpression::ACCESS::VALUE ) {
		v = static_cast<HHuginn::HObject*>( obj->raw() )->field( *obj, index_ );
	} else {
		HHuginn::value_t& ref( static_cast<HHuginn::HObject*>( obj->raw() )->field_ref( index_ ) );
	  v = _thread->runtime().object_factory()->create_reference( ref );
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t HFrame::get_variable( HExpression::ACCESS access_, HStatement::statement_id_t statementId_, int index_ ) {
	M_PROLOG
	HFrame* f( this );
	while ( statementId_ != f->_statementId ) {
		f = f->_parent;
		M_ASSERT( f );
	}
	HHuginn::value_t v;
	/*
	 * It is very difficult to remove next following if() statement due to
	 * loop statements in Huginn language.
	 * For `for' and `while' loop statement variables that are local to their scope ({...})
	 * shall be created during first iteration and only updated during subsequent iterations.
	 *
	 * Current implementation has interesting side effect that local variables
	 * in all but last of iterations are destroyed in order of their definition
	 * and not in reverse order of their definition as it would be expected.
	 */
	if ( ( access_ == HExpression::ACCESS::REFERENCE ) && ( static_cast<int>( f->_variables.get_size() ) == index_ ) ) {
		M_ASSERT( f == this );
		f->_variables.push_back( HHuginn::value_t() );
	}
	if ( access_ == HExpression::ACCESS::VALUE ) {
		v = f->_variables[index_];
	} else {
	  v = _thread->runtime().object_factory()->create_reference( f->_variables[index_] );
	}
	return ( v );
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
	return ( _thread->runtime().object_factory()->create<HHuginn::HObjectReference>( *obj, _upCast, true, position_ ) );
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

