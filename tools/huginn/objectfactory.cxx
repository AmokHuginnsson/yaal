/* Read yaal/LICENSE.md file for copyright and licensing information. */

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
HHuginn::class_t get_class( HRuntime*, HObjectFactory* );
}

namespace integer {
HHuginn::class_t get_class( HRuntime*, HObjectFactory* );
}

namespace string {
HHuginn::class_t get_class( HRuntime*, HObjectFactory* );
}

namespace real {
HHuginn::class_t get_class( HRuntime*, HObjectFactory* );
}

namespace number {
HHuginn::class_t get_class( HRuntime*, HObjectFactory* );
}

namespace character {
HHuginn::class_t get_class( HRuntime*, HObjectFactory* );
}

namespace tuple {
HHuginn::class_t get_class( HRuntime*, HObjectFactory* );
}

namespace list {
HHuginn::class_t get_class( HRuntime*, HObjectFactory* );
}

namespace deque {
HHuginn::class_t get_class( HRuntime*, HObjectFactory* );
}

namespace dict {
HHuginn::class_t get_class( HRuntime*, HObjectFactory* );
}

namespace order {
HHuginn::class_t get_class( HRuntime*, HObjectFactory* );
}

namespace lookup {
HHuginn::class_t get_class( HRuntime*, HObjectFactory* );
}

namespace set {
HHuginn::class_t get_class( HRuntime*, HObjectFactory* );
}

namespace blob {
HHuginn::class_t get_class( HRuntime*, HObjectFactory* );
}

HObjectFactoryBase::HObjectFactoryBase( HRuntime* runtime_ )
	: _runtime( runtime_ )
	, _memoryPoolsStore()
	, _memoryPools() {
}

HObjectFactory::HObjectFactory( HRuntime* runtime_ )
	: HObjectFactoryBase( runtime_ )
	, _referencePool( this )
	, _taggedValuePool( this )
	, _functionReferencePool( this )
	, _methodPool( this )
	, _unboundMethodPool( this )
	, _boundMethodPool( this )
	, _objectPool( this )
	, _boolean( boolean::get_class( runtime_, this ) )
	, _integer( integer::get_class( runtime_, this ) )
	, _string( string::get_class( runtime_, this ) )
	, _real( real::get_class( runtime_, this ) )
	, _number( number::get_class( runtime_, this ) )
	, _character( character::get_class( runtime_, this ) )
	, _tuple( tuple::get_class( runtime_, this ) )
	, _list( list::get_class( runtime_, this ) )
	, _deque( deque::get_class( runtime_, this ) )
	, _dict( dict::get_class( runtime_, this ) )
	, _order( order::get_class( runtime_, this ) )
	, _lookup( lookup::get_class( runtime_, this ) )
	, _set( set::get_class( runtime_, this ) )
	, _blob( blob::get_class( runtime_, this ) )
	, _exception()
	, _stackFrameInfo()
	, _runtimeException()
	, _conversionException()
	, _arithmeticException()
	, _stringPool( this, _string.raw() )
	, _integerPool( this, _integer.raw() )
	, _booleanPool( this, _boolean.raw() )
	, _realPool( this, _real.raw() )
	, _numberPool( this, _number.raw() )
	, _characterPool( this, _character.raw() )
	, _tuplePool( this, _tuple.raw() )
	, _listPool( this, _list.raw() )
	, _dequePool( this, _deque.raw() )
	, _dictPool( this, _dict.raw() )
	, _lookupPool( this, _lookup.raw() )
	, _orderPool( this, _order.raw() )
	, _setPool( this, _set.raw() )
	, _blobPool( this, _blob.raw() ) {
	return;
}

void HObjectFactory::register_builtin_classes( void ) {
	M_PROLOG
	_exception = exception::get_class( _runtime );
	_stackFrameInfo = exception::HStackFrameInfo::get_class( _runtime );

	_runtime->huginn()->register_class( _boolean );
	_runtime->huginn()->register_class( _integer );
	_runtime->huginn()->register_class( _string );
	_runtime->huginn()->register_class( _real );
	_runtime->huginn()->register_class( _number );
	_runtime->huginn()->register_class( _character );
	_runtime->huginn()->register_class( _tuple );
	_runtime->huginn()->register_class( _list );
	_runtime->huginn()->register_class( _deque );
	_runtime->huginn()->register_class( _dict );
	_runtime->huginn()->register_class( _order );
	_runtime->huginn()->register_class( _lookup );
	_runtime->huginn()->register_class( _set );
	_runtime->huginn()->register_class( _blob );

	_runtime->huginn()->register_class( _exception, HHuginn::ACCESS::PUBLIC, HHuginn::VISIBILITY::GLOBAL );

	_runtimeException = exception::create_class(
		_runtime,
		"RuntimeException",
		"The `RuntimeException` is an exception class used to signal errors from Huginn's runtime engine.",
		HHuginn::VISIBILITY::GLOBAL
	);
	_conversionException = exception::create_class(
		_runtime,
		"ConversionException",
		"The `ConversionException` is an exception class used to signal invalid conversion attempts.",
		HHuginn::VISIBILITY::GLOBAL
	);
	_arithmeticException = exception::create_class(
		_runtime,
		"ArithmeticException",
		"The `ArithmeticException` is an exception class used to signal erroneous arithmetic operation.",
		HHuginn::VISIBILITY::GLOBAL
	);
	return;
	M_EPILOG
}

}

}

}

