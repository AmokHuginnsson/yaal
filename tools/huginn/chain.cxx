/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "chain.hxx"
#include "runtime.hxx"
#include "thread.hxx"
#include "helper.hxx"

namespace yaal {

namespace tools {

namespace huginn {

HHuginn::class_t HChain::get_class( HRuntime* runtime_, HHuginn::HClass const* origin_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"Chain",
			"The `Chain` class represents a *lazy* chained view of set of *iterable*s.",
			HHuginn::ACCESS::PRIVATE
		)
	);
	c->set_origin( origin_ );
	runtime_->huginn()->register_class( c );
	return ( c );
	M_EPILOG
}

HHuginn::value_t HChain::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int position_ ) const {
	M_PROLOG
	HHuginn::values_t vals;
	for ( HHuginn::value_t const& v : _source ) {
		vals.push_back( v->clone( thread_, const_cast<HHuginn::value_t*>( &v ), position_ ) );
	}
	return (
		thread_->object_factory().create<HChain>(
			HIterable::get_class(),
			yaal::move( vals )
		)
	);
	M_EPILOG
}

class HChainIterator : public HIteratorInterface {
protected:
	HHuginn::values_t& _source;
	int _colIdx;
	HHuginn::HIterable::iterator_t _it;
public:
	HChainIterator( HThread* thread_, HHuginn::values_t& source_, int position_ )
		: _source( source_ )
		, _colIdx( 0 )
		, _it() {
		if ( _colIdx < _source.get_size() ) {
			_it = static_cast<HHuginn::HIterable*>( _source.front().raw() )->iterator( thread_, position_ );
			ensure( thread_, position_ );
		}
		return;
	}
protected:
	virtual bool do_is_valid( HThread* thread_, int position_ ) override {
		return ( ( _colIdx < _source.get_size() ) && _it->is_valid( thread_, position_ ) );
	}
	virtual void do_next( HThread* thread_, int position_ ) override {
		_it->next( thread_, position_ );
		ensure( thread_, position_ );
		return;
	}
	virtual HHuginn::value_t do_value( HThread* thread_, int position_ ) override {
		return ( _it->value( thread_, position_ ) );
	}
private:
	HChainIterator( HChainIterator const& ) = delete;
	HChainIterator& operator = ( HChainIterator const& ) = delete;
private:
	void ensure( HThread* thread_, int position_ ) {
		while ( ! _it->is_valid( thread_, position_ ) ) {
			++ _colIdx;
			if ( _colIdx >= _source.get_size() ) {
				break;
			}
			_it = static_cast<HHuginn::HIterable*>( _source[_colIdx].raw() )->iterator( thread_, position_ );
		}
		return;
	}
};

HChain::iterator_t HChain::do_iterator( HThread* thread_, int position_ ) {
	return ( yaal::hcore::make_pointer<HChainIterator>( thread_, _source, position_ ) );
}

}

}

}

