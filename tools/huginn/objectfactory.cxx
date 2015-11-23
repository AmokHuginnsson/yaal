/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  objectfactory.cxx - this file is integral part of `yaal' project.

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
#include "objectfactory.hxx"
#include "exception.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace huginn {

namespace string {
HHuginn::class_t get_class( void );
}

namespace integer {
HHuginn::class_t get_class( void );
}

namespace list {
HHuginn::class_t get_class( void );
}

namespace deque {
HHuginn::class_t get_class( void );
}

namespace dict {
HHuginn::class_t get_class( void );
}

namespace order {
HHuginn::class_t get_class( void );
}

namespace lookup {
HHuginn::class_t get_class( void );
}

namespace set {
HHuginn::class_t get_class( void );
}

HObjectFactory::HObjectFactory( void )
	: _string( string::get_class() )
	, _integer( integer::get_class() )
	, _list( list::get_class() )
	, _deque( deque::get_class() )
	, _dict( dict::get_class() )
	, _order( order::get_class() )
	, _lookup( lookup::get_class() )
	, _set( set::get_class() )
	, _exception()
	, _conversionException()
	, _arithmeticException() {
	return;
}

void HObjectFactory::register_exception_classes( HHuginn* huginn_ ) {
	M_PROLOG
	_exception = exception::get_class( huginn_ );
	_conversionException = exception::create_class( huginn_, "ConversionException", _exception.raw() );
	_arithmeticException = exception::create_class( huginn_, "ArithmeticException", _exception.raw() );
	huginn_->register_class( _exception );
	huginn_->register_class( _conversionException );
	huginn_->register_class( _arithmeticException );
	return;
	M_EPILOG
}

HHuginn::value_t HObjectFactory::create_string( yaal::hcore::HString const& value_ ) const {
	return ( make_pointer<HHuginn::HString>( _string.raw(), value_ ) );
}

HHuginn::value_t HObjectFactory::create_integer( HHuginn::HInteger::value_type value_ ) const {
	return ( make_pointer<HHuginn::HInteger>( _integer.raw(), value_ ) );
}

HHuginn::value_t HObjectFactory::create_list( HHuginn::values_t const& values_ ) const {
	return ( make_pointer<HHuginn::HList>( _list.raw(), values_ ) );
}

HHuginn::value_t HObjectFactory::create_deque( HHuginn::HDeque::values_t const& values_ ) const {
	return ( make_pointer<HHuginn::HDeque>( _deque.raw(), values_ ) );
}

HHuginn::value_t HObjectFactory::create_dict( void ) const {
	return ( make_pointer<HHuginn::HDict>( _dict.raw() ) );
}

HHuginn::value_t HObjectFactory::create_dict( HHuginn::HDict::values_t const& data_, HHuginn::type_t keyType_ ) const {
	return ( make_pointer<HHuginn::HDict>( _dict.raw(), data_, keyType_ ) );
}

HHuginn::value_t HObjectFactory::create_order( void ) const {
	return ( make_pointer<HHuginn::HOrder>( _order.raw() ) );
}

HHuginn::value_t HObjectFactory::create_order( HHuginn::HOrder::values_t const& data_, HHuginn::type_t keyType_ ) const {
	return ( make_pointer<HHuginn::HOrder>( _order.raw(), data_, keyType_ ) );
}

HHuginn::value_t HObjectFactory::create_lookup( void ) const {
	return ( make_pointer<HHuginn::HLookup>( _lookup.raw() ) );
}

HHuginn::value_t HObjectFactory::create_lookup( HHuginn::HLookup::values_t const& data_ ) const {
	return ( make_pointer<HHuginn::HLookup>( _lookup.raw(), data_ ) );
}

HHuginn::value_t HObjectFactory::create_set( void ) const {
	return ( make_pointer<HHuginn::HSet>( _set.raw() ) );
}

HHuginn::value_t HObjectFactory::create_set( HHuginn::HSet::values_t const& data_ ) const {
	return ( make_pointer<HHuginn::HSet>( _set.raw(), data_ ) );
}

HHuginn::HClass const* HObjectFactory::exception_class( void ) const {
	return ( _exception.raw() );
}

HHuginn::HClass const* HObjectFactory::conversion_exception_class( void ) const {
	return ( _conversionException.raw() );
}

HHuginn::HClass const* HObjectFactory::arithmetic_exception_class( void ) const {
	return ( _arithmeticException.raw() );
}

}

}

}

