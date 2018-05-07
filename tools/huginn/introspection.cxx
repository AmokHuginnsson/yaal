/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/hhuginn.hxx"
#include "runtime.hxx"
#include "tools/stringalgo.hxx"
#include "iterator.hxx"
#include "helper.hxx"
#include "keyword.hxx"
#include "thread.hxx"
#include "packagefactory.hxx"
#include "objectfactory.hxx"
#include "commit_id.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

class HIntrospection : public HHuginn::HValue {
private:
	HHuginn::class_t _exceptionClass;
public:
	HIntrospection( HHuginn::HClass* class_ )
		: HValue( class_ )
		, _exceptionClass( class_exception( class_ ) ) {
		return;
	}
	static HHuginn::value_t version( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( "Introspection.version", values_, 0, 0, thread_, position_ );
		HObjectFactory& of( *thread_->runtime().object_factory() );
		HHuginn::value_t ver( of.create_string( PACKAGE_VERSION ) );
		HHuginn::value_t ci( of.create_string( COMMIT_ID ) );
		return ( of.create_tuple( { ver, ci } ) );
		M_EPILOG
	}
	static HHuginn::value_t symbol( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "Introspection.symbol", values_, { HHuginn::TYPE::STRING }, thread_, position_ );
		HRuntime& r( thread_->runtime() );
		HHuginn::value_t v( r.none_value() );
		HString const& name( get_string( values_[0] ) );
		HHuginn::identifier_id_t id( r.try_identifier_id( name ) );
		HHuginn::value_t const* pv( nullptr );
		if ( id != INVALID_IDENTIFIER ) {
			pv = r.get_function( id );
			if ( ! pv ) {
				pv = r.get_value( id );
			}
		}
		if ( pv ) {
			v = *pv;
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t attribute( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "Introspection.attribute", values_, { HHuginn::TYPE::UNKNOWN, HHuginn::TYPE::STRING }, thread_, position_ );
		HRuntime& r( thread_->runtime() );
		HHuginn::value_t v( r.none_value() );
		HString const& name( get_string( values_[1] ) );
		HHuginn::identifier_id_t id( r.try_identifier_id( name ) );
		if ( id != INVALID_IDENTIFIER ) {
			HHuginn::value_t& s( values_[0] );
			HHuginn::type_id_t t( s->type_id() );
			if ( t == HHuginn::TYPE::FUNCTION_REFERENCE ) {
				HHuginn::class_t c( r.get_class( static_cast<HHuginn::HFunctionReference*>( s.raw() )->identifier_id() ) );
				if ( !! c ) {
					int fi( c->field_index( id, HHuginn::HClass::MEMBER_TYPE::STATIC ) );
					if ( fi >= 0 ) {
						HHuginn::value_t const& f( c->field( fi ) );
						v = f->type_id() == HHuginn::TYPE::METHOD
							? r.object_factory()->create_unbound_method( c.raw(), static_cast<HHuginn::HClass::HMethod const*>( f.raw() )->function() )
							: f;
					}
				}
			} else {
				int idx( s->field_index( id ) );
				if ( idx >= 0 ) {
					v = s->field( s, idx );
				}
			}
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t list_attributes( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( "Introspection.list_attributes", values_, 1, 1, thread_, position_ );
		HRuntime& r( thread_->runtime() );
		HObjectFactory& of( *r.object_factory() );
		HHuginn::values_t attrs;
		HHuginn::value_t& v( values_[0] );
		HHuginn::HClass const* c( nullptr );
		if ( v->type_id() == HHuginn::TYPE::FUNCTION_REFERENCE ) {
			c = r.get_class( static_cast<HHuginn::HFunctionReference*>( v.raw() )->identifier_id() ).raw();
		}
		if ( ! c ) {
			c = v->get_class();
		}
		for ( HHuginn::identifier_id_t id : c->field_identifiers() ) {
			attrs.emplace_back( of.create_string( r.identifier_name( id ) ) );
		}
		return ( of.create_tuple( yaal::move( attrs ) ) );
		M_EPILOG
	}
	static HHuginn::value_t import( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "Introspection.import", values_, { HHuginn::TYPE::STRING }, thread_, position_ );
		HRuntime& r( thread_->runtime() );
		HString const& package( get_string( values_[0] ) );
		HHuginn::identifier_id_t id( r.try_identifier_id( package ) );
		bool classExists( !! r.get_class( id ) );
		bool functionExists( !! r.get_function( id ) );
		HHuginn::value_t const* rtv( r.get_value( id ) );
		bool enumerationExists( rtv && is_enum_class( rtv ) );
		if ( is_restricted( package ) ) {
			throw HHuginn::HHuginnRuntimeException(
				"`"_ys.append( package ).append( "' is restricted keyword." ),
				thread_->current_frame()->file_id(),
				position_
			);
		}
		if ( classExists || functionExists || rtv ) {
			throw HHuginn::HHuginnRuntimeException(
				hcore::to_string(
					enumerationExists ? "Enumeration" : ( classExists ? "Class" : ( functionExists ? "Function" : "Package alias" ) )
				).append( " of the same name already exists." ),
				thread_->current_frame()->file_id(),
				position_
			);
		}
		HHuginn::value_t v( r.none_value() );
		try {
			v = HPackageFactory::get_instance().create_package( &r, package, position_ );
		} catch ( HException const& e ) {
			thread_->raise( static_cast<HIntrospection*>( object_->raw() )->_exceptionClass.raw(), e.what(), position_ );
		}
		return ( v );
		M_EPILOG
	}
};

namespace package_factory {

class HTntrospectionCreator : public HPackageCreatorInterface {
protected:
	virtual HHuginn::value_t do_new_instance( HRuntime* );
} introspectionCreator;

HHuginn::value_t HTntrospectionCreator::do_new_instance( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"Introspection",
			"The `Introspection` package provides run time introspection facility.",
			HHuginn::ACCESS::PRIVATE
		)
	);
	HHuginn::field_definitions_t fd{
		{ "version",         runtime_->create_method( &HIntrospection::version ),         "return runtime version information." },
		{ "symbol",          runtime_->create_method( &HIntrospection::symbol ),          "( *name* ) - get global symbol by *name*." },
		{ "attribute",       runtime_->create_method( &HIntrospection::attribute ),       "( *object*, *name* ) - get *object*'s attribute (a field or method) by *name*." },
		{ "list_attributes", runtime_->create_method( &HIntrospection::list_attributes ), "( *object* ) - list attributes of given *object*." },
		{ "import",          runtime_->create_method( &HIntrospection::import ),          "( *package* ) - import given *package*." }
	};
	c->redefine( nullptr, fd );
	runtime_->huginn()->register_class( c );
	return ( runtime_->object_factory()->create<HIntrospection>( c.raw() ) );
	M_EPILOG
}

namespace {

bool registrar( void ) {
	M_PROLOG
	bool volatile failed = false;
	HPackageFactory& factory = HPackageFactory::get_instance();
	factory.register_package_creator( "Introspection", &introspectionCreator );
	return ( failed );
	M_EPILOG
}

bool volatile registered = registrar();

}

}

}

}

}

