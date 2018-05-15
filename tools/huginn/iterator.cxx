/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "iterator.hxx"
#include "runtime.hxx"
#include "thread.hxx"
#include "helper.hxx"

namespace yaal {

namespace tools {

namespace huginn {


HIterator::HIterator( HHuginn::HClass const* class_, HHuginn::value_t const& source_, HHuginn::HIterable::iterator_t const& impl_ )
	: HIterable( class_ )
	, _source( source_ )
	, _impl( impl_ ) {
}

HHuginn::HIterable::iterator_t HIterator::do_iterator( HThread*, int ) {
	return ( _impl );
}

int long HIterator::do_size( huginn::HThread* thread_, int position_ ) const {
	M_PROLOG
	return ( static_cast<HHuginn::HIterable const*>( _source.raw() )->size( thread_, position_ ) );
	M_EPILOG
}

HHuginn::value_t HIterator::is_valid( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "Iterator.is_valid", values_, 0, 0, thread_, position_ );
	HIterator* ri( static_cast<HIterator*>( object_->raw() ) );
	return ( ri->do_is_valid( thread_, position_ ) ? thread_->runtime().true_value() : thread_->runtime().false_value() );
	M_EPILOG
}

HHuginn::value_t HIterator::value( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "Iterator.value", values_, 0, 0, thread_, position_ );
	HIterator* ri( static_cast<HIterator*>( object_->raw() ) );
	return ( ri->do_value( thread_, position_ ) );
	M_EPILOG
}

HHuginn::value_t HIterator::next( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "Iterator.next", values_, 0, 0, thread_, position_ );
	HIterator* ri( static_cast<HIterator*>( object_->raw() ) );
	ri->do_next( thread_, position_ );
	return ( *object_ );
	M_EPILOG
}

HHuginn::class_t HIterator::get_class( HRuntime* runtime_ ) {
	HHuginn::class_t c(
		runtime_->create_class(
			"Iterator",
			"The `Iterator` class provides forward iterator view over an *iterable*.",
			HHuginn::ACCESS::PRIVATE,
			HHuginn::HClass::TYPE::BUILTIN
		)
	);
	HHuginn::field_definitions_t fd{
		{ "is_valid", runtime_->create_method( &HIterator::is_valid ), "check if this `Iterator` is in valid (dereferencable) state" },
		{ "value",    runtime_->create_method( &HIterator::value ),    "get value pointed by this `Iterator`" },
		{ "next",     runtime_->create_method( &HIterator::next ),     "move this `Iterator` to next position" }
	};
	c->redefine( nullptr, fd );
	runtime_->huginn()->register_class( c );
	return ( c );
}

}

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

