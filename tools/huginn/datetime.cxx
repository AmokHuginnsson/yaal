/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/hhuginn.hxx"
#include "runtime.hxx"
#include "tools/huginn/thread.hxx"
#include "tools/huginn/time.hxx"
#include "tools/huginn/clock.hxx"
#include "tools/sleep.hxx"
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

class HDateTime : public HPackage {
	HHuginn::class_t _clockClass;
public:
	HDateTime( HClass* class_ )
		: HPackage( class_ )
		, _clockClass( HClock::get_class( class_->runtime(), class_ ) ) {
		return;
	}
	static HHuginn::value_t now( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( "DateTime.now", values_, 0, 0, thread_, position_ );
		HObjectFactory& of( thread_->object_factory() );
		return ( of.create<huginn::HTime>( of.time_class(), now_local() ) );
		M_EPILOG
	}
	static HHuginn::value_t clock( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( "DateTime.clock", values_, 0, 0, thread_, position_ );
		HDateTime* dt( dynamic_cast<HDateTime*>( object_->raw() ) );
		M_ASSERT( dt );
		return ( thread_->object_factory().create<HClock>( dt->_clockClass.raw() ) );
		M_EPILOG
	}
	static HHuginn::value_t format( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		HObjectFactory& of( thread_->object_factory() );
		HDateTime& dtp( *static_cast<HDateTime*>( object_->raw() ) );
		verify_signature_by_class( "DateTime.format", values_, { of.string_class(), of.time_class() }, thread_, position_ );
		hcore::HString const& fmt( get_string( values_[0] ) );
		HTime const& dt( *static_cast<HTime*>( values_[1].raw() ) );
		hcore::HTime t( dt.value() );
		hcore::HString s;
		try {
			t.set_format( fmt );
			s = t.to_string();
		} catch ( hcore::HException const& e ) {
			thread_->raise( dtp.exception_class(), e.what(), position_ );
		}
		return ( of.create_string( yaal::move( s ) ) );
		M_EPILOG
	}
	static HHuginn::value_t sleep( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "DateTime.sleep", values_, { HHuginn::TYPE::INTEGER }, thread_, position_ );
		int long long nanoseconds( get_integer( values_[0] ) );
		if ( nanoseconds < 0 ) {
			throw HHuginn::HHuginnRuntimeException( "Negative sleep time: "_ys.append( nanoseconds ), thread_->current_frame()->file_id(), position_ );
		}
		sleep_for( time::duration( nanoseconds, time::UNIT::NANOSECOND ) );
		return ( *object_ );
		M_EPILOG
	}
};

namespace package_factory {

class HDateTimeCreator : public HPackageCreatorInterface {
protected:
	virtual HInstance do_new_instance( HRuntime* );
} datetimeCreator;

HPackageCreatorInterface::HInstance HDateTimeCreator::do_new_instance( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"DateTime",
			"The `DateTime` package provides date and time handling functionalities.",
			HHuginn::ACCESS::PRIVATE
		)
	);
	HHuginn::field_definitions_t fd{
		{ "now",    runtime_->create_method( &HDateTime::now ),    "get information about current point-in-time" },
		{ "clock",  runtime_->create_method( &HDateTime::clock ),  "create a stopper-watch instance" },
		{ "format", runtime_->create_method( &HDateTime::format ), "( *fmt*, *dt* ) - get string representation of time given by *dt* using *fmt* format specification" },
		{ "sleep",  runtime_->create_method( &HDateTime::sleep ),  "( *nanoseconds* ) - suspend program execution for specified amount of *nanoseconds*" }
	};
	c->redefine( nullptr, fd );
	return { c, runtime_->object_factory()->create<HDateTime>( c.raw() ) };
	M_EPILOG
}

namespace {

bool registrar( void ) {
	M_PROLOG
	bool volatile failed = false;
	HPackageFactory& factory = HPackageFactory::get_instance();
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

