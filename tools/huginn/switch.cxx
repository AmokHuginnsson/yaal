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
	int fileId_,
	executing_parser::range_t range_
) : HVirtualScope( id_, fileId_, range_ ),
	_expression(),
	_cases(),
	_default() {
	return;
}

void HSwitch::init(
	HHuginn::expression_t const& expression_,
	cases_t const& cases_,
	HHuginn::scope_t const& default_
) {
	_expression = expression_;
	_cases = cases_;
	_default = default_;
	return;
}

void HSwitch::do_execute_internal( HThread* thread_ ) const {
	M_PROLOG
	HHuginn::value_t switchValue( _expression->evaluate( thread_ ) );
	if ( thread_->can_continue() ) {
		bool matched( false );
		for (
			cases_t::const_iterator it( _cases.begin() ), end( _cases.end() );
			( it != end ) && thread_->can_continue();
			++ it
		) {
			HHuginn::value_t caseValue;
			if ( ! matched ) {
				caseValue = it->_expression->evaluate( thread_ );
			}
			if ( ! thread_->can_continue() ) {
				break;
			}
			if ( ! matched && ( switchValue->type_id() != caseValue->type_id() ) ) {
				throw HHuginn::HHuginnRuntimeException( "Case type does not match switch type.", file_id(), it->_expression->position() );
			}
			if ( matched || switchValue->operator_equals( thread_, switchValue, caseValue, it->_expression->position() ) ) {
				matched = true;
				it->_scope->execute( thread_ );
			}
		}
		if ( thread_->can_continue() && !! _default ) {
			_default->execute( thread_ );
		}
	}
	thread_->state_unbreak();
	return;
	M_EPILOG
}

}

}

}

