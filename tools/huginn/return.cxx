/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  return.cxx - this file is integral part of `yaal' project.

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
#include "return.hxx"
#include "expression.hxx"
#include "thread.hxx"

namespace yaal {

namespace tools {

namespace huginn {

HReturn::HReturn( HHuginn::expression_t const& expression_, int fileId_, int position_ )
	: HStatement( INVALID_STATEMENT_IDENTIFIER, fileId_, position_ )
	, _expression( expression_ ) {
	return;
}

void HReturn::do_execute( HThread* thread_ ) const {
	M_PROLOG
	if ( !! _expression ) {
		_expression->execute( thread_ );
	}
	thread_->break_execution( HFrame::STATE::RETURN, thread_->current_frame()->result() );
	return;
	M_EPILOG
}

}

}

}

