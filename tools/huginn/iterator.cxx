/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "iterator.hxx"
#include "runtime.hxx"
#include "thread.hxx"
#include "helper.hxx"
#include "keyword.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace huginn {

HIterator::HIterator( huginn::HClass const* class_, HHuginn::value_t const& source_, huginn::HIterable::iterator_t const& impl_ )
	: HIterable( class_ )
	, _source( source_ )
	, _impl( impl_ ) {
}

huginn::HIterable::iterator_t HIterator::do_iterator( HThread*, int ) {
	return ( _impl );
}

int long HIterator::do_size( huginn::HThread* thread_, int position_ ) const {
	M_PROLOG
	return ( static_cast<huginn::HIterable const*>( _source.raw() )->size( thread_, position_ ) );
	M_EPILOG
}

HHuginn::value_t HIterator::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int position_ ) const {
	HHuginn::value_t s( _source->clone( thread_, const_cast<HHuginn::value_t*>( &_source ), position_ ) );
	return (
		thread_->object_factory().create<HIterator>(
			HIterable::get_class(),
			s,
			static_cast<huginn::HIterable*>( s.raw() )->iterator( thread_, position_ )
		)
	);
}

HHuginn::value_t HIterator::is_valid( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "Iterator.is_valid", values_, 0, 0, thread_, position_ );
	HIterator* ri( static_cast<HIterator*>( object_->raw() ) );
	return ( thread_->runtime().boolean_value( ri->do_is_valid( thread_, position_ ) ) );
	M_EPILOG
}

HHuginn::value_t HIterator::value( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "Iterator.value", values_, 0, 0, thread_, position_ );
	HIterator* ri( static_cast<HIterator*>( object_->raw() ) );
	return ( ri->do_value( thread_, position_ ) );
	M_EPILOG
}

HHuginn::value_t HIterator::next( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "Iterator.next", values_, 0, 0, thread_, position_ );
	HIterator* ri( static_cast<HIterator*>( object_->raw() ) );
	ri->do_next( thread_, position_ );
	return ( *object_ );
	M_EPILOG
}

HHuginn::class_t HIterator::get_class( HRuntime* runtime_, huginn::HClass const* origin_ ) {
	HHuginn::class_t c(
		runtime_->create_class(
			"Iterator",
			"The `Iterator` class provides forward iterator view over an *iterable*.",
			HHuginn::ACCESS::PRIVATE
		)
	);
	HHuginn::field_definitions_t fd{
		{ "is_valid", runtime_->create_method( &HIterator::is_valid ), "check if this `Iterator` is in valid (dereferencable) state" },
		{ "value",    runtime_->create_method( &HIterator::value ),    "get value pointed by this `Iterator`" },
		{ "next",     runtime_->create_method( &HIterator::next ),     "move this `Iterator` to next position" }
	};
	c->redefine( nullptr, fd );
	c->set_origin( origin_ );
	runtime_->huginn()->register_class( c );
	return c;
}

HHuginn::class_t HIterableAdaptor::get_class( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"IterableAdaptor",
			"The `IterableAdaptor` allows for user defined classes that implement `iterator` interface to be used in built-in algorithms and in `for` statement.",
			HHuginn::ACCESS::PRIVATE
		)
	);
	runtime_->huginn()->register_class( c, HHuginn::VISIBILITY::HIDDEN );
	return c;
	M_EPILOG
}

HIterableAdaptor::HIterableAdaptor( huginn::HClass const* class_, HHuginn::value_t const& source_ )
	: huginn::HIterable( class_ )
	, _source( source_ ) {
	M_ASSERT( dynamic_cast<HObject*>( _source.raw() ) );
	return;
}

huginn::HIterable::iterator_t HIterableAdaptor::do_iterator( HThread* thread_, int position_ ) {
	M_PROLOG
	HObject* obj( static_cast<HObject*>( _source.raw() ) );
	HHuginn::value_t itVal( obj->call_method( thread_, _source, IDENTIFIER::INTERFACE::ITERATOR, HArguments( thread_ ), position_ ) );
	HObject* itObj( dynamic_cast<HObject*>( itVal.raw() ) );
	if ( ! itObj ) {
		throw HHuginn::HHuginnRuntimeException(
			"User defined `iterable`, "_ys
				.append( a_type_name( _source->get_class() ) )
				.append( ", returned invalid iterator object - " )
				.append( a_type_name( itVal->get_class() ) )
				.append( "." ),
			thread_->file_id(),
			position_
		);
	}
	return ( hcore::make_pointer<HIterableAdaptorIterator>( thread_, _source, itVal, position_ ) );
	M_EPILOG
}

int long HIterableAdaptor::do_size( huginn::HThread* thread_, int position_ ) const {
	HObject const* obj( static_cast<HObject const*>( _source.raw() ) );
	HHuginn::value_t sizeVal( obj->call_method( thread_, _source, IDENTIFIER::INTERFACE::GET_SIZE, HArguments( thread_ ), position_ ) );
	if ( sizeVal->type_id() != HHuginn::TYPE::INTEGER ) {
		throw HHuginn::HHuginnRuntimeException(
			"User defined `iterable`, "_ys
				.append( a_type_name( _source->get_class() ) )
				.append( ", returned an invalid type " )
				.append( a_type_name( sizeVal->get_class() ) )
				.append( " instead of an `integer` from supplied `get_size` method." ),
			thread_->file_id(),
			position_
		);
	}
	int long s( 0 );
	try {
		s = safe_int::cast<int long>( get_integer( sizeVal ) );
	} catch ( hcore::HException const& e ) {
		throw HHuginn::HHuginnRuntimeException( e.what(), thread_->file_id(), position_ );
	}
	return s;
}

HIterableAdaptorIterator::HIterableAdaptorIterator( HThread* thread_, HHuginn::value_t const& source_, HHuginn::value_t const& iterator_, int position_ )
	: _source( source_ )
	, _iterator( iterator_ )
	, _isValidMethod()
	, _valueMethod()
	, _nextMethod() {
	M_ASSERT( dynamic_cast<HObject*>( _iterator.raw() ) );
	HObject* it( static_cast<HObject*>( _iterator.raw() ) );
	HHuginn::value_t isValidField( it->get_method( thread_, _iterator, IDENTIFIER::INTERFACE::IS_VALID, position_ ) );
	huginn::HClass::HBoundMethod* isValidMethod( static_cast<huginn::HClass::HBoundMethod*>( isValidField.raw() ) );
	HHuginn::value_t valueField( it->get_method( thread_, _iterator, IDENTIFIER::INTERFACE::VALUE, position_ ) );
	huginn::HClass::HBoundMethod* valueMethod( static_cast<huginn::HClass::HBoundMethod*>( valueField.raw() ) );
	HHuginn::value_t nextField( it->get_method( thread_, _iterator, IDENTIFIER::INTERFACE::NEXT, position_ ) );
	huginn::HClass::HBoundMethod* nextMethod( static_cast<huginn::HClass::HBoundMethod*>( nextField.raw() ) );
	_isValidMethod = isValidMethod->function();
	_valueMethod = valueMethod->function();
	_nextMethod = nextMethod->function();
	return;
}

bool HIterableAdaptorIterator::do_is_valid( HThread* thread_, int position_ ) {
	HHuginn::value_t isValid( _isValidMethod( thread_, &_iterator, HArguments( thread_ ), position_ ) );
	if ( isValid->type_id() != HHuginn::TYPE::BOOLEAN ) {
		throw HHuginn::HHuginnRuntimeException(
			"User defined `iterator`, "_ys
				.append( a_type_name( _iterator->get_class() ) )
				.append( ", returned an invalid type " )
				.append( a_type_name( isValid->get_class() ) )
				.append( " instead of a `boolean` from supplied `is_valid` method." ),
			thread_->file_id(),
			position_
		);
	}
	return ( get_boolean( isValid ) );
}

HHuginn::value_t HIterableAdaptorIterator::do_value( HThread* thread_, int position_ ) {
	return ( _valueMethod( thread_, &_iterator, HArguments( thread_ ), position_ ) );
}

void HIterableAdaptorIterator::do_next( HThread* thread_, int position_ ) {
	_nextMethod( thread_, &_iterator, HArguments( thread_ ), position_ );
}

HIterable::HIterable( HClass const* class_ )
	: HValue( class_ ) {
	return;
}

HIterable::iterator_t HIterable::iterator( huginn::HThread* thread_, int position_ ) {
	return ( do_iterator( thread_, position_ ) );
}

int long HIterable::size( huginn::HThread* thread_, int position_ ) const {
	return ( do_size( thread_, position_ ) );
}

int long long HIterable::extract_index( huginn::HThread* thread_, HHuginn::value_t const& index_, int position_ ) const {
	M_PROLOG
	if ( index_->type_id() != HHuginn::TYPE::INTEGER ) {
		throw HHuginn::HHuginnRuntimeException(
			hcore::to_string( _errMsgHHuginn_[ERR_CODE::IDX_NOT_INT] ).append( a_type_name( index_->get_class() ) ),
			thread_->file_id(),
			position_
		);
	}
	int long long index( get_integer( index_ ) );
	int long s( size( thread_, position_ ) );
	if ( ( index < -s ) || ( index >= s ) ) {
		throw HHuginn::HHuginnRuntimeException( "Bad index.", thread_->file_id(), position_ );
	}
	if ( index < 0 ) {
		index += s;
	}
	return index;
	M_EPILOG
}

HIterable::ORange HIterable::extract_range( huginn::HThread* thread_, HHuginn::value_t const& from_, HHuginn::value_t const& to_, HHuginn::value_t const& step_, int position_ ) const {
	if ( !! from_ && ( from_->type_id() != HHuginn::TYPE::INTEGER ) ) {
		throw HHuginn::HHuginnRuntimeException( "Range operand `from` is not an integer.", thread_->file_id(), position_ );
	}
	if ( !! to_ && ( to_->type_id() != HHuginn::TYPE::INTEGER ) ) {
		throw HHuginn::HHuginnRuntimeException( "Range operand `to` is not an integer.", thread_->file_id(), position_ );
	}
	if ( !! step_ && ( step_->type_id() != HHuginn::TYPE::INTEGER ) ) {
		throw HHuginn::HHuginnRuntimeException( "Range operand `step` is not an integer.", thread_->file_id(), position_ );
	}
	int long s( size( thread_, position_ ) );
	int long step( step_ ? static_cast<int long>( get_integer( step_ ) ) : 1 );
	if ( step == 0 ) {
		throw HHuginn::HHuginnRuntimeException( "Range step cannot be zero.", thread_->file_id(), position_ );
	}
	int long from( from_ ? static_cast<int long>( get_integer( from_ ) ) : ( step > 0 ? 0 : s ) );
	int long to( to_ ? static_cast<int long>( get_integer( to_ ) ) : ( step > 0 ? s : -1 ) );

	bool valid( false );
	do {
		if ( step > 0 ) {
			if ( ( from >= s ) || ( to == 0 ) || ( to <= -s ) ) {
				break;
			}
			if ( from < -s ) {
				from = 0;
			} else if ( from < 0 ) {
				from += s;
			}
			if ( to > s ) {
				to = s;
			} else if ( to < 0 ) {
				to += s;
			}
			if ( to <= from ) {
				break;
			}
		} else {
			if ( ( to >= ( s - 1 ) ) || ( from < -s ) ) {
				break;
			}
			if ( from >= s ) {
				from = s - 1;
			} else if ( from < 0 ) {
				from += s;
			}
			if ( to < - ( s + 1 ) ) {
				to = -1;
			} else if ( to < -1 ) {
				to += s;
			}
			if ( from <= to ) {
				break;
			}
		}
		valid = true;
	} while ( false );
	return ( ORange{ from, to, step, valid } );
}

}

}

}

