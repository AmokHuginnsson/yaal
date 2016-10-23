/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  range.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_TOOLS_HUGINN_RANGE_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_RANGE_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "tools/huginn/iterator.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HRange : public HHuginn::HIterable {
	int long long _from;
	int long long _stop;
	int long long _step;
public:
	HRange( HHuginn::HClass const* class_, int long long from_, int long long stop_, int long long step_ )
		: HIterable( class_ )
		, _from( from_ )
		, _stop( stop_ )
		, _step( step_ ) {
	}
	int long long step( void ) const {
		return ( _step );
	}
	int long long stop( void ) const {
		return ( _stop );
	}
	static HHuginn::class_t get_class( HRuntime* runtime_ ) {
		M_PROLOG
		return (
			runtime_->create_class(
				"Range",
				nullptr,
				HHuginn::field_definitions_t{},
				"The `Range` class represents *lazy* *iterable* range of integer values."
			)
		);
		M_EPILOG
	}
protected:
	virtual int long do_size( void ) const override {
		return ( safe_cast<int long>( ( _stop + _step - ( _from + 1 ) ) / _step ) );
	}
private:
	virtual HIterator do_iterator( HThread*, int ) override;
private:
	virtual HHuginn::value_t do_clone( huginn::HRuntime* ) const override {
		return ( yaal::hcore::make_pointer<HRange>( HIterable::get_class(), _from, _stop, _step ) );
	}
};

class HRangeIterator : public HIteratorInterface {
	int long long _i;
	HRange const* _range;
	HObjectFactory* _objectFactory;
public:
	HRangeIterator( int long long from_, HRange const* range_ )
		: _i( from_ )
		, _range( range_ )
		, _objectFactory( range_->HIterable::get_class()->runtime()->object_factory() ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( HThread*, int ) override {
		return ( _objectFactory->create_integer( _i ) );
	}
	virtual bool do_is_valid( void ) override {
		return ( _range->step() > 0 ? _i < _range->stop() : _i > _range->stop() );
	}
	virtual void do_next( HThread*, int ) override {
		_i += _range->step();
	}
private:
	HRangeIterator( HRangeIterator const& ) = delete;
	HRangeIterator& operator = ( HRangeIterator const& ) = delete;
};

HRange::HIterator HRange::do_iterator( HThread*, int ) {
	HIterator::iterator_implementation_t impl( new ( memory::yaal ) HRangeIterator( _from, this ) );
	return ( HIterator( yaal::move( impl ) ) );
}

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_RANGE_HXX_INCLUDED */

