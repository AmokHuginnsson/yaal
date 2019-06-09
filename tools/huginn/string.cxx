/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstdio>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "string.hxx"
#include "runtime.hxx"
#include "builtin.hxx"
#include "iterator.hxx"
#include "helper.hxx"
#include "thread.hxx"
#include "objectfactory.hxx"
#include "instruction.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

namespace string {

class HStringIterator : public HIteratorInterface {
	HString* _string;
	HObjectFactory* _objectFactory;
	int long _index;
public:
	HStringIterator( HString* string_ )
		: _string( string_ )
		, _objectFactory( string_->get_class()->runtime()->object_factory() )
		, _index( 0 ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( HThread*, int ) override {
		return ( _objectFactory->create_character( _string->value()[ _index ] ) );
	}
	virtual bool do_is_valid( huginn::HThread*, int ) override {
		return ( _index < _string->value().get_size() );
	}
	virtual void do_next( HThread*, int ) override {
		++ _index;
	}
private:
	HStringIterator( HStringIterator const& ) = delete;
	HStringIterator& operator = ( HStringIterator const& ) = delete;
};

class HStringReverseIterator : public HIteratorInterface {
	HString* _string;
	HObjectFactory* _objectFactory;
	int long _index;
public:
	HStringReverseIterator( HString* string_ )
		: _string( string_ )
		, _objectFactory( string_->get_class()->runtime()->object_factory() )
		, _index( string_->value().get_length() - 1 ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( HThread*, int ) override {
		return ( _objectFactory->create_character( _string->value()[ _index ] ) );
	}
	virtual bool do_is_valid( HThread*, int ) override {
		return ( ( _index >= 0 ) && ( _index < _string->value().get_length() ) );
	}
	virtual void do_next( HThread*, int ) override {
		-- _index;
	}
private:
	HStringReverseIterator( HStringReverseIterator const& ) = delete;
	HStringReverseIterator& operator = ( HStringReverseIterator const& ) = delete;
};

class HReversedString : public huginn::HIterable {
	HHuginn::value_t _string;
public:
	HReversedString( HClass const* class_, HHuginn::value_t const& string_ )
		: HIterable( class_ )
		, _string( string_ ) {
		M_ASSERT( _string->type_id() == HHuginn::TYPE::STRING );
	}
	static HHuginn::class_t get_class( HRuntime* runtime_, HClass const* origin_ ) {
		M_PROLOG
		HHuginn::class_t c(
			make_pointer<HClass>(
				runtime_,
				"ReversedStringView",
				"The `ReversedStringView` class represents *lazy* *iterable* reversed view of a `string`.",
				origin_
			)
		);
		return ( c );
		M_EPILOG
	}
protected:
	virtual int long do_size( huginn::HThread* thread_, int position_ ) const override {
		return ( safe_int::cast<int long>( static_cast<HString const*>( _string.raw() )->size( thread_, position_ ) ) );
	}
private:
	virtual iterator_t do_iterator( HThread*, int ) override {
		return ( make_pointer<HStringReverseIterator>( static_cast<HString*>( _string.raw() ) ) );
	}
private:
	virtual HHuginn::value_t do_clone( huginn::HThread* thread_, HHuginn::value_t*, int ) const override {
		return ( thread_->object_factory().create<HReversedString>( HIterable::get_class(), _string ) );
	}
};

typedef int long ( yaal::hcore::HString::*finder_t )( yaal::hcore::HString const&, int long ) const;
typedef int long ( yaal::hcore::HString::*finder_raw_t )( hcore::HString const&, int long ) const;

inline HHuginn::value_t find( char const* name_, finder_t finder_, int long default_, huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( name_, values_, 1, 2, thread_, position_ );
	verify_arg_type( name_, values_, 0, HHuginn::TYPE::STRING, ARITY::MULTIPLE, thread_, position_ );
	int long startAt( default_ );
	if ( values_.get_size() > 1 ) {
		verify_arg_type( name_, values_, 1, HHuginn::TYPE::INTEGER, ARITY::MULTIPLE, thread_, position_ );
		startAt = static_cast<int long>( get_integer( values_[1] ) );
	}

	int long pos( (get_string( object_->raw() ).*finder_)( get_string( values_[0] ), startAt ) );
	return ( thread_->object_factory().create_integer( pos != hcore::HString::npos ? pos : -1 ) );
	M_EPILOG
}

inline HHuginn::value_t find_raw( char const* name_, finder_raw_t finder_, int long default_, huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( name_, values_, 1, 2, thread_, position_ );
	verify_arg_type( name_, values_, 0, HHuginn::TYPE::STRING, ARITY::MULTIPLE, thread_, position_ );
	int long startAt( default_ );
	if ( values_.get_size() > 1 ) {
		verify_arg_type( name_, values_, 1, HHuginn::TYPE::INTEGER, ARITY::MULTIPLE, thread_, position_ );
		startAt = static_cast<int long>( get_integer( values_[1] ) );
	}

	int long pos( (get_string( object_->raw() ).*finder_)( get_string( values_[0] ), startAt ) );
	return ( thread_->object_factory().create_integer( pos != hcore::HString::npos ? pos : -1 ) );
	M_EPILOG
}

class HHuginnStringFormatter {
public:
	enum class ALIGN {
		LEFT,
		CENTER,
		RIGHT
	};
	static char const FMT_OPEN = '{';
	static char const FMT_CLOSE = '}';
	static char const FMT_SPEC = ':';
private:
	HThread* _thread;
	hcore::HString const& _format;
	hcore::HString& _result;
	hcore::HString::const_iterator _it;
	HHuginn::values_t& _values;
	int _position;
public:
	HHuginnStringFormatter( HThread* thread_, hcore::HString const& format_, hcore::HString& result_, HHuginn::values_t& values_, int position_ )
		: _thread( thread_ )
		, _format( format_ )
		, _result( result_ )
		, _it( _format.begin() )
		, _values( values_ )
		, _position( position_ ) {
		return;
	}
	void ensure( bool condResult_, char const* msg_ ) {
		if ( ! condResult_ ) {
			throw HHuginn::HHuginnRuntimeException(
				hcore::to_string( msg_ ).append( " at: " ).append( _it - _format.begin() ), _thread->current_frame()->file_id(),
				_position
			);
		}
	}
	void format( void ) {
		M_PROLOG
		character_class_t const ALIGN_CLASS( "<^>", 3 );
		int fmtSubstCount( 0 );
		bool autoIndex( false );
		hcore::HString idxRaw;
		hcore::HString specRaw;
		hcore::HString formattedValue;
		char const* errMsg( "Invalid format specification" );
		int maxUsedValue( -1 );
		int valCount( static_cast<int>( _values.get_size() ) );
		for ( hcore::HString::const_iterator end( _format.end() ); _it != end; ++ _it ) {
			if ( *_it == FMT_OPEN ) {
				++ _it;
				ensure( _it != end, "Single '{' encountered in format string" );
				if ( *_it != FMT_OPEN ) {
					idxRaw.clear();
					while ( ( _it != end ) && is_digit( *_it ) ) {
						idxRaw.push_back( *_it );
						++ _it;
					}
					ensure( _it != end, errMsg );
					specRaw.clear();
					if ( *_it == FMT_SPEC ) {
						++ _it;
						while ( ( _it != end ) && ( *_it != FMT_CLOSE ) ) {
							specRaw.push_back( *_it );
							++ _it;
						}
						ensure( _it != end, errMsg );
					}
					ensure( *_it == FMT_CLOSE, errMsg );
					if ( fmtSubstCount > 0 ) {
						ensure( (  autoIndex && idxRaw.is_empty() ) || ( ! autoIndex && ! idxRaw.is_empty() ), "Cannot mix manual and automatic field numbering" );
					}
					autoIndex = idxRaw.is_empty();
					int idx( fmtSubstCount );
					if ( ! autoIndex ) {
						try {
							idx = lexical_cast<int>( idxRaw );
						} catch ( hcore::HException const& e ) {
							throw HHuginn::HHuginnRuntimeException( e.what(), _thread->current_frame()->file_id(), _position );
						}
					}
					++ fmtSubstCount;
					maxUsedValue = max( idx, maxUsedValue );
					ensure( idx < valCount, "Wrong value index" );
					HHuginn::value_t const& v( _values[idx] );
					HHuginn::type_id_t type( v->type_id() );
					BASE base( BASE::DEC );
					ALIGN align( ( type == HHuginn::TYPE::STRING ) || ( type == HHuginn::TYPE::CHARACTER ) ? ALIGN::LEFT : ALIGN::RIGHT );
					bool prefix( false );
					int width( 0 );
					int prec( -1 );
					code_point_t fill( ' '_ycp );
					if ( ! specRaw.is_empty() ) {
						code_point_t typeSpec( specRaw.back() );
						specRaw.pop_back();
						switch ( typeSpec.get() ) {
							case ( 'x' ): base = BASE::HEX; /* fallthrough */
							case ( 'o' ): if ( base == BASE::DEC ) { base = BASE::OCT; } /* fallthrough */
							case ( 'b' ): if ( base == BASE::DEC ) { base = BASE::BIN; } /* fallthrough */
							case ( 'd' ): {
								ensure( type == HHuginn::TYPE::INTEGER, "Expected an `integer` type" );
							} break;
							case ( 'f' ): {
								ensure( type == HHuginn::TYPE::REAL, "Expected a `real` type" );
							} break;
							case ( 'n' ): {
								ensure( type == HHuginn::TYPE::NUMBER, "Expected a `number` type" );
							} break;
							case ( 's' ): {
								ensure( type == HHuginn::TYPE::STRING, "Expected a `string` type" );
							} break;
							case ( 'c' ): {
								ensure( type == HHuginn::TYPE::CHARACTER, "Expected a `character` type" );
							} break;
							default: {
								ensure( false, "Invalid type specification" );
							} break;
						}
						if ( ! specRaw.is_empty() && ( specRaw.front() == '#'_ycp ) ) {
							prefix = true;
							specRaw.shift_left( 1 );
						}
						if ( ( specRaw.get_size() >= 2 ) && ALIGN_CLASS.has( specRaw[1] ) ) {
							fill = specRaw.front();
							specRaw.shift_left( 1 );
						}
						if ( ! specRaw.is_empty() && ALIGN_CLASS.has( specRaw.front() ) ) {
							char alignSpec( static_cast<char>( specRaw.front().get() ) );
							align = alignSpec == '<' ? ALIGN::LEFT : ( alignSpec == '^' ? ALIGN::CENTER : ALIGN::RIGHT );
							specRaw.shift_left( 1 );
						}
						if ( ! specRaw.is_empty() && ( specRaw.front() == '0'_ycp ) ) {
							fill = '0'_ycp;
							specRaw.shift_left( 1 );
						}
						if ( ! specRaw.is_empty() && ( specRaw.front() != '.' ) ) {
							try {
								int endWidth( 0 );
								width = stoi( specRaw, &endWidth );
								specRaw.shift_left( endWidth );
							} catch ( hcore::HException const& e ) {
								ensure( false, e.what() );
							}
						}
						if ( ! specRaw.is_empty() ) {
							ensure( ( ( type == HHuginn::TYPE::REAL ) || ( type == HHuginn::TYPE::NUMBER ) ) && ( specRaw.front() == '.' ), errMsg );
							specRaw.shift_left( 1 );
							try {
								prec = lexical_cast<int>( specRaw );
							} catch ( HLexicalCastException const& e ) {
								ensure( false, e.what() );
							}
							ensure( prec >= 0, "Invalid precision specification" );
						}
					}
					if ( ( type == HHuginn::TYPE::INTEGER ) && ( base != BASE::DEC ) ) {
						formattedValue.assign( int_to_str( static_cast<huginn::HInteger const*>( v.raw() )->value(), base, prefix ) );
					} else if ( ( ( type == HHuginn::TYPE::REAL ) || ( type == HHuginn::TYPE::NUMBER ) ) && ( prec >= 0 ) ) {
						if ( type == HHuginn::TYPE::REAL ) {
							static int const FMT_SIZE( 32 );
							char fmt[FMT_SIZE];
							snprintf( fmt, FMT_SIZE, "%%.%dLf", prec );
							static int const MAX_FLOAT_DIGIT_COUNT( 8192 );
							char buffer[MAX_FLOAT_DIGIT_COUNT] = "\0";
							int charCount( snprintf( buffer, MAX_FLOAT_DIGIT_COUNT, prec >= 0 ? fmt : "%Lf", get_real( v ) ) );
							formattedValue.assign( buffer, charCount );
						} else if ( prec >= 0 ) {
							hcore::HNumber n( get_number( v ) );
							n.round( prec );
							formattedValue = n.to_string();
						} else {
							formattedValue = get_number( v ).to_string();
						}
						hcore::HString::size_type dotPos( formattedValue.find( '.'_ycp ) );
						if ( dotPos == hcore::HString::npos ) {
							dotPos = formattedValue.get_length();
							formattedValue.push_back( '.'_ycp );
						}
						hcore::HString::size_type realPrec( formattedValue.get_length() - dotPos - 1 );
						if ( realPrec > prec ) {
							formattedValue.erase( dotPos + 1 + prec );
						} else if ( prec > realPrec ) {
							formattedValue.append( prec - realPrec, '0'_ycp );
						}
					} else {
						HHuginn::value_t sv( instruction::string( _thread, _values[idx], _position ) );
						formattedValue.assign( static_cast<HString*>( sv.raw() )->value() );
					}
					if ( width > formattedValue.get_length() ) {
						int space( static_cast<int>( width - formattedValue.get_length() ) );
						switch ( align ) {
							case ( ALIGN::LEFT ): {
								formattedValue.append( space, fill );
							} break;
							case ( ALIGN::CENTER ): {
								formattedValue.shift_right( space / 2, fill );
								formattedValue.append( space - space / 2, fill );
							} break;
							case ( ALIGN::RIGHT ): {
								if ( prefix && ( type == HHuginn::TYPE::INTEGER ) && ( base != BASE::DEC ) && ( fill == '0'_ycp ) ) {
									formattedValue.insert( 2, space, fill );
								} else {
									formattedValue.shift_right( space, fill );
								}
							} break;
						}
					}
					_result.append( formattedValue );
					continue;
				}
			} else if ( *_it == FMT_CLOSE ) {
				++ _it;
				ensure( ( _it != end ) && ( *_it == FMT_CLOSE ), "Single '}' encountered in format string" );
			}
			_result.append( *_it );
		}
		ensure( maxUsedValue == ( valCount - 1 ), "Not all values used in format" );
		return;
		M_EPILOG
	}
private:
	HHuginnStringFormatter( HHuginnStringFormatter const& ) = delete;
	HHuginnStringFormatter& operator = ( HHuginnStringFormatter const& ) = delete;
};

inline HHuginn::value_t format( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	hcore::HString const& fmt( static_cast<HString*>( object_->raw() )->value() );
	HHuginn::value_t v( thread_->object_factory().create_string() );
	hcore::HString& s( static_cast<HString*>( v.raw() )->value() );
	HHuginnStringFormatter hsf( thread_, fmt, s, values_, position_ );
	hsf.format();
	return ( v );
	M_EPILOG
}

inline HHuginn::value_t replace( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_signature( "string.replace", values_, { HHuginn::TYPE::STRING, HHuginn::TYPE::STRING }, thread_, position_ );
	static_cast<HString*>( object_->raw() )->value().replace( get_string( values_[0] ), get_string( values_[1] ) );
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t strip( hcore::HString&( hcore::HString::* trim_ )( hcore::HString const& ), huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "string.strip";
	verify_arg_count( name, values_, 0, 1, thread_, position_ );
	hcore::HString const* trimChars( nullptr );
	if ( values_.get_size() > 0 ) {
		verify_arg_type( name, values_, 0, HHuginn::TYPE::STRING, ARITY::UNARY, thread_, position_ );
		trimChars = &get_string( values_[0] );
	}
	hcore::HString dest( get_string( object_->raw() ) );
	int long len( dest.get_length() );
	if ( trimChars ) {
		(dest.*trim_)( *trimChars );
	} else {
		(dest.*trim_)( character_class<CHARACTER_CLASS::WHITESPACE>().data() );
	}
	HHuginn::value_t v;
	if ( dest.get_length() != len ) {
		v =  thread_->object_factory().create_string( yaal::move( dest ) );
	} else {
		v = *object_;
	}
	return ( v );
	M_EPILOG
}

inline HHuginn::value_t to_lower( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "string.to_lower", values_, 0, 0, thread_, position_ );
	static_cast<HString*>( object_->raw() )->value().lower();
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t to_upper( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "string.to_upper", values_, 0, 0, thread_, position_ );
	static_cast<HString*>( object_->raw() )->value().upper();
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t clear( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "string.clear", values_, 0, 0, thread_, position_ );
	static_cast<HString*>( object_->raw() )->value().clear();
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t starts_with( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_signature( "string.starts_with", values_, { HHuginn::TYPE::STRING }, thread_, position_ );
	hcore::HString const& subject( static_cast<HString*>( object_->raw() )->value() );
	hcore::HString const& object( get_string( values_[0] ) );
	return ( thread_->runtime().boolean_value( object.is_empty() || ( subject.find( object ) == 0 ) ) );
	M_EPILOG
}

inline HHuginn::value_t ends_with( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_signature( "string.ends_with", values_, { HHuginn::TYPE::STRING }, thread_, position_ );
	hcore::HString const& subject( static_cast<HString*>( object_->raw() )->value() );
	hcore::HString const& object( get_string( values_[0] ) );
	int long pos( subject.find_last( object ) );
	return (
		thread_->runtime().boolean_value(
			object.is_empty()
			|| ( ( pos != HString::npos ) && ( pos == ( subject.get_length() - object.get_length() ) ) )
		)
	);
	M_EPILOG
}

class HStringClass : public HClass {
public:
	typedef HStringClass this_type;
	typedef HClass base_type;
private:
	HHuginn::class_t _reversedStringClass;
public:
	HStringClass(
		HRuntime* runtime_,
		HObjectFactory* objectFactory_
	) : HClass(
			runtime_,
			objectFactory_,
			huginn::type_id( HHuginn::TYPE::STRING ),
			runtime_->identifier_id( type_name( HHuginn::TYPE::STRING ) ),
			"The `string` is a scalar type that is used to represent and operate on character strings. "
			"It supports basic operations of addition and comparisons, it also supports subscript and range operators.",
			&builtin::string
		)
		, _reversedStringClass( HReversedString::get_class( runtime_, this ) ) {
		HHuginn::field_definitions_t fd{
			{ "find",                 objectFactory_->create_method( &string::find,     "string.find",                 static_cast<finder_t>( &hcore::HString::find ),      0 ), "( *needle*, *from* ) - find position of substring *needle* that start not sooner than *from* position in the `string`" },
			{ "find_last",            objectFactory_->create_method( &string::find,     "string.find_last",            static_cast<finder_t>( &hcore::HString::find_last ), hcore::HString::npos + 0 ), "( *needle*, *before* ) - find position of substring *needle* that ends just before *before* in the `string`" },
			{ "find_one_of",          objectFactory_->create_method( &string::find_raw, "string.find_one_of",          static_cast<finder_raw_t>( &hcore::HString::find_one_of ), 0 ), "( *set* ) - find position of any of characters in given *set* that appears first in the `string` but not sooner than *from*" },
			{ "find_last_one_of",     objectFactory_->create_method( &string::find_raw, "string.find_last_one_of",     static_cast<finder_raw_t>( &hcore::HString::find_last_one_of ), hcore::HString::npos + 0 ), "( *set* ) - find position of any characters from given *set* that appears last just before *before* position in the `string`" },
			{ "find_other_than",      objectFactory_->create_method( &string::find_raw, "string.find_other_than",      static_cast<finder_raw_t>( &hcore::HString::find_other_than ), 0 ), "( *set* ) - find position of any of characters that is not present in given *set* that appears first in the `string` but not sooner than *from*"  },
			{ "find_last_other_than", objectFactory_->create_method( &string::find_raw, "string.find_last_other_than", static_cast<finder_raw_t>( &hcore::HString::find_last_other_than ), hcore::HString::npos + 0 ), "( *set* ) - find position of any characters that in not present in given *set* that appears last just before *before* position in the `string`" },
			{ "format",               objectFactory_->create_method( &string::format ),      "( *item...* ) - construct `string` based on *format template* using *item*s as format substitutions" },
			{ "replace",              objectFactory_->create_method( &string::replace ),     "( *what*, *with* ) - replace all occurrences of *what* `string` with *with* `string`" },
			{ "strip",                objectFactory_->create_method( &string::strip, &hcore::HString::trim ),       "( *set* ) - strip all occurrences of characters in *set* from both ends of the `string`" },
			{ "strip_left",           objectFactory_->create_method( &string::strip, &hcore::HString::trim_left ),  "( *set* ) - strip all occurrences of characters in *set* from the left side of the `string`" },
			{ "strip_right",          objectFactory_->create_method( &string::strip, &hcore::HString::trim_right ), "( *set* ) - strip all occurrences of characters in *set* from the right side of the `string`" },
			{ "to_lower",             objectFactory_->create_method( &string::to_lower ),    "turn all `string`'s characters to lower case" },
			{ "to_upper",             objectFactory_->create_method( &string::to_upper ),    "turn all `string`'s characters to upper case" },
			{ "starts_with",          objectFactory_->create_method( &string::starts_with ), "( *prefix* ) - check if this `string` starts with given *prefix*" },
			{ "ends_with",            objectFactory_->create_method( &string::ends_with ),   "( *suffix* ) - check if this `string` ends with given *suffix*" },
			{ "clear",                objectFactory_->create_method( &string::clear ),       "erase `string` content" }
		};
		redefine( nullptr, fd );
		return;
	}
	HClass const* reversed_string_class( void ) const {
		return ( _reversedStringClass.raw() );
	}
protected:
	void do_finalize_registration( huginn::HRuntime* runtime_ ) {
		runtime_->huginn()->register_class( _reversedStringClass );
	}
};

HHuginn::class_t get_class( HRuntime*, HObjectFactory* );
HHuginn::class_t get_class( HRuntime* runtime_, HObjectFactory* objectFactory_ ) {
	M_PROLOG
	return ( make_pointer<HStringClass>( runtime_, objectFactory_ ) );
	M_EPILOG
}

HHuginn::value_t reversed_view( huginn::HThread* thread_, HHuginn::value_t const& value_ ) {
	M_PROLOG
	M_ASSERT( value_->type_id() == HHuginn::TYPE::STRING );
	HStringClass const* sc( static_cast<HStringClass const*>( value_->get_class() ) );
	return ( thread_->object_factory().create<HReversedString>( sc->reversed_string_class(), value_ ) );
	M_EPILOG
}

}

HString::HString( HClass const* class_, yaal::hcore::HString const& value_ )
	: HIterable( class_ )
	, _value( value_ ) {
	return;
}

HHuginn::value_t HString::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int ) const {
	return ( thread_->runtime().object_factory()->create_string( _value ) );
}

huginn::HIterable::iterator_t HString::do_iterator( huginn::HThread*, int ) {
	return ( make_pointer<string::HStringIterator>( this ) );
}

int long HString::do_size( huginn::HThread*, int ) const {
	return ( _value.get_length() );
}

bool HString::do_operator_equals( HThread*, HHuginn::value_t const&, HHuginn::value_t const& other_, int ) const {
	return ( _value == static_cast<HString const*>( other_.raw() )->_value );
}

bool HString::do_operator_less( HThread*, HHuginn::value_t const&, HHuginn::value_t const& other_, int ) const {
	return ( _value < static_cast<HString const*>( other_.raw() )->_value );
}

bool HString::do_operator_greater( HThread*, HHuginn::value_t const&, HHuginn::value_t const& other_, int ) const {
	return ( _value > static_cast<HString const*>( other_.raw() )->_value );
}

}

}

}

