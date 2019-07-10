/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/hhuginn.hxx"
#include "hcore/safe_int.hxx"
#include "runtime.hxx"
#include "helper.hxx"
#include "thread.hxx"
#include "instruction.hxx"
#include "builtin.hxx"
#include "packagefactory.hxx"
#include "objectfactory.hxx"
#include "range.hxx"
#include "filter.hxx"
#include "mapper.hxx"
#include "enumerator.hxx"
#include "zip.hxx"
#include "slice.hxx"
#include "chain.hxx"
#include "product.hxx"
#include "tuple.hxx"
#include "list.hxx"
#include "deque.hxx"
#include "dict.hxx"
#include "order.hxx"
#include "lookup.hxx"
#include "set.hxx"
#include "string.hxx"
#include "stream.hxx"
#include "exception.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

class HAlgorithms : public huginn::HValue {
	HHuginn::class_t _iteratorClass;
	HHuginn::class_t _filterClass;
	HHuginn::class_t _mapperClass;
	HHuginn::class_t _rangeClass;
	HHuginn::class_t _enumeratorClass;
	HHuginn::class_t _zipClass;
	HHuginn::class_t _sliceClass;
	HHuginn::class_t _chainClass;
	HHuginn::class_t _productClass;
	HHuginn::class_t _exceptionClass;
public:
	HAlgorithms( huginn::HClass* class_ )
		: HValue( class_ )
		, _iteratorClass( HIterator::get_class( class_->runtime(), class_ ) )
		, _filterClass( HFilter::get_class( class_->runtime(), class_ ) )
		, _mapperClass( HMapper::get_class( class_->runtime(), class_ ) )
		, _rangeClass( HRange::get_class( class_->runtime(), class_ ) )
		, _enumeratorClass( HEnumerator::get_class( class_->runtime(), class_ ) )
		, _zipClass( HZip::get_class( class_->runtime(), class_ ) )
		, _sliceClass( HSlice::get_class( class_->runtime(), class_ ) )
		, _chainClass( HChain::get_class( class_->runtime(), class_ ) )
		, _productClass( HProduct::get_class( class_->runtime(), class_ ) )
		, _exceptionClass( class_exception( class_ ) ) {
		return;
	}
	static HHuginn::value_t iterator( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		return ( static_cast<HAlgorithms*>( object_->raw() )->do_iterator( thread_, values_, position_ ) );
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
	static HHuginn::value_t zip( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		return ( static_cast<HAlgorithms*>( object_->raw() )->do_zip( thread_, values_, position_ ) );
	}
	static HHuginn::value_t chain( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		return ( static_cast<HAlgorithms*>( object_->raw() )->do_chain( thread_, values_, position_ ) );
	}
	static HHuginn::value_t product( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		return ( static_cast<HAlgorithms*>( object_->raw() )->do_product( thread_, values_, position_ ) );
	}
	static HHuginn::value_t reduce( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		char const name[] = "Algorithms.reduce";
		verify_arg_count( name, values_, 2, 3, thread_, position_ );
		HHuginn::value_t src( verify_arg_virtual_collection( name, values_, 0, ARITY::MULTIPLE, thread_, position_ ) );
		HHuginn::value_t callable( verify_arg_callable( name, values_, 1, ARITY::MULTIPLE, thread_, position_ ) );
		HHuginn::type_id_t t( callable->type_id() );
		int argCount( static_cast<int>( values_.get_size() ) );
		HHuginn::value_t accumulator( argCount == 3 ? values_[2] : HHuginn::value_t() );
		huginn::HIterable const* iterable( static_cast<huginn::HIterable const*>( src.raw() ) );
		if ( ! accumulator && ( iterable->size( thread_, position_ ) == 0 ) ) {
			throw HHuginn::HHuginnRuntimeException( "reduce() on empty.", thread_->current_frame()->file_id(), position_ );
		}
		huginn::HIterable::iterator_t it( const_cast<huginn::HIterable*>( iterable )->iterator( thread_, position_ ) );
		if ( ! accumulator ) {
			accumulator = it->value( thread_, position_ );
			it->next( thread_, position_ );
		}
		if ( t == HHuginn::TYPE::FUNCTION_REFERENCE ) {
			HHuginn::function_t function( static_cast<huginn::HFunctionReference const*>( callable.raw() )->function() );
			while ( thread_->can_continue() && it->is_valid( thread_, position_ ) ) {
				accumulator = function( thread_, nullptr, HArguments( thread_, it->value( thread_, position_ ), accumulator ), position_ );
				it->next( thread_, position_ );
			}
		} else if ( t == HHuginn::TYPE::UNBOUND_METHOD ) {
			huginn::HClass::HUnboundMethod* unboundMethod( static_cast<huginn::HClass::HUnboundMethod*>( callable.raw() ) );
			while ( thread_->can_continue() && it->is_valid( thread_, position_ ) ) {
				accumulator = unboundMethod->call( thread_, HArguments( thread_, it->value( thread_, position_ ), accumulator ), position_ );
				it->next( thread_, position_ );
			}
		} else {
			M_ASSERT( t == HHuginn::TYPE::BOUND_METHOD );
			huginn::HClass::HBoundMethod* boundMethod( static_cast<huginn::HClass::HBoundMethod*>( callable.raw() ) );
			while ( thread_->can_continue() && it->is_valid( thread_, position_ ) ) {
				accumulator = boundMethod->call( thread_, HArguments( thread_, it->value( thread_, position_ ), accumulator ), position_ );
				it->next( thread_, position_ );
			}
		}
		return ( accumulator );
	}
	static HHuginn::value_t materialize( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		char const name[] = "Algorithms.materialize";
		verify_arg_count( name, values_, 2, 2, thread_, position_ );
		HObjectFactory& of( thread_->object_factory() );
		HHuginn::value_t src( verify_arg_virtual_collection( name, values_, 0, ARITY::MULTIPLE, thread_, position_ ) );
		HClass const* streamClass( of.stream_class() );
		HHuginn::type_id_t t( verify_arg_type( name, values_, 1, classes_t{ of.function_reference_class(), streamClass, of.none_class() }, ARITY::MULTIPLE, thread_, position_ ) );
		HHuginn::value_t v;
		huginn::HIterable const* iterable( static_cast<huginn::HIterable const*>( src.raw() ) );
		huginn::HIterable::iterator_t it( const_cast<huginn::HIterable*>( iterable )->iterator( thread_, position_ ) );
		if ( t == streamClass->type_id() ) {
			HStream& stream( *static_cast<HStream*>( values_[1].raw() ) );
			while ( thread_->can_continue() && it->is_valid( thread_, position_ ) ) {
				v = it->value( thread_, position_ );
				stream.write( thread_, v, position_ );
				it->next( thread_, position_ );
			}
			return ( values_[1] );
		}
		if ( t == HHuginn::TYPE::NONE ) {
			while ( thread_->can_continue() && it->is_valid( thread_, position_ ) ) {
				it->value( thread_, position_ );
				it->next( thread_, position_ );
			}
			return ( of.none_value() );
		}
		huginn::HFunctionReference const& fr( *static_cast<huginn::HFunctionReference const*>( values_[1].raw() ) );
		if ( fr.function().id() == bit_cast<void const*>( &builtin::tuple ) ) {
			huginn::HTuple::values_t dest;
			while ( thread_->can_continue() && it->is_valid( thread_, position_ ) ) {
				dest.push_back( it->value( thread_, position_ ) );
				it->next( thread_, position_ );
			}
			v = thread_->object_factory().create_tuple( yaal::move( dest ) );
		} else if ( fr.function().id() == bit_cast<void const*>( &builtin::list ) ) {
			v = thread_->object_factory().create_list();
			huginn::HList::values_t& dest( static_cast<huginn::HList*>( v.raw() )->value() );
			while ( thread_->can_continue() && it->is_valid( thread_, position_ ) ) {
				dest.push_back( it->value( thread_, position_ ) );
				it->next( thread_, position_ );
			}
		} else if ( fr.function().id() == bit_cast<void const*>( &builtin::deque ) ) {
			v = thread_->object_factory().create_deque();
			huginn::HDeque::values_t& dest( static_cast<huginn::HDeque*>( v.raw() )->value() );
			while ( thread_->can_continue() && it->is_valid( thread_, position_ ) ) {
				dest.push_back( it->value( thread_, position_ ) );
				it->next( thread_, position_ );
			}
		} else if ( fr.function().id() == bit_cast<void const*>( &builtin::order ) ) {
			v = thread_->object_factory().create_order();
			huginn::HOrder* order( static_cast<huginn::HOrder*>( v.raw() ) );
			huginn::HOrder::values_t& dest( order->value() );
			HAnchorGuard<huginn::HOrder> ag( *order, thread_, position_ );
			huginn::HClass const* keyType( nullptr );
			while ( thread_->can_continue() && it->is_valid( thread_, position_ ) ) {
				HHuginn::value_t elem( it->value( thread_, position_ ) );
				huginn::HClass const* newKeyType( elem->get_class() );
				if ( ! is_comparable( newKeyType ) || ( keyType && ( newKeyType != keyType ) ) ) {
					throw HHuginn::HHuginnRuntimeException(
						"Invalid key type: "_ys.append( a_type_name( newKeyType ) ).append( "." ),
						thread_->current_frame()->file_id(),
						position_
					);
				}
				keyType = newKeyType;
				dest.insert( elem );
				it->next( thread_, position_ );
			}
			if ( keyType ) {
				order->update_key_type( thread_, keyType, position_ );
			}
		} else if ( fr.function().id() == bit_cast<void const*>( &builtin::set ) ) {
			v = thread_->object_factory().create_set();
			huginn::HSet* set( static_cast<huginn::HSet*>( v.raw() ) );
			huginn::HSet::values_t& dest( set->value() );
			HAnchorGuard<huginn::HSet> ag( *set, thread_, position_ );
			while ( thread_->can_continue() && it->is_valid( thread_, position_ ) ) {
				dest.insert( it->value( thread_, position_ ) );
				it->next( thread_, position_ );
			}
		} else if ( fr.function().id() == bit_cast<void const*>( &builtin::dict ) ) {
			v = thread_->object_factory().create_dict();
			huginn::HDict* dict( static_cast<huginn::HDict*>( v.raw() ) );
			huginn::HDict::values_t& dest( dict->value() );
			HAnchorGuard<huginn::HDict> ag( *dict, thread_, position_ );
			huginn::HClass const* keyType( nullptr );
			while ( thread_->can_continue() && it->is_valid( thread_, position_ ) ) {
				HHuginn::value_t tuple( it->value( thread_, position_ ) );
				if ( tuple->type_id() != HHuginn::TYPE::TUPLE ) {
					throw HHuginn::HHuginnRuntimeException(
						"Each value materialized into a `dict` must be a `tuple` not "_ys.append( a_type_name( tuple->get_class() ) ).append( "." ),
						thread_->current_frame()->file_id(),
						position_
					);
				}
				huginn::HTuple::values_t const& data( static_cast<huginn::HTuple const*>( tuple.raw() )->value() );
				if ( data.get_size() != 2 ) {
					throw HHuginn::HHuginnRuntimeException(
						"Each `tuple` materialized into a `dict` must be a key-value pair, i.e. contain exactly two elements.",
						thread_->current_frame()->file_id(),
						position_
					);
				}
				huginn::HClass const* newKeyType( data.front()->get_class() );
				if ( ! is_comparable( newKeyType ) || ( keyType && ( newKeyType != keyType ) ) ) {
					throw HHuginn::HHuginnRuntimeException(
						"Invalid key type: "_ys.append( a_type_name( newKeyType ) ).append( "." ),
						thread_->current_frame()->file_id(),
						position_
					);
				}
				keyType = newKeyType;
				dest.insert( make_pair( data.front(), data.back() ) );
				it->next( thread_, position_ );
			}
			if ( keyType ) {
				dict->update_key_type( thread_, keyType, position_ );
			}
		} else if ( fr.function().id() == bit_cast<void const*>( &builtin::lookup ) ) {
			v = thread_->object_factory().create_lookup();
			huginn::HLookup* lookup( static_cast<huginn::HLookup*>( v.raw() ) );
			huginn::HLookup::values_t& dest( lookup->value() );
			HAnchorGuard<huginn::HLookup> ag( *lookup, thread_, position_ );
			while ( thread_->can_continue() && it->is_valid( thread_, position_ ) ) {
				HHuginn::value_t tuple( it->value( thread_, position_ ) );
				if ( tuple->type_id() != HHuginn::TYPE::TUPLE ) {
					throw HHuginn::HHuginnRuntimeException(
						"Each value materialized into a `lookup` must be a `tuple` not "_ys.append( a_type_name( tuple->get_class() ) ).append( "." ),
						thread_->current_frame()->file_id(),
						position_
					);
				}
				huginn::HTuple::values_t const& data( static_cast<huginn::HTuple const*>( tuple.raw() )->value() );
				if ( data.get_size() != 2 ) {
					throw HHuginn::HHuginnRuntimeException(
						"Each `tuple` materialized into a `lookup` must be a key-value pair, i.e. contain exactly two elements.",
						thread_->current_frame()->file_id(),
						position_
					);
				}
				dest.insert( make_pair( data.front(), data.back() ) );
				it->next( thread_, position_ );
			}
		} else if ( fr.function().id() == bit_cast<void const*>( &builtin::string ) ) {
			hcore::HString s;
			while ( thread_->can_continue() && it->is_valid( thread_, position_ ) ) {
				HHuginn::value_t c( it->value( thread_, position_ ) );
				if ( c->type_id() != HHuginn::TYPE::CHARACTER ) {
					throw HHuginn::HHuginnRuntimeException(
						"A non-`character` type value materialized into a `string`.",
						thread_->current_frame()->file_id(),
						position_
					);
				}
				s.push_back( get_character( c ) );
				it->next( thread_, position_ );
			}
			v = thread_->object_factory().create_string( yaal::move( s ) );
		} else {
			throw HHuginn::HHuginnRuntimeException(
				"Invalid materialized type: `"_ys.append( thread_->runtime().function_name( fr.function().id() ) ).append( "`." ),
				thread_->current_frame()->file_id(),
				position_
			);
		}
		return ( v );
	}
	static HHuginn::value_t range( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		return ( static_cast<HAlgorithms*>( object_->raw() )->do_range( thread_, values_, position_ ) );
	}
	static HHuginn::value_t slice( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		return ( static_cast<HAlgorithms*>( object_->raw() )->do_slice( thread_, values_, position_ ) );
	}
	static HHuginn::value_t sorted( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Algorithms.sorted";
		verify_arg_count( name, values_, 1, 2, thread_, position_ );
		HHuginn::value_t src( verify_arg_virtual_collection( name, values_, 0, ARITY::MULTIPLE, thread_, position_ ) );
		HHuginn::type_id_t t( src->type_id() );
		HHuginn::value_t v( thread_->object_factory().create_list() );
		huginn::HList::values_t& dest( static_cast<huginn::HList*>( v.raw() )->value() );
		if ( t == HHuginn::TYPE::TUPLE ) {
			huginn::HTuple::values_t const& k( static_cast<huginn::HTuple const*>( src.raw() )->value() );
			dest = k;
		} else if ( t == HHuginn::TYPE::LIST ) {
			huginn::HList::values_t const& l( static_cast<huginn::HList const*>( src.raw() )->value() );
			dest = l;
		} else if ( t == HHuginn::TYPE::DEQUE ) {
			huginn::HDeque::values_t const& d( static_cast<huginn::HDeque const*>( src.raw() )->value() );
			dest.assign( d.begin(), d.end() );
		} else if ( t == HHuginn::TYPE::ORDER ) {
			huginn::HOrder::values_t const& s( static_cast<huginn::HOrder const*>( src.raw() )->value() );
			dest.assign( s.begin(), s.end() );
		} else if ( t == HHuginn::TYPE::SET ) {
			huginn::HSet::values_t const& s( static_cast<huginn::HSet const*>( src.raw() )->value() );
			dest.assign( s.begin(), s.end() );
		} else if ( t == HHuginn::TYPE::DICT ) {
			huginn::HDict::values_t const& s( static_cast<huginn::HDict const*>( src.raw() )->value() );
			for ( huginn::HDict::values_t::value_type const& e : s ) {
				dest.push_back( e.first );
			}
		} else if ( t == HHuginn::TYPE::LOOKUP ) {
			huginn::HLookup::values_t const& s( static_cast<huginn::HLookup const*>( src.raw() )->value() );
			for ( huginn::HLookup::values_t::value_type const& e : s ) {
				dest.push_back( e.first );
			}
		} else if ( t == HHuginn::TYPE::STRING ) {
			hcore::HString const& s( get_string( src ) );
			for ( code_point_t c : s ) {
				dest.push_back( thread_->object_factory().create_character( c ) );
			}
		} else {
			huginn::HIterable const* iterable( static_cast<huginn::HIterable const*>( src.raw() ) );
			huginn::HIterable::iterator_t it( const_cast<huginn::HIterable*>( iterable )->iterator( thread_, position_ ) );
			while ( thread_->can_continue() && it->is_valid( thread_, position_ ) ) {
				dest.push_back( it->value( thread_, position_ ) );
				it->next( thread_, position_ );
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
		HValueCompareHelper less( &instruction::less );
		less.anchor( thread_, position_ );
		return ( *min_element( values_.begin(), values_.end(), cref( less ) ) );
		M_EPILOG
	}
	static HHuginn::value_t max( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Algorithms.max";
		verify_arg_count( name, values_, 1, meta::max_signed<int>::value, thread_, position_ );
		HValueCompareHelper less( &instruction::less );
		less.anchor( thread_, position_ );
		return ( *max_element( values_.begin(), values_.end(), cref( less ) ) );
		M_EPILOG
	}
	static HHuginn::value_t reversed( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Algorithms.reversed";
		verify_arg_count( name, values_, 1, 1, thread_, position_ );
		HHuginn::type_id_t t( verify_arg_materialized_collection( name, values_, 0, ARITY::UNARY, thread_, position_ ) );
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
		HHuginn::value_t src( verify_arg_virtual_collection( name, values_, 0, ARITY::MULTIPLE, thread_, position_ ) );
		HHuginn::value_t callable( verify_arg_callable( name, values_, 1, ARITY::MULTIPLE, thread_, position_ ) );
		HHuginn::type_id_t t( callable->type_id() );
		HHuginn::value_t v;
		if ( t == HHuginn::TYPE::FUNCTION_REFERENCE ) {
			v = thread_->object_factory().create<HFilter>( _filterClass.raw(), src, static_cast<huginn::HFunctionReference const*>( callable.raw() )->function(), HHuginn::value_t() );
		} else {
			v = thread_->object_factory().create<HFilter>( _filterClass.raw(), src, HHuginn::function_t(), callable );
		}
		return ( v );
		M_EPILOG
	}
	HHuginn::value_t do_map( HThread* thread_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Algorithms.map";
		verify_arg_count( name, values_, 2, 2, thread_, position_ );
		HHuginn::value_t src( verify_arg_virtual_collection( name, values_, 0, ARITY::MULTIPLE, thread_, position_ ) );
		HHuginn::value_t callable( verify_arg_callable( name, values_, 1, ARITY::MULTIPLE, thread_, position_ ) );
		HHuginn::type_id_t t( callable->type_id() );
		HHuginn::value_t v;
		if ( t == HHuginn::TYPE::FUNCTION_REFERENCE ) {
			v = thread_->object_factory().create<HMapper>( _mapperClass.raw(), src, static_cast<huginn::HFunctionReference const*>( callable.raw() )->function(), HHuginn::value_t() );
		} else {
			v = thread_->object_factory().create<HMapper>( _mapperClass.raw(), src, HHuginn::function_t(), callable );
		}
		return ( v );
		M_EPILOG
	}
	HHuginn::value_t do_enumerate( HThread* thread_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Algorithms.enumerate";
		verify_arg_count( name, values_, 1, 2, thread_, position_ );
		HHuginn::value_t src( verify_arg_virtual_collection( name, values_, 0, ARITY::MULTIPLE, thread_, position_ ) );
		huginn::HInteger::value_type start( 0 );
		if ( values_.get_size() > 1 ) {
			verify_arg_type( name, values_, 1, HHuginn::TYPE::INTEGER, ARITY::MULTIPLE, thread_, position_ );
			start = get_integer( values_[1] );
		}
		return ( thread_->object_factory().create<HEnumerator>( _enumeratorClass.raw(), src, start ) );
		M_EPILOG
	}
	HHuginn::value_t do_zip( HThread* thread_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Algorithms.zip";
		verify_arg_count( name, values_, 1, meta::max_signed<int>::value, thread_, position_ );
		int cols( static_cast<int>( values_.get_size() ) );
		HHuginn::values_t values;
		for ( int i( 0 ); i < cols; ++ i ) {
			values.push_back( verify_arg_virtual_collection( name, values_, i, ARITY::MULTIPLE, thread_, position_ ) );
		}
		return ( thread_->object_factory().create<HZip>( _zipClass.raw(), values ) );
		M_EPILOG
	}
	HHuginn::value_t do_chain( HThread* thread_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Algorithms.chain";
		verify_arg_count( name, values_, 1, meta::max_signed<int>::value, thread_, position_ );
		int cols( static_cast<int>( values_.get_size() ) );
		HHuginn::values_t values;
		for ( int i( 0 ); i < cols; ++ i ) {
			values.push_back( verify_arg_virtual_collection( name, values_, i, ARITY::MULTIPLE, thread_, position_ ) );
		}
		return ( thread_->object_factory().create<HChain>( _chainClass.raw(), values ) );
		M_EPILOG
	}
	HHuginn::value_t do_product( HThread* thread_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Algorithms.product";
		verify_arg_count( name, values_, 1, meta::max_signed<int>::value, thread_, position_ );
		int cols( static_cast<int>( values_.get_size() ) );
		HHuginn::values_t values;
		for ( int i( 0 ); i < cols; ++ i ) {
			values.push_back( verify_arg_virtual_collection( name, values_, i, ARITY::MULTIPLE, thread_, position_ ) );
		}
		HProduct::axes_t axes;
		for ( HHuginn::value_t& v : values ) {
			huginn::HIterable const* col( static_cast<huginn::HIterable const*>( v.raw() ) );
			huginn::HIterable::iterator_t it( const_cast<huginn::HIterable*>( col )->iterator( thread_, position_ ) );
			HProduct::axes_t::value_type axis;
			while ( thread_->can_continue() && it->is_valid( thread_, position_ ) ) {
				axis.push_back( it->value( thread_, position_ ) );
				it->next( thread_, position_ );
			}
			if ( ! axis.is_empty() ) {
				axes.push_back( yaal::move( axis ) );
			}
		}
		return ( thread_->object_factory().create<HProduct>( _productClass.raw(), yaal::move( axes ) ) );
		M_EPILOG
	}
	HHuginn::value_t do_iterator( HThread* thread_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Algorithms.iterator";
		verify_arg_count( name, values_, 1, 1, thread_, position_ );
		HHuginn::value_t src( verify_arg_virtual_collection( name, values_, 0, ARITY::MULTIPLE, thread_, position_ ) );
		return (
			thread_->object_factory().create<HIterator>(
				_iteratorClass.raw(),
				src,
				static_cast<huginn::HIterable*>( src.raw() )->iterator( thread_, position_ )
			)
		);
		M_EPILOG
	}
	HHuginn::value_t do_slice( HThread* thread_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Algorithms.slice";
		verify_arg_count( name, values_, 2, 4, thread_, position_ );
		HHuginn::value_t src( verify_arg_virtual_collection( name, values_, 0, ARITY::MULTIPLE, thread_, position_ ) );
		verify_arg_type( name, values_, 1, HHuginn::TYPE::INTEGER, ARITY::MULTIPLE, thread_, position_ );
		int long long from( 0 );
		int long long stop( 0 );
		int long long step( 1 );
		int s( static_cast<int>( values_.get_size() ) );
		switch ( s ) {
			case ( 2 ): {
				stop = get_integer( values_[1] );
			} break;
			case ( 4 ): {
				verify_arg_type( name, values_, 3, HHuginn::TYPE::INTEGER, ARITY::MULTIPLE, thread_, position_ );
				step = get_integer( values_[3] );
			} /* fall-through */
			case ( 3 ): {
				verify_arg_type( name, values_, 2, HHuginn::TYPE::INTEGER, ARITY::MULTIPLE, thread_, position_ );
				from = get_integer( values_[1] );
				stop = get_integer( values_[2] );
			} break;
		}
		if (
			( ( step == 0 ) && ( stop != from ) )
			|| ( from < 0 )
			|| ( step < 0 )
			|| ( stop < from )
		) {
			throw HHuginn::HHuginnRuntimeException( "Invalid slice.", thread_->current_frame()->file_id(), position_ );
		}
		return ( thread_->object_factory().create<HSlice>( _sliceClass.raw(), src, from, stop, step ) );
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
		if (
			( ( step == 0 ) && ( stop != from ) )
			|| ( ( stop > from ) && ( step < 0 ) )
			|| ( ( stop < from ) && ( step > 0 ) )
		) {
			throw HHuginn::HHuginnRuntimeException( "Invalid range.", thread_->current_frame()->file_id(), position_ );
		}
		return ( thread_->object_factory().create<HRange>( _rangeClass.raw(), from, stop, step ) );
		M_EPILOG
	}
};

namespace package_factory {

class HAlgorithmsCreator : public HPackageCreatorInterface {
protected:
	virtual HInstance do_new_instance( HRuntime* );
} algorithmsCreator;

HPackageCreatorInterface::HInstance HAlgorithmsCreator::do_new_instance( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"Algorithms",
			"The `Algorithms` package contains basic low-level algorithms.",
			HHuginn::ACCESS::PRIVATE
		)
	);
	HHuginn::field_definitions_t fd{
		{ "filter",      runtime_->create_method( &HAlgorithms::filter ),      "( *iterable*, *callable* ) - create `Filter` object that iterates over *iterable* and returns only elements for which *callable* returns `boolean` equal `true`" },
		{ "map",         runtime_->create_method( &HAlgorithms::map ),         "( *iterable*, *callable* ) - create `Mapper` object that maps elements from *iterable* transforming each of them with *callable* when iterated over" },
		{ "enumerate",   runtime_->create_method( &HAlgorithms::enumerate ),   "( *iterable*[, *start*] ) - create `Enumerator` object that counts elements from *iterable* when iterated over" },
		{ "materialize", runtime_->create_method( &HAlgorithms::materialize ), "( *iterable*, *colType* ) - copy elements from *iterable* to newly created instance of *colType*" },
		{ "reduce",      runtime_->create_method( &HAlgorithms::reduce ),      "( *iterable*, *callable* [, *init*] ) - iteratively combine all elements from *iterable* using *callable(x,y)* and starting value of *init*" },
		{ "range",       runtime_->create_method( &HAlgorithms::range ),       "( [*from*,] *until* [, *step*] ) - produce iterable sequence of `integer` values ranging from *from* up until *until* using *step* increments" },
		{ "slice",       runtime_->create_method( &HAlgorithms::slice ),       "( *iterable*, [*from*,] *until* [, *step*] ) - produce an iterable view for selecting elements from a sequence ranging from *from* up until *until* using *step* increments" },
		{ "min",         runtime_->create_method( &HAlgorithms::min ),         "( *arg1*, *arg2*[, argN...] ) - find minimum element from given set" },
		{ "max",         runtime_->create_method( &HAlgorithms::max ),         "( *arg1*, *arg2*[, argN...] ) - find maximum element from given set" },
		{ "sorted",      runtime_->create_method( &HAlgorithms::sorted ),      "( *iterable* [, *callable*] ) - return content of *iterable* as sorted `list`, using *callable* to retrieve keys for element comparison" },
		{ "zip",         runtime_->create_method( &HAlgorithms::zip ),         "( *iterable1*, *iterable2*, ... ) - create zipped iterable view of a set of iterables" },
		{ "chain",       runtime_->create_method( &HAlgorithms::chain ),       "( *iterable1*, *iterable2*, ... ) - create iterable view of a chained set of iterables" },
		{ "product",     runtime_->create_method( &HAlgorithms::product ),     "( *iterable1*, *iterable2*, ... ) - create iterable view of a Cartesian product of a set of iterables" },
		{ "iterator",    runtime_->create_method( &HAlgorithms::iterator ),    "( *iterable* ) - create manual iterator object for given iterable" },
		{ "reversed",    runtime_->create_method( &HAlgorithms::reversed ),    "( *coll* ) - create reversed iterable view of a *coll* materialized collection" }
	};
	c->redefine( nullptr, fd );
	return { c, runtime_->object_factory()->create<HAlgorithms>( c.raw() ) };
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

