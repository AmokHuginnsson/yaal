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

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

class HStringIterator : public HIteratorInterface {
	HHuginn::HString* _string;
	HObjectFactory* _objectFactory;
	int _index;
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
typedef int long ( yaal::hcore::HString::*finder_raw_t )( char const*, int long ) const;

inline HHuginn::value_t find( char const* name_, finder_t finder_, int long default_, huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( name_, values_, 1, 2, position_ );
	verify_arg_type( name_, values_, 0, HHuginn::TYPE::STRING, false, position_ );
	int long startAt( default_ );
	if ( values_.get_size() > 1 ) {
		verify_arg_type( name_, values_, 1, HHuginn::TYPE::INTEGER, false, position_ );
		startAt = static_cast<int long>( get_integer( values_[1] ) );
	}

	int long pos( (get_string( object_->raw() ).*finder_)( get_string( values_[0] ), startAt ) );
	return ( thread_->object_factory().create_integer( pos != hcore::HString::npos ? pos : -1 ) );
	M_EPILOG
}

inline HHuginn::value_t find_raw( char const* name_, finder_raw_t finder_, int long default_, huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( name_, values_, 1, 2, position_ );
	verify_arg_type( name_, values_, 0, HHuginn::TYPE::STRING, false, position_ );
	int long startAt( default_ );
	if ( values_.get_size() > 1 ) {
		verify_arg_type( name_, values_, 1, HHuginn::TYPE::INTEGER, false, position_ );
		startAt = static_cast<int long>( get_integer( values_[1] ) );
	}

	int long pos( (get_string( object_->raw() ).*finder_)( get_string( values_[0] ).raw(), startAt ) );
	return ( thread_->object_factory().create_integer( pos != hcore::HString::npos ? pos : -1 ) );
	M_EPILOG
}

inline HHuginn::value_t replace( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	char const name[] = "string.replace";
	verify_arg_count( name, values_, 2, 2, position_ );
	verify_arg_type( name, values_, 0, HHuginn::TYPE::STRING, false, position_ );
	verify_arg_type( name, values_, 1, HHuginn::TYPE::STRING, false, position_ );
	static_cast<HHuginn::HString*>( object_->raw() )->value().replace( get_string( values_[0] ), get_string( values_[1] ) );
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t strip( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	char const name[] = "string.strip";
	verify_arg_count( name, values_, 0, 1, position_ );
	char const* trimChars( nullptr );
	if ( values_.get_size() > 0 ) {
		verify_arg_type( name, values_, 0, HHuginn::TYPE::STRING, true, position_ );
		trimChars = get_string( values_[0] ).raw();
	}
	HString dest( get_string( object_->raw() ) );
	int long len( dest.get_length() );
	if ( trimChars ) {
		dest.trim( trimChars );
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

HHuginn::class_t get_class( HRuntime* );
HHuginn::class_t get_class( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		make_pointer<HHuginn::HClass>(
			runtime_,
			type_id( HHuginn::TYPE::STRING ),
			runtime_->identifier_id( type_name( HHuginn::TYPE::STRING ) ),
			nullptr,
			HHuginn::field_definitions_t{
				{ "find",                 make_pointer<HHuginn::HClass::HMethod>( hcore::call( &string::find,     "string.find",                 static_cast<finder_t>( &HString::find ),      0, _1, _2, _3, _4 ) ), "( *needle*, *from* ) - find position of substring *needle* that start not sooner than *from* position in the string" },
				{ "find_last",            make_pointer<HHuginn::HClass::HMethod>( hcore::call( &string::find,     "string.find_last",            static_cast<finder_t>( &HString::find_last ), hcore::HString::npos + 0, _1, _2, _3, _4 ) ), "( *needle*, *before* ) - find position of substring *needle* that ends just before *before* in the string" },
				{ "find_one_of",          make_pointer<HHuginn::HClass::HMethod>( hcore::call( &string::find_raw, "string.find_one_of",          static_cast<finder_raw_t>( &HString::find_one_of ), 0, _1, _2, _3, _4 ) ), "( *set* ) - find position of any of characters in given *set* that appears first in the string but not sooner than *from*" },
				{ "find_last_one_of",     make_pointer<HHuginn::HClass::HMethod>( hcore::call( &string::find_raw, "string.find_last_one_of",     static_cast<finder_raw_t>( &HString::find_last_one_of ), hcore::HString::npos + 0, _1, _2, _3, _4 ) ), "( *set* ) - find position of any characters from given *set* that appears last just before *before* position in the string" },
				{ "find_other_than",      make_pointer<HHuginn::HClass::HMethod>( hcore::call( &string::find_raw, "string.find_other_than",      static_cast<finder_raw_t>( &HString::find_other_than ), 0, _1, _2, _3, _4 ) ), "( *set* ) - find position of any of characters that is not present in given *set* that appears first in the string but not sooner than *from*"  },
				{ "find_last_other_than", make_pointer<HHuginn::HClass::HMethod>( hcore::call( &string::find_raw, "string.find_last_other_than", static_cast<finder_raw_t>( &HString::find_last_other_than ), hcore::HString::npos + 0, _1, _2, _3, _4 ) ), "( *set* ) - find position of any characters that in not present in given *set* that appears last just before *before* position in the string" },
				{ "replace",              make_pointer<HHuginn::HClass::HMethod>( hcore::call( &string::replace, _1, _2, _3, _4 ) ), "( *what*, *with* ) - replace all occurrences of *what* substring with *with* substring" },
				{ "strip",                make_pointer<HHuginn::HClass::HMethod>( hcore::call( &string::strip, _1, _2, _3, _4 ) ), "strip( *set* ) - strip all occurrences of characters in *set* from both ends of the string" },
				{ "to_lower",             make_pointer<HHuginn::HClass::HMethod>( hcore::call( &string::to_lower, _1, _2, _3, _4 ) ), "turn all string's characters to lower case" },
				{ "to_upper",             make_pointer<HHuginn::HClass::HMethod>( hcore::call( &string::to_upper, _1, _2, _3, _4 ) ), "turn all string's characters to upper case" },
				{ "clear",                make_pointer<HHuginn::HClass::HMethod>( hcore::call( &string::clear, _1, _2, _3, _4 ) ), "erase string content" }
			},
			"The string is a scalar type that is used to represent and operate on character strings. "
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

HHuginn::value_t HHuginn::HString::do_clone( HRuntime* runtime_ ) const {
	return ( runtime_->object_factory()->create_string( _value ) );
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

