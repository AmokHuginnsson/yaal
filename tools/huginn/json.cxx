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
#include "time.hxx"
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

namespace {

static tools::model::HValue huginn_to_cxx( HThread* thread_, HHuginn::value_t const& v_, int position_ ) {
	M_PROLOG
	tools::model::HValue v;
	switch ( type_tag( v_->type_id() ) ) {
		case ( HHuginn::TYPE::STRING ): {
			v = get_string( v_ );
		} break;
		case ( HHuginn::TYPE::INTEGER ): {
			v = get_integer( v_ );
		} break;
		case ( HHuginn::TYPE::REAL ): {
			v = get_real( v_ );
		} break;
		case ( HHuginn::TYPE::NUMBER ): {
			v = get_number( v_ );
		} break;
		case ( HHuginn::TYPE::CHARACTER ): {
			v = hcore::to_string( get_character( v_ ) );
		} break;
		case ( HHuginn::TYPE::BOOLEAN ): {
			bool booleanValue( get_boolean( v_ ) );
			v = booleanValue ? tools::model::HValue::LITERAL::TRUE : tools::model::HValue::LITERAL::FALSE;
		} break;
		case ( HHuginn::TYPE::NONE ): {
			v = tools::model::HValue::LITERAL::NULL;
		} break;
		case ( HHuginn::TYPE::TUPLE ): {
			HTuple::values_t const& data( static_cast<HTuple const*>( v_.raw() )->value() );
			for ( HHuginn::value_t const& e : data ) {
				v.push_back( huginn_to_cxx( thread_, e, position_ ) );
			}
		} break;
		case ( HHuginn::TYPE::LIST ): {
			HList::values_t const& data( static_cast<HList const*>( v_.raw() )->value() );
			for ( HHuginn::value_t const& e : data ) {
				v.push_back( huginn_to_cxx( thread_, e, position_ ) );
			}
		} break;
		case ( HHuginn::TYPE::LOOKUP ): {
			HLookup::values_t const& data( static_cast<HLookup const*>( v_.raw() )->value() );
			tools::model::HValue::members_t& m( v.get_members() );
			for ( HLookup::values_t::value_type const& e : data ) {
				if ( e.first->type_id() != HHuginn::TYPE::STRING ) {
					throw HHuginn::HHuginnRuntimeException(
						"Keys in JSON objects must be `string`s, not "_ys.append( a_type_name( e.first->get_class() ) ).append( "." ),
						thread_->runtime().file_id(),
						position_
					);
				}
				m.insert( make_pair( get_string( e.first ), huginn_to_cxx( thread_, e.second, position_ ) ) );
			}
		} break;
		case ( HHuginn::TYPE::DEQUE ): {
			HDeque::values_t const& data( static_cast<HDeque const*>( v_.raw() )->value() );
			for ( HHuginn::value_t const& e : data ) {
				v.push_back( huginn_to_cxx( thread_, e, position_ ) );
			}
		} break;
		case ( HHuginn::TYPE::DICT ): {
			HDict const& dict( *static_cast<HDict const*>( v_.raw() ) );
			HDict::values_t const& data( dict.value() );
			tools::model::HValue::members_t& m( v.get_members() );
			if ( ! data.is_empty() && ( dict.key_type()->type_id() != HHuginn::TYPE::STRING ) ) {
				throw HHuginn::HHuginnRuntimeException(
					"Keys in JSON objects must be `string`s, not "_ys.append( a_type_name( dict.key_type() ) ).append( "." ),
					thread_->runtime().file_id(),
					position_
				);
			}
			for ( HLookup::values_t::value_type const& e : data ) {
				m.insert( make_pair( get_string( e.first ), huginn_to_cxx( thread_, e.second, position_ ) ) );
			}
		} break;
		case ( HHuginn::TYPE::ORDER ): {
			HOrder::values_t const& data( static_cast<HOrder const*>( v_.raw() )->value() );
			for ( HHuginn::value_t const& e : data ) {
				v.push_back( huginn_to_cxx( thread_, e, position_ ) );
			}
		} break;
		case ( HHuginn::TYPE::SET ): {
			HSet::values_t const& data( static_cast<HSet const*>( v_.raw() )->value() );
			for ( HHuginn::value_t const& e : data ) {
				v.push_back( huginn_to_cxx( thread_, e, position_ ) );
			}
		} break;
		default: {
			throw HHuginn::HHuginnRuntimeException(
				"Value of type "_ys.append( a_type_name( v_->get_class() ) ).append( " is not JSON serializable." ),
				thread_->runtime().file_id(),
				position_
			);
		}
	}
	return v;
	M_EPILOG
}

}

void json_serialize( huginn::HThread* thread_, yaal::tools::HHuginn::value_t const& value_, yaal::hcore::HStreamInterface& stream_, bool indent_, int position_ ) {
	M_PROLOG
	tools::HJSON json;
	json.element() = huginn_to_cxx( thread_, value_, position_ );
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
	static HHuginn::value_t cxx_to_huginn( HThread* thread_, tools::model::HValue const& v_, int position_ ) {
		M_PROLOG
		HObjectFactory& of( thread_->object_factory() );
		HHuginn::value_t v( of.none_value() );
		switch ( v_.type() ) {
			case ( tools::model::HValue::TYPE::STRING ): {
				v = of.create_string( v_.get_string() );
			} break;
			case ( tools::model::HValue::TYPE::INTEGER ): {
				v = of.create_integer( v_.get_integer() );
			} break;
			case ( tools::model::HValue::TYPE::REAL ): {
				v = of.create_real( v_.get_real() );
			} break;
			case ( tools::model::HValue::TYPE::NUMBER ): {
				v = of.create_number( v_.get_number() );
			} break;
			case ( tools::model::HValue::TYPE::TIME ): {
				v = of.create<huginn::HTime>( of.time_class(), v_.get_time() );
			} break;
			case ( tools::model::HValue::TYPE::LITERAL ): {
				tools::model::HValue::LITERAL literal( v_.get_literal() );
				if ( literal == tools::model::HValue::LITERAL::TRUE ) {
					v = of.true_value();
				} else if ( literal == tools::model::HValue::LITERAL::FALSE ) {
					v = of.false_value();
				}
			} break;
			case ( tools::model::HValue::TYPE::ARRAY ): {
				HHuginn::values_t data;
				for ( tools::model::HValue const& e : v_.get_elements() ) {
					data.push_back( cxx_to_huginn( thread_, e, position_ ) );
				}
				v = of.create_list( yaal::move( data ) );
			} break;
			case ( tools::model::HValue::TYPE::MAP ): {
				v = of.create_lookup();
				huginn::HLookup* lookup( static_cast<huginn::HLookup*>( v.raw() ) );
				huginn::HLookup::values_t& data( lookup->value() );
				HAnchorGuard<huginn::HLookup> ag( *lookup, thread_, position_ );
				for ( tools::model::HValue::members_t::value_type const& m : v_.get_members() ) {
					data.insert( make_pair( of.create_string( m.first ), cxx_to_huginn( thread_, m.second, position_ ) ) );
				}
			} break;
			case ( tools::model::HValue::TYPE::UNINITIALIZED ): break;
		}
		return v;
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

