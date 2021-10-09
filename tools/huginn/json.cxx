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
#include "json.hxx"
#include "packagefactory.hxx"
#include "objectfactory.hxx"
#include "tools/hjson.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

void json_serialize( huginn::HThread* thread_, yaal::tools::HHuginn::value_t const& value_, yaal::hcore::HStreamInterface& stream_, bool indent_, int position_ ) {
	M_PROLOG
	tools::HJSON json;
	json.element() = huginn_to_cxx( thread_, value_, "JSON", position_ );
	json.save( stream_, indent_ );
	return;
	M_EPILOG
}

class HJSON : public HPackage {
	enumeration::HEnumerationClass::ptr_t _parser;
public:
	HJSON( HClass* class_ )
		: HPackage( class_ )
		, _parser(
			add_enumeration_as_member(
				class_,
				enumeration::create_class(
					class_->runtime(),
					"PARSER",
					enumeration::descriptions_t{
						{ "STRICT",  "Parse according to strict RFC JSON specification.",            static_cast<int>( tools::HJSON::PARSER::STRICT ) },
						{ "RELAXED", "Use relaxed JSON5 (ECMAScript5 extension compatible) parser.", static_cast<int>( tools::HJSON::PARSER::RELAXED ) },
					},
					"The `PARSER` is set of possible JSON parsing modes.",
					HHuginn::VISIBILITY::PACKAGE,
					class_
				),
				"set of possible modes used for JSON parsing."
			)
		) {
		return;
	}
	static HHuginn::value_t save( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		HObjectFactory& of( thread_->object_factory() );
		verify_signature_by_class( "JSON.save", values_, 1, { nullptr, of.stream_class(), of.boolean_class() }, thread_, position_ );
		HStream& s( *static_cast<HStream*>( values_[1].raw() ) );
		bool indent( true );
		if ( values_.get_size() > 2 ) {
			indent = get_boolean( values_[2] );
		}
		json_serialize( thread_, values_[0], *s.raw(), indent, position_ );
		return ( *object_ );
		M_EPILOG
	}
	static HHuginn::value_t load( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		HObjectFactory& of( thread_->object_factory() );
		HJSON* package( static_cast<HJSON*>( object_->raw() ) );
		HParameter namedParameters[] = {
			{ "parser", package->_parser->enumeral_class() },
			{}
		};
		char const name[] = "JSON.load";
		verify_named_parameters( name, values_, namedParameters, thread_, position_ );
		verify_arg_count( name, values_, 1, 1, thread_, position_ );
		verify_arg_type( name, values_, 0, of.stream_class(), ARITY::MULTIPLE, thread_, position_ );
		HStream& s( *static_cast<HStream*>( values_[0].raw() ) );
		tools::HJSON json;
		HHuginn::value_t v( of.none_value() );
		try {
			if ( !! namedParameters[0].value() ) {
				HEnumeral::value_type val( get_enumeral( namedParameters[0].value() ) );
				json.load( *s.raw(), static_cast<tools::HJSON::PARSER>( val ) );
			} else {
				json.load( *s.raw() );
			}
			v = cxx_to_huginn( thread_, json.element(), position_ );
		} catch ( hcore::HException const& e ) {
			thread_->raise( static_cast<HJSON*>( object_->raw() )->exception_class(), e.what(), position_ );
		}
		return v;
		M_EPILOG
	}
private:
};

namespace package_factory {

class HJSONCreator : public HPackageCreatorInterface {
protected:
	virtual HInstance do_new_instance( HRuntime* );
} jsonCreator;

HPackageCreatorInterface::HInstance HJSONCreator::do_new_instance( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"JSON",
			"The `JSON` package provides JSON based encoding/decoding functionality.",
			HHuginn::ACCESS::PRIVATE
		)
	);
	HHuginn::field_definitions_t fd{
		{ "load", runtime_->create_method( &HJSON::load ), "( *stream*[, *parser*:*{mode}*] ) - load data from a JSON *stream*" },
		{ "save", runtime_->create_method( &HJSON::save ), "( *data*, *stream* ) - save *data* to *stream* in JSON format" }
	};
	c->redefine( nullptr, fd );
	return { c, runtime_->object_factory()->create<HJSON>( c.raw() ) };
	M_EPILOG
}

namespace {

bool registrar( void ) {
	M_PROLOG
	bool volatile failed = false;
	HPackageFactory& factory = HPackageFactory::get_instance();
	factory.register_package_creator( "JSON", &jsonCreator );
	return failed;
	M_EPILOG
}

bool volatile registered = registrar();

}

}

}

}

}

