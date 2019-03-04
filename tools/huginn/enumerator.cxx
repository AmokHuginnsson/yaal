/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "enumerator.hxx"
#include "runtime.hxx"
#include "thread.hxx"
#include "helper.hxx"

namespace yaal {

namespace tools {

namespace huginn {

HHuginn::class_t HEnumerator::get_class( HRuntime* runtime_, huginn::HClass const* origin_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"Enumerator",
			"The `Enumerator` class allows counting elements of *iterable* that are iterated over.",
			HHuginn::ACCESS::PRIVATE
		)
	);
	c->set_origin( origin_ );
	runtime_->huginn()->register_class( c );
	return ( c );
	M_EPILOG
}

HHuginn::value_t HEnumerator::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int position_ ) const {
	return ( thread_->object_factory().create<HEnumerator>( HIterable::get_class(), _source->clone( thread_, const_cast<HHuginn::value_t*>( &_source ), position_ ), _start ) );
}

class HEnumeratorIterator : public HIteratorInterface {
protected:
	huginn::HIterable::iterator_t _impl;
	huginn::HInteger::value_type _counter;
public:
	HEnumeratorIterator( huginn::HIterable::iterator_t&& iterator_, huginn::HInteger::value_type start_ )
		: _impl( yaal::move( iterator_ ) )
		, _counter( start_ ) {
		return;
	}
protected:
	virtual bool do_is_valid( HThread* thread_, int position_ ) override {
		return ( _impl->is_valid( thread_, position_ ) );
	}
	virtual void do_next( HThread* thread_, int position_ ) override {
		_impl->next( thread_, position_ );
		++ _counter;
	}
	virtual HHuginn::value_t do_value( HThread* thread_, int position_ ) override {
		HObjectFactory& of( thread_->object_factory() );
		return (
			of.create_tuple(
				HHuginn::values_t( {
					of.create_integer( _counter ),
					_impl->value( thread_, position_ )
				} )
			)
		);
	}
private:
	HEnumeratorIterator( HEnumeratorIterator const& ) = delete;
	HEnumeratorIterator& operator = ( HEnumeratorIterator const& ) = delete;
};

HEnumerator::iterator_t HEnumerator::do_iterator( HThread* thread_, int position_ ) {
	return ( hcore::make_pointer<HEnumeratorIterator>( static_cast<huginn::HIterable*>( _source.raw() )->iterator( thread_, position_ ), _start ) );
}

}

}

}

