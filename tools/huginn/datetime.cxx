/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  datetime.cxx - this file is integral part of `yaal' project.

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
#include "runtime.hxx"
#include "tools/huginn/thread.hxx"
#include "tools/huginn/time.hxx"
#include "tools/huginn/clock.hxx"
#include "helper.hxx"
#include "exception.hxx"
#include "packagefactory.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

class HDateTime : public HHuginn::HObject {
	HHuginn::class_t _timeClass;
	HHuginn::class_t _clockClass;
	HHuginn::class_t _exceptionClass;
public:
	HDateTime( HHuginn::HClass* class_ )
		: HObject( class_ )
		, _timeClass( HTime::get_class( class_->runtime() ) )
		, _clockClass( HClock::get_class( class_->runtime() ) )
		, _exceptionClass( exception::create_class( class_->runtime(), "DateTimeException" ) ) {
		class_->runtime()->huginn()->register_class( _timeClass );
		class_->runtime()->huginn()->register_class( _clockClass );
		return;
	}
	static HHuginn::value_t now( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( "DateTime.now", values_, 0, 0, position_ );
		HDateTime* dt( dynamic_cast<HDateTime*>( object_->raw() ) );
		M_ASSERT( dt );
		return ( make_pointer<huginn::HTime>( dt->_timeClass.raw(), now_local() ) );
		M_EPILOG
	}
	static HHuginn::value_t clock( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( "DateTime.clock", values_, 0, 0, position_ );
		HDateTime* dt( dynamic_cast<HDateTime*>( object_->raw() ) );
		M_ASSERT( dt );
		return ( make_pointer<HClock>( dt->_clockClass.raw() ) );
		M_EPILOG
	}
};

namespace package_factory {

class HDateTimeCreator : public HPackageCreatorInterface {
protected:
	virtual HHuginn::value_t do_new_instance( HRuntime* );
} datetimeCreator;

HHuginn::value_t HDateTimeCreator::do_new_instance( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"DateTime",
			nullptr,
			HHuginn::field_definitions_t{
				{ "now", make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HDateTime::now, _1, _2, _3, _4 ) ) },
				{ "clock", make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HDateTime::clock, _1, _2, _3, _4 ) ) }
			}
		)
	);
	runtime_->huginn()->register_class( c );
	return ( make_pointer<HDateTime>( c.raw() ) );
	M_EPILOG
}

namespace {

bool registrar( void ) {
	M_PROLOG
	bool volatile failed = false;
	HPackageFactory& factory = HPackageFactoryInstance::get_instance();
	factory.register_package_creator( "DateTime", &datetimeCreator );
	return ( failed );
	M_EPILOG
}

bool volatile registered = registrar();

}

}

}

}

}

