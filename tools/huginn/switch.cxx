/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "switch.hxx"
#include "thread.hxx"
#include "expression.hxx"
#include "scope.hxx"
#include "instruction.hxx"

namespace yaal {

namespace tools {

namespace huginn {

HSwitch::HSwitch(
	HHuginn::statement_id_t id_,
	HHuginn::expression_t const& expression_,
	cases_t const& cases_,
	HHuginn::scope_t const& default_,
	int fileId_,
	executing_parser::range_t range_
) : HStatement( id_, fileId_, range_ ),
	_expression( expression_ ),
	_cases( cases_ ),
	_default( default_ ) {
	return;
}

void HSwitch::do_execute( HThread* thread_ ) const {
	M_PROLOG
	thread_->create_loop_frame( this );
	HFrame* f( thread_->current_frame() );
	_expression->execute( thread_ );
	if ( thread_->can_continue() ) {
		HHuginn::value_t v( f->result() );
		bool matched( false );
		for (
			cases_t::const_iterator it( _cases.begin() ), end( _cases.end() );
			( it != end ) && thread_->can_continue();
			++ it
		) {
			if ( ! matched ) {
				it->_expression->execute( thread_ );
			}
			if ( thread_->can_continue() ) {
				if ( ! matched ) {
					if ( v->type_id() != f->result()->type_id() ) {
						throw HHuginn::HHuginnRuntimeException( "Case type does not match switch type.", file_id(), it->_expression->position() );
					}
				}
				if ( matched || v->operator_equals( thread_, v, f->result(), it->_expression->position() ) ) {
					matched = true;
					it->_scope->execute( thread_ );
				}
			} else {
				break;
			}
		}
		if ( thread_->can_continue() && !! _default ) {
			_default->execute( thread_ );
		}
	}
	thread_->pop_frame();
	thread_->state_unbreak();
	return;
	M_EPILOG
}

}

}

}

