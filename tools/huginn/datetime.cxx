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

class HDateTime : public HHuginn::HValue {
	HHuginn::class_t _timeClass;
	HHuginn::class_t _clockClass;
	HHuginn::class_t _exceptionClass;
public:
	HDateTime( HHuginn::HClass* class_ )
		: HValue( class_ )
		, _timeClass( HTime::get_class( class_->runtime() ) )
		, _clockClass( HClock::get_class( class_->runtime() ) )
		, _exceptionClass( package_exception( class_ ) ) {
		return;
	}
	static HHuginn::value_t now( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( "DateTime.now", values_, 0, 0, thread_, position_ );
		HDateTime* dt( dynamic_cast<HDateTime*>( object_->raw() ) );
		M_ASSERT( dt );
		return ( thread_->object_factory().create<huginn::HTime>( dt->_timeClass.raw(), now_local() ) );
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
	virtual HHuginn::value_t do_new_instance( HRuntime* );
} datetimeCreator;

HHuginn::value_t HDateTimeCreator::do_new_instance( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"DateTime",
			nullptr,
			"The `DateTime` package provides date and time handling functionalities."
		)
	);
	HHuginn::field_definitions_t fd{
		{ "now",   runtime_->create_method( c.raw(), &HDateTime::now ),   "get information about current point-in-time" },
		{ "clock", runtime_->create_method( c.raw(), &HDateTime::clock ), "create a stopper-watch instance" },
		{ "sleep", runtime_->create_method( c.raw(), &HDateTime::sleep ), "( *nanoseconds* ) - suspend program execution for specified amount of *nanoseconds*" }
	};
	c->redefine( nullptr, fd );
	runtime_->huginn()->register_class( c );
	return ( runtime_->object_factory()->create<HDateTime>( c.raw() ) );
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

