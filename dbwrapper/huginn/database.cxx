/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "config.hxx"

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/hhuginn.hxx"
#include "tools/huginn/runtime.hxx"
#include "tools/huginn/thread.hxx"
#include "tools/huginn/helper.hxx"
#include "tools/huginn/exception.hxx"
#include "tools/huginn/objectfactory.hxx"
#include "tools/huginn/packagefactory.hxx"
#include "databaseconnection.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::huginn;

namespace yaal {

namespace dbwrapper {

namespace huginn {

class HDatabase : public HValue {
	HHuginn::class_t _databaseConnectionClass;
	HHuginn::class_t _exceptionClass;
public:
	HDatabase( HClass* class_ )
		: HValue( class_ )
		, _databaseConnectionClass(
			add_class_as_type_reference(
				class_,
				HDatabaseConnection::get_class( class_->runtime(), class_, _exceptionClass )
			)
		)
		, _exceptionClass( class_exception( class_ ) ) {
		return;
	}
	static HHuginn::value_t connect( tools::huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		return ( static_cast<HDatabase*>( object_->raw() )->do_connect( thread_, values_, position_ ) );
		M_EPILOG
	}
private:
	HHuginn::value_t do_connect( tools::huginn::HThread* thread_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "Database.connect", values_, { HHuginn::TYPE::STRING }, thread_, position_ );
		return ( _databaseConnectionClass->create_instance( thread_, nullptr, values_, position_ ) );
		M_EPILOG
	}
};

namespace package_factory {

class HDatabaseCreator : public HPackageCreatorInterface {
protected:
	virtual HInstance do_new_instance( HRuntime* );
} databaseCreator;

HPackageCreatorInterface::HInstance HDatabaseCreator::do_new_instance( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"Database",
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
			".",
			HHuginn::ACCESS::PRIVATE
		)
	);
	HHuginn::field_definitions_t fd{
		{ "connect", runtime_->create_method( &HDatabase::connect ), "( *DSN* ) - create connection to the database specified by given *DSN*" }
	};
	c->redefine( nullptr, fd );
	return { c, runtime_->object_factory()->create<HDatabase>( c.raw() ) };
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

