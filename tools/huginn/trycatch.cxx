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
	HStatement::statement_id_t id_,
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
	HFrame* f( thread_->current_frame() );
	_control->execute( thread_ );
	f->commit_variable( value_, _control->position() );
	if ( f->can_continue() ) {
		_scope->execute( thread_ );
	}
	thread_->pop_frame();
	return;
	M_EPILOG
}

HTryCatch::HTryCatch( HHuginn::scope_t const& try_, catches_t const& catches_, int fileId_, executing_parser::range_t range_ )
	: HStatement( try_->id(), fileId_, range_ )
	, _try( try_ )
	, _catches( catches_ ) {
	_try->make_inline();
	return;
}

void HTryCatch::do_execute( huginn::HThread* thread_ ) const {
	M_PROLOG
	thread_->create_try_catch_frame( this );
	_try->execute( thread_ );
	if ( thread_->has_exception() ) {
		HFrame* f( thread_->current_frame() );
		HHuginn::value_t v( f->result() );
		bool handled( false );
		for ( HCatch const& c : _catches ) {
			if ( v->is_kind_of( c.type() ) ) {
				c.execute( thread_, v );
				handled = true;
				break;
			}
		}
		thread_->pop_frame();
		if ( ! handled ) {
			thread_->break_execution( HFrame::STATE::EXCEPTION, yaal::move( v ) );
		}
	} else {
		thread_->pop_frame();
	}
	return;
	M_EPILOG
}

}

}

}

