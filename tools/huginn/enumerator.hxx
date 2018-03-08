/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_ENUMERATOR_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_ENUMERATOR_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "tools/huginn/iterator.hxx"
#include "tools/huginn/thread.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HEnumerator : public HHuginn::HIterable {
	HHuginn::value_t _source;
	HHuginn::HInteger::value_type _start;
public:
	HEnumerator( HHuginn::HClass const* class_, HHuginn::value_t source_, HHuginn::HInteger::value_type start_ )
		: HIterable( class_ )
		, _source( source_ )
		, _start( start_ ) {
	}
	static HHuginn::class_t get_class( HRuntime* runtime_ ) {
		M_PROLOG
		HHuginn::class_t c(
			runtime_->create_class(
				"Enumerator",
				nullptr,
				"The `Enumerator` class allows counting elements of *iterable* that are iterated over."
			)
		);
		runtime_->huginn()->register_class( c );
		return ( c );
		M_EPILOG
	}
protected:
	virtual int long do_size( huginn::HThread* thread_, int position_ ) const override {
		return ( static_cast<HHuginn::HIterable const*>( _source.raw() )->size( thread_, position_ ) );
	}
private:
	virtual HIterator do_iterator( HThread*, int ) override;
private:
	virtual HHuginn::value_t do_clone( huginn::HThread* thread_, HHuginn::value_t*, int ) const override {
		return ( thread_->object_factory().create<HEnumerator>( HIterable::get_class(), _source, _start ) );
	}
};

class HEnumeratorIterator : public HIteratorInterface {
protected:
	HHuginn::HIterable::HIterator _impl;
	HHuginn::HInteger::value_type _counter;
public:
	HEnumeratorIterator( HHuginn::HIterable::HIterator&& iterator_, HHuginn::HInteger::value_type start_ )
		: _impl( yaal::move( iterator_ ) )
		, _counter( start_ ) {
		return;
	}
protected:
	virtual bool do_is_valid( HThread* thread_, int position_ ) override {
		return ( _impl.is_valid( thread_, position_ ) );
	}
	virtual void do_next( HThread* thread_, int position_ ) override {
		_impl.next( thread_, position_ );
		++ _counter;
	}
	virtual HHuginn::value_t do_value( HThread* thread_, int position_ ) override {
		HObjectFactory& of( thread_->object_factory() );
		return (
			of.create_tuple(
				HHuginn::values_t( {
					of.create_integer( _counter ),
					_impl.value( thread_, position_ )
				} )
			)
		);
	}
private:
	HEnumeratorIterator( HEnumeratorIterator const& ) = delete;
	HEnumeratorIterator& operator = ( HEnumeratorIterator const& ) = delete;
};

HEnumerator::HIterator HEnumerator::do_iterator( HThread* thread_, int position_ ) {
	HIterator::iterator_implementation_t impl;
	impl.reset( new ( memory::yaal ) HEnumeratorIterator( static_cast<HHuginn::HIterable*>( _source.raw() )->iterator( thread_, position_ ), _start ) );
	return ( HIterator( yaal::move( impl ) ) );
}

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_ENUMERATOR_HXX_INCLUDED */

