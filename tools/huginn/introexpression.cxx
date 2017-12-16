/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  introexpression.cxx - this file is integral part of `yaal' project.

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
#include "introexpression.hxx"
#include "thread.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace huginn {

HIntroExpression::HIntroExpression( HIntrospectorInterface* introspector_, int fileId_, int position_ )
	: HExpression( fileId_, position_ )
	, _introspector( introspector_ ) {
	return;
}

void HIntroExpression::do_execute( huginn::HThread* thread_ ) const {
	M_PROLOG
	HExpression::do_execute( thread_ );
	if ( thread_->runtime().current_thread() ) {
		thread_->current_frame()->set_position( position() );
		_introspector->introspect( thread_->runtime() );
	}
	return;
	M_EPILOG
}

void HIntroExpression::get_variable_direct_note(
	OExecutionStep const& execionStep_,
	huginn::HFrame* frame_
) {
	M_PROLOG
	frame_->values().emplace( frame_->get_variable( execionStep_._access, execionStep_._statementId, execionStep_._index ) );
	if ( execionStep_._access == HFrame::ACCESS::REFERENCE ) {
		frame_->note_variable( execionStep_._identifierId, execionStep_._statementId, execionStep_._index );
	}
	return;
	M_EPILOG
}

}

}

}

