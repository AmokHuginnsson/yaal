/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cmath>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "builtin.hxx"
#include "runtime.hxx"
#include "thread.hxx"
#include "helper.hxx"
#include "exception.hxx"
#include "keyword.hxx"
#include "operator.hxx"
#include "instruction.hxx"
#include "iterator.hxx"
#include "objectfactory.hxx"
#include "tools/streamtools.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace huginn {

namespace builtin {

HHuginn::value_t string( HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "string", values_, 1, 1, thread_, position_ );
	return ( instruction::string( thread_, values_[0], position_ ) );
	M_EPILOG
}

HHuginn::value_t integer( HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "integer", values_, 1, 1, thread_, position_ );
	return ( instruction::integer( thread_, values_[0], position_ ) );
	M_EPILOG
}

HHuginn::value_t real( HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "real", values_, 1, 1, thread_, position_ );
	return ( instruction::real( thread_, values_[0], position_ ) );
	M_EPILOG
}

HHuginn::value_t boolean( HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "boolean", values_, 1, 1, thread_, position_ );
	return ( instruction::boolean( thread_, values_[0], position_ ) );
	M_EPILOG
}

HHuginn::value_t character( HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "character", values_, 1, 1, thread_, position_ );
	return ( instruction::character( thread_, values_[0], position_ ) );
	M_EPILOG
}

HHuginn::value_t number( HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "number", values_, 1, 1, thread_, position_ );
	return ( instruction::number( thread_, values_[0], position_ ) );
	M_EPILOG
}

HHuginn::value_t tuple( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int ) {
	M_PROLOG
	HHuginn::values_t v( values_ );
	return ( thread_->object_factory().create_tuple( yaal::move( v ) ) );
	M_EPILOG
}

HHuginn::value_t list( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int ) {
	M_PROLOG
	HHuginn::value_t v( thread_->object_factory().create_list() );
	HHuginn::HList* l( static_cast<HHuginn::HList*>( v.raw() ) );
	for ( HHuginn::value_t const& e : values_ ) {
		l->push_back( e );
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t deque( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int ) {
	M_PROLOG
	HHuginn::value_t v( thread_->object_factory().create_deque() );
	HHuginn::HDeque* d( static_cast<HHuginn::HDeque*>( v.raw() ) );
	for ( HHuginn::value_t const& e : values_ ) {
		d->push_back( e );
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t dict( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "dict", values_, 0, 0, thread_, position_ );
	return ( thread_->object_factory().create_dict() );
	M_EPILOG
}

HHuginn::value_t order( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	HHuginn::value_t v( thread_->object_factory().create_order() );
	HHuginn::HOrder* o( static_cast<HHuginn::HOrder*>( v.raw() ) );
	for ( HHuginn::value_t const& e : values_ ) {
		o->insert( thread_, e, position_ );
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t lookup( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "lookup", values_, 0, 0, thread_, position_ );
	return ( thread_->object_factory().create_lookup() );
	M_EPILOG
}

HHuginn::value_t set( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	HHuginn::value_t v( thread_->object_factory().create_set() );
	HHuginn::HSet* s( static_cast<HHuginn::HSet*>( v.raw() ) );
	for ( HHuginn::value_t const& e : values_ ) {
		s->insert( thread_, e, position_ );
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t blob( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_signature( "blob", values_, { HHuginn::TYPE::INTEGER }, thread_, position_ );
	HHuginn::HInteger::value_type s( get_integer( values_[0] ) );
	if ( s <= 0 ) {
		throw HHuginn::HHuginnRuntimeException(
			"Invalid `blob` size requested: "_ys.append( s ),
			thread_->current_frame()->file_id(),
			position_
		);
	}
	int long rs( 0 );
	try {
		rs = safe_int::cast<int long>( s );
	} catch ( ... ) {
		throw HHuginn::HHuginnRuntimeException(
			"Requested `blob` size is too big: "_ys.append( s ),
			thread_->current_frame()->file_id(),
			position_
		);
	}
	HChunk c;
	try {
		c.realloc( rs, HChunk::STRATEGY::EXACT );
	} catch ( HException const& e ) {
		thread_->raise( thread_->object_factory().runtime_exception_class(), e.what(), position_ );
	}
	return ( thread_->object_factory().create_blob( yaal::move( c ) ) );
	M_EPILOG
}

HHuginn::value_t size( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "size", values_, 1, 1, thread_, position_ );
	HHuginn::value_t const& val( values_.front() );
	HHuginn::HValue const* v( val.raw() );
	int long long s( 0 );
	HHuginn::HIterable const* iterable( dynamic_cast<HHuginn::HIterable const*>( v ) );
	if ( iterable ) {
		s = iterable->size( thread_, position_ );
	} else if ( v->type_id() == HHuginn::TYPE::BLOB ) {
		s = static_cast<HHuginn::HBlob const*>( v )->get_size();
	} else if ( HHuginn::HObject const* o = dynamic_cast<HHuginn::HObject const*>( v ) ) {
		HHuginn::value_t res( o->call_method( thread_, val, IDENTIFIER::INTERFACE::GET_SIZE, HArguments( thread_ ), position_ ) );
		if ( res->type_id() != HHuginn::TYPE::INTEGER ) {
			throw HHuginn::HHuginnRuntimeException(
				"User supplied `get_size' method returned an invalid type "_ys
					.append( a_type_name( res->get_class() ) )
					.append( " instead of an `integer'." ),
				thread_->current_frame()->file_id(),
				position_
			);
		}
		s = get_integer( res );
	} else {
		throw HHuginn::HHuginnRuntimeException(
			"Getting size of "_ys.append( a_type_name( v->get_class() ) ).append( "s is not supported." ),
			thread_->current_frame()->file_id(),
			position_
		);
	}
	return ( thread_->object_factory().create_integer( s ) );
	M_EPILOG
}

HHuginn::value_t type( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "type", values_, 1, 1, thread_, position_ );
	HHuginn::HValue const* v( values_.front().raw() );
	return ( v->get_class()->constructor() );
	M_EPILOG
}

HHuginn::value_t copy( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "copy", values_, 1, 1, thread_, position_ );
	HHuginn::value_t& v( values_.front() );
	return ( v->clone( thread_, &v, position_ ) );
	M_EPILOG
}

HHuginn::value_t observe( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "observe", values_, 1, 1, thread_, position_ );
	HHuginn::value_t const& v( values_.front() );
	if ( v->type_id() == HHuginn::TYPE::OBSERVER ) {
		throw HHuginn::HHuginnRuntimeException(
			"Making an *observer* out of an *observer*.",
			thread_->current_frame()->file_id(),
			position_
		);
	}
	HObjectFactory& of( thread_->object_factory() );
	return ( of.create<HHuginn::HObserver>( of.observer_class(), v ) );
	M_EPILOG
}

HHuginn::value_t use( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_signature( "use", values_, { HHuginn::TYPE::OBSERVER }, thread_, position_ );
	HHuginn::HObserver const* o( static_cast<HHuginn::HObserver const*>( values_.front().raw() ) );
	HHuginn::value_t v( o->value() );
	if ( !v ) {
		v = thread_->runtime().none_value();
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t square_root( char const* name_, huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( name_, values_, 1, 1, thread_, position_ );
	HHuginn::type_id_t t( verify_arg_numeric( name_, values_, 0, ARITY::UNARY, thread_, position_ ) );
	HHuginn::value_t v( thread_->runtime().none_value() );
	if ( t == HHuginn::TYPE::NUMBER ) {
		HNumber const& val( get_number( values_[0] ) );
		if ( val >= number::N0 ) {
			v = thread_->object_factory().create_number( math::square_root( val ) );
		}
	} else {
		double long val( get_real( values_[0] ) );
		if ( val >= 0 ) {
			v = thread_->object_factory().create_real( math::square_root( val ) );
		}
	}
	if ( v == thread_->runtime().none_value() ) {
		thread_->raise( thread_->object_factory().arithmetic_exception_class(), "bad domain", position_ );
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t n_ary_summation( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "∑";
	verify_arg_count( name, values_, 1, 1, thread_, position_ );
	verify_arg_collection( name, values_, 0, ARITY::UNARY, ONTICALLY::VIRTUAL, thread_, position_ );
	HHuginn::value_t accumulator( thread_->runtime().none_value() );
	HHuginn::HIterable const* src( static_cast<HHuginn::HIterable const*>( values_[0].raw() ) );
	HHuginn::HIterable::iterator_t it( const_cast<HHuginn::HIterable*>( src )->iterator( thread_, position_ ) );
	if ( ! it->is_valid( thread_, position_ ) ) {
		throw HHuginn::HHuginnRuntimeException( "∑ on empty.", thread_->current_frame()->file_id(), position_ );
	}
	accumulator = it->value( thread_, position_ );
	HHuginn::HClass const* c( accumulator->get_class() );
	it->next( thread_, position_ );
	while ( it->is_valid( thread_, position_ ) && thread_->can_continue() ) {
		HHuginn::value_t v( it->value( thread_, position_ ) );
		if ( v->get_class() != c ) {
			throw HHuginn::HHuginnRuntimeException( "A non-uniform set under ∑.", thread_->current_frame()->file_id(), position_ );
		}
		instruction::add( thread_, accumulator, v, position_ );
		it->next( thread_, position_ );
	}
	return ( accumulator );
	M_EPILOG
}

HHuginn::value_t print( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "print", values_, 1, 1, thread_, position_ );
	HHuginn::HValue const* v( values_.front().raw() );
	yaal::hcore::HStreamInterface& out( thread_->runtime().huginn()->output_stream() );
	HHuginn::type_id_t typeId( v->type_id() );
	if ( typeId == HHuginn::TYPE::INTEGER ) {
		out << static_cast<HHuginn::HInteger const*>( v )->value();
	} else if ( typeId == HHuginn::TYPE::REAL ) {
		out << static_cast<HHuginn::HReal const*>( v )->value();
	} else if ( typeId == HHuginn::TYPE::STRING ) {
		out << static_cast<HHuginn::HString const*>( v )->value();
	} else if ( typeId == HHuginn::TYPE::NUMBER ) {
		out << static_cast<HHuginn::HNumber const*>( v )->value();
	} else if ( typeId == HHuginn::TYPE::BOOLEAN ) {
		out << static_cast<HHuginn::HBoolean const*>( v )->value();
	} else if ( typeId == HHuginn::TYPE::CHARACTER ) {
		out << static_cast<HHuginn::HCharacter const*>( v )->value();
	} else {
		throw HHuginn::HHuginnRuntimeException(
			"Printing `"_ys.append( v->get_class()->name() ).append( "'s is not supported." ),
			thread_->current_frame()->file_id(),
			position_
		);
	}
	out << flush;
	return ( thread_->runtime().none_value() );
	M_EPILOG
}

HHuginn::value_t input( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "input", values_, 0, 0, thread_, position_ );
	yaal::hcore::HString l;
	int len( static_cast<int>( thread_->runtime().huginn()->input_stream().read_until( l, HStreamInterface::eols, false ) ) );
	return ( len > 0 ? thread_->object_factory().create_string( l ) : thread_->runtime().none_value() );
	M_EPILOG
}

#undef assert
HHuginn::value_t assert( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "assert";
	verify_arg_count( name, values_, 2, 3, thread_, position_ );
	verify_arg_type( name, values_, 0, HHuginn::TYPE::BOOLEAN, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name, values_, 1, HHuginn::TYPE::STRING, ARITY::MULTIPLE, thread_, position_ );
	if ( ! get_boolean( values_[0] ) ) {
		int argc( static_cast<int>( values_.get_size() ) );
		HString message( get_string( values_[argc - 1] ) );
		if ( argc > 2 ) {
			message.append( " " ).append( get_string( values_[1] ) );
		}
		throw HHuginn::HHuginnRuntimeException( message, thread_->current_frame()->file_id(), position_ );
	}
	return ( thread_->runtime().none_value() );
	M_EPILOG
}

}

}

}

}

