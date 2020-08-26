/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "break.hxx"
#include "thread.hxx"

namespace yaal {

namespace tools {

namespace huginn {

HBreak::HBreak( HThread::STATE state_, int fileId_, executing_parser::range_t range_ )
	: HStatement( INVALID_STATEMENT_IDENTIFIER, fileId_, range_ )
	, _state( state_ ) {
	return;
}

void HBreak::do_execute( HThread* thread_ ) const {
	M_PROLOG
	if ( thread_->can_continue() ) {
		thread_->state_transition( HThread::STATE::NORMAL, _state );
	}
	return;
	M_EPILOG
}

}

}

}

