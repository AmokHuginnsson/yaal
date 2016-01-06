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
		, _objectFactory( string_->get_class()->huginn()->object_factory() )
		, _index( 0 ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( void ) override {
		return ( _objectFactory->create_character( _string->value()[ _index ] ) );
	}
	virtual bool do_is_valid( void ) override {
		return ( _index < _string->value().get_size() );
	}
	virtual void do_next( void ) override {
		++ _index;
	}
private:
	HStringIterator( HStringIterator const& ) = delete;
	HStringIterator& operator = ( HStringIterator const& ) = delete;
};

namespace string {

inline HHuginn::value_t find( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	char const name[] = "string.find";
	verify_arg_count( name, values_, 1, 2, position_ );
	verify_arg_type( name, values_, 0, HHuginn::TYPE::STRING, false, position_ );
	int long startAt( 0 );
	if ( values_.get_size() > 1 ) {
		verify_arg_type( name, values_, 1, HHuginn::TYPE::INTEGER, false, position_ );
		startAt = static_cast<int long>( get_integer( values_[1] ) );
	}

	int long pos( get_string( object_->raw() ).find( get_string( values_[0] ), startAt ) );
	return ( thread_->object_factory().create_integer( pos != hcore::HString::npos ? pos : -1 ) );
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

HHuginn::class_t get_class( HHuginn* );
HHuginn::class_t get_class( HHuginn* huginn_ ) {
	M_PROLOG
	HHuginn::class_t c(
		make_pointer<HHuginn::HClass>(
			huginn_,
			type_id( HHuginn::TYPE::STRING ),
			huginn_->identifier_id( type_name( HHuginn::TYPE::STRING ) ),
			nullptr,
			HHuginn::field_names_t{
				"find",
				"strip",
				"to_lower",
				"to_upper"
			},
			HHuginn::values_t{
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &string::find, _1, _2, _3, _4 ) ),
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &string::strip, _1, _2, _3, _4 ) ),
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &string::to_lower, _1, _2, _3, _4 ) ),
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &string::to_upper, _1, _2, _3, _4 ) )
			}
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

HHuginn::value_t HHuginn::HString::do_clone( HHuginn* huginn_ ) const {
	return ( huginn_->object_factory()->create_string( _value ) );
}

HHuginn::HIterable::HIterator HHuginn::HString::do_iterator( void ) {
	HIterator::iterator_implementation_t impl( new ( memory::yaal ) HStringIterator( this ) );
	return ( HIterator( yaal::move( impl ) ) );
}

int long HHuginn::HString::do_size( void ) const {
	return ( _value.get_length() );
}

}

}

