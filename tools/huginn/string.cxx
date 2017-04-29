/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  string.cxx - this file is integral part of `yaal' project.

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
	virtual bool do_is_valid( void ) override {
		return ( _index < _string->value().get_size() );
	}
	virtual void do_next( HThread*, int ) override {
		++ _index;
	}
private:
	HStringIterator( HStringIterator const& ) = delete;
	HStringIterator& operator = ( HStringIterator const& ) = delete;
};

namespace string {

typedef int long ( yaal::hcore::HString::*finder_t )( yaal::hcore::HString const&, int long ) const;
typedef int long ( yaal::hcore::HString::*finder_raw_t )( HString const&, int long ) const;

inline HHuginn::value_t find( char const* name_, finder_t finder_, int long default_, huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( name_, values_, 1, 2, position_ );
	verify_arg_type( name_, values_, 0, HHuginn::TYPE::STRING, ARITY::MULTIPLE, position_ );
	int long startAt( default_ );
	if ( values_.get_size() > 1 ) {
		verify_arg_type( name_, values_, 1, HHuginn::TYPE::INTEGER, ARITY::MULTIPLE, position_ );
		startAt = static_cast<int long>( get_integer( values_[1] ) );
	}

	int long pos( (get_string( object_->raw() ).*finder_)( get_string( values_[0] ), startAt ) );
	return ( thread_->object_factory().create_integer( pos != hcore::HString::npos ? pos : -1 ) );
	M_EPILOG
}

inline HHuginn::value_t find_raw( char const* name_, finder_raw_t finder_, int long default_, huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( name_, values_, 1, 2, position_ );
	verify_arg_type( name_, values_, 0, HHuginn::TYPE::STRING, ARITY::MULTIPLE, position_ );
	int long startAt( default_ );
	if ( values_.get_size() > 1 ) {
		verify_arg_type( name_, values_, 1, HHuginn::TYPE::INTEGER, ARITY::MULTIPLE, position_ );
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
	HHuginn::values_t const& _values;
	int _position;
public:
	HHuginnStringFormatter( HThread* thread_, HString const& format_, HString& result_, HHuginn::values_t const& values_, int position_ )
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
			throw HHuginn::HHuginnRuntimeException( hcore::to_string( msg_ ).append( _it - _format.begin() ), _position );
		}
	}
	void format( void ) {
		M_PROLOG
		int fmtSubstCount( 0 );
		bool autoIndex( false );
		HString idxRaw;
		char const* errMsg( "Invalid format specification at: " );
		int maxUsedValue( -1 );
		int valCount( static_cast<int>( _values.get_size() ) );
		for ( HString::const_iterator end( _format.end() ); _it != end; ++ _it ) {
			if ( *_it == FMT_OPEN ) {
				++ _it;
				ensure( _it != end, "Single '{' encountered in format string at: " );
				if ( *_it != FMT_OPEN ) {
					idxRaw.clear();
					while ( ( _it != end ) && _digit_.has( static_cast<u32_t>( *_it ) ) ) { /* *TODO* *FIXME* Remove static cast after UCS in HString is implemented. */
						idxRaw.append( *_it );
						++ _it;
					}
					ensure( _it != end, errMsg );
					if ( *_it == FMT_SPEC ) {
						++ _it;
						ensure( _it != end, errMsg );
					}
					ensure( *_it == FMT_CLOSE, errMsg );
					if ( fmtSubstCount > 0 ) {
						ensure( (  autoIndex && idxRaw.is_empty() ) || ( ! autoIndex && ! idxRaw.is_empty() ), "Cannot mix manual and automatic field numbering at: " );
					}
					autoIndex = idxRaw.is_empty();
					int idx( fmtSubstCount );
					if ( ! autoIndex ) {
						try {
							idx = lexical_cast<int>( idxRaw );
						} catch ( HException const& e ) {
							throw HHuginn::HHuginnRuntimeException( e.what(), _position );
						}
					}
					++ fmtSubstCount;
					maxUsedValue = max( idx, maxUsedValue );
					ensure( idx < valCount, "Wrong value index at: " );
					HHuginn::value_t v( value_builtin::string( _thread, _values[idx], _position ) );
					_result.append( static_cast<HHuginn::HString*>( v.raw() )->value() );
					continue;
				}
			} else if ( *_it == FMT_CLOSE ) {
				++ _it;
				ensure( ( _it != end ) && ( *_it == FMT_CLOSE ), "Single '}' encountered in format string at: " );
			}
			_result.append( *_it );
		}
		ensure( maxUsedValue == ( valCount - 1 ), "Not all values used in format at: " );
		return;
		M_EPILOG
	}
private:
	HHuginnStringFormatter( HHuginnStringFormatter const& ) = delete;
	HHuginnStringFormatter& operator = ( HHuginnStringFormatter const& ) = delete;
};

inline HHuginn::value_t format( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	HString const& fmt( static_cast<HHuginn::HString*>( object_->raw() )->value() );
	HHuginn::value_t v( thread_->object_factory().create_string() );
	HString& s( static_cast<HHuginn::HString*>( v.raw() )->value() );
	HHuginnStringFormatter hsf( thread_, fmt, s, values_, position_ );
	hsf.format();
	return ( v );
	M_EPILOG
}

inline HHuginn::value_t replace( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_signature( "string.replace", values_, { HHuginn::TYPE::STRING, HHuginn::TYPE::STRING }, position_ );
	static_cast<HHuginn::HString*>( object_->raw() )->value().replace( get_string( values_[0] ), get_string( values_[1] ) );
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t strip( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	char const name[] = "string.strip";
	verify_arg_count( name, values_, 0, 1, position_ );
	HString const* trimChars( nullptr );
	if ( values_.get_size() > 0 ) {
		verify_arg_type( name, values_, 0, HHuginn::TYPE::STRING, ARITY::UNARY, position_ );
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

inline HHuginn::value_t to_lower( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	char const name[] = "string.to_lower";
	verify_arg_count( name, values_, 0, 0, position_ );
	static_cast<HHuginn::HString*>( object_->raw() )->value().lower();
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t to_upper( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	char const name[] = "string.to_upper";
	verify_arg_count( name, values_, 0, 0, position_ );
	static_cast<HHuginn::HString*>( object_->raw() )->value().upper();
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t clear( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	char const name[] = "string.clear";
	verify_arg_count( name, values_, 0, 0, position_ );
	static_cast<HHuginn::HString*>( object_->raw() )->value().clear();
	return ( *object_ );
	M_EPILOG
}

HHuginn::class_t get_class( HRuntime*, HObjectFactory* );
HHuginn::class_t get_class( HRuntime* runtime_, HObjectFactory* objectFactory_ ) {
	M_PROLOG
	HHuginn::class_t c(
		make_pointer<HHuginn::HClass>(
			runtime_,
			type_id( HHuginn::TYPE::STRING ),
			runtime_->identifier_id( type_name( HHuginn::TYPE::STRING ) ),
			nullptr,
			HHuginn::field_definitions_t{
				{ "find",                 objectFactory_->create<HHuginn::HClass::HMethod>( hcore::call( &string::find,     "string.find",                 static_cast<finder_t>( &HString::find ),      0, _1, _2, _3, _4 ) ), "( *needle*, *from* ) - find position of substring *needle* that start not sooner than *from* position in the string" },
				{ "find_last",            objectFactory_->create<HHuginn::HClass::HMethod>( hcore::call( &string::find,     "string.find_last",            static_cast<finder_t>( &HString::find_last ), hcore::HString::npos + 0, _1, _2, _3, _4 ) ), "( *needle*, *before* ) - find position of substring *needle* that ends just before *before* in the string" },
				{ "find_one_of",          objectFactory_->create<HHuginn::HClass::HMethod>( hcore::call( &string::find_raw, "string.find_one_of",          static_cast<finder_raw_t>( &HString::find_one_of ), 0, _1, _2, _3, _4 ) ), "( *set* ) - find position of any of characters in given *set* that appears first in the string but not sooner than *from*" },
				{ "find_last_one_of",     objectFactory_->create<HHuginn::HClass::HMethod>( hcore::call( &string::find_raw, "string.find_last_one_of",     static_cast<finder_raw_t>( &HString::find_last_one_of ), hcore::HString::npos + 0, _1, _2, _3, _4 ) ), "( *set* ) - find position of any characters from given *set* that appears last just before *before* position in the string" },
				{ "find_other_than",      objectFactory_->create<HHuginn::HClass::HMethod>( hcore::call( &string::find_raw, "string.find_other_than",      static_cast<finder_raw_t>( &HString::find_other_than ), 0, _1, _2, _3, _4 ) ), "( *set* ) - find position of any of characters that is not present in given *set* that appears first in the string but not sooner than *from*"  },
				{ "find_last_other_than", objectFactory_->create<HHuginn::HClass::HMethod>( hcore::call( &string::find_raw, "string.find_last_other_than", static_cast<finder_raw_t>( &HString::find_last_other_than ), hcore::HString::npos + 0, _1, _2, _3, _4 ) ), "( *set* ) - find position of any characters that in not present in given *set* that appears last just before *before* position in the string" },
				{ "format",               objectFactory_->create<HHuginn::HClass::HMethod>( hcore::call( &string::format, _1, _2, _3, _4 ) ),   "( *item...* ) - construct string based on *format template* using *item*s as format substitutions" },
				{ "replace",              objectFactory_->create<HHuginn::HClass::HMethod>( hcore::call( &string::replace, _1, _2, _3, _4 ) ),  "( *what*, *with* ) - replace all occurrences of *what* substring with *with* substring" },
				{ "strip",                objectFactory_->create<HHuginn::HClass::HMethod>( hcore::call( &string::strip, _1, _2, _3, _4 ) ),    "strip( *set* ) - strip all occurrences of characters in *set* from both ends of the string" },
				{ "to_lower",             objectFactory_->create<HHuginn::HClass::HMethod>( hcore::call( &string::to_lower, _1, _2, _3, _4 ) ), "turn all string's characters to lower case" },
				{ "to_upper",             objectFactory_->create<HHuginn::HClass::HMethod>( hcore::call( &string::to_upper, _1, _2, _3, _4 ) ), "turn all string's characters to upper case" },
				{ "clear",                objectFactory_->create<HHuginn::HClass::HMethod>( hcore::call( &string::clear, _1, _2, _3, _4 ) ),    "erase string content" }
			},
			"The `string` is a scalar type that is used to represent and operate on character strings. "
			"It supports basic operations of addition and comparisons, it also supports subscript and range operators."
		)
	);
	return ( c );
	M_EPILOG
}

}

}

HHuginn::HString::HString( HHuginn::HClass const* class_, yaal::hcore::HString const& value_ )
	: HIterable( class_ )
	, _value( value_ ) {
	return;
}

HHuginn::value_t HHuginn::HString::do_clone( huginn::HThread* thread_, int ) const {
	return ( thread_->runtime().object_factory()->create_string( _value ) );
}

HHuginn::HIterable::HIterator HHuginn::HString::do_iterator( huginn::HThread*, int ) {
	HIterator::iterator_implementation_t impl( new ( memory::yaal ) HStringIterator( this ) );
	return ( HIterator( yaal::move( impl ) ) );
}

int long HHuginn::HString::do_size( void ) const {
	return ( _value.get_length() );
}

}

}

