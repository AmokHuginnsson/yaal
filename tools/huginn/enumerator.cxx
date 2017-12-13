/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  enumerator.cxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "enumerator.hxx"
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

namespace enumerator {

class HEnumeratorClass : public HHuginn::HClass {
public:
	typedef HEnumeratorClass this_type;
	typedef HHuginn::HClass base_type;
public:
	HEnumeratorClass(
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
			HHuginn::field_definitions_t{},
			doc_
		) {
		HHuginn::field_definitions_t fd;
		fd.reserve( descriptions_.get_size() + 1 );
		int id( 0 );
		for ( enumerator::HDesctiption const& d : descriptions_ ) {
			fd.emplace_back(
				d.name(),
				runtime_->object_factory()->create<HHuginn::HEnumerator>( this, id ++ ),
				d.doc()
			);
		}
		fd.emplace_back( "to_string", runtime_->object_factory()->create_method( hcore::call( &HEnumeratorClass::to_string, _1, _2, _3, _4 ) ), "Get enumerator name." );
		redefine( base_, fd );
		return;
	}
	yaal::hcore::HString const& enumerator_name( int id_ ) const {
		return ( runtime()->identifier_name( field_identifiers()[id_] ) );
	}
	static HHuginn::value_t to_string( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( "to_string", values_, 0, 0, thread_, position_ );
		HHuginn::HEnumerator* e( static_cast<HHuginn::HEnumerator*>( object_->raw() ) );
		HEnumeratorClass const* ec( static_cast<HEnumeratorClass const*>( e->get_class() ) );
		HString s( ec->name() );
		return ( thread_->object_factory().create_string( s.append( "." ).append( ec->enumerator_name( e->value() ) ) ) );
		M_EPILOG
	}
private:
	virtual HHuginn::value_t do_create_instance( huginn::HThread* thread_, HHuginn::values_t&, int position_ ) const {
		M_PROLOG
		throw HHuginn::HHuginnRuntimeException( "Explicit construction of enumerator `"_ys.append( name() ).append( "' instances is forbidden." ), thread_->current_frame()->file_id(), position_ );
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
						make_pointer<HEnumeratorClass>(
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

HHuginn::HEnumerator::HEnumerator( HHuginn::HClass const* class_, value_type value_ )
	: HValue( class_ )
	, _value( value_ ) {
	return;
}

HHuginn::value_t HHuginn::HEnumerator::do_clone( huginn::HThread* thread_, int position_ ) const {
	throw HHuginn::HHuginnRuntimeException( "Copy semantics is not supported on enumerators.", thread_->current_frame()->file_id(), position_ );
}

}

}

