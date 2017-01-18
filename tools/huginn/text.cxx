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
#include "runtime.hxx"
#include "tools/stringalgo.hxx"
#include "tools/hstringstream.hxx"
#include "tools/streamtools.hxx"
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

class HText : public HHuginn::HValue {
public:
	enum class BASE {
		HEX, OCT, BIN
	};
	HText( HHuginn::HClass* class_ )
		: HValue( class_ ) {
		return;
	}
	static HHuginn::value_t split( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		verify_signature( "Text.split", values_, { HHuginn::TYPE::STRING, HHuginn::TYPE::STRING }, position_ );
		typedef HArray<HString> strings_t;
		strings_t strings( string::split<strings_t>( get_string( values_[0] ), get_string( values_[1] ) ) );
		HObjectFactory* of( thread_->runtime().object_factory() );
		HHuginn::value_t l( of->create_list() );
		for ( HString const& s : strings ) {
			static_cast<HHuginn::HList*>( l.raw() )->push_back( of->create_string( s ) );
		}
		return ( l );
		M_EPILOG
	}
	static HHuginn::value_t join( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
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
				.append( "() first argument must be a flat uniform collection of strings, not " )
				.append( a_type_name( values_[0]->get_class() ) )
				.append( "." ),
				position_
			);
		}
		verify_arg_type( name, values_, 1, HHuginn::TYPE::STRING, ARITY::MULTIPLE, position_ );
		HString s;
		HHuginn::HIterable* coll( static_cast<HHuginn::HIterable*>( const_cast<HHuginn::HValue*>( values_[0].raw() ) ) );
		HString const& sep( get_string( values_[1] ) );
		HHuginn::HIterable::HIterator it( coll->iterator( thread_, position_ ) );
		bool addSep( false );
		while ( it.is_valid() ) {
			HHuginn::value_t v( it.value( thread_, position_ ) );
			if ( v->type_id() != HHuginn::TYPE::STRING ) {
				throw HHuginn::HHuginnRuntimeException(
					""_ys.append( name )
					.append( "() first argument must be a flat uniform collection of strings, but it contains " )
					.append( a_type_name( v->get_class() ) )
					.append( "." ),
					position_
				);
			}
			if ( addSep ) {
				s.append( sep );
			}
			s.append( get_string( v.raw() ) );
			addSep = true;
			it.next( thread_, position_ );
		}
		return ( thread_->runtime().object_factory()->create_string( s ) );
		M_EPILOG
	}
	static HHuginn::value_t distance( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		verify_signature( "Text.distance", values_, { HHuginn::TYPE::STRING, HHuginn::TYPE::STRING }, position_ );
		return ( thread_->runtime().object_factory()->create_integer( string::distance::levenshtein_damerau( get_string( values_[0] ), get_string( values_[1] ) ) ) );
		M_EPILOG
	}
	static HHuginn::value_t repeat( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		verify_signature( "Text.repeat", values_, { HHuginn::TYPE::STRING, HHuginn::TYPE::INTEGER }, position_ );
		HString out;
		HString const& s( get_string( values_[0] ) );
		int count( static_cast<int>( get_integer( values_[1] ) ) );
		if ( count < 0 ) {
			throw HHuginn::HHuginnRuntimeException( "Negative repeat count: "_ys.append( count ), position_ );
		}
		out.reserve( count * s.get_length() );
		for ( int i( 0 ); i < count; ++ i ) {
			out.append( s );
		}
		return ( thread_->runtime().object_factory()->create_string( out ) );
		M_EPILOG
	}
	static HHuginn::value_t int_base_to_str( char const* name_, BASE base_, huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		HString name( "Text." );
		name.append( name_ );
		verify_signature( name, values_, { HHuginn::TYPE::INTEGER }, position_ );
		HStringStream ss;
		HHuginn::HInteger::value_type v( get_integer( values_[0] ) );
		switch ( base_ ) {
			case ( BASE::HEX ): ss << "0x" << hex << v; break;
			case ( BASE::OCT ): ss << "0o" << oct << v; break;
			case ( BASE::BIN ): {
				HStringStream bs;
				bs << bin << v;
				HString s( bs.string() );
				s.trim_left( "0" );
				if ( s.is_empty() ) {
					s.assign( "0" );
				}
				ss << "0b" << s;
			} break;
		}
		return ( thread_->runtime().object_factory()->create_string( ss.string() ) );
		M_EPILOG
	}
};

namespace package_factory {

class HTextCreator : public HPackageCreatorInterface {
protected:
	virtual HHuginn::value_t do_new_instance( HRuntime* );
} textCreator;

HHuginn::value_t HTextCreator::do_new_instance( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"Text",
			nullptr,
			HHuginn::field_definitions_t{
				{ "split",    make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HText::split, _1, _2, _3, _4 ) ), "( *str*, *sep* ) - split `string` *str* by separator *sep* into a `list` of `string`s" },
				{ "join",     make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HText::join, _1, _2, _3, _4 ) ), "( *coll*, *sep* ) - join all string from *coll* into one `string` using *sep* as separator" },
				{ "distance", make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HText::distance, _1, _2, _3, _4 ) ), "( *first*, *second* ) - calculate Damerau-Levenshtein distance between *first* and *second* `string`s" },
				{ "repeat",   make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HText::repeat, _1, _2, _3, _4 ) ), "( *seed*, *count* ) - construct new `string` by repeating *seed* `string` *count* times" },
				{ "hex",      make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HText::int_base_to_str, "hex", HText::BASE::HEX, _1, _2, _3, _4 ) ), "( *int* ) - convert *int* value to a `string` using hexadecimal representation" },
				{ "oct",      make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HText::int_base_to_str, "oct", HText::BASE::OCT, _1, _2, _3, _4 ) ), "( *int* ) - convert *int* value to a `string` using octal representation" },
				{ "bin",      make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HText::int_base_to_str, "bin", HText::BASE::BIN, _1, _2, _3, _4 ) ), "( *int* ) - convert *int* value to a `string` using binary representation" }
			},
			"The `Text` package provides various text manipulation algorithms."
		)
	);
	runtime_->huginn()->register_class( c );
	return ( make_pointer<HText>( c.raw() ) );
	M_EPILOG
}

namespace {

bool registrar( void ) {
	M_PROLOG
	bool volatile failed = false;
	HPackageFactory& factory = HPackageFactory::get_instance();
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

