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

HReturn::HReturn( HHuginn::expression_t const& expression_, int fileId_, executing_parser::range_t range_ )
	: HStatement( INVALID_STATEMENT_IDENTIFIER, fileId_, range_ )
	, _expression( expression_ ) {
	_expression->mark_final();
	return;
}

void HReturn::do_execute( HThread* thread_ ) const {
	M_PROLOG
	if ( !! _expression ) {
		_expression->execute( thread_ );
	}
	if ( thread_->can_continue() ) {
		thread_->state_set( HThread::STATE::RETURN );
	}
	return;
	M_EPILOG
}

}

}

}

