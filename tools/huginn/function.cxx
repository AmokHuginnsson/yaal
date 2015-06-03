/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  function.cxx - this file is integral part of `yaal' project.

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
#include "function.hxx"
#include "thread.hxx"
#include "expression.hxx"
#include "scope.hxx"
#include "helper.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace huginn {

HFunction::HFunction( yaal::hcore::HString const& name_,
	parameter_names_t const& parameterNames_,
	HHuginn::scope_t const& scope_,
	expressions_t const& defaultValues_ )
	: _name( name_ ),
	_parameterNames( parameterNames_ ),
	_defaultValues( defaultValues_ ),
	_scope( scope_ ) {
	return;
}

HHuginn::value_t HFunction::execute( huginn::HThread* thread_, HHuginn::HObject* object_, HHuginn::values_t const& values_, int position_ ) const {
	M_PROLOG
	verify_arg_count(
		_name,
		values_,
		static_cast<int>( _parameterNames.get_size() - _defaultValues.get_size() ),
		static_cast<int>( _parameterNames.get_size() ),
		position_
	);
	int upCast( 0 );
	if ( object_ && ( object_->field_index( _name ) >= 0 ) ) {
		HHuginn::HClass const* c( object_->get_class() );
		while ( c ) {
			int idx( c->field_index( _name ) );
			if ( ( idx >= 0 ) && ( c->function( idx ).id() == this ) ) {
				break;
			}
			c = c->super();
			++ upCast;
		}
		if ( ! c ) {
			upCast = 0;
		}
	}
	thread_->create_function_frame( object_, upCast );
	HFrame* f( thread_->current_frame() );
	for (
		int i( 0 ),
			NAME_COUNT( static_cast<int>( _parameterNames.get_size() ) ),
			VALUE_COUNT( static_cast<int>( values_.get_size() ) ),
			DEFAULT_VALUE_COUNT( static_cast<int>( _defaultValues.get_size() ) );
		i < NAME_COUNT;
		++ i
	) {
		if ( i < VALUE_COUNT ) {
			thread_->current_frame()->set_variable( _parameterNames[i], values_[i], position_ );
		} else {
			int defaultValueIndex( i - ( NAME_COUNT - DEFAULT_VALUE_COUNT ) );
			_defaultValues[defaultValueIndex]->execute( thread_ );
			if ( ! thread_->can_continue() ) {
				break;
			}
			thread_->current_frame()->set_variable( _parameterNames[i], f->result(), position_ );
		}
	}
	if ( thread_->can_continue() ) {
		_scope->execute( thread_ );
	}
	HHuginn::value_t res( thread_->current_frame()->result() );
	thread_->pop_frame();
	return ( res );
	M_EPILOG
}

}

}

}

