/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "string.hxx"
#include "runtime.hxx"
#include "iterator.hxx"
#include "helper.hxx"
#include "thread.hxx"
#include "objectfactory.hxx"
#include "value_builtin.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

namespace string {

class HStringIterator : public HIteratorInterface {
	HHuginn::HString* _string;
	HObjectFactory* _objectFactory;
	int long _index;
public:
	HStringIterator( HHuginn::HString* string_ )
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
	HHuginn::HString* _string;
	HObjectFactory* _objectFactory;
	int long _index;
public:
	HStringReverseIterator( HHuginn::HString* string_ )
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
		return ( _index >= 0 );
	}
	virtual void do_next( HThread*, int ) override {
		-- _index;
	}
private:
	HStringReverseIterator( HStringReverseIterator const& ) = delete;
	HStringReverseIterator& operator = ( HStringReverseIterator const& ) = delete;
};

class HReversedString : public HHuginn::HIterable {
	HHuginn::value_t _string;
public:
	HReversedString( HHuginn::HClass const* class_, HHuginn::value_t const& string_ )
		: HIterable( class_ )
		, _string( string_ ) {
		M_ASSERT( _string->type_id() == HHuginn::TYPE::STRING );
	}
	static HHuginn::class_t get_class( HRuntime* runtime_ ) {
		M_PROLOG
		HHuginn::class_t c(
			runtime_->create_class(
				"ReversedStringView",
				nullptr,
				HHuginn::field_definitions_t{},
				"The `ReversedStringView` class represents *lazy* *iterable* reversed view of a `string`."
			)
		);
		return ( c );
		M_EPILOG
	}
protected:
	virtual int long do_size( huginn::HThread* thread_, int position_ ) const override {
		return ( safe_int::cast<int long>( static_cast<HHuginn::HString const*>( _string.raw() )->size( thread_, position_ ) ) );
	}
private:
	virtual HIterator do_iterator( HThread*, int ) override {
		HIterator::iterator_implementation_t impl( new ( memory::yaal ) HStringReverseIterator( static_cast<HHuginn::HString*>( _string.raw() ) ) );
		return ( HIterator( yaal::move( impl ) ) );
	}
private:
	virtual HHuginn::value_t do_clone( huginn::HThread* thread_, HHuginn::value_t*, int ) const override {
		return ( thread_->object_factory().create<HReversedString>( HIterable::get_class(), _string ) );
	}
};

typedef int long ( yaal::hcore::HString::*finder_t )( yaal::hcore::HString const&, int long ) const;
typedef int long ( yaal::hcore::HString::*finder_raw_t )( HString const&, int long ) const;

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
	static char const FMT_OPEN = '{';
	static char const FMT_CLOSE = '}';
	static char const FMT_SPEC = ':';
private:
	HThread* _thread;
	HString const& _format;
	HString& _result;
	HString::const_iterator _it;
	HHuginn::values_t& _values;
	int _position;
public:
	HHuginnStringFormatter( HThread* thread_, HString const& format_, HString& result_, HHuginn::values_t& values_, int position_ )
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
		int fmtSubstCount( 0 );
		bool autoIndex( false );
		HString idxRaw;
		HString specRaw;
		HString formatedValue;
		char const* errMsg( "Invalid format specification" );
		int maxUsedValue( -1 );
		int valCount( static_cast<int>( _values.get_size() ) );
		for ( HString::const_iterator end( _format.end() ); _it != end; ++ _it ) {
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
					if ( *_it == FMT_SPEC ) {
						++ _it;
						specRaw.clear();
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
						} catch ( HException const& e ) {
							throw HHuginn::HHuginnRuntimeException( e.what(), _thread->current_frame()->file_id(), _position );
						}
					}
					++ fmtSubstCount;
					maxUsedValue = max( idx, maxUsedValue );
					ensure( idx < valCount, "Wrong value index" );
					HHuginn::value_t const& v( _values[idx] );
					HHuginn::type_id_t type( v->type_id() );
					BASE base( BASE::DEC );
					bool prefix( false );
					int width( 0 );
					code_point_t fill( ' '_ycp );
					if ( ! specRaw.is_empty() ) {
						prefix = specRaw.front() == '#'_ycp;
						code_point_t typeSpec( specRaw.back() );
						specRaw.pop_back();
						if ( prefix ) {
							specRaw.shift_left( 1 );
						}
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
						if ( ! specRaw.is_empty() ) {
							if ( specRaw.front() == '0'_ycp ) {
								fill = '0'_ycp;
								specRaw.shift_left( 1 );
							}
							try {
								width = lexical_cast<int>( specRaw );
							} catch ( HLexicalCastException const& e ) {
								ensure( false, e.what() );
							}
						}
					}
					if ( ( type == HHuginn::TYPE::INTEGER ) && ( base != BASE::DEC ) ) {
						formatedValue.assign( int_to_str( static_cast<HHuginn::HInteger const*>( v.raw() )->value(), base, prefix ) );
					} else {
						HHuginn::value_t sv( value_builtin::string( _thread, _values[idx], _position ) );
						formatedValue.assign( static_cast<HHuginn::HString*>( sv.raw() )->value() );
					}
					if ( width > formatedValue.get_length() ) {
						formatedValue.shift_right( width - formatedValue.get_length(), fill );
					}
					_result.append( formatedValue );
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
	HString const& fmt( static_cast<HHuginn::HString*>( object_->raw() )->value() );
	HHuginn::value_t v( thread_->object_factory().create_string() );
	HString& s( static_cast<HHuginn::HString*>( v.raw() )->value() );
	HHuginnStringFormatter hsf( thread_, fmt, s, values_, position_ );
	hsf.format();
	return ( v );
	M_EPILOG
}

inline HHuginn::value_t replace( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_signature( "string.replace", values_, { HHuginn::TYPE::STRING, HHuginn::TYPE::STRING }, thread_, position_ );
	static_cast<HHuginn::HString*>( object_->raw() )->value().replace( get_string( values_[0] ), get_string( values_[1] ) );
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t strip( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "string.strip";
	verify_arg_count( name, values_, 0, 1, thread_, position_ );
	HString const* trimChars( nullptr );
	if ( values_.get_size() > 0 ) {
		verify_arg_type( name, values_, 0, HHuginn::TYPE::STRING, ARITY::UNARY, thread_, position_ );
		trimChars = &get_string( values_[0] );
	}
	HString dest( get_string( object_->raw() ) );
	int long len( dest.get_length() );
	if ( trimChars ) {
		dest.trim( *trimChars );
	} else {
		dest.trim();
	}
	HHuginn::value_t v;
	if ( dest.get_length() != len ) {
		v =  thread_->object_factory().create_string( dest );
	} else {
		v = *object_;
	}
	return ( v );
	M_EPILOG
}

inline HHuginn::value_t to_lower( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "string.to_lower";
	verify_arg_count( name, values_, 0, 0, thread_, position_ );
	static_cast<HHuginn::HString*>( object_->raw() )->value().lower();
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t to_upper( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "string.to_upper";
	verify_arg_count( name, values_, 0, 0, thread_, position_ );
	static_cast<HHuginn::HString*>( object_->raw() )->value().upper();
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t clear( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "string.clear";
	verify_arg_count( name, values_, 0, 0, thread_, position_ );
	static_cast<HHuginn::HString*>( object_->raw() )->value().clear();
	return ( *object_ );
	M_EPILOG
}

class HStringClass : public HHuginn::HClass {
public:
	typedef HStringClass this_type;
	typedef HHuginn::HClass base_type;
private:
	HHuginn::class_t _reversedStringClass;
public:
	HStringClass(
		HRuntime* runtime_,
		HObjectFactory* objectFactory_
	) : HHuginn::HClass(
			runtime_,
			huginn::type_id( HHuginn::TYPE::STRING ),
			runtime_->identifier_id( type_name( HHuginn::TYPE::STRING ) ),
			nullptr,
			HHuginn::field_definitions_t{
				{ "find",                 objectFactory_->create_method( hcore::call( &string::find,     "string.find",                 static_cast<finder_t>( &HString::find ),      0, _1, _2, _3, _4 ) ), "( *needle*, *from* ) - find position of substring *needle* that start not sooner than *from* position in the string" },
				{ "find_last",            objectFactory_->create_method( hcore::call( &string::find,     "string.find_last",            static_cast<finder_t>( &HString::find_last ), hcore::HString::npos + 0, _1, _2, _3, _4 ) ), "( *needle*, *before* ) - find position of substring *needle* that ends just before *before* in the string" },
				{ "find_one_of",          objectFactory_->create_method( hcore::call( &string::find_raw, "string.find_one_of",          static_cast<finder_raw_t>( &HString::find_one_of ), 0, _1, _2, _3, _4 ) ), "( *set* ) - find position of any of characters in given *set* that appears first in the string but not sooner than *from*" },
				{ "find_last_one_of",     objectFactory_->create_method( hcore::call( &string::find_raw, "string.find_last_one_of",     static_cast<finder_raw_t>( &HString::find_last_one_of ), hcore::HString::npos + 0, _1, _2, _3, _4 ) ), "( *set* ) - find position of any characters from given *set* that appears last just before *before* position in the string" },
				{ "find_other_than",      objectFactory_->create_method( hcore::call( &string::find_raw, "string.find_other_than",      static_cast<finder_raw_t>( &HString::find_other_than ), 0, _1, _2, _3, _4 ) ), "( *set* ) - find position of any of characters that is not present in given *set* that appears first in the string but not sooner than *from*"  },
				{ "find_last_other_than", objectFactory_->create_method( hcore::call( &string::find_raw, "string.find_last_other_than", static_cast<finder_raw_t>( &HString::find_last_other_than ), hcore::HString::npos + 0, _1, _2, _3, _4 ) ), "( *set* ) - find position of any characters that in not present in given *set* that appears last just before *before* position in the string" },
				{ "format",               objectFactory_->create_method( hcore::call( &string::format, _1, _2, _3, _4 ) ),   "( *item...* ) - construct string based on *format template* using *item*s as format substitutions" },
				{ "replace",              objectFactory_->create_method( hcore::call( &string::replace, _1, _2, _3, _4 ) ),  "( *what*, *with* ) - replace all occurrences of *what* substring with *with* substring" },
				{ "strip",                objectFactory_->create_method( hcore::call( &string::strip, _1, _2, _3, _4 ) ),    "strip( *set* ) - strip all occurrences of characters in *set* from both ends of the string" },
				{ "to_lower",             objectFactory_->create_method( hcore::call( &string::to_lower, _1, _2, _3, _4 ) ), "turn all string's characters to lower case" },
				{ "to_upper",             objectFactory_->create_method( hcore::call( &string::to_upper, _1, _2, _3, _4 ) ), "turn all string's characters to upper case" },
				{ "clear",                objectFactory_->create_method( hcore::call( &string::clear, _1, _2, _3, _4 ) ),    "erase string content" }
			},
			"The `string` is a scalar type that is used to represent and operate on character strings. "
			"It supports basic operations of addition and comparisons, it also supports subscript and range operators."
		)
		, _reversedStringClass( HReversedString::get_class( runtime_ ) ) {
		return;
	}
	HHuginn::HClass const* reversed_string_class( void ) const {
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

}

HHuginn::HString::HString( HHuginn::HClass const* class_, yaal::hcore::HString const& value_ )
	: HIterable( class_ )
	, _value( value_ ) {
	return;
}

HHuginn::value_t HHuginn::HString::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int ) const {
	return ( thread_->runtime().object_factory()->create_string( _value ) );
}

HHuginn::HIterable::HIterator HHuginn::HString::do_iterator( huginn::HThread*, int ) {
	HIterator::iterator_implementation_t impl( new ( memory::yaal ) string::HStringIterator( this ) );
	return ( HIterator( yaal::move( impl ) ) );
}

int long HHuginn::HString::do_size( huginn::HThread*, int ) const {
	return ( _value.get_length() );
}

}

}

