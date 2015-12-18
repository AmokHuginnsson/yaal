/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  while.cxx - this file is integral part of `yaal' project.

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
#include "while.hxx"
#include "helper.hxx"
#include "thread.hxx"
#include "expression.hxx"
#include "scope.hxx"

namespace yaal {

namespace tools {

namespace huginn {

HWhile::HWhile( HHuginn::expression_t const& condition_, HHuginn::scope_t const& loop_ )
	: HStatement()
	, _condition( condition_ )
	, _loop( loop_ ) {
	_loop->make_inline();
	return;
}

void HWhile::do_execute( huginn::HThread* thread_ ) const {
	M_PROLOG
	thread_->create_loop_frame();
	HFrame* f( thread_->current_frame() );
	while ( f->can_continue() ) {
		_condition->execute( thread_ );
		if ( f->can_continue() ) {
			HHuginn::value_t v( f->result() );
			if ( v->type_id() != HHuginn::TYPE::BOOLEAN ) {
				throw HHuginn::HHuginnRuntimeException( "`While' argument is not a boolean.", _condition->position() );
			}
			if ( static_cast<HHuginn::HBoolean*>( v.raw() )->value() ) {
				_loop->execute( thread_ );
				f->continue_execution();
			} else {
				break;
			}
		}
	}
	thread_->pop_frame();
	return;
	M_EPILOG
}

}

}

}

