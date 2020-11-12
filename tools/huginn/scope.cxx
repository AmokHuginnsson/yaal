/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "scope.hxx"
#include "expression.hxx"
#include "thread.hxx"

namespace yaal {

namespace tools {

namespace huginn {

HVirtualScope::HVirtualScope( HHuginn::statement_id_t id_, int fileId_, executing_parser::range_t range_ )
	: HStatement( id_, fileId_, range_ ) {
	return;
}

void HVirtualScope::do_execute( HThread* thread_ ) const {
	M_PROLOG
	execute_internal( thread_ );
	return;
	M_EPILOG
}

HScope::HScope( HHuginn::statement_id_t id_, int fileId_, executing_parser::range_t range_ )
	: HVirtualScope( id_, fileId_, range_ )
	, _statements() {
	return;
}

int HScope::add_statement( statement_t statement_ ) {
	M_PROLOG
	_statements.emplace_back( statement_ );
	return ( static_cast<int>( _statements.get_size() - 1 ) );
	M_EPILOG
}

void HScope::remove_statement( int index_ ) {
	M_PROLOG
	_statements.erase( _statements.begin() + index_ );
	return;
	M_EPILOG
}

int HScope::statement_position_at( int index_ ) const {
	return ( _statements[index_]->position() );
}

void HScope::do_execute_internal( HThread* thread_ ) const {
	M_PROLOG
	M_DEBUG_CODE( HFrame* f( thread_->current_frame() ) );
	for ( statement_t const& s : _statements ) {
		s->execute( thread_ );
		M_ASSERT( f->values().is_empty() );
		if ( ! thread_->can_continue() ) {
			break;
		}
	}
	return;
	M_EPILOG
}

void HScope::finalize_function( void ) {
	M_PROLOG
	if ( _statements.is_empty() ) {
		return;
	}
	HExpression* expr( dynamic_cast<HExpression*>( _statements.back().raw() ) );
	if ( ! expr ) {
		return;
	}
	expr->mark_final();
	return;
	M_EPILOG
}

HIncrementalScope::HIncrementalScope( HHuginn::statement_id_t id_, int fileId_, executing_parser::range_t range_ )
	: HScope( id_, fileId_, range_ ) {
	return;
}

void HIncrementalScope::do_execute( HThread* thread_ ) const {
	M_PROLOG
	do_execute_internal( thread_ );
	return;
	M_EPILOG
}

}

}

}

