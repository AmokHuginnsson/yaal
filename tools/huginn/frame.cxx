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

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace huginn {

HFrame::HFrame(
	HThread* thread_,
	HFrame* parent_,
	HHuginn::HObject* object_,
	int upCast_,
	bool bump_,
	bool loop_
) : _thread( thread_ )
	, _parent( parent_ )
	, _object( object_ )
	, _upCast( upCast_ )
	, _variables()
	, _operations()
	, _values()
	, _result( _none_ )
	, _number( parent_ ? ( parent_->_number + ( bump_ ? 1 : 0 ) ) : 1 )
	, _loop( loop_ )
	, _state( STATE::NORMAL ) {
	return;
}

HThread* HFrame::thread( void ) const {
	return ( _thread );
}

int HFrame::number( void ) const {
	return ( _number );
}

HFrame* HFrame::parent( void ) {
	return ( _parent );
}

bool HFrame::is_loop( void ) const {
	return ( _loop );
}

bool HFrame::can_continue( void ) const {
	return ( _state == STATE::NORMAL );
}

void HFrame::break_execution( STATE state_ ) {
	_state = state_;
	return;
}

void HFrame::continue_execution( void ) {
	if ( _state == STATE::CONTINUE ) {
		_state = STATE::NORMAL;
	}
	return;
}

void HFrame::set_result( HHuginn::value_t const& result_ ) {
	M_PROLOG
	_result = result_;
	return;
	M_EPILOG
}

HHuginn::value_t HFrame::result( void ) const {
	return ( _result );
}

void HFrame::reset( void ) {
	M_PROLOG
	_operations.clear();
	_values.clear();
	return;
	M_EPILOG
}

HHuginn::value_t HFrame::try_reference( yaal::hcore::HString const& name_, int position_ ) {
	M_PROLOG
	variables_t::iterator it( _variables.find( name_ ) );
	HHuginn::value_t v;
	int fieldIdx( -1 );
	if ( it != _variables.end() ) {
		v = it->second;
	} else if ( _object && ( ( fieldIdx = _object->field_index( name_ ) ) >= 0 ) ) {
		v = _object->field( fieldIdx );
	} else if ( _object && ( name_ == "this" ) ) {
		v = _object->get_pointer();
		M_ASSERT( !! v );
	} else if ( _object && ( name_ == "super" ) ) {
		HHuginn::value_t p = _object->get_pointer();
		M_ASSERT( !! p );
		v = make_pointer<HHuginn::HObjectReference>( p, _upCast, true, position_ );
	} else if ( _parent && ( _parent->_number == _number ) ) {
		v = _parent->try_reference( name_, position_ );
	} else {
		HHuginn::function_t f( _thread->huginn().get_function( name_ ) );
		if ( !! f ) {
			v = make_pointer<HHuginn::HFunctionReference>( name_, f );
		}
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t HFrame::get_reference( yaal::hcore::HString const& name_, int position_ ) {
	M_PROLOG
	HHuginn::value_t v( try_reference( name_, position_ ) );
	if ( ! v ) {
		throw HHuginn::HHuginnRuntimeException( "Name `"_ys.append( name_ ).append( "' is not defined." ), position_ );
	}
	return ( v );
	M_EPILOG
}

void HFrame::set_variable( yaal::hcore::HString const& name_, HHuginn::value_t const& value_, int position_ ) {
	M_PROLOG
	HHuginn::value_t ref( make_variable( name_, position_ ) );
	static_cast<HHuginn::HReference*>( ref.raw() )->value() = value_;
	return;
	M_EPILOG
}

HHuginn::value_t HFrame::make_variable( yaal::hcore::HString const& name_, int ) {
	M_PROLOG
	HFrame* f( this );
	HHuginn::value_t* v( nullptr );
	while ( f ) {
		int fieldIdx( -1 );
		if ( f->_object && ( ( fieldIdx = f->_object->field_index( name_ ) ) >= 0 ) ) {
			v = &( f->_object->field( fieldIdx ) );
			break;
		}
		variables_t::iterator it( f->_variables.find( name_ ) );
		if ( it != f->_variables.end() ) {
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
		v = &( _variables.insert( make_pair( name_, _none_ ) ).first->second );
	}
	return ( make_pointer<HHuginn::HReference>( *v ) );
	M_EPILOG
}

operations_t& HFrame::operations( void ) {
	return ( _operations );
}

HFrame::values_t& HFrame::values( void ) {
	return ( _values );
}

HHuginn::HObject* HFrame::object( void ) const {
	return ( _object ? _object : ( ( _parent && _parent->_number == _number ) ? _parent->object() : nullptr ) );
}

}

}

}

