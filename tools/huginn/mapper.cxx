/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "mapper.hxx"
#include "runtime.hxx"
#include "thread.hxx"
#include "helper.hxx"

namespace yaal {

namespace tools {

namespace huginn {

HHuginn::class_t HMapper::get_class( HRuntime* runtime_, huginn::HClass const* origin_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"Mapper",
			"The `Mapper` class represent *lazy* transformation of one *iterable* into another. Resulting *iterable* produces values from original *iterable* transformed by user supplied unary function.",
			HHuginn::ACCESS::PRIVATE
		)
	);
	c->set_origin( origin_ );
	runtime_->huginn()->register_class( c );
	return ( c );
	M_EPILOG
}

HHuginn::value_t HMapper::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int position_ ) const {
	return (
		thread_->object_factory().create<HMapper>(
			HIterable::get_class(),
			_source->clone( thread_, const_cast<HHuginn::value_t*>( &_source ), position_ ),
			_callable
		)
	);
}

class HMapperIterator : public HIteratorInterface {
protected:
	huginn::HIterable::iterator_t _impl;
	HHuginn::value_t& _callable;
public:
	HMapperIterator( huginn::HIterable::iterator_t&& iterator_, HHuginn::value_t& callable_ )
		: _impl( yaal::move( iterator_ ) )
		, _callable( callable_ ) {
		return;
	}
protected:
	virtual bool do_is_valid( HThread* thread_, int position_ ) override {
		return ( thread_->can_continue() && _impl->is_valid( thread_, position_ ) );
	}
	virtual void do_next( HThread* thread_, int position_ ) override {
		_impl->next( thread_, position_ );
	}
	virtual HHuginn::value_t do_value( HThread* thread_, int position_ ) override {
		return ( _callable->operator_call( thread_, _callable, HArguments( thread_, _impl->value( thread_, position_ ) ), position_ ) );
	}
private:
	HMapperIterator( HMapperIterator const& ) = delete;
	HMapperIterator& operator = ( HMapperIterator const& ) = delete;
};

HMapper::iterator_t HMapper::do_iterator( HThread* thread_, int position_ ) {
	iterator_t impl( hcore::make_pointer<HMapperIterator>( static_cast<huginn::HIterable*>( _source.raw() )->iterator( thread_, position_ ), _callable ) );
	return ( impl );
}

}

}

}

