/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "if.hxx"
#include "helper.hxx"
#include "thread.hxx"
#include "expression.hxx"
#include "scope.hxx"

namespace yaal {

namespace tools {

namespace huginn {

HIf::HIf(
	HHuginn::statement_id_t id_,
	OCompiler::OScopeContext::active_scopes_t const& ifClause_,
	HHuginn::scope_t const& elseClause_,
	int fileId_,
	executing_parser::range_t range_
) : HVirtualScope( id_, fileId_, range_ )
	, _ifClauses( ifClause_ )
	, _elseClause( elseClause_ ) {
	for ( OCompiler::OActiveScope& as : _ifClauses ) {
		as._scope->make_inline();
	}
	if ( !! _elseClause ) {
		_elseClause->make_inline();
	}
	return;
}

void HIf::do_execute_internal( huginn::HThread* thread_ ) const {
	M_PROLOG
	thread_->create_scope_frame( this );
	HFrame* f( thread_->current_frame() );
	bool done( false );
	for (
		if_clauses_t::const_iterator it( _ifClauses.begin() ), end( _ifClauses.end() );
		( it != end ) && ! done && thread_->can_continue();
		++ it
	) {
		it->_expression->execute( thread_ );
		if ( thread_->can_continue() ) {
			HHuginn::value_t v( yaal::move( f->result() ) );
			if ( v->type_id() != HHuginn::TYPE::BOOLEAN ) {
				throw HHuginn::HHuginnRuntimeException( "`If` argument is not a boolean.", file_id(), it->_expression->position() );
			}
			if ( static_cast<HBoolean*>( v.raw() )->value() ) {
				done = true;
				it->_scope->execute_internal( thread_ );
			}
		} else {
			break;
		}
	}
	if ( ! done && thread_->can_continue() && !! _elseClause ) {
		_elseClause->execute( thread_ );
	}
	thread_->pop_frame();
	return;
	M_EPILOG
}

}

}

}

