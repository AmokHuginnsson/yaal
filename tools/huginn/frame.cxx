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
#include "thread.hxx"
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
	, _namedVariables()
	, _operations()
	, _values()
	, _result()
	, _number( 0 )
	, _type( TYPE::SCOPE )
	, _state( STATE::NORMAL )
	, _statementId( INVALID_STATEMENT_IDENTIFIER ) {
	return;
}

void HFrame::init(
	TYPE type_,
	HStatement::statement_id_t statementId_,
	HHuginn::value_t* object_,
	int upCast_
) {
	M_PROLOG
	_type = type_;
	_statementId = statementId_;
	_object = object_;
	_upCast = upCast_;
	_result = _thread->huginn().none_value();
	_number = _parent ? ( _parent->_number + ( ( type_ == TYPE::FUNCTION ) ? 1 : 0 ) ) : 1;
	return;
	M_EPILOG
}

void HFrame::break_execution( STATE state_ ) {
	_state = state_;
	_values.clear();
	_operations.clear();
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

void HFrame::reset( void ) {
	M_PROLOG
	M_ASSERT( _values.is_empty() );
	M_ASSERT( _operations.is_empty() );
	_statementId = INVALID_STATEMENT_IDENTIFIER;
	_result.reset();
	_namedVariables.clear();
	_variables.clear();
	_state = STATE::NORMAL;
	return;
	M_EPILOG
}

HHuginn::value_t HFrame::get_reference( HHuginn::identifier_id_t identifierId_, int position_ ) {
	M_PROLOG
	HHuginn::value_t v;
	HFrame* f( this );
	while ( f ) {
		named_variables_t::iterator it( f->_namedVariables.find( identifierId_ ) );
		HHuginn::HObject* obj( f->_object ? static_cast<HHuginn::HObject*>( f->_object->raw() ) : nullptr );
		if ( it != f->_namedVariables.end() ) {
			v = it->second;
			break;
		} else if ( obj && ( identifierId_ == KEYWORD::THIS_IDENTIFIER ) ) {
			v = *f->_object;
			M_ASSERT( !! v );
			break;
		} else if ( obj && ( identifierId_ == KEYWORD::SUPER_IDENTIFIER ) ) {
			HHuginn::value_t p = *f->_object;
			M_ASSERT( !! p );
			v = make_pointer<HHuginn::HObjectReference>( p, _upCast, true, position_ );
			break;
		} else if ( f->_parent && ( f->_parent->_number == _number ) ) {
			f = f->_parent;
		} else {
			break;
		}
	}
	if ( ! v ) {
		HHuginn::function_t fun( _thread->huginn().get_function( identifierId_ ) );
		if ( !! fun ) {
			v = make_pointer<HHuginn::HFunctionReference>( identifierId_, fun );
		} else {
			v = _thread->huginn().get_package( identifierId_ );
			if ( ! v ) {
				throw HHuginn::HHuginnRuntimeException(
					"Name `"_ys.append( _thread->huginn().identifier_name( identifierId_ ) ).append( "' is not defined." ),
					position_
				);
			}
		}
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t HFrame::get_field( HExpression::ACCESS access_, int index_ ) {
	M_PROLOG
	HFrame* f( this );
	while ( ! f->_object ) {
		f = f->_parent;
		M_ASSERT( f );
	}
	HHuginn::value_t v;
	if ( access_ == HExpression::ACCESS::VALUE ) {
		v = static_cast<HHuginn::HObject*>( f->_object->raw() )->field( *f->_object, index_ );
	} else {
		HHuginn::value_t& ref( static_cast<HHuginn::HObject*>( f->_object->raw() )->field_ref( index_ ) );
	  v = make_pointer<HHuginn::HReference>( ref );
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t HFrame::get_variable( int scopeUp_, int index_ ) {
	M_PROLOG
	HFrame* f( this );
	while ( scopeUp_ > 0 ) {
		f = f->_parent;
		M_ASSERT( f );
	}
	return ( f->_variables[index_] );
	M_EPILOG
}

void HFrame::set_variable( HHuginn::identifier_id_t identifierId_, HHuginn::value_t const& value_, int position_ ) {
	M_PROLOG
	HHuginn::value_t ref( make_variable( identifierId_, position_ ) );
	static_cast<HHuginn::HReference*>( ref.raw() )->value() = value_;
	return;
	M_EPILOG
}

HHuginn::value_t HFrame::make_variable( HHuginn::identifier_id_t identifierId_, int ) {
	M_PROLOG
	HFrame* f( this );
	HHuginn::value_t* v( nullptr );
	while ( f ) {
		int fieldIdx( -1 );
		HHuginn::HObject* obj( f->_object ? static_cast<HHuginn::HObject*>( f->_object->raw() ) : nullptr );
		if ( obj && ( ( fieldIdx = obj->field_index( identifierId_ ) ) >= 0 ) ) {
			v = &( obj->field_ref( fieldIdx ) );
			break;
		}
		named_variables_t::iterator it( f->_namedVariables.find( identifierId_ ) );
		if ( it != f->_namedVariables.end() ) {
			v = &( it->second );
			break;
		}
		if ( f->_parent && ( f->_parent->_number == _number ) ) {
			f = f->_parent;
		} else {
			f = nullptr;
		}
	}
	if ( ! v ) {
		v = &( _namedVariables.insert( make_pair( identifierId_, _thread->huginn().none_value() ) ).first->second );
	}
	return ( make_pointer<HHuginn::HReference>( *v ) );
	M_EPILOG
}

HHuginn::HObject* HFrame::object( void ) const {
	return ( _object ? static_cast<HHuginn::HObject*>( _object->raw() ) : ( ( _parent && _parent->_number == _number ) ? _parent->object() : nullptr ) );
}

}

}

}

