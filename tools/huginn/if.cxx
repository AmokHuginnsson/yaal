/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  if.cxx - this file is integral part of `yaal' project.

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
#include "if.hxx"
#include "helper.hxx"
#include "thread.hxx"
#include "expression.hxx"
#include "scope.hxx"

namespace yaal {

namespace tools {

namespace huginn {

HIf::HIf(
	OCompiler::OScopeContext::active_scopes_t const& ifClause_,
	HHuginn::scope_t const& elseClause_
) : HStatement(),
	_ifClauses( ifClause_ ),
	_elseClause( elseClause_ ) {
	for ( OCompiler::OActiveScope& as : _ifClauses ) {
		as._scope->make_inline();
	}
	if ( !! _elseClause ) {
		_elseClause->make_inline();
	}
	return;
}

void HIf::do_execute( huginn::HThread* thread_ ) const {
	M_PROLOG
	thread_->create_scope_frame();
	HFrame* f( thread_->current_frame() );
	bool done( false );
	for (
		if_clauses_t::const_iterator it( _ifClauses.begin() ), end( _ifClauses.end() );
		( it != end ) && ! done && f->can_continue();
		++ it
	) {
		it->_expression->execute( thread_ );
		if ( f->can_continue() ) {
			HHuginn::value_t v( f->result() );
			if ( v->type_id() != HHuginn::TYPE::BOOLEAN ) {
				throw HHuginn::HHuginnRuntimeException( "`If' argument is not a boolean.", it->_expression->position() );
			}
			if ( static_cast<HHuginn::HBoolean*>( v.raw() )->value() ) {
				done = true;
				it->_scope->execute( thread_ );
			}
		} else {
			break;
		}
	}
	if ( ! done && f->can_continue() && !! _elseClause ) {
		_elseClause->execute( thread_ );
	}
	thread_->pop_frame();
	return;
	M_EPILOG
}

}

}

}

