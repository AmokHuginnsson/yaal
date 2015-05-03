/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  scope.cxx - this file is integral part of `yaal' project.

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
#include "scope.hxx"
#include "thread.hxx"

namespace yaal {

namespace tools {

namespace huginn {

HScope::HScope( void )
	: _statements() {
	return;
}

void HScope::add_statement( statement_t statement_ ) {
	M_PROLOG
	_statements.emplace_back( statement_ );
	return;
	M_EPILOG
}

void HScope::do_execute( HThread* thread_ ) const {
	M_PROLOG
	thread_->create_scope_frame();
	for ( statement_t const& s : _statements ) {
		s->execute( thread_ );
		M_ASSERT( thread_->current_frame()->values().is_empty() );
		M_ASSERT( thread_->current_frame()->operations().is_empty() );
		if ( ! thread_->can_continue() ) {
			break;
		}
	}
	thread_->pop_frame();
	return;
	M_EPILOG
}

}

}

}

