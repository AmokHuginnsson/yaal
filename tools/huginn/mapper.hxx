/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  mapper.hxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_MAPPER_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_MAPPER_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "tools/huginn/iterator.hxx"

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
				HHuginn::field_definitions_t{},
				"The `Mapper` class represent *lazy* transformation of one *iterable* into another. Resulting *iterable* produces values from original *iterable* transformed by user supplied unary function."
			)
		);
		runtime_->huginn()->register_class( c );
		return ( c );
		M_EPILOG
	}
protected:
	virtual int long do_size( void ) const override {
		return ( static_cast<HHuginn::HIterable const*>( _source.raw() )->size() );
	}
private:
	virtual HIterator do_iterator( HThread*, int ) override;
private:
	virtual HHuginn::value_t do_clone( huginn::HThread* thread_, int ) const override {
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
	virtual bool do_is_valid( void ) override {
		return ( _impl.is_valid() );
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
		return ( _function( thread_, nullptr, HHuginn::values_t( { _impl.value( thread_, position_ ) } ), position_ ) );
	}
};

class HMethodMapperIterator : public HMapperIterator {
	HHuginn::HClass::HBoundMethod& _method;
public:
	HMethodMapperIterator( HHuginn::HIterable::HIterator&& iterator_, HHuginn::HClass::HBoundMethod& method_ )
		: HMapperIterator( yaal::move( iterator_ ) )
		, _method( method_ ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( HThread* thread_, int position_ ) override {
		return ( _method.call( thread_, HHuginn::values_t( { _impl.value( thread_, position_ ) } ), position_ ) );
	}
};

HMapper::HIterator HMapper::do_iterator( HThread* thread_, int position_ ) {
	HIterator::iterator_implementation_t impl;
	if ( !! _function ) {
		impl.reset( new ( memory::yaal ) HFunctionMapperIterator( static_cast<HHuginn::HIterable*>( _source.raw() )->iterator( thread_, position_ ), _function ) );
	} else {
		impl.reset( new ( memory::yaal ) HMethodMapperIterator( static_cast<HHuginn::HIterable*>( _source.raw() )->iterator( thread_, position_ ), *static_cast<HHuginn::HClass::HBoundMethod*>( _method.raw() ) ) );
	}
	return ( HIterator( yaal::move( impl ) ) );
}

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_MAPPER_HXX_INCLUDED */

