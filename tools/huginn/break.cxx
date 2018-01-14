/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "break.hxx"
#include "thread.hxx"

namespace yaal {

namespace tools {

namespace huginn {

HBreak::HBreak( HFrame::STATE state_, int fileId_, int position_ )
	: HStatement( INVALID_STATEMENT_IDENTIFIER, fileId_, position_ )
	, _state( state_ ) {
	return;
}

void HBreak::do_execute( HThread* thread_ ) const {
	M_PROLOG
	thread_->break_execution( _state );
	return;
	M_EPILOG
}

}

}

}

