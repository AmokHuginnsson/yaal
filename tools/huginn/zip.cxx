/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "zip.hxx"
#include "runtime.hxx"
#include "thread.hxx"
#include "helper.hxx"

namespace yaal {

namespace tools {

namespace huginn {

HHuginn::class_t HZip::get_class( HRuntime* runtime_, huginn::HClass const* origin_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"Zip",
			"The `Zip` class represents a *lazy* zipping of set of *iterable*s into another. Resulting *iterable* produces number of `tuple`s equal to minimum length of source *iterable*s.",
			HHuginn::ACCESS::PRIVATE
		)
	);
	c->set_origin( origin_ );
	runtime_->huginn()->register_class( c );
	return c;
	M_EPILOG
}

HHuginn::value_t HZip::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int position_ ) const {
	M_PROLOG
	HHuginn::values_t vals;
	for ( HHuginn::value_t const& v : _source ) {
		vals.push_back( v->clone( thread_, const_cast<HHuginn::value_t*>( &v ), position_ ) );
	}
	return (
		thread_->object_factory().create<HZip>(
			HIterable::get_class(),
			yaal::move( vals )
		)
	);
	M_EPILOG
}

class HZipIterator : public HIteratorInterface {
public:
	typedef yaal::hcore::HArray<huginn::HIterable::iterator_t> iterators_t;
protected:
	iterators_t _impl;
	HHuginn::values_t _cache;
public:
	HZipIterator( iterators_t&& iterators_ )
		: _impl( yaal::move( iterators_ ) )
		, _cache() {
		_cache.reserve( _impl.get_size() );
		return;
	}
protected:
	virtual bool do_is_valid( HThread* thread_, int position_ ) override {
		if ( ! thread_->can_continue() ) {
			return ( false );
		}
		if ( _cache.is_empty() ) {
			for ( iterators_t::value_type& it : _impl ) {
				if ( ! it->is_valid( thread_, position_ ) ) {
					break;
				}
				_cache.emplace_back( it->value( thread_, position_ ) );
				it->next( thread_, position_ );
			}
		}
		return ( _cache.get_size() == _impl.get_size() );
	}
	virtual void do_next( HThread*, int ) override {
		_cache.clear();
		return;
	}
	virtual HHuginn::value_t do_value( HThread* thread_, int position_ ) override {
		if ( _cache.is_empty() ) {
			do_is_valid( thread_, position_ );
		}
		if ( _cache.get_size() < _impl.get_size() ) {
			throw HHuginn::HHuginnRuntimeException(
				"Getting value from an invalid iterator.",
				thread_->file_id(),
				position_
			);
		}
		huginn::HTuple::values_t data( _cache );
		HObjectFactory& of( thread_->object_factory() );
		return ( of.create_tuple( yaal::move( data ) ) );
	}
private:
	HZipIterator( HZipIterator const& ) = delete;
	HZipIterator& operator = ( HZipIterator const& ) = delete;
};

HZip::iterator_t HZip::do_iterator( HThread* thread_, int position_ ) {
	HZipIterator::iterators_t data;
	data.reserve( _source.get_size() );
	for ( HHuginn::value_t& v : _source ) {
		data.emplace_back( static_cast<huginn::HIterable*>( v.raw() )->iterator( thread_, position_ ) );
	}
	return ( yaal::hcore::make_pointer<HZipIterator>( yaal::move( data ) ) );
}

}

}

}

