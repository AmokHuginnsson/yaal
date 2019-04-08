/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "objectfactory.hxx"
#include "runtime.hxx"
#include "keyword.hxx"
#include "exception.hxx"
#include "iterator.hxx"
#include "stream.hxx"
#include "hcore/hfile.hxx"
#include "hcore/hlog.hxx"

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
	, _taggedValuePool( this )
	, _objectPool( this )
	, _noneClass( make_pointer<huginn::HClass>( runtime_, HHuginn::TYPE::NONE, IDENTIFIER::BUILTIN::TYPE_NONE, "A type of `none` value." ) )
	, _observer( make_pointer<huginn::HClass>( runtime_, HHuginn::TYPE::OBSERVER, IDENTIFIER::BUILTIN::TYPE_OBSERVER, "The `*observer*` is a type representing a reference cycle breaking, non-owning weak \"pointer\" to a value." ) )
	, _reference( make_pointer<huginn::HClass>( runtime_, HHuginn::TYPE::REFERENCE, IDENTIFIER::BUILTIN::TYPE_REFERENCE, "Write only reference. Allows assign operator to work." ) )
	, _functionReference( make_pointer<huginn::HClass>( runtime_, HHuginn::TYPE::FUNCTION_REFERENCE, IDENTIFIER::BUILTIN::TYPE_FUNCTION_REFERENCE, "The `*function_reference*` is a Huginn's way of providing information about a value's *runtime* type." ) )
	, _objectReference( make_pointer<huginn::HClass>( runtime_, HHuginn::TYPE::OBJECT_REFERENCE, IDENTIFIER::BUILTIN::TYPE_OBJECT_REFERENCE, "The `*object_reference*` is a up-casting reference allowing to access super class methods." ) )
	, _method( make_pointer<huginn::HClass>( runtime_, HHuginn::TYPE::METHOD, IDENTIFIER::BUILTIN::TYPE_METHOD, "Raw *uncallable* method." ) )
	, _unboundMethod( make_pointer<huginn::HClass>( runtime_, HHuginn::TYPE::UNBOUND_METHOD, IDENTIFIER::BUILTIN::TYPE_UNBOUND_METHOD, "A reference to a callable but unbound method." ) )
	, _boundMethod( make_pointer<huginn::HClass>( runtime_, HHuginn::TYPE::BOUND_METHOD, IDENTIFIER::BUILTIN::TYPE_BOUND_METHOD, "A reference to a callable method with a valid runtime value bound to it." ) )
	, _variadicParameters( make_pointer<huginn::HClass>( runtime_, HHuginn::TYPE::VARIADIC_PARAMETERS, IDENTIFIER::BUILTIN::TYPE_VARIADIC_PARAMETERS, "Variadic parameters pack." ) )
	, _namedParameters( make_pointer<huginn::HClass>( runtime_, HHuginn::TYPE::NAMED_PARAMETERS, IDENTIFIER::BUILTIN::TYPE_NAMED_PARAMETERS, "Named parameters pack." ) )
	, _referencePool( this, _reference.raw() )
	, _functionReferencePool( this, _functionReference.raw() )
	, _methodPool( this, _method.raw() )
	, _unboundMethodPool( this, _unboundMethod.raw() )
	, _boundMethodPool( this, _boundMethod.raw() )
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
	, _iterableAdaptor()
	, _stream()
	, _none( create<huginn::HValue>( _noneClass.raw() ) )
	, _true( create<HBoolean>( _boolean.raw(), true ) )
	, _false( create<HBoolean>( _boolean.raw(), false ) )
	, _stringPool( this, _string.raw() )
	, _integerPool( this, _integer.raw() )
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

	_runtime->huginn()->register_class( _noneClass, HHuginn::VISIBILITY::GLOBAL );
	_runtime->huginn()->register_class( _observer, HHuginn::VISIBILITY::GLOBAL );
	_runtime->huginn()->register_class( _reference, HHuginn::VISIBILITY::GLOBAL );
	_runtime->huginn()->register_class( _functionReference, HHuginn::VISIBILITY::GLOBAL );
	_runtime->huginn()->register_class( _objectReference, HHuginn::VISIBILITY::GLOBAL );
	_runtime->huginn()->register_class( _method, HHuginn::VISIBILITY::GLOBAL );
	_runtime->huginn()->register_class( _unboundMethod, HHuginn::VISIBILITY::GLOBAL );
	_runtime->huginn()->register_class( _boundMethod, HHuginn::VISIBILITY::GLOBAL );
	_runtime->huginn()->register_class( _variadicParameters, HHuginn::VISIBILITY::GLOBAL );
	_runtime->huginn()->register_class( _namedParameters, HHuginn::VISIBILITY::GLOBAL );
	_runtime->huginn()->register_class( _boolean, HHuginn::VISIBILITY::GLOBAL );
	_runtime->huginn()->register_class( _integer, HHuginn::VISIBILITY::GLOBAL );
	_runtime->huginn()->register_class( _string, HHuginn::VISIBILITY::GLOBAL );
	_runtime->huginn()->register_class( _real, HHuginn::VISIBILITY::GLOBAL );
	_runtime->huginn()->register_class( _number, HHuginn::VISIBILITY::GLOBAL );
	_runtime->huginn()->register_class( _character, HHuginn::VISIBILITY::GLOBAL );
	_runtime->huginn()->register_class( _tuple, HHuginn::VISIBILITY::GLOBAL );
	_runtime->huginn()->register_class( _list, HHuginn::VISIBILITY::GLOBAL );
	_runtime->huginn()->register_class( _deque, HHuginn::VISIBILITY::GLOBAL );
	_runtime->huginn()->register_class( _dict, HHuginn::VISIBILITY::GLOBAL );
	_runtime->huginn()->register_class( _order, HHuginn::VISIBILITY::GLOBAL );
	_runtime->huginn()->register_class( _lookup, HHuginn::VISIBILITY::GLOBAL );
	_runtime->huginn()->register_class( _set, HHuginn::VISIBILITY::GLOBAL );
	_runtime->huginn()->register_class( _blob, HHuginn::VISIBILITY::GLOBAL );

	_runtime->huginn()->register_class( _exception, HHuginn::VISIBILITY::GLOBAL );

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

	_iterableAdaptor = HIterableAdaptor::get_class( _runtime );
	_stream = HStream::get_class( _runtime );

	return;
	M_EPILOG
}

namespace {

template <typename T>
inline int try_release( HHuginn::value_t& owner_ ) {
	T& coll( *static_cast<T*>( owner_.raw() ) );
	int freed( 0 );
	if ( ! coll.value().is_empty() ) {
		coll.clear();
		freed = 1;
	}
	return ( freed );
}

inline int try_release( huginn::HObject& object_, HHuginn::value_t& none_ ) {
	int freed( 0 );
	int fieldCount( static_cast<int>( object_.get_class()->field_definitions().get_size() ) );
	for ( int i( 0 ); i < fieldCount; ++ i ) {
		HHuginn::value_t& f( object_.field_ref( i ) );
		HHuginn::type_id_t t( f->type_id() );
		if ( ( t != HHuginn::TYPE::NONE ) && ( t != HHuginn::TYPE::METHOD ) ) {
			f = none_;
			freed = 1;
		}
	}
	return ( freed );
}

}

int HObjectFactory::break_cycles( long_lived_t const& longLived_, void** used_, int count_ ) {
	M_PROLOG
	int freed( 0 );
	for ( int i( 0 ); ( freed == 0 ) && ( i < count_ ); ++ i ) {
		HHuginn::value_t owner( static_cast<typename HHuginn::value_t::shared_t*>( used_[i] ) );
		if ( longLived_.find( owner.raw() ) != longLived_.end() ) {
			continue;
		}
		switch ( owner->type_id().get() ) {
			case ( static_cast<int>( HHuginn::TYPE::LIST ) ):   freed = try_release<huginn::HList>( owner );   break;
			case ( static_cast<int>( HHuginn::TYPE::DEQUE ) ):  freed = try_release<huginn::HDeque>( owner );  break;
			case ( static_cast<int>( HHuginn::TYPE::DICT ) ):   freed = try_release<huginn::HDict>( owner );   break;
			case ( static_cast<int>( HHuginn::TYPE::LOOKUP ) ): freed = try_release<huginn::HLookup>( owner ); break;
			case ( static_cast<int>( HHuginn::TYPE::ORDER ) ):  freed = try_release<huginn::HOrder>( owner );  break;
			case ( static_cast<int>( HHuginn::TYPE::SET ) ):    freed = try_release<huginn::HSet>( owner );    break;
			default: {
				if ( HObject* o = dynamic_cast<HObject*>( owner.raw() ) ) {
					freed = try_release( *o, _none );
				}
			}
		}
	}
	return ( freed );
	M_EPILOG
}

void HObjectFactory::cleanup( long_lived_t const& longLived_ ) {
	M_PROLOG
	typedef typename pool_type_info<huginn::HList>::pool_t   list_pool_t;
	typedef typename pool_type_info<huginn::HDeque>::pool_t  deque_pool_t;
	typedef typename pool_type_info<huginn::HDict>::pool_t   dict_pool_t;
	typedef typename pool_type_info<huginn::HLookup>::pool_t lookup_pool_t;
	typedef typename pool_type_info<huginn::HOrder>::pool_t  order_pool_t;
	typedef typename pool_type_info<huginn::HSet>::pool_t    set_pool_t;
	typedef HBoundCall<int ( void**, int )> gc_t;
	list_pool_t&   listPool( get_pool<huginn::HList>() );
	deque_pool_t&  dequePool( get_pool<huginn::HDeque>() );
	dict_pool_t&   dictPool( get_pool<huginn::HDict>() );
	lookup_pool_t& lookupPool( get_pool<huginn::HLookup>() );
	order_pool_t&  orderPool( get_pool<huginn::HOrder>() );
	set_pool_t&    setPool( get_pool<huginn::HSet>() );
	gc_t gc( hcore::call( &HObjectFactory::break_cycles, this, cref( longLived_ ), _1, _2 ) );
	int dangling( 0 );
	int freed( 0 );
	do {
		freed = 0;
		freed += listPool.run_gc( gc );
		freed += dequePool.run_gc( gc );
		freed += dictPool.run_gc( gc );
		freed += lookupPool.run_gc( gc );
		freed += orderPool.run_gc( gc );
		freed += setPool.run_gc( gc );
		dangling += freed;
	} while ( freed > 0 );
	if ( dangling > 0 ) {
		hcore::HString errMsg;
		errMsg
			.assign( "At least " )
			.append( dangling )
			.append( " reference cycle" )
			.append( dangling > 1 ? "s" : "" )
			.append( " found!" );
		hcore::log( LOG_LEVEL::ERROR ) << errMsg << endl;
		cerr << errMsg << endl;
	}
	return;
	M_EPILOG
}

}

}

}

