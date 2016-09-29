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
#include "runtime.hxx"
#include "exception.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace huginn {

namespace boolean {
HHuginn::class_t get_class( HRuntime* );
}

namespace integer {
HHuginn::class_t get_class( HRuntime* );
}

namespace string {
HHuginn::class_t get_class( HRuntime* );
}

namespace real {
HHuginn::class_t get_class( HRuntime* );
}

namespace number {
HHuginn::class_t get_class( HRuntime* );
}

namespace character {
HHuginn::class_t get_class( HRuntime* );
}

namespace list {
HHuginn::class_t get_class( HRuntime* );
}

namespace deque {
HHuginn::class_t get_class( HRuntime* );
}

namespace dict {
HHuginn::class_t get_class( HRuntime* );
}

namespace order {
HHuginn::class_t get_class( HRuntime* );
}

namespace lookup {
HHuginn::class_t get_class( HRuntime* );
}

namespace set {
HHuginn::class_t get_class( HRuntime* );
}

HObjectFactory::HObjectFactory( HRuntime* runtime_ )
	: _runtime( runtime_ )
	, _boolean( boolean::get_class( runtime_ ) )
	, _integer( integer::get_class( runtime_ ) )
	, _string( string::get_class( runtime_ ) )
	, _real( real::get_class( runtime_ ) )
	, _number( number::get_class( runtime_ ) )
	, _character( character::get_class( runtime_ ) )
	, _list( list::get_class( runtime_ ) )
	, _deque( deque::get_class( runtime_ ) )
	, _dict( dict::get_class( runtime_ ) )
	, _order( order::get_class( runtime_ ) )
	, _lookup( lookup::get_class( runtime_ ) )
	, _set( set::get_class( runtime_ ) )
	, _exception()
	, _conversionException()
	, _arithmeticException()
	, _stringPool()
	, _stringAllocator( _stringPool )
	, _integerPool()
	, _integerAllocator( _integerPool )
	, _booleanPool()
	, _booleanAllocator( _booleanPool )
	, _realPool()
	, _realAllocator( _realPool )
	, _numberPool()
	, _numberAllocator( _numberPool )
	, _characterPool()
	, _characterAllocator( _characterPool )
	, _referencePool()
	, _referenceAllocator( _referencePool )
	, _functionReferencePool()
	, _functionReferenceAllocator( _functionReferencePool )
	, _boundMethodPool()
	, _boundMethodAllocator( _boundMethodPool )
	, _objectPool()
	, _objectAllocator( _objectPool ) {
	return;
}

void HObjectFactory::register_builtin_classes( void ) {
	M_PROLOG
	_exception = exception::get_class( _runtime );

	_runtime->huginn()->register_class( _boolean );
	_runtime->huginn()->register_class( _integer );
	_runtime->huginn()->register_class( _string );
	_runtime->huginn()->register_class( _real );
	_runtime->huginn()->register_class( _number );
	_runtime->huginn()->register_class( _character );
	_runtime->huginn()->register_class( _list );
	_runtime->huginn()->register_class( _deque );
	_runtime->huginn()->register_class( _dict );
	_runtime->huginn()->register_class( _order );
	_runtime->huginn()->register_class( _lookup );
	_runtime->huginn()->register_class( _set );

	_runtime->huginn()->register_class( _exception, true );

	_conversionException = exception::create_class( _runtime, "ConversionException", _exception.raw() );
	_arithmeticException = exception::create_class( _runtime, "ArithmeticException", _exception.raw() );
	return;
	M_EPILOG
}

}

}

}

