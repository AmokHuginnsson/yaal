/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "iterator.hxx"

namespace yaal {

namespace tools {

HHuginn::HIterable::HIterable( HClass const* class_ )
	: HValue( class_ ) {
	return;
}

HHuginn::HIterable::iterator_t HHuginn::HIterable::iterator( huginn::HThread* thread_, int position_ ) {
	return ( do_iterator( thread_, position_ ) );
}

int long HHuginn::HIterable::size( huginn::HThread* thread_, int position_ ) const {
	return ( do_size( thread_, position_ ) );
}

HHuginn::HInvalidatingIterable::HInvalidatingIterable( HClass const* class_ )
	: HIterable( class_ )
	, _observers() {
	return;
}

void HHuginn::HInvalidatingIterable::notify( huginn::HNotifableIterator* iter_ ) {
	_observers.push_back( iter_ );
}

void HHuginn::HInvalidatingIterable::forget( huginn::HNotifableIterator* iter_ ) {
	_observers.erase( find( _observers.begin(), _observers.end(), iter_ ) );
}

void HHuginn::HInvalidatingIterable::invalidate( void ) {
	for ( huginn::HNotifableIterator* it : _observers ) {
		it->invalidate();
	}
}

void HHuginn::HInvalidatingIterable::invalidate( void const* id_ ) {
	for ( huginn::HNotifableIterator* it : _observers ) {
		if ( it->node_id() == id_ ) {
			it->skip();
		}
	}
}

}

}

