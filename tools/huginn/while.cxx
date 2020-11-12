/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "while.hxx"
#include "helper.hxx"
#include "thread.hxx"
#include "expression.hxx"
#include "scope.hxx"

namespace yaal {

namespace tools {

namespace huginn {

HWhile::HWhile(
	HHuginn::statement_id_t id_,
	int fileId_,
	executing_parser::range_t range_
) : HVirtualScope( id_, fileId_, range_ )
	, _condition()
	, _loop() {
	return;
}

void HWhile::init(
	HHuginn::expression_t const& condition_,
	HHuginn::scope_t const& loop_
) {
	_condition = condition_;
	_loop = loop_;
	return;
}

void HWhile::do_execute_internal( huginn::HThread* thread_ ) const {
	M_PROLOG
	while ( thread_->can_continue() ) {
		HHuginn::value_t v( _condition->evaluate( thread_ ) );
		if ( thread_->can_continue() ) {
			if ( v->type_id() != HHuginn::TYPE::BOOLEAN ) {
				throw HHuginn::HHuginnRuntimeException( "`While` argument is not a boolean.", file_id(), _condition->position() );
			}
			if ( static_cast<HBoolean const*>( v.raw() )->value() ) {
				_loop->execute_internal( thread_ );
				thread_->state_transition( HThread::STATE::CONTINUE, HThread::STATE::NORMAL );
			} else {
				break;
			}
		}
	}
	thread_->state_unbreak();
	return;
	M_EPILOG
}

}

}

}

