/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstring>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/hhuginn.hxx"
#include "runtime.hxx"
#include "thread.hxx"
#include "stream.hxx"
#include "helper.hxx"
#include "exception.hxx"
#include "model.hxx"
#include "packagefactory.hxx"
#include "objectfactory.hxx"
#include "tools/hyaml.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

class HYAML : public HPackage {
public:
	HYAML( HClass* class_ )
		: HPackage( class_ ) {
		return;
	}
	static HHuginn::value_t save( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		HObjectFactory& of( thread_->object_factory() );
		verify_signature_by_class( "YAML.save", values_, 1, { nullptr, of.stream_class() }, thread_, position_ );
		HStream& s( *static_cast<HStream*>( values_[1].raw() ) );
		tools::HYAML yaml;
		yaml.add_document() = huginn_to_cxx( thread_, values_[0], "YAML", position_ );
		yaml.save( *s.raw() );
		return ( *object_ );
		M_EPILOG
	}
	static HHuginn::value_t load( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		HObjectFactory& of( thread_->object_factory() );
		verify_signature_by_class( "YAML.load", values_, 0, { of.stream_class() }, thread_, position_ );
		HStream& s( *static_cast<HStream*>( values_[0].raw() ) );
		tools::HYAML yaml;
		HHuginn::value_t v( of.none_value() );
		try {
			yaml.load( *s.raw() );
			v = cxx_to_huginn( thread_, yaml.documents().front(), position_ );
		} catch ( hcore::HException const& e ) {
			thread_->raise( static_cast<HYAML*>( object_->raw() )->exception_class(), e.what(), position_ );
		}
		return v;
		M_EPILOG
	}
private:
};

namespace package_factory {

class HYAMLCreator : public HPackageCreatorInterface {
protected:
	virtual HInstance do_new_instance( HRuntime* );
} yamlCreator;

HPackageCreatorInterface::HInstance HYAMLCreator::do_new_instance( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"YAML",
			"The `YAML` package provides YAML based encoding/decoding functionality.",
			HHuginn::ACCESS::PRIVATE
		)
	);
	HHuginn::field_definitions_t fd{
		{ "load", runtime_->create_method( &HYAML::load ), "( *stream* ) - load data from a YAML *stream*" },
		{ "save", runtime_->create_method( &HYAML::save ), "( *data*, *stream* ) - save *data* to *stream* in YAML format" }
	};
	c->redefine( nullptr, fd );
	return { c, runtime_->object_factory()->create<HYAML>( c.raw() ) };
	M_EPILOG
}

namespace {

bool registrar( void ) {
	M_PROLOG
	bool volatile failed = false;
	HPackageFactory& factory = HPackageFactory::get_instance();
	factory.register_package_creator( "YAML", &yamlCreator );
	return failed;
	M_EPILOG
}

bool volatile registered = registrar();

}

}

}

}

}

