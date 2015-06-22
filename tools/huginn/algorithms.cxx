/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  algorithms.cxx - this file is integral part of `yaal' project.

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

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/hhuginn.hxx"
#include "iterator.hxx"
#include "helper.hxx"
#include "packagefactory.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

class HRangeIterator : public HIteratorInterface {
	int _i;
	int _stop;
	int _step;
public:
	HRangeIterator( int from_, int stop_, int step_ )
		: _i( from_ )
		, _stop( stop_ )
		, _step( step_ ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( void ) override {
		return ( make_pointer<HHuginn::HInteger>( _i ) );
	}
	virtual bool do_is_valid( void ) override {
		return ( _i < _stop );
	}
	virtual void do_next( void ) override {
		_i += _step;
	}
private:
	HRangeIterator( HRangeIterator const& ) = delete;
	HRangeIterator& operator = ( HRangeIterator const& ) = delete;
};

class HRange : public HHuginn::HIterable {
	int _from;
	int _stop;
	int _step;
public:
	HRange( HHuginn::HClass* class_, int from_, int stop_, int step_ )
		: HIterable( class_ )
		, _from( from_ )
		, _stop( stop_ )
		, _step( step_ ) {
	}
	static HHuginn::class_t get_class( HHuginn*  huginn_ ) {
		M_PROLOG
		HHuginn::class_t c(
			make_pointer<HHuginn::HClass>(
				huginn_,
				HHuginn::HType::register_type( "Range", huginn_ ),
				nullptr,
				HHuginn::HClass::field_names_t{},
				HHuginn::values_t{}
			)
		);
		return ( c );
		M_EPILOG
	}
private:
	virtual HIterator do_iterator( void ) override {
		HIterator::iterator_implementation_t impl( new ( memory::yaal ) HRangeIterator( _from, _stop, _step ) );
		return ( HIterator( yaal::move( impl ) ) );
	}
};

class HAlgorithms : public HHuginn::HObject {
	HHuginn::class_t _rangeClass;
public:
	HAlgorithms( HHuginn::HClass* class_ )
		: HObject( class_ )
		, _rangeClass( HRange::get_class( class_->huginn() ) ) {
		class_->huginn()->register_class( _rangeClass );
		return;
	}
	static HHuginn::value_t range( huginn::HThread*, HHuginn::HObject* object_, HHuginn::values_t const& values_, int position_ ) {
		return ( static_cast<HAlgorithms*>( object_ )->do_range( values_, position_ ) );
	}
private:
	HHuginn::value_t do_range( HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Algorithms.range";
		verify_arg_count( name, values_, 1, 3, position_ );
		verify_arg_type( name, values_, 0, HHuginn::TYPE::INTEGER, false, position_ );
		int from( 0 );
		int stop( 0 );
		int step( 1 );
		int s( static_cast<int>( values_.get_size() ) );
		switch ( s ) {
			case ( 1 ): {
				stop = static_cast<int>( get_integer( values_[0] ) );
			} break;
			case ( 3 ): {
				step = static_cast<int>( get_integer( values_[2] ) );
			} /* fall-through */
			case ( 2 ): {
				from = static_cast<int>( get_integer( values_[0] ) );
				stop = static_cast<int>( get_integer( values_[1] ) );
			} break;
		}
		return ( make_pointer<HRange>( _rangeClass.raw(), from, stop, step ) );
		M_EPILOG
	}
};

namespace package_factory {

class HAlgorithmsCreator : public HPackageCreatorInterface {
protected:
	virtual HHuginn::value_t do_new_instance( HHuginn* );
} algorithmsCreator;

HHuginn::value_t HAlgorithmsCreator::do_new_instance( HHuginn* huginn_ ) {
	M_PROLOG
	HHuginn::type_t t( HHuginn::HType::register_type( "Algorithms", huginn_ ) );
	HHuginn::class_t c(
		make_pointer<HHuginn::HClass>(
			huginn_,
			t,
			nullptr,
			HHuginn::HClass::field_names_t{
				"range"
			},
			HHuginn::values_t{
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HAlgorithms::range, _1, _2, _3, _4 ) )
			}
		)
	);
	huginn_->register_class( c );
	return ( make_pointer<HAlgorithms>( c.raw() ) );
	M_EPILOG
}

namespace {

bool registrar( void ) {
	M_PROLOG
	bool volatile failed = false;
	HPackageFactory& factory = HPackageFactoryInstance::get_instance();
	factory.register_package_creator( "Algorithms", &algorithmsCreator );
	return ( failed );
	M_EPILOG
}

bool volatile registered = registrar();

}

}

}

}

}

