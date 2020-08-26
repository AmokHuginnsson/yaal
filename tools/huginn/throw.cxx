/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "throw.hxx"
#include "expression.hxx"
#include "thread.hxx"

using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace huginn {

HThrow::HThrow( HHuginn::expression_t const& expression_, int fileId_, executing_parser::range_t range_ )
	: HStatement( INVALID_STATEMENT_IDENTIFIER, fileId_, range_ )
	, _expression( expression_ ) {
	return;
}

void HThrow::do_execute( HThread* thread_ ) const {
	M_PROLOG
	if ( !! _expression ) {
		_expression->execute( thread_ );
	}
	HHuginn::value_t v( thread_->current_frame()->result() );
	thread_->state_throw( yaal::move( v ), file_id(), position() );
	return;
	M_EPILOG
}

}

}

}

