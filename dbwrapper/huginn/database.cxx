/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  database.cxx - this file is integral part of `yaal' project.

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
#include "tools/huginn/thread.hxx"
#include "tools/huginn/helper.hxx"
#include "tools/huginn/exception.hxx"
#include "tools/huginn/packagefactory.hxx"
#include "databaseconnection.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::huginn;

namespace yaal {

namespace dbwrapper {

namespace huginn {

class HDatabase : public HHuginn::HObject {
	HHuginn::class_t _exceptionClass;
	HHuginn::class_t _databaseConnectionClass;
public:
	HDatabase( HHuginn::HClass* class_ )
		: HObject( class_ )
		, _exceptionClass( exception::create_class( class_->huginn(), "DatabaseException" ) )
		, _databaseConnectionClass( HDatabaseConnection::get_class( class_->huginn(), _exceptionClass ) ) {
		class_->huginn()->register_class( _databaseConnectionClass );
		return;
	}
	static HHuginn::value_t connect( tools::huginn::HThread* thread_, HHuginn::HObject* object_, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		return ( static_cast<HDatabase*>( object_ )->do_connect( thread_, values_, position_ ) );
		M_EPILOG
	}
private:
	HHuginn::value_t do_connect( tools::huginn::HThread* thread_, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Database.connect";
		verify_arg_count( name, values_, 1, 1, position_ );
		verify_arg_type( name, values_, 0, HHuginn::TYPE::STRING, true, position_ );
		return ( _databaseConnectionClass->create_instance( thread_, nullptr, values_, position_ ) );
		M_EPILOG
	}
};

namespace package_factory {

class HDatabaseCreator : public HPackageCreatorInterface {
protected:
	virtual HHuginn::value_t do_new_instance( HHuginn* );
} databaseCreator;

HHuginn::value_t HDatabaseCreator::do_new_instance( HHuginn* huginn_ ) {
	M_PROLOG
	HHuginn::type_t t( HHuginn::HType::register_type( "Database", huginn_ ) );
	HHuginn::class_t c(
		make_pointer<HHuginn::HClass>(
			huginn_,
			t,
			nullptr,
			HHuginn::HClass::field_names_t{
				"connect"
			},
			HHuginn::values_t{
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HDatabase::connect, _1, _2, _3, _4 ) )
			}
		)
	);
	huginn_->register_class( c );
	return ( make_pointer<HDatabase>( c.raw() ) );
	M_EPILOG
}

namespace {

bool registrar( void ) {
	M_PROLOG
	bool volatile failed = false;
	HPackageFactory& factory = HPackageFactoryInstance::get_instance();
	factory.register_package_creator( "Database", &databaseCreator );
	return ( failed );
	M_EPILOG
}

bool volatile registered = registrar();

}

}

}

}

}

