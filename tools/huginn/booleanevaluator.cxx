/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  booleanevaluator.cxx - this file is integral part of `yaal' project.

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
#include "booleanevaluator.hxx"
#include "thread.hxx"
#include "expression.hxx"
#include "tools/hhuginn.hxx"

namespace yaal {

namespace tools {

namespace huginn {

HBooleanEvaluator::HBooleanEvaluator( expressions_t const& expressions_, OPERATOR operator_ )
	: HValue( HHuginn::TYPE::BOOLEAN ),
	_expressions( expressions_ ),
	_operator( operator_ ) {
	return;
}

bool HBooleanEvaluator::execute( huginn::HThread* thread_ ) {
	M_PROLOG
	bool all( true );
	HFrame* f( thread_->current_frame() );
	for ( HHuginn::expression_t const& e : _expressions ) {
		e->execute( thread_ );
		if ( ! thread_->can_continue() ) {
			break;
		}
		HHuginn::value_t result( f->result() );
		if ( result->type() != HHuginn::TYPE::BOOLEAN ) {
			throw HHuginn::HHuginnRuntimeException( _errMsgHHuginn_[ERR_CODE::OPS_NOT_BOOL], e->position() );
		}
		bool v( static_cast<HHuginn::HBoolean*>( result.raw() )->value() );
		if ( ( ( _operator == OPERATOR::BOOLEAN_AND ) && ! v ) || ( ( _operator == OPERATOR::BOOLEAN_OR ) && v ) ) {
			all = false;
			break;
		}
	}
	bool ret( _operator == OPERATOR::BOOLEAN_AND ? all : ! all );
	return ( ret );
	M_EPILOG
}

}

}

}

