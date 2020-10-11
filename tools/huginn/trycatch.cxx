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
	HHuginn::expression_t const& control_,
	HHuginn::scope_t const& scope_
) : HStatement( id_, scope_->file_id(), scope_->range() )
	, _type( type_ )
	, _control( control_ )
	, _scope( scope_ ) {
	_scope->make_inline();
	return;
}

void HTryCatch::HCatch::execute( HThread* thread_, HHuginn::value_t value_ ) const {
	M_PROLOG
	thread_->create_scope_frame( this );
	_control->commit( thread_, yaal::move( value_ ) );
	if ( thread_->can_continue() ) {
		_scope->execute( thread_ );
	}
	thread_->pop_frame();
	return;
	M_EPILOG
}

HTryCatch::HTryCatch( HHuginn::scope_t const& try_, catches_t const& catches_, int fileId_, executing_parser::range_t range_ )
	: HVirtualScope( try_->id(), fileId_, range_ )
	, _try( try_ )
	, _catches( catches_ ) {
	_try->make_inline();
	return;
}

void HTryCatch::do_execute_internal( huginn::HThread* thread_ ) const {
	M_PROLOG
	thread_->create_try_catch_frame( this );
	_try->execute_internal( thread_ );
	if ( thread_->has_exception() ) {
		HHuginn::value_t v( thread_->exception() );
		for ( HCatch const& c : _catches ) {
			if ( v->is_kind_of( c.type() ) ) {
				thread_->clean_exception();
				c.execute( thread_, v );
				break;
			}
		}
	}
	thread_->pop_frame();
	return;
	M_EPILOG
}

}

}

}

