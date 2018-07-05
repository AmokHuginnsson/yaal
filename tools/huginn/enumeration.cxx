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

class HEnumerationClass::HEnumeralClass : public HHuginn::HClass {
public:
	typedef HEnumeralClass this_type;
	typedef HHuginn::HClass base_type;
public:
	HEnumeralClass(
		HEnumerationClass const* enumerationClass_,
		HRuntime* runtime_,
		HHuginn::type_id_t typeId_,
		HHuginn::identifier_id_t identifierId_,
		yaal::hcore::HString const& doc_
	) : HHuginn::HClass(
		runtime_,
		typeId_,
		identifierId_,
		doc_,
		HHuginn::ACCESS::PRIVATE
	) {
		set_origin( enumerationClass_ );
	}
	HEnumerationClass const* enumeration_class( void ) const {
		return ( static_cast<HEnumerationClass const*>( origin() ) );
	}
protected:
	virtual HHuginn::value_t do_create_instance( huginn::HThread* thread_, HHuginn::values_t&, int position_ ) const {
		M_PROLOG
		throw HHuginn::HHuginnRuntimeException( "Explicit construction of enumeral `"_ys.append( name() ).append( "' instances is forbidden." ), thread_->current_frame()->file_id(), position_ );
		M_EPILOG
	}
private:
	HEnumeralClass( HEnumeralClass const& ) = delete;
	HEnumeralClass& operator = ( HEnumeralClass const& ) = delete;
};

HEnumerationClass::HEnumerationClass(
	HRuntime* runtime_,
	HHuginn::type_id_t typeId_,
	HHuginn::identifier_id_t identifierId_,
	descriptions_t const& descriptions_,
	yaal::hcore::HString const& doc_,
	HHuginn::HClass const* origin_,
	HHuginn::VISIBILITY visibility_
) : HHuginn::HClass(
		runtime_,
		typeId_,
		identifierId_,
		doc_,
		HHuginn::ACCESS::PRIVATE
	)
	, _valueClass() {
	M_PROLOG
	HString valueName( name() );
	HObjectFactory& of( *runtime_->object_factory() );
	valueName.append( name().find_one_of( character_class( CHARACTER_CLASS::LOWER_CASE_LETTER ).data() ) != HString::npos ? "Enumeral" : "_ENUMERAL" );
	_valueClass = runtime_->create_class(
		HRuntime::class_constructor_t(
			[this, &runtime_, &valueName] ( HHuginn::type_id_t tid_ ) -> HHuginn::class_t {
				return (
					make_pointer<HEnumeralClass>(
						this,
						runtime_,
						tid_,
						runtime_->identifier_id( valueName ),
						"The `"_ys.append( valueName ).append( "` class represents value of `" ).append( name() ).append( "` enumeration." )
					)
				);
			}
		)
	);
	_valueClass->redefine(
		nullptr,
		HHuginn::field_definitions_t{
			{ "to_string", runtime_->create_method( &HEnumerationClass::to_string ), "Get enumeration name." }
		}
	);
	runtime_->huginn()->register_class( _valueClass, visibility_ );
	HHuginn::field_definitions_t fd;
	fd.reserve( descriptions_.get_size() + 1 );
	int id( 0 );
	for ( enumeration::HDesctiption const& d : descriptions_ ) {
		if ( d.value() != HDesctiption::UNDEFINED ) {
			id = d.value();
		}
		fd.emplace_back(
			d.name(),
			of.create<HHuginn::HEnumeral>( _valueClass.raw(), runtime_->identifier_id( d.name() ), id ),
			d.doc()
		);
		++ id;
	}
	redefine( nullptr, fd );
	if ( origin_ ) {
		set_origin( origin_ );
	}
	return;
	M_EPILOG
}

HHuginn::value_t HEnumerationClass::enumeral( HHuginn::HEnumeral::value_type id_ ) const {
	M_PROLOG
	HHuginn::value_t v;
	for ( HHuginn::value_t const& e : field_definitions() ) {
		if ( static_cast<HHuginn::HEnumeral const*>( e.raw() )->value() == id_ ) {
			v = e;
			break;
		}
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t HEnumerationClass::to_string( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "to_string", values_, 0, 0, thread_, position_ );
	HHuginn::HEnumeral* e( static_cast<HHuginn::HEnumeral*>( object_->raw() ) );
	HEnumerationClass const* ec( static_cast<HEnumeralClass const*>( e->get_class() )->enumeration_class() );
	HString s( ec->name() );
	return ( thread_->object_factory().create_string( s.append( "." ).append( thread_->runtime().identifier_name( e->identifier() ) ) ) );
	M_EPILOG
}

HHuginn::value_t HEnumerationClass::do_create_instance( huginn::HThread* thread_, HHuginn::values_t&, int position_ ) const {
	M_PROLOG
	throw HHuginn::HHuginnRuntimeException( "Explicit construction of enumeration `"_ys.append( name() ).append( "' instances is forbidden." ), thread_->current_frame()->file_id(), position_ );
	M_EPILOG
}

HHuginn::class_t create_class(
	HRuntime* runtime_,
	yaal::hcore::HString const& name_,
	descriptions_t const& descriptions_,
	yaal::hcore::HString const& doc_,
	HHuginn::VISIBILITY visibility_,
	HHuginn::HClass const* origin_
) {
	HHuginn::identifier_id_t classIdentifier( runtime_->identifier_id( name_ ) );
	HHuginn::class_t c( runtime_ ? runtime_->get_class( classIdentifier ) : nullptr );
	if ( ! c ) {
		c =	runtime_->create_class(
			HRuntime::class_constructor_t(
				[&runtime_, &classIdentifier, &descriptions_, &doc_, &origin_, &visibility_] ( HHuginn::type_id_t typeId_ ) -> HHuginn::class_t {
					return (
						make_pointer<HEnumerationClass>(
							runtime_,
							typeId_,
							classIdentifier,
							descriptions_,
							doc_,
							origin_,
							visibility_
						)
					);
				}
			)
		);
		runtime_->huginn()->register_class( c, visibility_ );
	}
	return ( c );
}

}

}

HHuginn::HEnumeral::HEnumeral( HHuginn::HClass const* class_, HHuginn::identifier_id_t identifier_, value_type value_ )
	: HValue( class_ )
	, _identifier( identifier_ )
	, _value( value_ ) {
	return;
}

HHuginn::value_t HHuginn::HEnumeral::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int position_ ) const {
	throw HHuginn::HHuginnRuntimeException( "Copy semantics is not supported on enumerations.", thread_->current_frame()->file_id(), position_ );
}

}

}

