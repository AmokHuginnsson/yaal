/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "enumeration.hxx"
#include "tools/hhuginn.hxx"
#include "iterator.hxx"
#include "helper.hxx"
#include "thread.hxx"
#include "objectfactory.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

namespace enumeration {

class HEnumerationClass : public HHuginn::HClass {
public:
	typedef HEnumerationClass this_type;
	typedef HHuginn::HClass base_type;
public:
	HEnumerationClass(
		HRuntime* runtime_,
		HHuginn::type_id_t typeId_,
		HHuginn::identifier_id_t identifierId_,
		descriptions_t const& descriptions_,
		HHuginn::HClass const* base_,
		yaal::hcore::HString const& doc_
	) : HHuginn::HClass(
			runtime_,
			typeId_,
			identifierId_,
			base_,
			doc_
		) {
		HHuginn::field_definitions_t fd;
		fd.reserve( descriptions_.get_size() + 1 );
		int id( 0 );
		for ( enumeration::HDesctiption const& d : descriptions_ ) {
			fd.emplace_back(
				d.name(),
				runtime_->object_factory()->create<HHuginn::HEnumeration>( this, id ++ ),
				d.doc()
			);
		}
		fd.emplace_back( "to_string", runtime_->object_factory()->create_method( this, hcore::call( &HEnumerationClass::to_string, _1, _2, _3, _4 ) ), "Get enumeration name." );
		redefine( base_, fd );
		return;
	}
	yaal::hcore::HString const& enumeration_name( int id_ ) const {
		return ( runtime()->identifier_name( field_identifiers()[id_] ) );
	}
	static HHuginn::value_t to_string( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( "to_string", values_, 0, 0, thread_, position_ );
		HHuginn::HEnumeration* e( static_cast<HHuginn::HEnumeration*>( object_->raw() ) );
		HEnumerationClass const* ec( static_cast<HEnumerationClass const*>( e->get_class() ) );
		HString s( ec->name() );
		return ( thread_->object_factory().create_string( s.append( "." ).append( ec->enumeration_name( e->value() ) ) ) );
		M_EPILOG
	}
private:
	virtual HHuginn::value_t do_create_instance( huginn::HThread* thread_, HHuginn::values_t&, int position_ ) const {
		M_PROLOG
		throw HHuginn::HHuginnRuntimeException( "Explicit construction of enumeration `"_ys.append( name() ).append( "' instances is forbidden." ), thread_->current_frame()->file_id(), position_ );
		M_EPILOG
	}
};

HHuginn::class_t create_class(
	HRuntime* runtime_,
	yaal::hcore::HString const& name_,
	descriptions_t const& descriptions_,
	yaal::hcore::HString const& doc_,
	HHuginn::HClass const* base_
) {
	HHuginn::identifier_id_t classIdentifier( runtime_->identifier_id( name_ ) );
	HHuginn::class_t c( runtime_ ? runtime_->get_class( classIdentifier ) : nullptr );
	if ( ! c ) {
		c =	runtime_->create_class(
			HRuntime::class_constructor_t(
				[&runtime_, &classIdentifier, &descriptions_, &doc_, &base_] ( HHuginn::type_id_t typeId_ ) -> HHuginn::class_t {
					return (
						make_pointer<HEnumerationClass>(
							runtime_,
							typeId_,
							classIdentifier,
							descriptions_,
							base_,
							doc_
						)
					);
				}
			)
		);
	}
	if ( runtime_ ) {
		runtime_->huginn()->register_class( c, HHuginn::ACCESS::PUBLIC );
	}
	runtime_->huginn()->register_class( c );
	return ( c );
}

}

}

HHuginn::HEnumeration::HEnumeration( HHuginn::HClass const* class_, value_type value_ )
	: HValue( class_ )
	, _value( value_ ) {
	return;
}

HHuginn::value_t HHuginn::HEnumeration::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int position_ ) const {
	throw HHuginn::HHuginnRuntimeException( "Copy semantics is not supported on enumerations.", thread_->current_frame()->file_id(), position_ );
}

}

}

