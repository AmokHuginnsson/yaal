/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  operatingsystem.cxx - this file is integral part of `yaal' project.

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

#include <cstdlib>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/hhuginn.hxx"
#include "tools/huginn/thread.hxx"
#include "helper.hxx"
#include "exception.hxx"
#include "packagefactory.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

class HOperatingSystem : public HHuginn::HObject {
	HHuginn::class_t _exceptionClass;
public:
	HOperatingSystem( HHuginn::HClass* class_ )
		: HObject( class_ )
		, _exceptionClass( exception::create_class( class_->huginn(), "OperatingSystemException" ) ) {
		return;
	}
	static HHuginn::value_t env( huginn::HThread*, HHuginn::HObject*, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		char const name[] = "OperatingSystem.env";
		verify_arg_count( name, values_, 1, 1, position_ );
		verify_arg_type( name, values_, 0, HHuginn::TYPE::STRING, true, position_ );
		char const* val( ::getenv( get_string( values_[0] ).raw() ) );
		return ( val ? pointer_static_cast<HHuginn::HValue>( make_pointer<HHuginn::HString>( val ) ) : _none_ );
		M_EPILOG
	}
};

namespace package_factory {

class HOperatingSystemCreator : public HPackageCreatorInterface {
protected:
	virtual HHuginn::value_t do_new_instance( HHuginn* );
} operatingsystemCreator;

HHuginn::value_t HOperatingSystemCreator::do_new_instance( HHuginn* huginn_ ) {
	M_PROLOG
	HHuginn::type_t t( HHuginn::HType::register_type( "OperatingSystem", huginn_ ) );
	HHuginn::class_t c(
		make_pointer<HHuginn::HClass>(
			huginn_,
			t,
			nullptr,
			HHuginn::HClass::field_names_t{
				"env"
			},
			HHuginn::values_t{
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HOperatingSystem::env, _1, _2, _3, _4 ) )
			}
		)
	);
	huginn_->register_class( c );
	return ( make_pointer<HOperatingSystem>( c.raw() ) );
	M_EPILOG
}

namespace {

bool registrar( void ) {
	M_PROLOG
	bool volatile failed = false;
	HPackageFactory& factory = HPackageFactoryInstance::get_instance();
	factory.register_package_creator( "OperatingSystem", &operatingsystemCreator );
	return ( failed );
	M_EPILOG
}

bool volatile registered = registrar();

}

}

}

}

}

