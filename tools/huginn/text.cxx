/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  text.cxx - this file is integral part of `yaal' project.

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
#include "tools/hhuginn.hxx"
#include "tools/stringalgo.hxx"
#include "iterator.hxx"
#include "helper.hxx"
#include "thread.hxx"
#include "packagefactory.hxx"
#include "objectfactory.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

class HText : public HHuginn::HObject {
public:
	HText( HHuginn::HClass* class_ )
		: HObject( class_ ) {
		return;
	}
	static HHuginn::value_t split( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
		char const name[] = "Text.split";
		verify_arg_count( name, values_, 2, 2, position_ );
		verify_arg_type( name, values_, 0, HHuginn::TYPE::STRING, false, position_ );
		verify_arg_type( name, values_, 1, HHuginn::TYPE::STRING, false, position_ );
		typedef HArray<HString> strings_t;
		strings_t strings( string::split<strings_t>( get_string( values_[0] ), get_string( values_[1] ) ) );
		HObjectFactory* of( thread_->huginn().object_factory() );
		HHuginn::value_t l( of->create_list() );
		for ( HString const& s : strings ) {
			static_cast<HHuginn::HList*>( l.raw() )->push_back( of->create_string( s ) );
		}
		return ( l );
	}
	static HHuginn::value_t join( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
		char const name[] = "Text.join";
		verify_arg_count( name, values_, 2, 2, position_ );
		HHuginn::type_id_t t( values_[0]->type_id() );
		if (
			( t != HHuginn::TYPE::LIST )
			&& ( t != HHuginn::TYPE::DEQUE )
			&& ( t != HHuginn::TYPE::ORDER )
			&& ( t != HHuginn::TYPE::SET )
		) {
			throw HHuginn::HHuginnRuntimeException(
				""_ys.append( name )
				.append( "() first argument must be a flat uniform collection of strings, not a " )
				.append( values_[0]->get_class()->name() )
				.append( "'." ),
				position_
			);
		}
		verify_arg_type( name, values_, 1, HHuginn::TYPE::STRING, false, position_ );
		HString s;
		HHuginn::HIterable* coll( static_cast<HHuginn::HIterable*>( const_cast<HHuginn::HValue*>( values_[0].raw() ) ) );
		HString const& sep( get_string( values_[1] ) );
		HHuginn::HIterable::HIterator it( coll->iterator() );
		bool addSep( false );
		while ( it.is_valid() ) {
			HHuginn::value_t v( it.value() );
			if ( v->type_id() != HHuginn::TYPE::STRING ) {
				throw HHuginn::HHuginnRuntimeException(
					""_ys.append( name )
					.append( "() first argument must be a flat uniform collection of strings, but it contains a " )
					.append( v->get_class()->name() )
					.append( "'." ),
					position_
				);
			}
			if ( addSep ) {
				s.append( sep );
			}
			s.append( get_string( v.raw() ) );
			addSep = true;
			it.next();
		}
		HHuginn::value_t v( thread_->huginn().object_factory()->create_string( s ) );
		return ( v );
	}
};

namespace package_factory {

class HTextCreator : public HPackageCreatorInterface {
protected:
	virtual HHuginn::value_t do_new_instance( HHuginn* );
} textCreator;

HHuginn::value_t HTextCreator::do_new_instance( HHuginn* huginn_ ) {
	M_PROLOG
	HHuginn::class_t c(
		huginn_->create_class(
			"Text",
			nullptr,
			HHuginn::field_definitions_t{
				{ "split", make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HText::split, _1, _2, _3, _4 ) ) },
				{ "join",  make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HText::join, _1, _2, _3, _4 ) ) }
			}
		)
	);
	huginn_->register_class( c );
	return ( make_pointer<HText>( c.raw() ) );
	M_EPILOG
}

namespace {

bool registrar( void ) {
	M_PROLOG
	bool volatile failed = false;
	HPackageFactory& factory = HPackageFactoryInstance::get_instance();
	factory.register_package_creator( "Text", &textCreator );
	return ( failed );
	M_EPILOG
}

bool volatile registered = registrar();

}

}

}

}

}

