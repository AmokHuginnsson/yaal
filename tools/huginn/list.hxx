/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  list.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_TOOLS_HUGINN_LIST_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_LIST_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "tools/huginn/runtime.hxx"
#include "tools/huginn/objectfactory.hxx"
#include "tools/huginn/iterator.hxx"

namespace yaal {

namespace tools {

namespace huginn {

namespace list {

HHuginn::value_t sort( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );

}

class HListReverseIterator : public HIteratorInterface {
	HHuginn::HList* _list;
	int long _index;
public:
	HListReverseIterator( HThread* thread_, HHuginn::HList* list_, int position_ )
		: _list( list_ )
		, _index( list_->size( thread_, position_ ) - 1 ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( HThread*, int ) override {
		return ( _list->get( _index ) );
	}
	virtual bool do_is_valid( HThread*, int ) override {
		return ( _index >= 0 );
	}
	virtual void do_next( HThread*, int ) override {
		-- _index;
	}
private:
	HListReverseIterator( HListReverseIterator const& ) = delete;
	HListReverseIterator& operator = ( HListReverseIterator const& ) = delete;
};

class HReversedList : public HHuginn::HIterable {
	HHuginn::value_t _list;
public:
	HReversedList( HHuginn::HClass const* class_, HHuginn::value_t const& list_ )
		: HIterable( class_ )
		, _list( list_ ) {
		M_ASSERT( _list->type_id() == HHuginn::TYPE::LIST );
	}
	static HHuginn::class_t get_class( HRuntime* runtime_ ) {
		M_PROLOG
		HHuginn::class_t c(
			runtime_->create_class(
				"ReversedListView",
				nullptr,
				HHuginn::field_definitions_t{},
				"The `ReversedListView` class represents *lazy* *iterable* reversed view of a `list`."
			)
		);
		runtime_->huginn()->register_class( c );
		return ( c );
		M_EPILOG
	}
protected:
	virtual int long do_size( huginn::HThread* thread_, int position_ ) const override {
		return ( safe_int::cast<int long>( static_cast<HHuginn::HList const*>( _list.raw() )->size( thread_, position_ ) ) );
	}
private:
	virtual HIterator do_iterator( HThread* thread_, int position_ ) override {
		HIterator::iterator_implementation_t impl( new ( memory::yaal ) HListReverseIterator( thread_, static_cast<HHuginn::HList*>( _list.raw() ), position_ ) );
		return ( HIterator( yaal::move( impl ) ) );
	}
private:
	virtual HHuginn::value_t do_clone( huginn::HThread* thread_, HHuginn::value_t*, int ) const override {
		return ( thread_->object_factory().create<HReversedList>( HIterable::get_class(), _list ) );
	}
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_LIST_HXX_INCLUDED */

