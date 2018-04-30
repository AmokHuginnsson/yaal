/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/hhuginn.hxx"
#include "runtime.hxx"
#include "tools/huginn/thread.hxx"
#include "helper.hxx"
#include "exception.hxx"
#include "compiledregularexpression.hxx"
#include "objectfactory.hxx"
#include "packagefactory.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

class HRegularExpressions : public HHuginn::HValue {
	HHuginn::class_t _compiledRegularExpressionClass;
	HHuginn::class_t _exceptionClass;
public:
	HRegularExpressions( HHuginn::HClass* class_ )
		: HValue( class_ )
		, _compiledRegularExpressionClass( HCompiledRegularExpression::get_class( class_->runtime(), _exceptionClass ) )
		, _exceptionClass( class_exception( class_ ) ) {
		return;
	}
	static HHuginn::value_t compile( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		return ( static_cast<HRegularExpressions*>( object_->raw() )->do_compile( thread_, values_, position_ ) );
		M_EPILOG
	}
private:
	HHuginn::value_t do_compile( huginn::HThread* thread_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "RegularExpressions.compile", values_, { HHuginn::TYPE::STRING }, thread_, position_ );
		return ( _compiledRegularExpressionClass->create_instance( thread_, nullptr, values_, position_ ) );
		M_EPILOG
	}
};

namespace package_factory {

class HRegularExpressionsCreator : public HPackageCreatorInterface {
protected:
	virtual HHuginn::value_t do_new_instance( HRuntime* );
} regularexpressionsCreator;

HHuginn::value_t HRegularExpressionsCreator::do_new_instance( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"RegularExpressions",
			"The `RegularExpressions` package provides support for regular expressions based text searches.",
			HHuginn::ACCESS::PRIVATE
		)
	);
	HHuginn::field_definitions_t fd{
		{ "compile", runtime_->create_method( &HRegularExpressions::compile ), "( *pattern* ) - compile given regular expression pattern to be used for text searches" }
	};
	c->redefine( nullptr, fd );
	runtime_->huginn()->register_class( c );
	return ( runtime_->object_factory()->create<HRegularExpressions>( c.raw() ) );
	M_EPILOG
}

namespace {

bool registrar( void ) {
	M_PROLOG
	bool volatile failed = false;
	HPackageFactory& factory = HPackageFactory::get_instance();
	factory.register_package_creator( "RegularExpressions", &regularexpressionsCreator );
	return ( failed );
	M_EPILOG
}

bool volatile registered = registrar();

}

}

}

}

}

