/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  switch.cxx - this file is integral part of `yaal' project.

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
#include "switch.hxx"
#include "thread.hxx"
#include "expression.hxx"
#include "scope.hxx"
#include "value_builtin.hxx"

namespace yaal {

namespace tools {

namespace huginn {

HSwitch::HSwitch(
	HStatement::statement_id_t id_,
	HHuginn::expression_t const& expression_,
	cases_t const& cases_,
	HHuginn::scope_t const& default_,
	int fileId_,
	int position_
) : HStatement( id_, fileId_, position_ ),
	_expression( expression_ ),
	_cases( cases_ ),
	_default( default_ ) {
	return;
}

void HSwitch::do_execute( HThread* thread_ ) const {
	M_PROLOG
	thread_->create_loop_frame( this );
	HFrame* f( thread_->current_frame() );
	_expression->execute( thread_ );
	if ( f->can_continue() ) {
		HHuginn::value_t v( f->result() );
		if ( ! OCompiler::is_comparable( v->type_id() ) ) {
			throw HHuginn::HHuginnRuntimeException( "Expression does not evaluate to comparable type.", file_id(), _expression->position() );
		}
		bool matched( false );
		for ( cases_t::const_iterator it( _cases.begin() ), end( _cases.end() );
			( it != end ) && f->can_continue(); ++ it ) {
			if ( ! matched ) {
				it->_expression->execute( thread_ );
			}
			if ( f->can_continue() ) {
				if ( ! matched ) {
					if ( v->type_id() != f->result()->type_id() ) {
						throw HHuginn::HHuginnRuntimeException( "Case type does not match switch type.", file_id(), it->_expression->position() );
					}
				}
				if ( matched || value_builtin::equals( thread_, v, f->result(), it->_expression->position() ) ) {
					matched = true;
					it->_scope->execute( thread_ );
				}
			} else {
				break;
			}
		}
		if ( f->can_continue() && !! _default ) {
			_default->execute( thread_ );
		}
	}
	thread_->pop_frame();
	return;
	M_EPILOG
}

}

}

}

