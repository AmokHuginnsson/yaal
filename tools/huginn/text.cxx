/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cwctype>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/hhuginn.hxx"
#include "runtime.hxx"
#include "iterator.hxx"
#include "helper.hxx"
#include "thread.hxx"
#include "stream.hxx"
#include "enumeration.hxx"
#include "hcore/hcore.hxx"
#include "packagefactory.hxx"
#include "objectfactory.hxx"
#include "tools/stringalgo.hxx"
#include "tools/hstringstream.hxx"
#include "tools/util.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

class HText : public HPackage {
	HHuginn::class_t _streamClass;
	enumeration::HEnumerationClass::ptr_t _characterClassClass;
public:
	HText( HHuginn::HClass* class_ )
		: HPackage( class_ )
		, _streamClass( HStream::get_class( class_->runtime() ) )
		, _characterClassClass(
			add_enumeration_as_member(
				class_,
				enumeration::create_class(
					class_->runtime(),
					"CHARACTER_CLASS",
					enumeration::descriptions_t{
						{ "WHITESPACE",                    "a set of white space characters",         static_cast<int>( CHARACTER_CLASS::WHITESPACE ) },
						{ "BIN_DIGIT",                     "a set of binary digits",                  static_cast<int>( CHARACTER_CLASS::BIN_DIGIT ) },
						{ "OCT_DIGIT",                     "a set of octal digits",                   static_cast<int>( CHARACTER_CLASS::OCT_DIGIT ) },
						{ "DIGIT",                         "a set of decimal digits",                 static_cast<int>( CHARACTER_CLASS::DIGIT ) },
						{ "HEX_DIGIT",                     "a set of hexadecimal digits",             static_cast<int>( CHARACTER_CLASS::HEX_DIGIT ) },
						{ "LETTER",                        "a set of Latin letters",                  static_cast<int>( CHARACTER_CLASS::LETTER ) },
						{ "LOWER_CASE_LETTER",             "a set of lower case Latin letters",       static_cast<int>( CHARACTER_CLASS::LOWER_CASE_LETTER ) },
						{ "UPPER_CASE_LETTER",             "a set of upper case Latin letters",       static_cast<int>( CHARACTER_CLASS::UPPER_CASE_LETTER ) },
						{ "WORD",                          "a set of word like characters",           static_cast<int>( CHARACTER_CLASS::WORD ) },
						{ "VOWEL",                         "a set of vowels",                         static_cast<int>( CHARACTER_CLASS::VOWEL ) },
						{ "GREEK",                         "a set of Greek letters",                  static_cast<int>( CHARACTER_CLASS::GREEK ) },
						{ "LOWER_CASE_GREEK",              "a set of lower case Greek letters",       static_cast<int>( CHARACTER_CLASS::LOWER_CASE_GREEK ) },
						{ "UPPER_CASE_GREEK",              "a set of upper case Greek letters",       static_cast<int>( CHARACTER_CLASS::UPPER_CASE_GREEK ) },
						{ "SUBSCRIPT",                     "a set of subscript characters",           static_cast<int>( CHARACTER_CLASS::SUBSCRIPT ) },
						{ "SUBSCRIPT_DIGIT",               "a set of subscript digits",               static_cast<int>( CHARACTER_CLASS::SUBSCRIPT_DIGIT ) },
						{ "SUBSCRIPT_LETTER",              "a set of subscript letters",              static_cast<int>( CHARACTER_CLASS::SUBSCRIPT_LETTER ) },
						{ "SUBSCRIPT_LOWER_CASE_LETTER",   "a set of subscript lower case letters",   static_cast<int>( CHARACTER_CLASS::SUBSCRIPT_LOWER_CASE_LETTER ) },
						{ "SUBSCRIPT_UPPER_CASE_LETTER",   "a set of subscript upper case letters",   static_cast<int>( CHARACTER_CLASS::SUBSCRIPT_UPPER_CASE_LETTER ) },
						{ "SUPERSCRIPT",                   "a set of superscript characters",         static_cast<int>( CHARACTER_CLASS::SUPERSCRIPT ) },
						{ "SUPERSCRIPT_DIGIT",             "a set of superscript digits",             static_cast<int>( CHARACTER_CLASS::SUPERSCRIPT_DIGIT ) },
						{ "SUPERSCRIPT_LETTER",            "a set of superscript letters",            static_cast<int>( CHARACTER_CLASS::SUPERSCRIPT_LETTER ) },
						{ "SUPERSCRIPT_LOWER_CASE_LETTER", "a set of superscript lower case letters", static_cast<int>( CHARACTER_CLASS::SUPERSCRIPT_LOWER_CASE_LETTER ) },
						{ "SUPERSCRIPT_UPPER_CASE_LETTER", "a set of superscript upper case letters", static_cast<int>( CHARACTER_CLASS::SUPERSCRIPT_UPPER_CASE_LETTER ) }
					},
					"The `CHARACTER_CLASS` is a set of interesting character classes.",
					HHuginn::VISIBILITY::PACKAGE,
					class_
				),
				"a set of interesting character classes"
			)
		) {
		return;
	}
	static HHuginn::value_t stream( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Text.stream";
		verify_arg_count( name, values_, 0, 1, thread_, position_ );
		HText& t( *static_cast<HText*>( object_->raw() ) );
		HStreamInterface::ptr_t stream( make_pointer<HStringStream>() );
		HStringStream& ss( *static_cast<HStringStream*>( stream.raw() ) );
		if ( values_.get_size() > 0 ) {
			ss << get_string( values_[0] );
		}
		return ( thread_->object_factory().create<HStream>( t._streamClass.raw(), stream ) );
		M_EPILOG
	}
	static HHuginn::value_t split( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "Text.split", values_, { HHuginn::TYPE::STRING, HHuginn::TYPE::STRING }, thread_, position_ );
		typedef HArray<HString> strings_t;
		strings_t strings( string::split<strings_t>( get_string( values_[0] ), get_string( values_[1] ) ) );
		HObjectFactory* of( thread_->runtime().object_factory() );
		HHuginn::value_t l( of->create_list() );
		for ( HString& s : strings ) {
			static_cast<HHuginn::HList*>( l.raw() )->push_back( of->create_string( yaal::move( s ) ) );
		}
		return ( l );
		M_EPILOG
	}
	static HHuginn::value_t join( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Text.join";
		verify_arg_count( name, values_, 2, 2, thread_, position_ );
		HHuginn::type_id_t t( values_[0]->type_id() );
		if (
			( t != HHuginn::TYPE::TUPLE )
			&& ( t != HHuginn::TYPE::LIST )
			&& ( t != HHuginn::TYPE::DEQUE )
			&& ( t != HHuginn::TYPE::ORDER )
			&& ( t != HHuginn::TYPE::SET )
		) {
			throw HHuginn::HHuginnRuntimeException(
				""_ys.append( name )
					.append( "() first argument must be a flat uniform collection of strings, not " )
					.append( a_type_name( values_[0]->get_class() ) )
					.append( "." ),
				thread_->current_frame()->file_id(),
				position_
			);
		}
		verify_arg_type( name, values_, 1, HHuginn::TYPE::STRING, ARITY::MULTIPLE, thread_, position_ );
		HString s;
		HHuginn::HIterable* coll( static_cast<HHuginn::HIterable*>( const_cast<HHuginn::HValue*>( values_[0].raw() ) ) );
		HString const& sep( get_string( values_[1] ) );
		HHuginn::HIterable::iterator_t it( coll->iterator( thread_, position_ ) );
		bool addSep( false );
		while ( thread_->can_continue() && it->is_valid( thread_, position_ ) ) {
			HHuginn::value_t v( it->value( thread_, position_ ) );
			if ( v->type_id() != HHuginn::TYPE::STRING ) {
				throw HHuginn::HHuginnRuntimeException(
					""_ys.append( name )
						.append( "() first argument must be a flat uniform collection of strings, but it contains " )
						.append( a_type_name( v->get_class() ) )
						.append( "." ),
					thread_->current_frame()->file_id(),
					position_
				);
			}
			if ( addSep ) {
				s.append( sep );
			}
			s.append( get_string( v.raw() ) );
			addSep = true;
			it->next( thread_, position_ );
		}
		return ( thread_->runtime().object_factory()->create_string( yaal::move( s ) ) );
		M_EPILOG
	}
	static HHuginn::value_t distance( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "Text.distance", values_, { HHuginn::TYPE::STRING, HHuginn::TYPE::STRING }, thread_, position_ );
		return ( thread_->runtime().object_factory()->create_integer( string::distance( get_string( values_[0] ), get_string( values_[1] ) ) ) );
		M_EPILOG
	}
	static HHuginn::value_t character_class( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		HText& t( *static_cast<HText*>( object_->raw() ) );
		verify_signature_by_class( "Text.character_class", values_, { t._characterClassClass->enumeral_class() }, thread_, position_ );
		HHuginn::HEnumeral::value_type val( get_enumeral( values_[0] ) );
		character_class_t const* cc8( nullptr );
		HCharacterClass<char16_t> const* cc16( nullptr );
		switch ( static_cast<CHARACTER_CLASS>( val ) ) {
			case ( CHARACTER_CLASS::WHITESPACE ):                    cc8  = &hcore::character_class<CHARACTER_CLASS::WHITESPACE>();                    break;
			case ( CHARACTER_CLASS::BIN_DIGIT ):                     cc8  = &hcore::character_class<CHARACTER_CLASS::BIN_DIGIT>();                     break;
			case ( CHARACTER_CLASS::OCT_DIGIT ):                     cc8  = &hcore::character_class<CHARACTER_CLASS::OCT_DIGIT>();                     break;
			case ( CHARACTER_CLASS::DIGIT ):                         cc8  = &hcore::character_class<CHARACTER_CLASS::DIGIT>();                         break;
			case ( CHARACTER_CLASS::HEX_DIGIT ):                     cc8  = &hcore::character_class<CHARACTER_CLASS::HEX_DIGIT>();                     break;
			case ( CHARACTER_CLASS::LETTER ):                        cc8  = &hcore::character_class<CHARACTER_CLASS::LETTER>();                        break;
			case ( CHARACTER_CLASS::LOWER_CASE_LETTER ):             cc8  = &hcore::character_class<CHARACTER_CLASS::LOWER_CASE_LETTER>();             break;
			case ( CHARACTER_CLASS::UPPER_CASE_LETTER ):             cc8  = &hcore::character_class<CHARACTER_CLASS::UPPER_CASE_LETTER>();             break;
			case ( CHARACTER_CLASS::WORD ):                          cc8  = &hcore::character_class<CHARACTER_CLASS::WORD>();                          break;
			case ( CHARACTER_CLASS::VOWEL ):                         cc8  = &hcore::character_class<CHARACTER_CLASS::VOWEL>();                         break;
			case ( CHARACTER_CLASS::GREEK ):                         cc16 = &hcore::character_class<CHARACTER_CLASS::GREEK>();                         break;
			case ( CHARACTER_CLASS::LOWER_CASE_GREEK ):              cc16 = &hcore::character_class<CHARACTER_CLASS::LOWER_CASE_GREEK>();              break;
			case ( CHARACTER_CLASS::UPPER_CASE_GREEK ):              cc16 = &hcore::character_class<CHARACTER_CLASS::UPPER_CASE_GREEK>();              break;
			case ( CHARACTER_CLASS::SUBSCRIPT ):                     cc16 = &hcore::character_class<CHARACTER_CLASS::SUBSCRIPT>();                     break;
			case ( CHARACTER_CLASS::SUBSCRIPT_DIGIT ):               cc16 = &hcore::character_class<CHARACTER_CLASS::SUBSCRIPT_DIGIT>();               break;
			case ( CHARACTER_CLASS::SUBSCRIPT_LETTER ):              cc16 = &hcore::character_class<CHARACTER_CLASS::SUBSCRIPT_LETTER>();              break;
			case ( CHARACTER_CLASS::SUBSCRIPT_LOWER_CASE_LETTER ):   cc16 = &hcore::character_class<CHARACTER_CLASS::SUBSCRIPT_LOWER_CASE_LETTER>();   break;
			case ( CHARACTER_CLASS::SUBSCRIPT_UPPER_CASE_LETTER ):   cc16 = &hcore::character_class<CHARACTER_CLASS::SUBSCRIPT_UPPER_CASE_LETTER>();   break;
			case ( CHARACTER_CLASS::SUPERSCRIPT ):                   cc16 = &hcore::character_class<CHARACTER_CLASS::SUPERSCRIPT>();                   break;
			case ( CHARACTER_CLASS::SUPERSCRIPT_DIGIT ):             cc16 = &hcore::character_class<CHARACTER_CLASS::SUPERSCRIPT_DIGIT>();             break;
			case ( CHARACTER_CLASS::SUPERSCRIPT_LETTER ):            cc16 = &hcore::character_class<CHARACTER_CLASS::SUPERSCRIPT_LETTER>();            break;
			case ( CHARACTER_CLASS::SUPERSCRIPT_LOWER_CASE_LETTER ): cc16 = &hcore::character_class<CHARACTER_CLASS::SUPERSCRIPT_LOWER_CASE_LETTER>(); break;
			case ( CHARACTER_CLASS::SUPERSCRIPT_UPPER_CASE_LETTER ): cc16 = &hcore::character_class<CHARACTER_CLASS::SUPERSCRIPT_UPPER_CASE_LETTER>(); break;
		}
		return (
			cc8
				? thread_->runtime().object_factory()->create_string( HString( cc8->data(), cc8->size() ) )
				: thread_->runtime().object_factory()->create_string( HString( cc16->data(), cc16->size() ) )
		);
		M_EPILOG
	}
	static HHuginn::value_t repeat( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "Text.repeat", values_, { HHuginn::TYPE::STRING, HHuginn::TYPE::INTEGER }, thread_, position_ );
		HString out;
		HString const& s( get_string( values_[0] ) );
		int count( static_cast<int>( get_integer( values_[1] ) ) );
		if ( count < 0 ) {
			throw HHuginn::HHuginnRuntimeException( "Negative repeat count: "_ys.append( count ), thread_->current_frame()->file_id(), position_ );
		}
		out.reserve( count * s.get_length() );
		for ( int i( 0 ); i < count; ++ i ) {
			out.append( s );
		}
		return ( thread_->runtime().object_factory()->create_string( yaal::move( out ) ) );
		M_EPILOG
	}
	static HHuginn::value_t int_base_to_str( char const* name_, BASE base_, huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( name_, values_, { HHuginn::TYPE::INTEGER }, thread_, position_ );
		return (
			thread_->runtime().object_factory()->create_string(
				int_to_str( get_integer( values_[0] ), base_, true )
			)
		);
		M_EPILOG
	}
	static HHuginn::value_t substitute_environment( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Text.substitute_environment";
		verify_arg_count( name, values_, 1, 2, thread_, position_ );
		verify_arg_type( name, values_, 0, HHuginn::TYPE::STRING, ARITY::MULTIPLE, thread_, position_ );
		bool recursively( true );
		if ( values_.get_size() > 1 ) {
			verify_arg_type( name, values_, 1, HHuginn::TYPE::BOOLEAN, ARITY::MULTIPLE, thread_, position_ );
			recursively = get_boolean( values_[1] );
		}
		HString s( get_string( values_[0] ) );
		hcore::substitute_environment( s, recursively ? ENV_SUBST_MODE::RECURSIVE : ENV_SUBST_MODE::ONE_LAYER );
		return ( thread_->object_factory().create_string( yaal::move( s ) ) );
		M_EPILOG
	}
	static HHuginn::value_t capitalize( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "Text.capitalize", values_, { HHuginn::TYPE::STRING }, thread_, position_ );
		HString s( get_string( values_[0] ) );
		if ( ! s.is_empty() ) {
			s.lower();
			s.set_at( 0, code_point_t( static_cast<code_point_t::value_type>( std::towupper( static_cast<wint_t>( s[0].get() ) ) ) ) );
		}
		return ( thread_->runtime().object_factory()->create_string( yaal::move( s ) ) );
		M_EPILOG
	}
	static HHuginn::value_t ordinal_cardinal( char const* name_, yaal::hcore::HString ( *func_ )( int ), huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( name_, values_, { HHuginn::TYPE::INTEGER }, thread_, position_ );
		HString s;
		try {
		 int i( safe_int::cast<int>( get_integer( values_[0] ) ) );
		 s = func_( i );
		} catch ( HException const& e ) {
			thread_->raise( thread_->object_factory().conversion_exception_class(), e.what(), position_ );
		}
		return ( thread_->runtime().object_factory()->create_string( yaal::move( s ) ) );
		M_EPILOG
	}
	static HHuginn::value_t parse_boolean_flag( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "Text.parse_boolean_flag", values_, { HHuginn::TYPE::STRING }, thread_, position_ );
		HString const& s( get_string( values_[0] ) );
		bool flag( false );
		try {
			flag = lexical_cast<bool>( s );
		} catch ( HException const& e ) {
			thread_->raise( thread_->object_factory().conversion_exception_class(), e.what(), position_ );
		}
		return ( flag ? thread_->runtime().true_value() : thread_->runtime().false_value() );
		M_EPILOG
	}
};

namespace package_factory {

class HTextCreator : public HPackageCreatorInterface {
protected:
	virtual HInstance do_new_instance( HRuntime* );
} textCreator;

HPackageCreatorInterface::HInstance HTextCreator::do_new_instance( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"Text",
			"The `Text` package provides various text manipulation algorithms.",
			HHuginn::ACCESS::PRIVATE
		)
	);
	HHuginn::field_definitions_t fd{
		{ "stream",   runtime_->create_method( &HText::stream ),   "( [*str*] ) - create read/write text `Stream` object" },
		{ "split",    runtime_->create_method( &HText::split ),    "( *str*, *sep* ) - split `string` *str* by separator *sep* into a `list` of `string`s" },
		{ "join",     runtime_->create_method( &HText::join ),     "( *coll*, *sep* ) - join all string from *coll* into one `string` using *sep* as separator" },
		{ "distance", runtime_->create_method( &HText::distance ), "( *first*, *second* ) - calculate Damerau-Levenshtein distance between *first* and *second* `string`s" },
		{ "repeat",   runtime_->create_method( &HText::repeat ),   "( *seed*, *count* ) - construct new `string` by repeating *seed* `string` *count* times" },
		{ "hex",      runtime_->create_method( &HText::int_base_to_str, "Text.hex", BASE::HEX ), "( *int* ) - convert *int* value to a `string` using hexadecimal representation" },
		{ "oct",      runtime_->create_method( &HText::int_base_to_str, "Text.oct", BASE::OCT ), "( *int* ) - convert *int* value to a `string` using octal representation" },
		{ "bin",      runtime_->create_method( &HText::int_base_to_str, "Text.bin", BASE::BIN ), "( *int* ) - convert *int* value to a `string` using binary representation" },
		{ "ordinal",  runtime_->create_method( &HText::ordinal_cardinal, "Text.ordinal",  &util::ordinal ),    "( *int* ) - get text representation of an ordinal number *int*" },
		{ "cardinal", runtime_->create_method( &HText::ordinal_cardinal, "Text.cardinal", &util::cardinal ),   "( *int* ) - get text representation of a cardinal number *int*" },
		{ "capitalize",             runtime_->create_method( &HText::capitalize ),             "( *str* ) - return \"Capitalized\" version of input \"capItaLiZeD\" *str*" },
		{ "parse_boolean_flag",     runtime_->create_method( &HText::parse_boolean_flag ),     "( *str* ) - interpret *str* as a boolean flag" },
		{ "character_class",        runtime_->create_method( &HText::character_class ),        "( *class* ) - get given character *class*" },
		{ "substitute_environment", runtime_->create_method( &HText::substitute_environment ), "( *str*[, *recursively*] ) - (*recursively*) substitute environment variables in *str*" }
	};
	c->redefine( nullptr, fd );
	return { c, runtime_->object_factory()->create<HText>( c.raw() ) };
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

