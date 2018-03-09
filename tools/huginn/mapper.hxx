/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_MAPPER_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_MAPPER_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "tools/huginn/iterator.hxx"
#include "tools/huginn/thread.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HMapper : public HHuginn::HIterable {
	HHuginn::value_t _source;
	HHuginn::function_t _function;
	HHuginn::value_t _method;
public:
	HMapper( HHuginn::HClass const* class_, HHuginn::value_t source_, HHuginn::function_t function_, HHuginn::value_t method_ )
		: HIterable( class_ )
		, _source( source_ )
		, _function( function_ )
		, _method( method_ ) {
	}
	static HHuginn::class_t get_class( HRuntime* runtime_ ) {
		M_PROLOG
		HHuginn::class_t c(
			runtime_->create_class(
				"Mapper",
				nullptr,
				"The `Mapper` class represent *lazy* transformation of one *iterable* into another. Resulting *iterable* produces values from original *iterable* transformed by user supplied unary function."
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
		return ( thread_->object_factory().create<HMapper>( HIterable::get_class(), _source, _function, _method ) );
	}
};

class HMapperIterator : public HIteratorInterface {
protected:
	HHuginn::HIterable::HIterator _impl;
public:
	HMapperIterator( HHuginn::HIterable::HIterator&& iterator_ )
		: _impl( yaal::move( iterator_ ) ) {
		return;
	}
protected:
	virtual bool do_is_valid( HThread* thread_, int position_ ) override {
		return ( _impl.is_valid( thread_, position_ ) );
	}
	virtual void do_next( HThread* thread_, int position_ ) override {
		_impl.next( thread_, position_ );
	}
private:
	HMapperIterator( HMapperIterator const& ) = delete;
	HMapperIterator& operator = ( HMapperIterator const& ) = delete;
};

class HFunctionMapperIterator : public HMapperIterator {
	HHuginn::function_t const& _function;
public:
	HFunctionMapperIterator( HHuginn::HIterable::HIterator&& iterator_, HHuginn::function_t const& function_ )
		: HMapperIterator( yaal::move( iterator_ ) )
		, _function( function_ ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( HThread* thread_, int position_ ) override {
		return ( _function( thread_, nullptr, HArguments( thread_, _impl.value( thread_, position_ ) ), position_ ) );
	}
};

class HUnboundMethodMapperIterator : public HMapperIterator {
	HHuginn::HClass::HUnboundMethod& _method;
public:
	HUnboundMethodMapperIterator( HHuginn::HIterable::HIterator&& iterator_, HHuginn::HClass::HUnboundMethod& method_ )
		: HMapperIterator( yaal::move( iterator_ ) )
		, _method( method_ ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( HThread* thread_, int position_ ) override {
		return ( _method.call( thread_, HArguments( thread_, _impl.value( thread_, position_ ) ), position_ ) );
	}
};

class HBoundMethodMapperIterator : public HMapperIterator {
	HHuginn::HClass::HBoundMethod& _method;
public:
	HBoundMethodMapperIterator( HHuginn::HIterable::HIterator&& iterator_, HHuginn::HClass::HBoundMethod& method_ )
		: HMapperIterator( yaal::move( iterator_ ) )
		, _method( method_ ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( HThread* thread_, int position_ ) override {
		return ( _method.call( thread_, HArguments( thread_, _impl.value( thread_, position_ ) ), position_ ) );
	}
};

HMapper::HIterator HMapper::do_iterator( HThread* thread_, int position_ ) {
	HIterator::iterator_implementation_t impl;
	if ( !! _function ) {
		impl.reset( new ( memory::yaal ) HFunctionMapperIterator( static_cast<HHuginn::HIterable*>( _source.raw() )->iterator( thread_, position_ ), _function ) );
	} else if ( _method->type_id() == HHuginn::TYPE::UNBOUND_METHOD ) {
		impl.reset( new ( memory::yaal ) HUnboundMethodMapperIterator( static_cast<HHuginn::HIterable*>( _source.raw() )->iterator( thread_, position_ ), *static_cast<HHuginn::HClass::HUnboundMethod*>( _method.raw() ) ) );
	} else {
		impl.reset( new ( memory::yaal ) HBoundMethodMapperIterator( static_cast<HHuginn::HIterable*>( _source.raw() )->iterator( thread_, position_ ), *static_cast<HHuginn::HClass::HBoundMethod*>( _method.raw() ) ) );
	}
	return ( HIterator( yaal::move( impl ) ) );
}

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_MAPPER_HXX_INCLUDED */

