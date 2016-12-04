/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  filter.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_TOOLS_HUGINN_FILTER_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_FILTER_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "tools/huginn/iterator.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HFilter : public HHuginn::HIterable {
	HHuginn::value_t _source;
	HHuginn::function_t _function;
	HHuginn::value_t _method;
public:
	HFilter( HHuginn::HClass const* class_, HHuginn::value_t source_, HHuginn::function_t function_, HHuginn::value_t method_ )
		: HIterable( class_ )
		, _source( source_ )
		, _function( function_ )
		, _method( method_ ) {
	}
	static HHuginn::class_t get_class( HRuntime* runtime_ ) {
		M_PROLOG
		return (
			runtime_->create_class(
				"Filter",
				nullptr,
				HHuginn::field_definitions_t{},
				"The `Filter` class represents a *lazy* filtering transformation of one *iterable* into another. Resulting *iterable* produces only values matching predicate."
			)
		);
		M_EPILOG
	}
protected:
	virtual int long do_size( void ) const override {
		throw HHuginn::HHuginnRuntimeException( "Getting size of `Filter' is an invalid operation.", 0 );
	}
private:
	virtual HIterator do_iterator( HThread*, int ) override;
private:
	virtual HHuginn::value_t do_clone( huginn::HRuntime* ) const override {
		return ( yaal::hcore::make_pointer<HFilter>( HIterable::get_class(), _source, _function, _method ) );
	}
};

class HFilterIterator : public HIteratorInterface {
protected:
	HHuginn::HIterable::HIterator _impl;
	HHuginn::value_t _value;
public:
	HFilterIterator( HHuginn::HIterable::HIterator&& iterator_ )
		: _impl( yaal::move( iterator_ ) )
		, _value() {
		return;
	}
protected:
	virtual bool do_is_valid( void ) override {
		return ( _impl.is_valid() );
	}
	virtual void do_next( HThread* thread_, int position_ ) override {
		_impl.next( thread_, position_ );
		scan( thread_, position_ );
	}
	virtual HHuginn::value_t do_value( HThread*, int ) override {
		return ( _value );
	}
	void scan( HThread* thread_, int position_ ) {
		while ( _impl.is_valid() ) {
			_value = _impl.value( thread_, position_ );
			if ( do_test( thread_, position_ ) ) {
				break;
			}
			_impl.next( thread_, position_ );
		}
	}
	virtual bool do_test( HThread*, int ) = 0;
private:
	HFilterIterator( HFilterIterator const& ) = delete;
	HFilterIterator& operator = ( HFilterIterator const& ) = delete;
};

class HFunctionFilterIterator : public HFilterIterator {
	HHuginn::function_t const& _function;
public:
	HFunctionFilterIterator( HHuginn::HIterable::HIterator&& iterator_, HHuginn::function_t const& function_, HThread* thread_, int position_ )
		: HFilterIterator( yaal::move( iterator_ ) )
		, _function( function_ ) {
		scan( thread_, position_ );
		return;
	}
protected:
	virtual bool do_test( HThread* thread_, int position_ ) override {
		HHuginn::value_t v( _function( thread_, nullptr, HHuginn::values_t( { _value } ), position_ ) );
		if ( v->type_id() != HHuginn::TYPE::BOOLEAN ) {
			throw HHuginn::HHuginnRuntimeException(
				hcore::to_string( "Filter function returned wrong type, expected `boolean' got: `" ).append( v->get_class()->name() ).append( "'." ),
				position_
			);
		}
		return ( static_cast<HHuginn::HBoolean*>( v.raw() )->value() );
	}
};

class HMethodFilterIterator : public HFilterIterator {
	HHuginn::HClass::HBoundMethod& _method;
public:
	HMethodFilterIterator( HHuginn::HIterable::HIterator&& iterator_, HHuginn::HClass::HBoundMethod& method_, HThread* thread_, int position_ )
		: HFilterIterator( yaal::move( iterator_ ) )
		, _method( method_ ) {
		scan( thread_, position_ );
		return;
	}
protected:
	virtual bool do_test( HThread* thread_, int position_ ) override {
		HHuginn::value_t v( _method.call( thread_, HHuginn::values_t( { _impl.value( thread_, position_ ) } ), position_ ) );
		if ( v->type_id() != HHuginn::TYPE::BOOLEAN ) {
			throw HHuginn::HHuginnRuntimeException(
				hcore::to_string( "Filter functor returned wrong type, expected `boolean' got: `" ).append( v->get_class()->name() ).append( "'." ),
				position_
			);
		}
		return ( static_cast<HHuginn::HBoolean*>( v.raw() )->value() );
	}
};

HFilter::HIterator HFilter::do_iterator( HThread* thread_, int position_ ) {
	HIterator::iterator_implementation_t impl;
	if ( !! _function ) {
		impl.reset( new ( memory::yaal ) HFunctionFilterIterator( static_cast<HHuginn::HIterable*>( _source.raw() )->iterator( thread_, position_ ), _function, thread_, position_ ) );
	} else {
		impl.reset( new ( memory::yaal ) HMethodFilterIterator( static_cast<HHuginn::HIterable*>( _source.raw() )->iterator( thread_, position_ ), *static_cast<HHuginn::HClass::HBoundMethod*>( _method.raw() ), thread_, position_ ) );
	}
	return ( HIterator( yaal::move( impl ) ) );
}

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_FILTER_HXX_INCLUDED */

