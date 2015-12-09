/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  mathematics.cxx - this file is integral part of `yaal' project.

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

#include <cmath>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/hhuginn.hxx"
#include "tools/huginn/thread.hxx"
#include "helper.hxx"
#include "exception.hxx"
#include "objectfactory.hxx"
#include "packagefactory.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

class HMathematics : public HHuginn::HObject {
	HHuginn::class_t _exceptionClass;
public:
	HMathematics( HHuginn::HClass* class_ )
		: HObject( class_ )
		, _exceptionClass( exception::create_class( class_->huginn(), "MathematicsException" ) ) {
		return;
	}
	static HHuginn::value_t square_root( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.square_root";
		verify_arg_count( name, values_, 1, 1, position_ );
		HHuginn::type_t t( verify_arg_numeric( name, values_, 0, true, position_ ) );
		HHuginn::value_t v( thread_->huginn().none_value() );
		if ( t == HHuginn::TYPE::NUMBER ) {
			HNumber val( get_number( values_[0] ) );
			if ( val >= 0 ) {
				v = thread_->object_factory().create_number( hcore::square_root( val ) );
			}
		} else {
			double long val( get_real( values_[0] ) );
			if ( val >= 0 ) {
				v = thread_->object_factory().create_real( yaal::square_root( val ) );
			}
		}
		if ( v == thread_->huginn().none_value() ) {
			thread_->raise( static_cast<HMathematics*>( object_->raw() )->_exceptionClass.raw(), "bad domain", position_ );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t round( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.round";
		verify_arg_count( name, values_, 1, 2, position_ );
		HHuginn::type_t t( verify_arg_numeric( name, values_, 0, false, position_ ) );
		int to( 0 );
		if ( values_.get_size() > 1 ) {
			verify_arg_type( name, values_, 1, HHuginn::TYPE::INTEGER, false, position_ );
			to = static_cast<int>( get_integer( values_[1] ) );
		}
		HHuginn::value_t v( thread_->huginn().none_value() );
		if ( t == HHuginn::TYPE::NUMBER ) {
			HNumber val( get_number( values_[0] ) );
			try {
				v = thread_->object_factory().create_number( val.round( to ) );
			} catch ( HNumberException const& e ) {
				thread_->raise( static_cast<HMathematics*>( object_->raw() )->_exceptionClass.raw(), e.what(), position_ );
			}
		} else {
			if ( to == 0 ) {
				double long val( get_real( values_[0] ) );
				v = thread_->object_factory().create_real( std::roundl( val ) );
			} else {
				thread_->raise( static_cast<HMathematics*>( object_->raw() )->_exceptionClass.raw(), "rounding to nth place on real not supported", position_ );
			}
		}
		return ( v );
		M_EPILOG
	}
};

namespace package_factory {

class HMathematicsCreator : public HPackageCreatorInterface {
protected:
	virtual HHuginn::value_t do_new_instance( HHuginn* );
} mathematicsCreator;

HHuginn::value_t HMathematicsCreator::do_new_instance( HHuginn* huginn_ ) {
	M_PROLOG
	HHuginn::type_t t( HHuginn::HType::register_type( "Mathematics", huginn_ ) );
	HHuginn::class_t c(
		make_pointer<HHuginn::HClass>(
			huginn_,
			t,
			nullptr,
			HHuginn::HClass::field_names_t{
				"square_root",
				"round"
			},
			HHuginn::values_t{
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HMathematics::square_root, _1, _2, _3, _4 ) ),
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HMathematics::round, _1, _2, _3, _4 ) )
			}
		)
	);
	huginn_->register_class( c );
	return ( make_pointer<HMathematics>( c.raw() ) );
	M_EPILOG
}

namespace {

bool registrar( void ) {
	M_PROLOG
	bool volatile failed = false;
	HPackageFactory& factory = HPackageFactoryInstance::get_instance();
	factory.register_package_creator( "Mathematics", &mathematicsCreator );
	return ( failed );
	M_EPILOG
}

bool volatile registered = registrar();

}

}

}

}

}

