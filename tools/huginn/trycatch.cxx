/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "trycatch.hxx"
#include "thread.hxx"
#include "expression.hxx"
#include "scope.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;

namespace yaal {

namespace tools {

namespace huginn {

HTryCatch::HCatch::HCatch(
	HHuginn::statement_id_t id_,
	HHuginn::identifier_id_t type_,
	int fileId_,
	executing_parser::range_t range_
) : HVirtualScope( id_, fileId_, range_ )
	, _type( type_ )
	, _control()
	, _scope() {
	return;
}

void HTryCatch::HCatch::init(
	HHuginn::expression_t const& control_,
	HHuginn::scope_t const& scope_
) {
	_control = control_;
	_scope = scope_;
	return;
}

void HTryCatch::HCatch::do_execute_internal( HThread* thread_ ) const {
	M_PROLOG
	HHuginn::value_t ex( thread_->exception() );
	thread_->clean_exception();
	_control->commit( thread_, yaal::move( ex ) );
	if ( thread_->can_continue() ) {
		_scope->execute( thread_ );
	}
	return;
	M_EPILOG
}

HTryCatch::HTryCatch( HHuginn::scope_t const& try_, catches_t&& catches_, int fileId_, executing_parser::range_t range_ )
	: HVirtualScope( try_->id(), fileId_, range_ )
	, _try( try_ )
	, _catches( yaal::move( catches_ ) ) {
	return;
}

void HTryCatch::do_execute_internal( huginn::HThread* thread_ ) const {
	M_PROLOG
	_try->execute_internal( thread_ );
	if ( thread_->has_exception() ) {
		HHuginn::value_t const& v( thread_->exception() );
		for ( HHuginn::scope_t const& c : _catches ) {
			if ( v->is_kind_of( static_cast<HCatch const*>( c.raw() )->type() ) ) {
				c->execute( thread_ );
				break;
			}
		}
	}
	return;
	M_EPILOG
}

}

}

}

