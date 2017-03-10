/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  lookup.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_TOOLS_HUGINN_LOOKUP_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_LOOKUP_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "tools/huginn/iterator.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HReversedLookup : public HHuginn::HIterable {
	HHuginn::value_t _lookup;
public:
	HReversedLookup( HHuginn::HClass const* class_, HHuginn::value_t const& lookup_ )
		: HIterable( class_ )
		, _lookup( lookup_ ) {
		M_ASSERT( _lookup->type_id() == HHuginn::TYPE::LOOKUP );
	}
	static HHuginn::class_t get_class( HRuntime* runtime_ ) {
		M_PROLOG
		HHuginn::class_t c(
			runtime_->create_class(
				"ReversedLookupView",
				nullptr,
				HHuginn::field_definitions_t{},
				"The `ReversedLookupView` class represents *lazy* *iterable* reversed view of a `lookup`."
			)
		);
		runtime_->huginn()->register_class( c );
		return ( c );
		M_EPILOG
	}
protected:
	virtual int long do_size( void ) const override {
		return ( safe_int::cast<int long>( static_cast<HHuginn::HLookup const*>( _lookup.raw() )->size() ) );
	}
private:
	virtual HIterator do_iterator( HThread*, int ) override;
private:
	virtual HHuginn::value_t do_clone( huginn::HThread* thread_, int ) const override {
		return ( thread_->object_factory().create<HReversedLookup>( HIterable::get_class(), _lookup ) );
	}
};

class HLookupReverseIterator : public HIteratorInterface {
	HHuginn::HLookup::values_t* _lookup;
	HHuginn::HLookup::values_t::reverse_iterator _it;
public:
	HLookupReverseIterator( HHuginn::HLookup::values_t* lookup_ )
		: _lookup( lookup_ ), _it( lookup_->rbegin() ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( HThread*, int ) override {
		return ( _it->first );
	}
	virtual bool do_is_valid( void ) override {
		return ( _it != _lookup->rend() );
	}
	virtual void do_next( HThread*, int ) override {
		++ _it;
	}
private:
	HLookupReverseIterator( HLookupReverseIterator const& ) = delete;
	HLookupReverseIterator& operator = ( HLookupReverseIterator const& ) = delete;
};


HReversedLookup::HIterator HReversedLookup::do_iterator( HThread*, int ) {
	HIterator::iterator_implementation_t impl( new ( memory::yaal ) HLookupReverseIterator( &static_cast<HHuginn::HLookup*>( _lookup.raw() )->value() ) );
	return ( HIterator( yaal::move( impl ) ) );
}

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_LOOKUP_HXX_INCLUDED */

