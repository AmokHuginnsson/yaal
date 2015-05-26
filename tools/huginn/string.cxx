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

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

HHuginn::type_t const HHuginn::TYPE::STRING( HHuginn::HType::register_type( "string", nullptr ) );

namespace huginn {

class HStringIterator : public HIteratorInterface {
	HHuginn::HString* _string;
	int _index;
public:
	HStringIterator( HHuginn::HString* string_ )
		: _string( string_ ),
		_index( 0 ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( void ) override {
		return ( make_pointer<HHuginn::HCharacter>( _string->value()[ _index ] ) );
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

inline HHuginn::value_t find( huginn::HThread*, HHuginn::HObject* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	if ( values_.get_size() != 1 ) {
		throw HHuginn::HHuginnRuntimeException(
			"string.find() expects exactly one parameter, got: "_ys
			.append( values_.get_size() ),
			position_
		);
	}
	if ( values_[0]->type() != HHuginn::TYPE::STRING ) {
		throw HHuginn::HHuginnRuntimeException(
			"string.find() argument must be a string, not a `"_ys.append( values_[0]->type()->name() ).append( "'." ),
			position_
		);
	}
	HHuginn::HString* s( static_cast<HHuginn::HString*>( object_ ) );
	int long pos( s->value().find( static_cast<HHuginn::HString const*>( values_[0].raw() )->value() ) );
	return ( make_pointer<HHuginn::HInteger>( pos != hcore::HString::npos ? pos : -1 ) );
	M_EPILOG
}

HHuginn::HClass _stringClass_(
	nullptr,
	HHuginn::TYPE::STRING,
	nullptr,
	/* methods */ {
		"find"
	}, {
		make_pointer<HHuginn::HClass::HMethod>( hcore::call( &string::find, _1, _2, _3, _4 ) ),
	}
);

}

}

HHuginn::HString::HString( yaal::hcore::HString const& value_ )
	: HIterable( &string::_stringClass_ )
	, _value( value_ ) {
	return;
}

yaal::hcore::HString const& HHuginn::HString::value( void ) const {
	return ( _value );
}

yaal::hcore::HString& HHuginn::HString::value( void ) {
	return ( _value );
}

HHuginn::value_t HHuginn::HString::do_clone( void ) const {
	return ( make_pointer<HString>( _value ) );
}

HHuginn::HIterable::HIterator HHuginn::HString::do_iterator( void ) {
	HIterator::iterator_implementation_t impl( new ( memory::yaal ) HStringIterator( this ) );
	return ( HIterator( yaal::move( impl ) ) );
}

}

}

