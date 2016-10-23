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

#include "config.hxx"

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/hhuginn.hxx"
#include "tools/huginn/runtime.hxx"
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
		, _exceptionClass( exception::create_class( class_->runtime(), "DatabaseException", "The `DatabaseException` exception type for `Database` package." ) )
		, _databaseConnectionClass( HDatabaseConnection::get_class( class_->runtime(), _exceptionClass ) ) {
		return;
	}
	static HHuginn::value_t connect( tools::huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		return ( static_cast<HDatabase*>( object_->raw() )->do_connect( thread_, values_, position_ ) );
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
	virtual HHuginn::value_t do_new_instance( HRuntime* );
} databaseCreator;

HHuginn::value_t HDatabaseCreator::do_new_instance( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"Database",
			nullptr,
			HHuginn::field_definitions_t{
				{ "connect", make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HDatabase::connect, _1, _2, _3, _4 ) ), "( *DSN* ) - create connection to the database specified by given *DSN*" }
			},
			"The `Database` package provides access to various database engines through database agnostic interface. Following drivers are supported:"
#ifdef HAVE_LIBSQLITE3
			" SQLite3"
#endif
#ifdef HAVE_LIBPQ
			" PostgreSQL"
#endif
#ifdef HAVE_LIBMYSQLCLIENT
			" MySQL"
#endif
#ifdef HAVE_LIBFBCLIENT
			" Firebird"
#endif
#ifdef HAVE_LIBCLNTSH
			" Oracle"
#endif
			"."
		)
	);
	runtime_->huginn()->register_class( c );
	return ( make_pointer<HDatabase>( c.raw() ) );
	M_EPILOG
}

namespace {

bool registrar( void ) {
	M_PROLOG
	bool volatile failed = false;
	HPackageFactory& factory = HPackageFactory::get_instance();
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

