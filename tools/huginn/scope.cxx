/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "scope.hxx"
#include "thread.hxx"

namespace yaal {

namespace tools {

namespace huginn {

HScope::HScope( HHuginn::statement_id_t id_, int fileId_, executing_parser::range_t range_ )
	: HStatement( id_, fileId_, range_ )
	, _statements()
	, _inline( false ) {
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

void HScope::do_execute( HThread* thread_ ) const {
	M_PROLOG
	if ( ! _inline ) {
		thread_->create_scope_frame( this );
	}
	HFrame* f( thread_->current_frame() );
	for ( statement_t const& s : _statements ) {
		s->execute( thread_ );
		M_ASSERT( f->values().is_empty() );
		if ( ! f->can_continue() ) {
			break;
		}
	}
	if ( ! _inline ) {
		thread_->pop_frame();
	}
	return;
	M_EPILOG
}

void HScope::make_inline( void ) {
	_inline = true;
}

}

}

}

