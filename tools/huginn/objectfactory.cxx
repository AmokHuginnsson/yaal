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

namespace boolean {
HHuginn::class_t get_class( HHuginn* );
}

namespace integer {
HHuginn::class_t get_class( HHuginn* );
}

namespace string {
HHuginn::class_t get_class( HHuginn* );
}

namespace real {
HHuginn::class_t get_class( HHuginn* );
}

namespace number {
HHuginn::class_t get_class( HHuginn* );
}

namespace character {
HHuginn::class_t get_class( HHuginn* );
}

namespace list {
HHuginn::class_t get_class( HHuginn* );
}

namespace deque {
HHuginn::class_t get_class( HHuginn* );
}

namespace dict {
HHuginn::class_t get_class( HHuginn* );
}

namespace order {
HHuginn::class_t get_class( HHuginn* );
}

namespace lookup {
HHuginn::class_t get_class( HHuginn* );
}

namespace set {
HHuginn::class_t get_class( HHuginn* );
}

HObjectFactory::HObjectFactory( HHuginn* huginn_ )
	: _boolean( boolean::get_class( huginn_ ) )
	, _integer( integer::get_class( huginn_ ) )
	, _string( string::get_class( huginn_ ) )
	, _real( real::get_class( huginn_ ) )
	, _number( number::get_class( huginn_ ) )
	, _character( character::get_class( huginn_ ) )
	, _list( list::get_class( huginn_ ) )
	, _deque( deque::get_class( huginn_ ) )
	, _dict( dict::get_class( huginn_ ) )
	, _order( order::get_class( huginn_ ) )
	, _lookup( lookup::get_class( huginn_ ) )
	, _set( set::get_class( huginn_ ) )
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

}

}

}

