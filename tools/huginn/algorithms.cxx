/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/hhuginn.hxx"
#include "hcore/safe_int.hxx"
#include "runtime.hxx"
#include "helper.hxx"
#include "thread.hxx"
#include "value_builtin.hxx"
#include "packagefactory.hxx"
#include "objectfactory.hxx"
#include "range.hxx"
#include "filter.hxx"
#include "mapper.hxx"
#include "enumerator.hxx"
#include "tuple.hxx"
#include "list.hxx"
#include "deque.hxx"
#include "dict.hxx"
#include "order.hxx"
#include "lookup.hxx"
#include "set.hxx"
#include "string.hxx"
#include "exception.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

class HAlgorithms : public HHuginn::HValue {
	HHuginn::class_t _filterClass;
	HHuginn::class_t _mapperClass;
	HHuginn::class_t _rangeClass;
	HHuginn::class_t _enumeratorClass;
	HHuginn::class_t _exceptionClass;
public:
	HAlgorithms( HHuginn::HClass* class_ )
		: HValue( class_ )
		, _filterClass( HFilter::get_class( class_->runtime() ) )
		, _mapperClass( HMapper::get_class( class_->runtime() ) )
		, _rangeClass( HRange::get_class( class_->runtime() ) )
		, _enumeratorClass( HEnumerator::get_class( class_->runtime() ) )
		, _exceptionClass( package_exception( class_ ) ) {
		return;
	}
	static HHuginn::value_t filter( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		return ( static_cast<HAlgorithms*>( object_->raw() )->do_filter( thread_, values_, position_ ) );
	}
	static HHuginn::value_t map( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		return ( static_cast<HAlgorithms*>( object_->raw() )->do_map( thread_, values_, position_ ) );
	}
	static HHuginn::value_t enumerate( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		return ( static_cast<HAlgorithms*>( object_->raw() )->do_enumerate( thread_, values_, position_ ) );
	}
	static HHuginn::value_t reduce( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		char const name[] = "Algorithms.reduce";
		verify_arg_count( name, values_, 2, 3, thread_, position_ );
		verify_arg_collection( name, values_, 0, ARITY::MULTIPLE, ONTICALLY::VIRTUAL, thread_, position_ );
		HHuginn::type_id_t t( verify_arg_type( name, values_, 1, { HHuginn::TYPE::FUNCTION_REFERENCE, HHuginn::TYPE::METHOD, HHuginn::TYPE::BOUND_METHOD }, ARITY::MULTIPLE, thread_, position_ ) );
		int argCount( static_cast<int>( values_.get_size() ) );
		HHuginn::value_t accumulator( argCount == 3 ? values_[2] : HHuginn::value_t() );
		HHuginn::HIterable const* src( static_cast<HHuginn::HIterable const*>( values_[0].raw() ) );
		if ( ! accumulator && ( src->size( thread_, position_ ) == 0 ) ) {
			throw HHuginn::HHuginnRuntimeException( "reduce() on empty.", thread_->current_frame()->file_id(), position_ );
		}
		HHuginn::HIterable::HIterator it( const_cast<HHuginn::HIterable*>( src )->iterator( thread_, position_ ) );
		if ( ! accumulator ) {
			accumulator = it.value( thread_, position_ );
			it.next( thread_, position_ );
		}
		if ( t == HHuginn::TYPE::FUNCTION_REFERENCE ) {
			HHuginn::function_t function( static_cast<HHuginn::HFunctionReference const*>( values_[1].raw() )->function() );
			while ( it.is_valid( thread_, position_ ) && thread_->can_continue() ) {
				accumulator = function( thread_, nullptr, HArguments( thread_, it.value( thread_, position_ ), accumulator ), position_ );
				it.next( thread_, position_ );
			}
		} else if ( t == HHuginn::TYPE::METHOD ) {
			HHuginn::HClass::HMethod* method( const_cast<HHuginn::HClass::HMethod*>( static_cast<HHuginn::HClass::HMethod const*>( values_[1].raw() ) ) );
			while ( it.is_valid( thread_, position_ ) && thread_->can_continue() ) {
				accumulator = method->call( thread_, HArguments( thread_, it.value( thread_, position_ ), accumulator ), position_ );
				it.next( thread_, position_ );
			}
		} else {
			HHuginn::HClass::HBoundMethod* boundMethod( const_cast<HHuginn::HClass::HBoundMethod*>( static_cast<HHuginn::HClass::HBoundMethod const*>( values_[1].raw() ) ) );
			while ( it.is_valid( thread_, position_ ) && thread_->can_continue() ) {
				accumulator = boundMethod->call( thread_, HArguments( thread_, it.value( thread_, position_ ), accumulator ), position_ );
				it.next( thread_, position_ );
			}
		}
		return ( accumulator );
	}
	static HHuginn::value_t materialize( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		char const name[] = "Algorithms.materialize";
		verify_arg_count( name, values_, 2, 2, thread_, position_ );
		verify_arg_collection( name, values_, 0, ARITY::MULTIPLE, ONTICALLY::VIRTUAL, thread_, position_ );
		verify_arg_type( name, values_, 1, HHuginn::TYPE::FUNCTION_REFERENCE, ARITY::MULTIPLE, thread_, position_ );
		HHuginn::HFunctionReference const& fr( *static_cast<HHuginn::HFunctionReference const*>( values_[1].raw() ) );
		HHuginn::value_t v;
		HHuginn::HIterable const* src( static_cast<HHuginn::HIterable const*>( values_[0].raw() ) );
		HHuginn::HIterable::HIterator it( const_cast<HHuginn::HIterable*>( src )->iterator( thread_, position_ ) );
		if ( fr.function().id() == bit_cast<void const*>( &huginn_builtin::tuple ) ) {
			HHuginn::HTuple::values_t dest;
			while ( thread_->can_continue() && it.is_valid( thread_, position_ ) ) {
				dest.push_back( it.value( thread_, position_ ) );
				it.next( thread_, position_ );
			}
			v = thread_->object_factory().create_tuple( yaal::move( dest ) );
		} else if ( fr.function().id() == bit_cast<void const*>( &huginn_builtin::list ) ) {
			v = thread_->object_factory().create_list();
			HHuginn::HList::values_t& dest( static_cast<HHuginn::HList*>( v.raw() )->value() );
			while ( thread_->can_continue() && it.is_valid( thread_, position_ ) ) {
				dest.push_back( it.value( thread_, position_ ) );
				it.next( thread_, position_ );
			}
		} else if ( fr.function().id() == bit_cast<void const*>( &huginn_builtin::deque ) ) {
			v = thread_->object_factory().create_deque();
			HHuginn::HDeque::values_t& dest( static_cast<HHuginn::HDeque*>( v.raw() )->value() );
			while ( thread_->can_continue() && it.is_valid( thread_, position_ ) ) {
				dest.push_back( it.value( thread_, position_ ) );
				it.next( thread_, position_ );
			}
		} else if ( fr.function().id() == bit_cast<void const*>( &huginn_builtin::order ) ) {
			v = thread_->object_factory().create_order();
			HHuginn::HOrder* order( static_cast<HHuginn::HOrder*>( v.raw() ) );
			HHuginn::HOrder::values_t& dest( order->value() );
			order->anchor( thread_, position_ );
			while ( thread_->can_continue() && it.is_valid( thread_, position_ ) ) {
				dest.insert( it.value( thread_, position_ ) );
				it.next( thread_, position_ );
			}
			order->detach();
		} else if ( fr.function().id() == bit_cast<void const*>( &huginn_builtin::set ) ) {
			v = thread_->object_factory().create_set();
			HHuginn::HSet* set( static_cast<HHuginn::HSet*>( v.raw() ) );
			HHuginn::HSet::values_t& dest( set->value() );
			set->anchor( thread_, position_ );
			while ( thread_->can_continue() && it.is_valid( thread_, position_ ) ) {
				dest.insert( it.value( thread_, position_ ) );
				it.next( thread_, position_ );
			}
			set->detach();
		} else {
			throw HHuginn::HHuginnRuntimeException(
				"Invalid materialized type: `"_ys.append( thread_->runtime().function_name( fr.function().id() ) ).append( "'." ),
				thread_->current_frame()->file_id(),
				position_
			);
		}
		return ( v );
	}
	static HHuginn::value_t range( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		return ( static_cast<HAlgorithms*>( object_->raw() )->do_range( thread_, values_, position_ ) );
	}
	static HHuginn::value_t sorted( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Algorithms.sorted";
		verify_arg_count( name, values_, 1, 2, thread_, position_ );
		HHuginn::type_id_t t( verify_arg_collection( name, values_, 0, ARITY::MULTIPLE, ONTICALLY::VIRTUAL, thread_, position_ ) );
		HHuginn::value_t v( thread_->object_factory().create_list() );
		HHuginn::HList::values_t& dest( static_cast<HHuginn::HList*>( v.raw() )->value() );
		if ( t == HHuginn::TYPE::TUPLE ) {
			HHuginn::HTuple::values_t const& k( static_cast<HHuginn::HTuple const*>( values_[0].raw() )->value() );
			dest = k;
		} else if ( t == HHuginn::TYPE::LIST ) {
			HHuginn::HList::values_t const& l( static_cast<HHuginn::HList const*>( values_[0].raw() )->value() );
			dest = l;
		} else if ( t == HHuginn::TYPE::DEQUE ) {
			HHuginn::HDeque::values_t const& d( static_cast<HHuginn::HDeque const*>( values_[0].raw() )->value() );
			dest.assign( d.begin(), d.end() );
		} else if ( t == HHuginn::TYPE::ORDER ) {
			HHuginn::HOrder::values_t const& s( static_cast<HHuginn::HOrder const*>( values_[0].raw() )->value() );
			dest.assign( s.begin(), s.end() );
		} else if ( t == HHuginn::TYPE::SET ) {
			HHuginn::HSet::values_t const& s( static_cast<HHuginn::HSet const*>( values_[0].raw() )->value() );
			dest.assign( s.begin(), s.end() );
		} else if ( t == HHuginn::TYPE::DICT ) {
			HHuginn::HDict::values_t const& s( static_cast<HHuginn::HDict const*>( values_[0].raw() )->value() );
			for ( HHuginn::HDict::values_t::value_type const& e : s ) {
				dest.push_back( e.first );
			}
		} else if ( t == HHuginn::TYPE::LOOKUP ) {
			HHuginn::HLookup::values_t const& s( static_cast<HHuginn::HLookup const*>( values_[0].raw() )->value() );
			for ( HHuginn::HLookup::values_t::value_type const& e : s ) {
				dest.push_back( e.first );
			}
		} else if ( t == HHuginn::TYPE::STRING ) {
			HString const& s( get_string( values_[0] ) );
			for ( code_point_t c : s ) {
				dest.push_back( thread_->object_factory().create_character( c ) );
			}
		} else {
			HHuginn::HIterable const* src( static_cast<HHuginn::HIterable const*>( values_[0].raw() ) );
			HHuginn::HIterable::HIterator it( const_cast<HHuginn::HIterable*>( src )->iterator( thread_, position_ ) );
			while ( it.is_valid( thread_, position_ ) && thread_->can_continue() ) {
				dest.push_back( it.value( thread_, position_ ) );
				it.next( thread_, position_ );
			}
		}
		if ( values_.get_size() > 1 ) {
			list::sort( thread_, &v, HArguments( thread_, values_[1] ), position_ );
		} else {
			list::sort( thread_, &v, HArguments( thread_ ), position_ );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t min( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Algorithms.min";
		verify_arg_count( name, values_, 1, meta::max_signed<int>::value, thread_, position_ );
		HHuginn::HValueCompareHelper less( &value_builtin::less );
		less.anchor( thread_, position_ );
		return ( *min_element( values_.begin(), values_.end(), cref( less ) ) );
		M_EPILOG
	}
	static HHuginn::value_t max( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Algorithms.max";
		verify_arg_count( name, values_, 1, meta::max_signed<int>::value, thread_, position_ );
		HHuginn::HValueCompareHelper less( &value_builtin::less );
		less.anchor( thread_, position_ );
		return ( *max_element( values_.begin(), values_.end(), cref( less ) ) );
		M_EPILOG
	}
	static HHuginn::value_t reversed( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Algorithms.reversed";
		verify_arg_count( name, values_, 1, 1, thread_, position_ );
		HHuginn::type_id_t t( verify_arg_collection( name, values_, 0, ARITY::UNARY, ONTICALLY::MATERIALIZED, thread_, position_ ) );
		HHuginn::value_t v;
		switch ( t.get() ) {
			case ( static_cast<int>( HHuginn::TYPE::TUPLE ) ): {
				v = tuple::reversed_view( thread_, values_[0] );
			} break;
			case ( static_cast<int>( HHuginn::TYPE::LIST ) ): {
				v = list::reversed_view( thread_, values_[0] );
			} break;
			case ( static_cast<int>( HHuginn::TYPE::DEQUE ) ): {
				v = deque::reversed_view( thread_, values_[0] );
			} break;
			case ( static_cast<int>( HHuginn::TYPE::DICT ) ): {
				v = dict::reversed_view( thread_, values_[0] );
			} break;
			case ( static_cast<int>( HHuginn::TYPE::LOOKUP ) ): {
				v = lookup::reversed_view( thread_, values_[0] );
			} break;
			case ( static_cast<int>( HHuginn::TYPE::ORDER ) ): {
				v = order::reversed_view( thread_, values_[0] );
			} break;
			case ( static_cast<int>( HHuginn::TYPE::SET ) ): {
				v = set::reversed_view( thread_, values_[0] );
			} break;
			case ( static_cast<int>( HHuginn::TYPE::STRING ) ): {
				v = string::reversed_view( thread_, values_[0] );
			} break;
			default: {
				M_ASSERT( !"Invalid code path!"[0] );
			}
		}
		return ( v );
		M_EPILOG
	}
private:
	HHuginn::value_t do_filter( HThread* thread_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Algorithms.filter";
		verify_arg_count( name, values_, 2, 2, thread_, position_ );
		verify_arg_collection( name, values_, 0, ARITY::MULTIPLE, ONTICALLY::VIRTUAL, thread_, position_ );
		HHuginn::type_id_t t( verify_arg_type( name, values_, 1, { HHuginn::TYPE::FUNCTION_REFERENCE, HHuginn::TYPE::METHOD, HHuginn::TYPE::BOUND_METHOD }, ARITY::MULTIPLE, thread_, position_ ) );
		HHuginn::value_t v;
		if ( t == HHuginn::TYPE::FUNCTION_REFERENCE ) {
			v = thread_->object_factory().create<HFilter>( _filterClass.raw(), values_[0], static_cast<HHuginn::HFunctionReference const*>( values_[1].raw() )->function(), HHuginn::value_t() );
		} else {
			v = thread_->object_factory().create<HFilter>( _filterClass.raw(), values_[0], HHuginn::function_t(), values_[1] );
		}
		return ( v );
		M_EPILOG
	}
	HHuginn::value_t do_map( HThread* thread_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Algorithms.map";
		verify_arg_count( name, values_, 2, 2, thread_, position_ );
		verify_arg_collection( name, values_, 0, ARITY::MULTIPLE, ONTICALLY::VIRTUAL, thread_, position_ );
		HHuginn::type_id_t t( verify_arg_type( name, values_, 1, { HHuginn::TYPE::FUNCTION_REFERENCE, HHuginn::TYPE::METHOD, HHuginn::TYPE::BOUND_METHOD }, ARITY::MULTIPLE, thread_, position_ ) );
		HHuginn::value_t v;
		if ( t == HHuginn::TYPE::FUNCTION_REFERENCE ) {
			v = thread_->object_factory().create<HMapper>( _mapperClass.raw(), values_[0], static_cast<HHuginn::HFunctionReference const*>( values_[1].raw() )->function(), HHuginn::value_t() );
		} else {
			v = thread_->object_factory().create<HMapper>( _mapperClass.raw(), values_[0], HHuginn::function_t(), values_[1] );
		}
		return ( v );
		M_EPILOG
	}
	HHuginn::value_t do_enumerate( HThread* thread_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Algorithms.enumerate";
		verify_arg_count( name, values_, 1, 2, thread_, position_ );
		verify_arg_collection( name, values_, 0, ARITY::MULTIPLE, ONTICALLY::VIRTUAL, thread_, position_ );
		HHuginn::HInteger::value_type start( 0 );
		if ( values_.get_size() > 1 ) {
			verify_arg_type( name, values_, 1, HHuginn::TYPE::INTEGER, ARITY::MULTIPLE, thread_, position_ );
			start = get_integer( values_[1] );
		}
		return ( thread_->object_factory().create<HEnumerator>( _enumeratorClass.raw(), values_[0], start ) );
		M_EPILOG
	}
	HHuginn::value_t do_range( HThread* thread_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Algorithms.range";
		verify_arg_count( name, values_, 1, 3, thread_, position_ );
		verify_arg_type( name, values_, 0, HHuginn::TYPE::INTEGER, ARITY::MULTIPLE, thread_, position_ );
		int long long from( 0 );
		int long long stop( 0 );
		int long long step( 1 );
		int s( static_cast<int>( values_.get_size() ) );
		switch ( s ) {
			case ( 1 ): {
				stop = get_integer( values_[0] );
			} break;
			case ( 3 ): {
				verify_arg_type( name, values_, 2, HHuginn::TYPE::INTEGER, ARITY::MULTIPLE, thread_, position_ );
				step = get_integer( values_[2] );
			} /* fall-through */
			case ( 2 ): {
				verify_arg_type( name, values_, 1, HHuginn::TYPE::INTEGER, ARITY::MULTIPLE, thread_, position_ );
				from = get_integer( values_[0] );
				stop = get_integer( values_[1] );
			} break;
		}
		if ( ( ( step == 0 ) && ( stop != from ) ) || ( ( stop > from ) && ( step < 0 ) ) || ( ( stop < from ) && ( step > 0 ) ) ) {
			throw HHuginn::HHuginnRuntimeException( "Invalid range.", thread_->current_frame()->file_id(), position_ );
		}
		return ( thread_->object_factory().create<HRange>( _rangeClass.raw(), from, stop, step ) );
		M_EPILOG
	}
};

namespace package_factory {

class HAlgorithmsCreator : public HPackageCreatorInterface {
protected:
	virtual HHuginn::value_t do_new_instance( HRuntime* );
} algorithmsCreator;

HHuginn::value_t HAlgorithmsCreator::do_new_instance( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"Algorithms",
			nullptr,
			"The `Algorithms` package contains basic low-level algorithms."
		)
	);
	HHuginn::field_definitions_t fd{
		{ "filter",      runtime_->create_method( c.raw(), &HAlgorithms::filter ),      "( *iterable*, *callable* ) - create `Filter` object that iterates over *iterable* and returns only elements for which *callable* returns `boolean` equal `true`" },
		{ "map",         runtime_->create_method( c.raw(), &HAlgorithms::map ),         "( *iterable*, *callable* ) - create `Mapper` object that maps elements from *iterable* transforming each of them with *callable* when iterated over" },
		{ "enumerate",   runtime_->create_method( c.raw(), &HAlgorithms::enumerate ),   "( *iterable*[, *start*] ) - create `Enumerator` object that counts elements from *iterable* when iterated over" },
		{ "materialize", runtime_->create_method( c.raw(), &HAlgorithms::materialize ), "( *iterable*, *colType* ) - copy elements from *iterable* to newly created instance of *colType*" },
		{ "reduce",      runtime_->create_method( c.raw(), &HAlgorithms::reduce ),      "( *iterable*, *callable* [, *init*] ) - iteratively combine all elements from *iterable* using *callable(x,y)* and starting value of *init*" },
		{ "range",       runtime_->create_method( c.raw(), &HAlgorithms::range ),       "( [*from*,] *until* [, *step*] ) - produce iterable sequence of `integer` values ranging from *from* up until *until* using *step* increments" },
		{ "min",         runtime_->create_method( c.raw(), &HAlgorithms::min ),         "( *arg1*, *arg2*[, argN...] ) - find minimum element from given set" },
		{ "max",         runtime_->create_method( c.raw(), &HAlgorithms::max ),         "( *arg1*, *arg2*[, argN...] ) - find maximum element from given set" },
		{ "sorted",      runtime_->create_method( c.raw(), &HAlgorithms::sorted ),      "( *iterable* [, *callable*] ) - return content of *iterable* as sorted `list`, using *callable* to retrieve keys for element comparison" },
		{ "reversed",    runtime_->create_method( c.raw(), &HAlgorithms::reversed ),    "( *coll* ) - create reversed iterable view of a *coll* materialized collection" }
	};
	c->redefine( nullptr, fd );
	runtime_->huginn()->register_class( c );
	return ( runtime_->object_factory()->create<HAlgorithms>( c.raw() ) );
	M_EPILOG
}

namespace {

bool registrar( void ) {
	M_PROLOG
	bool volatile failed = false;
	HPackageFactory& factory = HPackageFactory::get_instance();
	factory.register_package_creator( "Algorithms", &algorithmsCreator );
	return ( failed );
	M_EPILOG
}

bool volatile registered = registrar();

}

}

}

}

}

