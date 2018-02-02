/* Read yaal/LICENSE.md file for copyright and licensing information. */

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
	if ( thread_->can_continue() ) {
		thread_->break_execution( HFrame::STATE::RETURN, thread_->current_frame()->result() );
	}
	return;
	M_EPILOG
}

}

}

}

