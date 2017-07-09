/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  set.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_TOOLS_HUGINN_SET_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_SET_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "tools/huginn/iterator.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HReversedSet : public HHuginn::HIterable {
	HHuginn::value_t _set;
public:
	HReversedSet( HHuginn::HClass const* class_, HHuginn::value_t const& set_ )
		: HIterable( class_ )
		, _set( set_ ) {
		M_ASSERT( _set->type_id() == HHuginn::TYPE::SET );
	}
	static HHuginn::class_t get_class( HRuntime* runtime_ ) {
		M_PROLOG
		HHuginn::class_t c(
			runtime_->create_class(
				"ReversedSetView",
				nullptr,
				HHuginn::field_definitions_t{},
				"The `ReversedSetView` class represents *lazy* *iterable* reversed view of a `set`."
			)
		);
		runtime_->huginn()->register_class( c );
		return ( c );
		M_EPILOG
	}
protected:
	virtual int long do_size( huginn::HThread* thread_, int position_ ) const override {
		return ( safe_int::cast<int long>( static_cast<HHuginn::HSet const*>( _set.raw() )->size( thread_, position_ ) ) );
	}
private:
	virtual HIterator do_iterator( HThread*, int ) override;
private:
	virtual HHuginn::value_t do_clone( huginn::HThread* thread_, int ) const override {
		return ( thread_->object_factory().create<HReversedSet>( HIterable::get_class(), _set ) );
	}
};

class HSetReverseIterator : public HIteratorInterface {
	HHuginn::HSet::values_t* _set;
	HHuginn::HSet::values_t::reverse_iterator _it;
public:
	HSetReverseIterator( HHuginn::HSet::values_t* set_ )
		: _set( set_ ), _it( set_->rbegin() ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( HThread*, int ) override {
		return ( *_it );
	}
	virtual bool do_is_valid( HThread*, int ) override {
		return ( _it != _set->rend() );
	}
	virtual void do_next( HThread*, int ) override {
		++ _it;
	}
private:
	HSetReverseIterator( HSetReverseIterator const& ) = delete;
	HSetReverseIterator& operator = ( HSetReverseIterator const& ) = delete;
};


HReversedSet::HIterator HReversedSet::do_iterator( HThread*, int ) {
	HIterator::iterator_implementation_t impl( new ( memory::yaal ) HSetReverseIterator( &static_cast<HHuginn::HSet*>( _set.raw() )->value() ) );
	return ( HIterator( yaal::move( impl ) ) );
}

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_SET_HXX_INCLUDED */

