/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cmath>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "instruction.hxx"
#include "runtime.hxx"
#include "thread.hxx"
#include "helper.hxx"
#include "exception.hxx"
#include "keyword.hxx"
#include "operator.hxx"
#include "builtin.hxx"
#include "objectfactory.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace huginn {

namespace instruction {

HHuginn::value_t subscript(
	HThread* thread_,
	HFrame::ACCESS subscript_,
	HHuginn::value_t& base_,
	HHuginn::value_t const& index_,
	int position_
) {
	HHuginn::type_id_t baseType( base_->type_id() );
	HHuginn::value_t res;
	HObjectFactory& of( *thread_->runtime().object_factory() );
	if ( ( baseType == HHuginn::TYPE::TUPLE ) || ( baseType == HHuginn::TYPE::LIST ) || ( baseType == HHuginn::TYPE::DEQUE ) || ( baseType == HHuginn::TYPE::STRING ) ) {
		if ( index_->type_id() != HHuginn::TYPE::INTEGER ) {
			throw HHuginn::HHuginnRuntimeException(
				hcore::to_string( _errMsgHHuginn_[ERR_CODE::IDX_NOT_INT] ).append( a_type_name( index_->get_class() ) ),
				thread_->current_frame()->file_id(),
				position_
			);
		}
		HHuginn::HInteger const* i( static_cast<HHuginn::HInteger const*>( index_.raw() ) );
		int long long index( i->value() );
		int long size( static_cast<HHuginn::HIterable*>( base_.raw() )->size( thread_, position_ ) );
		if ( ( index < -size ) || ( index >= size ) ) {
			throw HHuginn::HHuginnRuntimeException( "Bad index.", thread_->current_frame()->file_id(), position_ );
		}
		if ( index < 0 ) {
			index += size;
		}
		if ( baseType == HHuginn::TYPE::TUPLE ) {
			if ( subscript_ != HFrame::ACCESS::VALUE ) {
				throw HHuginn::HHuginnRuntimeException( "`tuple` does not support item assignment.", thread_->current_frame()->file_id(), position_ );
			}
			HHuginn::HTuple* t( static_cast<HHuginn::HTuple*>( base_.raw() ) );
			res = t->get( index );
		} else if ( baseType == HHuginn::TYPE::LIST ) {
			HHuginn::HList* l( static_cast<HHuginn::HList*>( base_.raw() ) );
			res = ( subscript_ == HFrame::ACCESS::VALUE ? l->get( index ) : of.create_reference( l->get_ref( index ) ) );
		} else if ( baseType == HHuginn::TYPE::DEQUE ) {
			HHuginn::HDeque* d( static_cast<HHuginn::HDeque*>( base_.raw() ) );
			res = ( subscript_ == HFrame::ACCESS::VALUE ? d->get( index ) : of.create_reference( d->get_ref( index ) ) );
		} else {
			M_ASSERT( baseType == HHuginn::TYPE::STRING );
			if ( subscript_ != HFrame::ACCESS::VALUE ) {
				throw HHuginn::HHuginnRuntimeException( "`string` does not support item assignment.", thread_->current_frame()->file_id(), position_ );
			}
			HHuginn::HString* s( static_cast<HHuginn::HString*>( base_.raw() ) );
			res = thread_->object_factory().create_character( s->value()[static_cast<int>( index )] );
		}
	} else if ( baseType == HHuginn::TYPE::DICT ) {
		HHuginn::HDict* d( static_cast<HHuginn::HDict*>( base_.raw() ) );
		res = ( subscript_ == HFrame::ACCESS::VALUE ? d->get( thread_, index_, position_ ) : of.create_reference( d->get_ref( thread_, index_, thread_->runtime().none_value(), position_ ) ) );
	} else if ( baseType == HHuginn::TYPE::LOOKUP ) {
		HHuginn::HLookup* l( static_cast<HHuginn::HLookup*>( base_.raw() ) );
		res = ( subscript_ == HFrame::ACCESS::VALUE ? l->get( thread_, index_, position_ ) : of.create_reference( l->get_ref( thread_, index_, thread_->runtime().none_value(), position_ ) ) );
	} else {
		if ( subscript_ == HFrame::ACCESS::VALUE ) {
			if ( HHuginn::HObject const* o = dynamic_cast<HHuginn::HObject const*>( base_.raw() ) ) {
				res = o->call_method( thread_, base_, IDENTIFIER::INTERFACE::SUBSCRIPT, HArguments( thread_, index_ ), position_ );
			} else {
				HHuginn::HClass const* c( base_->get_class() );
				int idx( c->field_index( IDENTIFIER::INTERFACE::SUBSCRIPT ) );
				if ( idx >= 0 ) {
					HHuginn::HClass::HMethod const& m( *static_cast<HHuginn::HClass::HMethod const*>( c->field( idx ).raw() ) );
					res = m.function()( thread_, const_cast<HHuginn::value_t*>( &base_ ), HArguments( thread_, index_ ), position_ );
				}
			}
		}
		if ( ! res ) {
			throw HHuginn::HHuginnRuntimeException(
				"Subscript is not supported on "_ys.append( a_type_name( base_->get_class() ) ).append( "." ),
				thread_->current_frame()->file_id(),
				position_
			);
		}
	}
	return ( res );
}

HHuginn::value_t range(
	HThread* thread_,
	HHuginn::value_t& base_,
	HHuginn::value_t const& from_,
	HHuginn::value_t const& to_,
	HHuginn::value_t const& step_,
	int position_
) {
	/*
	 * from_, to_ and step_ can be passed as nullptrs and not as none_value.
	 */
	HHuginn::type_id_t baseType( base_->type_id() );
	HHuginn::value_t res;
	if ( ( baseType == HHuginn::TYPE::TUPLE ) || ( baseType == HHuginn::TYPE::LIST ) || ( baseType == HHuginn::TYPE::DEQUE ) || ( baseType == HHuginn::TYPE::STRING ) ) {
		if ( !! from_ && ( from_->type_id() != HHuginn::TYPE::INTEGER ) ) {
			throw HHuginn::HHuginnRuntimeException( "Range operand `from' is not an integer.", thread_->current_frame()->file_id(), position_ );
		}
		if ( !! to_ && ( to_->type_id() != HHuginn::TYPE::INTEGER ) ) {
			throw HHuginn::HHuginnRuntimeException( "Range operand `to' is not an integer.", thread_->current_frame()->file_id(), position_ );
		}
		if ( !! step_ && ( step_->type_id() != HHuginn::TYPE::INTEGER ) ) {
			throw HHuginn::HHuginnRuntimeException( "Range operand `step' is not an integer.", thread_->current_frame()->file_id(), position_ );
		}
		int long size( static_cast<HHuginn::HIterable*>( base_.raw() )->size( thread_, position_ ) );
		if ( baseType == HHuginn::TYPE::TUPLE ) {
			res = thread_->object_factory().create_tuple();
		} else if ( baseType == HHuginn::TYPE::LIST ) {
			res = thread_->object_factory().create_list();
		} else if ( baseType == HHuginn::TYPE::DEQUE ) {
			res = thread_->object_factory().create_deque();
		} else {
			res = thread_->object_factory().create_string();
		}
		HHuginn::HInteger const* integer( static_cast<HHuginn::HInteger const*>( step_.raw() ) );
		int long step( integer ? static_cast<int long>( integer->value() ) : 1 );
		if ( step == 0 ) {
			throw HHuginn::HHuginnRuntimeException( "Range step cannot be zero.", thread_->current_frame()->file_id(), position_ );
		}
		HHuginn::HInteger const* integerFrom = static_cast<HHuginn::HInteger const*>( from_.raw() );
		HHuginn::HInteger const* integerTo = static_cast<HHuginn::HInteger const*>( to_.raw() );
		int long from( integerFrom ? static_cast<int long>( integerFrom->value() ) : ( step > 0 ? 0 : size ) );
		int long to( integerTo ? static_cast<int long>( integerTo->value() ) : ( step > 0 ? size : -1 ) );

		do {
			if ( step > 0 ) {
				if ( ( from >= size ) || ( to == 0 ) || ( to <= -size ) ) {
					break;
				}
				if ( from < -size ) {
					from = 0;
				} else if ( from < 0 ) {
					from += size;
				}
				if ( to > size ) {
					to = size;
				} else if ( to < 0 ) {
					to += size;
				}
				if ( to <= from ) {
					break;
				}
			} else {
				if ( ( to >= ( size - 1 ) ) || ( from < -size ) ) {
					break;
				}
				if ( from >= size ) {
					from = size - 1;
				} else if ( from < 0 ) {
					from += size;
				}
				if ( to < - ( size + 1 ) ) {
					to = -1;
				} else if ( to < -1 ) {
					to += size;
				}
				if ( from <= to ) {
					break;
				}
			}
			if ( baseType == HHuginn::TYPE::TUPLE ) {
				HHuginn::HTuple* t( static_cast<HHuginn::HTuple*>( base_.raw() ) );
				HHuginn::values_t v;
				if ( step > 0 ) {
					for ( int long i( from ); i < to; i += step ) {
						v.push_back( t->get( i ) );
					}
				} else {
					for ( int long i( from ); i > to; i += step ) {
						v.push_back( t->get( i ) );
					}
				}
				res = thread_->object_factory().create_tuple( yaal::move( v ) );
			} else if ( baseType == HHuginn::TYPE::LIST ) {
				HHuginn::HList* l( static_cast<HHuginn::HList*>( base_.raw() ) );
				HHuginn::HList* r( static_cast<HHuginn::HList*>( res.raw() ) );
				if ( step > 0 ) {
					for ( int long i( from ); i < to; i += step ) {
						r->push_back( l->get( i ) );
					}
				} else {
					for ( int long i( from ); i > to; i += step ) {
						r->push_back( l->get( i ) );
					}
				}
			} else if ( baseType == HHuginn::TYPE::DEQUE ) {
				HHuginn::HDeque* l( static_cast<HHuginn::HDeque*>( base_.raw() ) );
				HHuginn::HDeque* r( static_cast<HHuginn::HDeque*>( res.raw() ) );
				if ( step > 0 ) {
					for ( int long i( from ); i < to; i += step ) {
						r->push_back( l->get( i ) );
					}
				} else {
					for ( int long i( from ); i > to; i += step ) {
						r->push_back( l->get( i ) );
					}
				}
			} else {
				M_ASSERT( baseType == HHuginn::TYPE::STRING );
				hcore::HString const& s( static_cast<HHuginn::HString*>( base_.raw() )->value() );
				hcore::HString& r( static_cast<HHuginn::HString*>( res.raw() )->value() );
				if ( step == 1 ) {
					r.assign( s, from, to - from );
				} else {
					if ( step > 0 ) {
						for ( int long i( from ); i < to; i += step ) {
							r.push_back( s[ i ] );
						}
					} else {
						for ( int long i( from ); i > to; i += step ) {
							r.push_back( s[ i ] );
						}
					}
				}
			}
		} while ( false );
	} else {
		throw HHuginn::HHuginnRuntimeException(
			"Range operator is not supported on "_ys.append( a_type_name( base_->get_class() ) ).append( "." ),
			thread_->current_frame()->file_id(),
			position_
		);
	}
	return ( res );
}

void no_such_member( HThread*, yaal::hcore::HString const&, HHuginn::identifier_id_t, int, HHuginn::HClass const* ) __attribute__(( noreturn ));
inline void no_such_member(
	HThread* thread_,
	yaal::hcore::HString const& name_,
	HHuginn::identifier_id_t memberId_,
	int position_,
	HHuginn::HClass const* context_ = nullptr
) {
	HString message;
	HRuntime& rt( thread_->runtime() );
	message
		.append( "`" )
		.append( name_ )
		.append( "' does not have `" )
		.append( rt.identifier_name( memberId_ ) )
		.append( "' member (did you mean `" );
	if ( context_ ) {
		message.append( rt.suggestion( context_, memberId_ ) );
	} else {
		message.append( rt.suggestion( memberId_ ) );
	}
	message.append( "'?)." ),
	throw HHuginn::HHuginnRuntimeException( message, thread_->current_frame()->file_id(), position_ );
}

HHuginn::value_t member( HThread* thread_, HFrame::ACCESS access_, HHuginn::value_t& v_, HHuginn::identifier_id_t memberId_, int position_ ) {
	M_PROLOG
	HRuntime& rt( thread_->runtime() );
	HHuginn::value_t m;
	HHuginn::HClass const* cls( v_->get_class() );
	if ( cls->is_complex() ) {
		int fi( v_->field_index( memberId_ ) );
		if ( fi >= 0 ) {
			if ( access_ == HFrame::ACCESS::BOUND_CALL ) {
				m = v_->field( fi );
			} else if ( access_ == HFrame::ACCESS::VALUE ) {
				m = v_->field( v_, fi );
			} else if ( ! v_.unique() ) {
				if ( HHuginn::HObject* o = dynamic_cast<HHuginn::HObject*>( v_.raw() ) ) {
					m = rt.object_factory()->create_reference( o->field_ref( fi ) );
				} else {
					throw HHuginn::HHuginnRuntimeException( "Assignment to read-only location.", thread_->current_frame()->file_id(), position_ );
				}
			} else {
				throw HHuginn::HHuginnRuntimeException( "Assignment to temporary.", thread_->current_frame()->file_id(), position_ );
			}
		} else {
			fi = cls->field_index( memberId_, HHuginn::HClass::MEMBER_TYPE::STATIC );
			if ( fi >= 0 ) {
				throw HHuginn::HHuginnRuntimeException(
					"`"_ys
						.append( rt.identifier_name( memberId_ ) )
						.append( "' member of `" )
						.append( cls->name() )
						.append( "' must be accessed from static context." ),
					thread_->current_frame()->file_id(),
					position_
				);
			}
			if ( access_ == HFrame::ACCESS::REFERENCE ) {
				no_such_member( thread_, cls->name(), memberId_, position_, cls );
			}
			HHuginn::value_t s( rt.object_factory()->create_string( rt.identifier_name( memberId_ ) ) );
			if ( HHuginn::HObject* o = dynamic_cast<HHuginn::HObject*>( v_.raw() ) ) {
				m = o->call_method( thread_, v_, IDENTIFIER::INTERFACE::MEMBER, HArguments( thread_, s ), position_ );
			} else if ( ( fi = cls->field_index( IDENTIFIER::INTERFACE::MEMBER ) ) >= 0 ) {
				HHuginn::HClass::HMethod const& method( *static_cast<HHuginn::HClass::HMethod const*>( cls->field( fi ).raw() ) );
				m = method.function()( thread_, &v_, HArguments( thread_, s ), position_ );
			} else {
				no_such_member( thread_, cls->name(), memberId_, position_, cls );
			}
		}
	} else if ( HHuginn::HObjectReference* oref = dynamic_cast<HHuginn::HObjectReference*>( v_.raw() ) ) { /* Handle `super' keyword. */
		if ( access_ == HFrame::ACCESS::REFERENCE ) {
			throw HHuginn::HHuginnRuntimeException( "Changing upcasted reference.", thread_->current_frame()->file_id(), position_ );
		}
		int fi( oref->field_index( memberId_ ) );
		if ( fi >= 0 ) {
			m = oref->field( thread_, fi, position_ );
		} else if ( ( memberId_ == IDENTIFIER::KEYWORD::CONSTRUCTOR ) && ( oref->reference_class()->type() == HHuginn::HClass::TYPE::BUILTIN ) ) {
			m = rt.object_factory()->create_bound_method(
				hcore::call(
					&HHuginn::HObject::init_base, _1, _2, _3, _4
				),
				oref->value()
			);
		} else {
			no_such_member( thread_, oref->reference_class()->name(), memberId_, position_ );
		}
	} else if ( v_->type_id() == HHuginn::TYPE::FUNCTION_REFERENCE ) {
		HHuginn::HFunctionReference* fr( static_cast<HHuginn::HFunctionReference*>( v_.raw() ) );
		HHuginn::identifier_id_t funcId( fr->identifier_id() );
		HHuginn::HClass const* c( fr->juncture() );
		if ( ! c ) {
			c = rt.get_class( funcId ).raw();
		}
		if ( ! c ) {
			throw HHuginn::HHuginnRuntimeException(
				"`"_ys.append( rt.identifier_name( funcId ) ).append( "' is not a compound object." ),
				thread_->current_frame()->file_id(),
				position_
			);
		}
		int fi( c->field_index( memberId_, HHuginn::HClass::MEMBER_TYPE::STATIC ) );
		if ( fi < 0 ) {
			no_such_member( thread_, c->name(), memberId_, position_ );
		}
		if ( access_ == HFrame::ACCESS::REFERENCE ) {
			throw HHuginn::HHuginnRuntimeException( "Assignment to read-only location.", thread_->current_frame()->file_id(), position_ );
		}
		HHuginn::value_t const& f( c->field( fi ) );
		m = f->type_id() == HHuginn::TYPE::METHOD
			? rt.object_factory()->create_unbound_method( c, static_cast<HHuginn::HClass::HMethod const*>( f.raw() )->function() )
			: f;
	} else {
		throw HHuginn::HHuginnRuntimeException( "`"_ys.append( cls->name() ).append( "' is not a compound object." ), thread_->current_frame()->file_id(), position_ );
	}
	M_ASSERT( !! m );
	return ( m );
	M_EPILOG
}

namespace {
enum class OPERATION {
	OPEN,
	CLOSED
};

void fallback_arithmetic( HThread* thread_, HHuginn::identifier_id_t methodIdentifier_, char const* oper_, HHuginn::value_t& v1_, HHuginn::value_t const& v2_, int position_ ) {
	HHuginn::value_t v;
	HHuginn::type_id_t t( v1_->type_id() );
	if ( HHuginn::HObject const* o = dynamic_cast<HHuginn::HObject const*>( v1_.raw() ) ) {
		v = o->call_method( thread_, v1_, methodIdentifier_, HArguments( thread_, v2_ ), position_ );
		if ( v->type_id() != t ) {
			throw HHuginn::HHuginnRuntimeException(
				"Arithmetic method `"_ys
					.append( thread_->runtime().identifier_name( methodIdentifier_ ) )
					.append( "' on " )
					.append( a_type_name( v1_->get_class() ) )
					.append( " returned result of incompatible type " )
					.append( a_type_name( v->get_class() ) )
					.append( "." ),
				thread_->current_frame()->file_id(),
				position_
			);
		}
	} else {
		HHuginn::HClass const* c( v1_->get_class() );
		int idx( c->field_index( methodIdentifier_ ) );
		if ( idx >= 0 ) {
			HHuginn::HClass::HMethod const& m( *static_cast<HHuginn::HClass::HMethod const*>( c->field( idx ).raw() ) );
			v = m.function()( thread_, &v1_, HArguments( thread_, v2_ ), position_ );
			M_ASSERT( ( v->type_id() == t ) || ! thread_->can_continue() );
		} else {
			throw HHuginn::HHuginnRuntimeException(
				"There is no `"_ys.append( oper_ ).append( "' operator for " ).append( a_type_name( t ) ).append( "." ),
				thread_->current_frame()->file_id(),
				position_
			);
		}
	}
	return;
}

HHuginn::value_t fallback_unary_arithmetic( HThread* thread_, HHuginn::identifier_id_t methodIdentifier_, char const* oper_, HHuginn::value_t const& v_, OPERATION operation_, int position_ ) {
	HHuginn::value_t v;
	HHuginn::type_id_t t( v_->type_id() );
	if ( HHuginn::HObject const* o = dynamic_cast<HHuginn::HObject const*>( v_.raw() ) ) {
		v = o->call_method( thread_, v_, methodIdentifier_, HArguments( thread_ ), position_ );
		if ( ( operation_ == OPERATION::CLOSED ) && ( v->type_id() != t ) ) {
			throw HHuginn::HHuginnRuntimeException(
				"Arithmetic method `"_ys
					.append( thread_->runtime().identifier_name( methodIdentifier_ ) )
					.append( "' on " )
					.append( a_type_name( v_->get_class() ) )
					.append( " returned result of incompatible type " )
					.append( a_type_name( v->get_class() ) )
					.append( "." ),
				thread_->current_frame()->file_id(),
				position_
			);
		}
	} else {
		HHuginn::HClass const* c( v_->get_class() );
		int idx( c->field_index( methodIdentifier_ ) );
		if ( idx >= 0 ) {
			HHuginn::HClass::HMethod const& m( *static_cast<HHuginn::HClass::HMethod const*>( c->field( idx ).raw() ) );
			v = m.function()( thread_, const_cast<HHuginn::value_t*>( &v_ ), HArguments( thread_ ), position_ );
			M_ASSERT( ( operation_ == OPERATION::OPEN ) || ( v->type_id() == t ) || ! thread_->can_continue() );
		} else {
			throw HHuginn::HHuginnRuntimeException(
				"There is no `"_ys.append( oper_ ).append( "' operator for " ).append( a_type_name( v_->get_class() ) ).append( "." ),
				thread_->current_frame()->file_id(),
				position_
			);
		}
	}
	return ( v );
}
}

void add( HThread* thread_, HHuginn::value_t& v1_, HHuginn::value_t const& v2_, int position_ ) {
	M_ASSERT( v1_->type_id() == v2_->type_id() );
	HHuginn::type_id_t typeId( v1_->type_id() );
	if ( typeId == HHuginn::TYPE::INTEGER ) {
		static_cast<HHuginn::HInteger*>( v1_.raw() )->value() += static_cast<HHuginn::HInteger const*>( v2_.raw() )->value();
	} else if ( typeId == HHuginn::TYPE::REAL ) {
		static_cast<HHuginn::HReal*>( v1_.raw() )->value() += static_cast<HHuginn::HReal const*>( v2_.raw() )->value();
	} else if ( typeId == HHuginn::TYPE::STRING ) {
		static_cast<HHuginn::HString*>( v1_.raw() )->value().append( static_cast<HHuginn::HString const*>( v2_.raw() )->value() );
	} else if ( typeId == HHuginn::TYPE::NUMBER ) {
		static_cast<HHuginn::HNumber*>( v1_.raw() )->value() += static_cast<HHuginn::HNumber const*>( v2_.raw() )->value();
	} else {
		fallback_arithmetic( thread_, IDENTIFIER::INTERFACE::ADD, op_to_str( OPERATOR::PLUS ), v1_, v2_, position_ );
	}
	return;
}

void sub( HThread* thread_, HHuginn::value_t& v1_, HHuginn::value_t const& v2_, int position_ ) {
	M_ASSERT( v1_->type_id() == v2_->type_id() );
	HHuginn::type_id_t typeId( v1_->type_id() );
	if ( typeId == HHuginn::TYPE::INTEGER ) {
		static_cast<HHuginn::HInteger*>( v1_.raw() )->value() -= static_cast<HHuginn::HInteger const*>( v2_.raw() )->value();
	} else if ( typeId == HHuginn::TYPE::REAL ) {
		static_cast<HHuginn::HReal*>( v1_.raw() )->value() -= static_cast<HHuginn::HReal const*>( v2_.raw() )->value();
	} else if ( typeId == HHuginn::TYPE::NUMBER ) {
		static_cast<HHuginn::HNumber*>( v1_.raw() )->value() -= static_cast<HHuginn::HNumber const*>( v2_.raw() )->value();
	} else {
		fallback_arithmetic( thread_, IDENTIFIER::INTERFACE::SUBTRACT, op_to_str( OPERATOR::MINUS ), v1_, v2_, position_ );
	}
	return;
}

void mul( HThread* thread_, HHuginn::value_t& v1_, HHuginn::value_t const& v2_, int position_ ) {
	M_ASSERT( v1_->type_id() == v2_->type_id() );
	HHuginn::type_id_t typeId( v1_->type_id() );
	if ( typeId == HHuginn::TYPE::INTEGER ) {
		static_cast<HHuginn::HInteger*>( v1_.raw() )->value() *= static_cast<HHuginn::HInteger const*>( v2_.raw() )->value();
	} else if ( typeId == HHuginn::TYPE::REAL ) {
		static_cast<HHuginn::HReal*>( v1_.raw() )->value() *= static_cast<HHuginn::HReal const*>( v2_.raw() )->value();
	} else if ( typeId == HHuginn::TYPE::NUMBER ) {
		static_cast<HHuginn::HNumber*>( v1_.raw() )->value() *= static_cast<HHuginn::HNumber const*>( v2_.raw() )->value();
	} else {
		fallback_arithmetic( thread_, IDENTIFIER::INTERFACE::MULTIPLY, op_to_str( OPERATOR::MULTIPLY ), v1_, v2_, position_ );
	}
	return;
}

void div( HThread* thread_, HHuginn::value_t& v1_, HHuginn::value_t const& v2_, int position_ ) {
	M_ASSERT( v1_->type_id() == v2_->type_id() );
	HHuginn::type_id_t typeId( v1_->type_id() );
	char const* err( "Division by zero." );
	if ( typeId == HHuginn::TYPE::INTEGER ) {
		HHuginn::HInteger::value_type& numerator( static_cast<HHuginn::HInteger*>( v1_.raw() )->value() );
		HHuginn::HInteger::value_type denominator( static_cast<HHuginn::HInteger const*>( v2_.raw() )->value() );
		if ( ( denominator != 0 ) && ( ( numerator != meta::min_signed<HHuginn::HInteger::value_type>::value ) || ( denominator != -1 ) ) ) {
			numerator /= denominator;
		} else {
			if ( denominator ) {
				err = "Division overflow.";
			}
			v1_ = thread_->runtime().none_value();
		}
	} else if ( typeId == HHuginn::TYPE::REAL ) {
		HHuginn::HReal::value_type denominator( static_cast<HHuginn::HReal const*>( v2_.raw() )->value() );
		if ( denominator != 0.0l ) {
			static_cast<HHuginn::HReal*>( v1_.raw() )->value() /= denominator;
		} else {
			v1_ = thread_->runtime().none_value();
		}
	} else if ( typeId == HHuginn::TYPE::NUMBER ) {
		HHuginn::HNumber::value_type const& denominator( static_cast<HHuginn::HNumber const*>( v2_.raw() )->value() );
		if ( denominator != number::N0 ) {
			static_cast<HHuginn::HNumber*>( v1_.raw() )->value() /= denominator;
		} else {
			v1_ = thread_->runtime().none_value();
		}
	} else {
		fallback_arithmetic( thread_, IDENTIFIER::INTERFACE::DIVIDE, op_to_str( OPERATOR::DIVIDE ), v1_, v2_, position_ );
	}
	HRuntime& rt( thread_->runtime() );
	if ( v1_ == rt.none_value() ) {
		thread_->raise( rt.object_factory()->arithmetic_exception_class(), err, position_ );
	}
}

void mod( HThread* thread_, HHuginn::value_t& v1_, HHuginn::value_t const& v2_, int position_ ) {
	M_ASSERT( v1_->type_id() == v2_->type_id() );
	HHuginn::type_id_t typeId( v1_->type_id() );
	char const* err( "Division by zero." );
	if ( typeId == HHuginn::TYPE::INTEGER ) {
		HHuginn::HInteger::value_type& numerator( static_cast<HHuginn::HInteger*>( v1_.raw() )->value() );
		HHuginn::HInteger::value_type denominator( static_cast<HHuginn::HInteger const*>( v2_.raw() )->value() );
		if ( ( denominator != 0 ) && ( ( numerator != meta::min_signed<HHuginn::HInteger::value_type>::value ) || ( denominator != -1 ) ) ) {
			numerator %= denominator;
		} else {
			if ( denominator ) {
				err = "Division overflow.";
			}
			v1_ = thread_->runtime().none_value();
		}
	} else if ( typeId == HHuginn::TYPE::REAL ) {
		HHuginn::HReal::value_type denominator( static_cast<HHuginn::HReal const*>( v2_.raw() )->value() );
		if ( denominator != 0.0l ) {
			double long& v( static_cast<HHuginn::HReal*>( v1_.raw() )->value() );
			v = fmodl( v, denominator );
		} else {
			v1_ = thread_->runtime().none_value();
		}
	} else if ( typeId == HHuginn::TYPE::NUMBER ) {
		HHuginn::HNumber::value_type const& denominator( static_cast<HHuginn::HNumber const*>( v2_.raw() )->value() );
		if ( denominator != number::N0 ) {
			static_cast<HHuginn::HNumber*>( v1_.raw() )->value() %= denominator;
		} else {
			v1_ = thread_->runtime().none_value();
		}
	} else {
		fallback_arithmetic( thread_, IDENTIFIER::INTERFACE::MODULO, op_to_str( OPERATOR::MODULO ), v1_, v2_, position_ );
	}
	HRuntime& rt( thread_->runtime() );
	if ( v1_ == rt.none_value() ) {
		thread_->raise( rt.object_factory()->arithmetic_exception_class(), err, position_ );
	}
	return;
}

void pow( HThread* thread_, HHuginn::value_t& v1_, HHuginn::value_t const& v2_, int position_ ) {
	M_ASSERT( v1_->type_id() == v2_->type_id() );
	HHuginn::value_t res;
	HHuginn::type_id_t typeId( v1_->type_id() );
	if ( typeId == HHuginn::TYPE::REAL ) {
		HHuginn::HReal::value_type& val( static_cast<HHuginn::HReal*>( v1_.raw() )->value() );
		HHuginn::HReal::value_type exp( static_cast<HHuginn::HReal const*>( v2_.raw() )->value() );
		if ( ( val != 0.L ) || ( exp != 0.L ) ) {
			val = ::powl( val, exp );
		} else {
			thread_->raise( thread_->runtime().object_factory()->arithmetic_exception_class(), "indeterminate form 0^0", position_ );
		}
	} else if ( typeId == HHuginn::TYPE::NUMBER ) {
		do {
			HNumber const& exp( static_cast<HHuginn::HNumber const*>( v2_.raw() )->value() );
			int long long expV( 0 );
			try {
				expV = exp.to_integer();
			} catch ( HNumberException const& ) {
				thread_->raise( thread_->runtime().object_factory()->arithmetic_exception_class(), "Exponent too big: "_ys.append( exp.to_string() ), position_ );
				break;
			}
			try {
				static_cast<HHuginn::HNumber*>( v1_.raw() )->value() ^= expV;
			} catch ( HNumberException const& ex ) {
				thread_->raise( thread_->runtime().object_factory()->arithmetic_exception_class(), ex.what(), position_ );
			}
		} while ( false );
	} else {
		fallback_arithmetic( thread_, IDENTIFIER::INTERFACE::POWER, op_to_str( OPERATOR::POWER ), v1_, v2_, position_ );
	}
	return;
}

HHuginn::value_t factorial( HThread* thread_, HHuginn::value_t const& v_, int position_ ) {
	HHuginn::value_t res;
	HHuginn::type_id_t typeId( v_->type_id() );
	if ( typeId == HHuginn::TYPE::NUMBER ) {
		HNumber const& n( static_cast<HHuginn::HNumber const*>( v_.raw() )->value() );
		HRuntime& rt( thread_->runtime() );
		if ( n < number::N0 ) {
			thread_->raise( rt.object_factory()->arithmetic_exception_class(), "Factorial from negative.", position_ );
		} else if ( ! n.is_integral() ) {
			thread_->raise( rt.object_factory()->arithmetic_exception_class(), "Factorial from fraction.", position_ );
		} else {
			res = thread_->object_factory().create_number( HNumber( number::factorial( n.to_integer() ) ) );
		}
	} else {
		throw HHuginn::HHuginnRuntimeException(
			"There is no `!` operator for "_ys.append( a_type_name( v_->get_class() ) ).append( "." ),
			thread_->current_frame()->file_id(), position_
		);
	}
	return ( res );
}

HHuginn::value_t abs( HThread* thread_, HHuginn::value_t const& v_, int position_ ) {
	HHuginn::value_t res;
	HHuginn::type_id_t typeId( v_->type_id() );
	if ( typeId == HHuginn::TYPE::INTEGER ) {
		int long long v( static_cast<HHuginn::HInteger const*>( v_.raw() )->value() );
		if ( v >= 0 ) {
			res = v_;
		} else if ( v != meta::min_signed<HHuginn::HInteger::value_type>::value ) {
			res = thread_->object_factory().create_integer( -v );
		} else {
			thread_->raise( thread_->runtime().object_factory()->arithmetic_exception_class(), "Integer overflow.", position_ );
		}
	} else if ( typeId == HHuginn::TYPE::REAL ) {
		double long v( static_cast<HHuginn::HReal const*>( v_.raw() )->value() );
		if ( v >= 0 ) {
			res = v_;
		} else {
			res = thread_->object_factory().create_real( -v );
		}
	} else if ( typeId == HHuginn::TYPE::NUMBER ) {
		yaal::hcore::HNumber const& v( static_cast<HHuginn::HNumber const*>( v_.raw() )->value() );
		if ( v >= 0 ) {
			res = v_;
		} else {
			res = thread_->object_factory().create_number( -v );
		}
	} else {
		res = fallback_unary_arithmetic( thread_, IDENTIFIER::INTERFACE::MODULUS, op_to_str( OPERATOR::MODULUS ), v_, OPERATION::OPEN, position_ );
	}
	return ( res );
}

HHuginn::value_t neg( HThread* thread_, HHuginn::value_t const& v_, int position_ ) {
	HHuginn::value_t res;
	HHuginn::type_id_t typeId( v_->type_id() );
	if ( typeId == HHuginn::TYPE::INTEGER ) {
		HHuginn::HInteger::value_type v( static_cast<HHuginn::HInteger const*>( v_.raw() )->value() );
		if ( v != meta::min_signed<HHuginn::HInteger::value_type>::value ) {
			res = thread_->object_factory().create_integer( -v );
		} else {
			thread_->raise( thread_->runtime().object_factory()->arithmetic_exception_class(), "Integer overflow.", position_ );
		}
	} else if ( typeId == HHuginn::TYPE::REAL ) {
		res = thread_->object_factory().create_real( -static_cast<HHuginn::HReal const*>( v_.raw() )->value() );
	} else if ( typeId == HHuginn::TYPE::NUMBER ) {
		res = thread_->object_factory().create_number( -static_cast<HHuginn::HNumber const*>( v_.raw() )->value() );
	} else {
		res = fallback_unary_arithmetic( thread_, IDENTIFIER::INTERFACE::NEGATE, op_to_str( OPERATOR::NEGATE ), v_, OPERATION::CLOSED, position_ );
	}
	return ( res );
}

int long hash( HThread* thread_, HHuginn::value_t const& v_, int position_ ) {
	int long rt( 0 );
	HHuginn::type_id_t typeId( v_->type_id() );
	if ( typeId == HHuginn::TYPE::INTEGER ) {
		rt = hcore::hash<int long long>()( static_cast<HHuginn::HInteger const*>( v_.raw() )->value() );
	} else if ( typeId == HHuginn::TYPE::REAL ) {
		rt = hcore::hash<double long>()( static_cast<HHuginn::HReal const*>( v_.raw() )->value() );
	} else if ( typeId == HHuginn::TYPE::STRING ) {
		rt = hcore::hash<hcore::HString>()( static_cast<HHuginn::HString const*>( v_.raw() )->value() );
	} else if ( typeId == HHuginn::TYPE::NUMBER ) {
		rt = hcore::hash<HNumber>()( static_cast<HHuginn::HNumber const*>( v_.raw() )->value() );
	} else if ( typeId == HHuginn::TYPE::CHARACTER ) {
		rt = hcore::hash<code_point_t>()( static_cast<HHuginn::HCharacter const*>( v_.raw() )->value() );
	} else if ( typeId == HHuginn::TYPE::BOOLEAN ) {
		rt = hcore::hash<bool>()( static_cast<HHuginn::HBoolean const*>( v_.raw() )->value() );
	} else if ( typeId == HHuginn::TYPE::FUNCTION_REFERENCE ) {
		rt = hcore::hash<int long>()( static_cast<HHuginn::HFunctionReference const*>( v_.raw() )->identifier_id().get() );
	} else if ( typeId != HHuginn::TYPE::NONE ) {
		HHuginn::value_t res;
		if ( HHuginn::HObject const* o = dynamic_cast<HHuginn::HObject const*>( v_.raw() ) ) {
			res = o->call_method( thread_, v_, IDENTIFIER::INTERFACE::HASH, HArguments( thread_ ), position_ );
			if ( res->type_id() != HHuginn::TYPE::INTEGER ) {
				throw HHuginn::HHuginnRuntimeException(
					"User supplied `hash' function returned an invalid type "_ys
						.append( a_type_name( res->get_class() ) )
						.append( " instead of an `integer'." ),
					thread_->current_frame()->file_id(),
					position_
				);
			}
		} else {
			HHuginn::HClass const* c( v_->get_class() );
			int idx( c->field_index( IDENTIFIER::INTERFACE::HASH ) );
			if ( idx >= 0 ) {
				HHuginn::HClass::HMethod const& m( *static_cast<HHuginn::HClass::HMethod const*>( c->field( idx ).raw() ) );
				res = m.function()( thread_, const_cast<HHuginn::value_t*>( &v_ ), HArguments( thread_ ), position_ );
				M_ASSERT( res->type_id() == HHuginn::TYPE::INTEGER );
			} else {
				throw HHuginn::HHuginnRuntimeException(
					"There is no `hash' operator for "_ys
						.append( a_type_name( v_->get_class() ) )
						.append( "." ),
					thread_->current_frame()->file_id(),
					position_
				);
			}
		}
		rt = hcore::hash<int long long>()( static_cast<HHuginn::HInteger const*>( res.raw() )->value() );
	}
	return ( rt );
}

namespace {
bool fallback_compare( HThread* thread_, HHuginn::identifier_id_t methodIdentifier_, char const* oper_, HHuginn::value_t const& v1_, HHuginn::value_t const& v2_, int position_ ) {
	HHuginn::HObject const* o( dynamic_cast<HHuginn::HObject const*>( v1_.raw() ) );
	HHuginn::value_t v;
	if ( o ) {
		v = o->call_method( thread_, v1_, methodIdentifier_, HArguments( thread_, v2_ ), position_ );
		if ( v->type_id() != HHuginn::TYPE::BOOLEAN ) {
			throw HHuginn::HHuginnRuntimeException(
				"Comparison method `"_ys
					.append( thread_->runtime().identifier_name( methodIdentifier_ ) )
					.append( "' returned non-boolean result of " )
					.append( a_type_name( v->get_class() ) )
					.append( " type." ),
				thread_->current_frame()->file_id(),
				position_
			);
		}
	} else {
		HHuginn::HClass const* c( v1_->get_class() );
		int idx( c->field_index( methodIdentifier_ ) );
		if ( idx >= 0 ) {
			HHuginn::HClass::HMethod const& m( *static_cast<HHuginn::HClass::HMethod const*>( c->field( idx ).raw() ) );
			v = m.function()( thread_, const_cast<HHuginn::value_t*>( &v1_ ), HArguments( thread_, v2_ ), position_ );
			M_ASSERT( ( v->type_id() == HHuginn::TYPE::BOOLEAN ) || ! thread_->can_continue() );
		} else {
			throw HHuginn::HHuginnRuntimeException(
				"There is no `"_ys.append( oper_ ).append( "' operator for " ).append( a_type_name( v1_->get_class() ) ).append( "." ),
				thread_->current_frame()->file_id(),
				position_
			);
		}
	}
	HHuginn::HBoolean* b( static_cast<HHuginn::HBoolean*>( v.raw() ) );
	return ( b->value() );
}
}

bool equals( HThread* thread_, HHuginn::value_t const& v1_, HHuginn::value_t const& v2_, int position_ ) {
	bool noneOperand( ( v1_->type_id() == HHuginn::TYPE::NONE ) || ( v2_->type_id() == HHuginn::TYPE::NONE ) );
	M_ASSERT( noneOperand || ( v1_->type_id() == v2_->type_id() ) );
	bool res( false );
	if ( ! noneOperand ) {
		HHuginn::type_id_t typeId( v1_->type_id() );
		switch ( typeId.get() ) {
			case ( static_cast<int>( HHuginn::TYPE::INTEGER ) ): {
				res = static_cast<HHuginn::HInteger const*>( v1_.raw() )->value() == static_cast<HHuginn::HInteger const*>( v2_.raw() )->value();
			} break;
			case ( static_cast<int>( HHuginn::TYPE::REAL ) ): {
				res = static_cast<HHuginn::HReal const*>( v1_.raw() )->value() == static_cast<HHuginn::HReal const*>( v2_.raw() )->value();
			} break;
			case ( static_cast<int>( HHuginn::TYPE::STRING ) ): {
				res = static_cast<HHuginn::HString const*>( v1_.raw() )->value() == static_cast<HHuginn::HString const*>( v2_.raw() )->value();
			} break;
			case ( static_cast<int>( HHuginn::TYPE::NUMBER ) ): {
				res = static_cast<HHuginn::HNumber const*>( v1_.raw() )->value() == static_cast<HHuginn::HNumber const*>( v2_.raw() )->value();
			} break;
			case ( static_cast<int>( HHuginn::TYPE::CHARACTER ) ): {
				res = static_cast<HHuginn::HCharacter const*>( v1_.raw() )->value() == static_cast<HHuginn::HCharacter const*>( v2_.raw() )->value();
			} break;
			case ( static_cast<int>( HHuginn::TYPE::BOOLEAN ) ): {
				res = static_cast<HHuginn::HBoolean const*>( v1_.raw() )->value() == static_cast<HHuginn::HBoolean const*>( v2_.raw() )->value();
			} break;
			case ( static_cast<int>( HHuginn::TYPE::FUNCTION_REFERENCE ) ): {
				res = static_cast<HHuginn::HFunctionReference const*>( v1_.raw() )->identifier_id() == static_cast<HHuginn::HFunctionReference const*>( v2_.raw() )->identifier_id();
			} break;
			default: {
				HHuginn::HEnumeral const* enumeral1( dynamic_cast<HHuginn::HEnumeral const*>( v1_.raw() ) );
				if ( enumeral1 ) {
					res = enumeral1->value() == static_cast<HHuginn::HEnumeral const*>( v2_.raw() )->value();
				} else {
					res = fallback_compare( thread_, IDENTIFIER::INTERFACE::EQUALS, op_to_str( OPERATOR::EQUALS ), v1_, v2_, position_ );
				}
			}
		}
	} else {
		res = v1_->type_id() == v2_->type_id();
	}
	return ( res );
}

bool less( HThread* thread_, HHuginn::value_t const& v1_, HHuginn::value_t const& v2_, int position_ ) {
	M_ASSERT( v1_->type_id() == v2_->type_id() );
	bool res( false );
	HHuginn::type_id_t typeId( v1_->type_id() );
	if ( typeId == HHuginn::TYPE::INTEGER ) {
		res = static_cast<HHuginn::HInteger const*>( v1_.raw() )->value() < static_cast<HHuginn::HInteger const*>( v2_.raw() )->value();
	} else if ( typeId == HHuginn::TYPE::REAL ) {
		res = static_cast<HHuginn::HReal const*>( v1_.raw() )->value() < static_cast<HHuginn::HReal const*>( v2_.raw() )->value();
	} else if ( typeId == HHuginn::TYPE::STRING ) {
		res = static_cast<HHuginn::HString const*>( v1_.raw() )->value() < static_cast<HHuginn::HString const*>( v2_.raw() )->value();
	} else if ( typeId == HHuginn::TYPE::NUMBER ) {
		res = static_cast<HHuginn::HNumber const*>( v1_.raw() )->value() < static_cast<HHuginn::HNumber const*>( v2_.raw() )->value();
	} else if ( typeId == HHuginn::TYPE::CHARACTER ) {
		res = static_cast<HHuginn::HCharacter const*>( v1_.raw() )->value() < static_cast<HHuginn::HCharacter const*>( v2_.raw() )->value();
	} else if ( typeId == HHuginn::TYPE::FUNCTION_REFERENCE ) {
		res = static_cast<HHuginn::HFunctionReference const*>( v1_.raw() )->identifier_id() < static_cast<HHuginn::HFunctionReference const*>( v2_.raw() )->identifier_id();
	} else {
		res = fallback_compare( thread_, IDENTIFIER::INTERFACE::LESS, op_to_str( OPERATOR::LESS ), v1_, v2_, position_ );
	}
	return ( res );
}

bool checked_less( HThread* thread_, HHuginn::value_t const& v1_, HHuginn::value_t const& v2_, int position_ ) {
	HHuginn::HClass const* c1( v1_->get_class() );
	HHuginn::HClass const* c2( v2_->get_class() );
	if ( c1 != c2 ) {
		operands_type_mismatch( op_to_str( OPERATOR::LESS ), c1, c2, thread_->current_frame()->file_id(), position_ );
	}
	return ( less( thread_, v1_, v2_, position_ ) );
}

bool greater( HThread* thread_, HHuginn::value_t const& v1_, HHuginn::value_t const& v2_, int position_ ) {
	M_ASSERT( v1_->type_id() == v2_->type_id() );
	bool res( false );
	HHuginn::type_id_t typeId( v1_->type_id() );
	if ( typeId == HHuginn::TYPE::INTEGER ) {
		res = static_cast<HHuginn::HInteger const*>( v1_.raw() )->value() > static_cast<HHuginn::HInteger const*>( v2_.raw() )->value();
	} else if ( typeId == HHuginn::TYPE::REAL ) {
		res = static_cast<HHuginn::HReal const*>( v1_.raw() )->value() > static_cast<HHuginn::HReal const*>( v2_.raw() )->value();
	} else if ( typeId == HHuginn::TYPE::STRING ) {
		res = static_cast<HHuginn::HString const*>( v1_.raw() )->value() > static_cast<HHuginn::HString const*>( v2_.raw() )->value();
	} else if ( typeId == HHuginn::TYPE::NUMBER ) {
		res = static_cast<HHuginn::HNumber const*>( v1_.raw() )->value() > static_cast<HHuginn::HNumber const*>( v2_.raw() )->value();
	} else if ( typeId == HHuginn::TYPE::CHARACTER ) {
		res = static_cast<HHuginn::HCharacter const*>( v1_.raw() )->value() > static_cast<HHuginn::HCharacter const*>( v2_.raw() )->value();
	} else {
		res = fallback_compare( thread_, IDENTIFIER::INTERFACE::GREATER, op_to_str( OPERATOR::GREATER ), v1_, v2_, position_ );
	}
	return ( res );
}

bool less_or_equal( HThread* thread_, HHuginn::value_t const& v1_, HHuginn::value_t const& v2_, int position_ ) {
	M_ASSERT( v1_->type_id() == v2_->type_id() );
	bool res( false );
	HHuginn::type_id_t typeId( v1_->type_id() );
	if ( typeId == HHuginn::TYPE::INTEGER ) {
		res = static_cast<HHuginn::HInteger const*>( v1_.raw() )->value() <= static_cast<HHuginn::HInteger const*>( v2_.raw() )->value();
	} else if ( typeId == HHuginn::TYPE::REAL ) {
		res = static_cast<HHuginn::HReal const*>( v1_.raw() )->value() <= static_cast<HHuginn::HReal const*>( v2_.raw() )->value();
	} else if ( typeId == HHuginn::TYPE::STRING ) {
		res = static_cast<HHuginn::HString const*>( v1_.raw() )->value() <= static_cast<HHuginn::HString const*>( v2_.raw() )->value();
	} else if ( typeId == HHuginn::TYPE::NUMBER ) {
		res = static_cast<HHuginn::HNumber const*>( v1_.raw() )->value() <= static_cast<HHuginn::HNumber const*>( v2_.raw() )->value();
	} else if ( typeId == HHuginn::TYPE::CHARACTER ) {
		res = static_cast<HHuginn::HCharacter const*>( v1_.raw() )->value() <= static_cast<HHuginn::HCharacter const*>( v2_.raw() )->value();
	} else {
		res = fallback_compare( thread_, IDENTIFIER::INTERFACE::LESS_OR_EQUAL, op_to_str( OPERATOR::LESS_OR_EQUAL ), v1_, v2_, position_ );
	}
	return ( res );
}

bool greater_or_equal( HThread* thread_, HHuginn::value_t const& v1_, HHuginn::value_t const& v2_, int position_ ) {
	M_ASSERT( v1_->type_id() == v2_->type_id() );
	bool res( false );
	HHuginn::type_id_t typeId( v1_->type_id() );
	if ( typeId == HHuginn::TYPE::INTEGER ) {
		res = static_cast<HHuginn::HInteger const*>( v1_.raw() )->value() >= static_cast<HHuginn::HInteger const*>( v2_.raw() )->value();
	} else if ( typeId == HHuginn::TYPE::REAL ) {
		res = static_cast<HHuginn::HReal const*>( v1_.raw() )->value() >= static_cast<HHuginn::HReal const*>( v2_.raw() )->value();
	} else if ( typeId == HHuginn::TYPE::STRING ) {
		res = static_cast<HHuginn::HString const*>( v1_.raw() )->value() >= static_cast<HHuginn::HString const*>( v2_.raw() )->value();
	} else if ( typeId == HHuginn::TYPE::NUMBER ) {
		res = static_cast<HHuginn::HNumber const*>( v1_.raw() )->value() >= static_cast<HHuginn::HNumber const*>( v2_.raw() )->value();
	} else if ( typeId == HHuginn::TYPE::CHARACTER ) {
		res = static_cast<HHuginn::HCharacter const*>( v1_.raw() )->value() >= static_cast<HHuginn::HCharacter const*>( v2_.raw() )->value();
	} else {
		res = fallback_compare( thread_, IDENTIFIER::INTERFACE::GREATER_OR_EQUAL, op_to_str( OPERATOR::GREATER_OR_EQUAL ), v1_, v2_, position_ );
	}
	return ( res );
}

bool is_element_of( HThread* thread_, OPERATOR operator_, HHuginn::value_t const& v1_, HHuginn::value_t const& v2_, int position_ ) {
	M_ASSERT( is_collection_like( v2_->get_class() ) || ( v2_->type_id() > type_id( HHuginn::TYPE::UNKNOWN ) ) );
	bool res( false );
	HHuginn::type_id_t typeId( v2_->type_id() );
	if ( typeId == HHuginn::TYPE::TUPLE ) {
		HHuginn::HTuple const& tuple( *static_cast<HHuginn::HTuple const*>( v2_.raw() ) );
		res = tuple.find( thread_, position_, v1_ ) != HHuginn::HTuple::npos;
	} else if ( typeId == HHuginn::TYPE::LIST ) {
		HHuginn::HList const& list( *static_cast<HHuginn::HList const*>( v2_.raw() ) );
		res = list.find( thread_, position_, v1_ ) != HHuginn::HList::npos;
	} else if ( typeId == HHuginn::TYPE::DEQUE ) {
		HHuginn::HDeque const& deque( *static_cast<HHuginn::HDeque const*>( v2_.raw() ) );
		res = deque.find( thread_, position_, v1_ ) != HHuginn::HDeque::npos;
	} else if ( typeId == HHuginn::TYPE::DICT ) {
		HHuginn::HDict const& dict( *static_cast<HHuginn::HDict const*>( v2_.raw() ) );
		res = dict.has_key( thread_, v1_, position_ );
	} else if ( typeId == HHuginn::TYPE::LOOKUP ) {
		HHuginn::HLookup const& lookup( *static_cast<HHuginn::HLookup const*>( v2_.raw() ) );
		res = lookup.has_key( thread_, v1_, position_ );
	} else if ( typeId == HHuginn::TYPE::ORDER ) {
		HHuginn::HOrder const& order( *static_cast<HHuginn::HOrder const*>( v2_.raw() ) );
		res = order.has_key( thread_, v1_, position_ );
	} else if ( typeId == HHuginn::TYPE::SET ) {
		HHuginn::HSet const& set( *static_cast<HHuginn::HSet const*>( v2_.raw() ) );
		res = set.has_key( thread_, v1_, position_ );
	} else if ( typeId == HHuginn::TYPE::STRING ) {
		HString const& string( static_cast<HHuginn::HString const*>( v2_.raw() )->value() );
		if ( v1_->type_id() != HHuginn::TYPE::CHARACTER ) {
			throw HHuginn::HHuginnRuntimeException(
				"Only `character`s can be elements of `string`s.",
				thread_->current_frame()->file_id(),
				position_
			);
		}
		res = string.find( static_cast<HHuginn::HCharacter const*>( v1_.raw() )->value() ) != HString::npos;
	} else if ( is_enum_class( v2_ ) ) {
		if ( is_enumeral( v1_ ) ) {
			res = static_cast<enumeration::HEnumerationClass::HEnumeralClass const*>( static_cast<HHuginn::HEnumeral const*>( v1_.raw() )->get_class() )->enumeration_class() == v2_->get_class();
		} else {
			throw HHuginn::HHuginnRuntimeException(
				"Only `ENUMERAL`s can be elements of `ENUMERATION`s.",
				thread_->current_frame()->file_id(),
				position_
			);
		}
	} else {
		res = fallback_compare( thread_, IDENTIFIER::INTERFACE::CONTAINS, op_to_str( operator_ ), v2_, v1_, position_ );
	}
	return ( res );
}

HHuginn::value_t boolean_xor( HThread* thread_, HHuginn::value_t const& v1_, HHuginn::value_t const& v2_, int ) {
	M_ASSERT( ( v1_->type_id() == HHuginn::TYPE::BOOLEAN ) && ( v2_->type_id() == HHuginn::TYPE::BOOLEAN ) );
	bool v1( static_cast<HHuginn::HBoolean const*>( v1_.raw() )->value() );
	bool v2( static_cast<HHuginn::HBoolean const*>( v2_.raw() )->value() );
	return ( thread_->runtime().boolean_value( ( v1 && ! v2 ) || ( ! v1 && v2 ) ) );
}

HHuginn::value_t boolean_not( HThread* thread_, HHuginn::value_t const& v_, int ) {
	M_ASSERT( v_->type_id() == HHuginn::TYPE::BOOLEAN );
	return ( thread_->runtime().boolean_value( ! static_cast<HHuginn::HBoolean const*>( v_.raw() )->value() ) );
}

namespace {

HHuginn::value_t fallback_conversion( HHuginn::type_id_t type_, HThread* thread_, HHuginn::identifier_id_t methodIdentifier_, HHuginn::value_t const& v_, int position_ ) {
	HHuginn::value_t res;
	if ( HHuginn::HObject const* o = dynamic_cast<HHuginn::HObject const*>( v_.raw() ) ) {
		res = o->call_method( thread_, v_, methodIdentifier_, HArguments( thread_ ), position_ );
		if ( res->type_id() != type_ ) {
			throw HHuginn::HHuginnRuntimeException(
				"User conversion method returned invalid type "_ys
					.append( a_type_name( res->get_class() ) )
					.append( " instead of " )
					.append( a_type_name( type_ ) )
					.append( "." ),
				thread_->current_frame()->file_id(),
				position_
			);
		}
	} else {
		HHuginn::HClass const* c( v_->get_class() );
		int idx( c->field_index( methodIdentifier_ ) );
		if ( idx >= 0 ) {
			HHuginn::HClass::HMethod const& m( *static_cast<HHuginn::HClass::HMethod const*>( c->field( idx ).raw() ) );
			res = m.function()( thread_, const_cast<HHuginn::value_t*>( &v_ ), HArguments( thread_ ), position_ );
			M_ASSERT( ( res->type_id() == type_ ) || ! thread_->can_continue() );
		} else {
			throw HHuginn::HHuginnRuntimeException(
				"Conversion from "_ys
					.append( a_type_name( v_->get_class() ) )
					.append( " to " )
					.append( a_type_name( type_ ) )
					.append( " is not supported." ),
				thread_->current_frame()->file_id(),
				position_
			);
		}
	}
	return ( res );
}

HHuginn::value_t fallback_string_conversion( HThread* thread_, HHuginn::value_t const& v_, int position_ ) {
	HHuginn::value_t v;
	if ( v_->get_class()->is_kind_of( IDENTIFIER::BUILTIN::STRING ) ) {
		v = thread_->object_factory().create_string( get_string( v_ ) );
	} else {
		HCycleTracker cycleTracker;
		v = thread_->runtime().object_factory()->create_string( instruction::string_representation( thread_, v_, cycleTracker, position_ ) );
	}
	return ( v );
}

char const* type_to_cycle_str( HHuginn::TYPE type_ ) {
	char const* sym( "/*cycle*/" );
	switch ( type_ ) {
		case ( HHuginn::TYPE::TUPLE ):  { sym = "/*(cycle)*/"; } break;
		case ( HHuginn::TYPE::LIST ):   { sym = "/*[cycle]*/"; } break;
		case ( HHuginn::TYPE::LOOKUP ): { sym = "/*{cycle}*/"; } break;
		default:;
	}
	return ( sym );
}

}

yaal::hcore::HString string_representation( HThread* thread_, HHuginn::value_t const& value_, HCycleTracker& cycleTracker_, int position_ ) {
	yaal::hcore::HString str;
	if ( ! cycleTracker_.track( value_ ) ) {
		return ( "none"_ys.append( type_to_cycle_str( static_cast<HHuginn::TYPE>( value_->type_id().get() ) ) ) );
	}
	switch ( static_cast<HHuginn::TYPE>( value_->type_id().get() ) ) {
		case ( HHuginn::TYPE::STRING ): {
			str.assign( '"' ).append( static_cast<HHuginn::HString const*>( value_.raw() )->value() ).append( '"' );
		} break;
		case ( HHuginn::TYPE::INTEGER ): {
			str = static_cast<HHuginn::HInteger const*>( value_.raw() )->value();
		} break;
		case ( HHuginn::TYPE::REAL ): {
			str = static_cast<HHuginn::HReal const*>( value_.raw() )->value();
		} break;
		case ( HHuginn::TYPE::NUMBER ): {
			str.assign( '$' ).append( static_cast<HHuginn::HNumber const*>( value_.raw() )->value().to_string() );
		} break;
		case ( HHuginn::TYPE::CHARACTER ): {
			str.assign( "'" ).append( static_cast<HHuginn::HCharacter const*>( value_.raw() )->value() ).append( "'" );
		} break;
		case ( HHuginn::TYPE::BOOLEAN ): {
			str = static_cast<HHuginn::HBoolean const*>( value_.raw() )->value() ? KEYWORD::TRUE : KEYWORD::FALSE;
		} break;
		case ( HHuginn::TYPE::NONE ): {
			str = "none";
		} break;
		case ( HHuginn::TYPE::FUNCTION_REFERENCE ): {
			if ( thread_ ) {
				HHuginn::HFunctionReference const& fr( *static_cast<HHuginn::HFunctionReference const*>( value_.raw() ) );
				HRuntime& r( thread_->runtime() );
				HHuginn::HClass const* c( r.get_class( fr.function().id() ) );
				HString const* originName( !!c ? r.package_name( c->origin() ) : nullptr );
				if ( originName ) {
					str.assign( *originName ).append( "." );
				}
				str.append( r.identifier_name( fr.identifier_id() ) );
			} else {
				str = type_name( HHuginn::TYPE::FUNCTION_REFERENCE );
			}
		} break;
		case ( HHuginn::TYPE::TUPLE ): {
			HHuginn::HTuple const* t( static_cast<HHuginn::HTuple const*>( value_.raw() ) );
			str = "(";
			bool next( false );
			for ( HHuginn::value_t const& v : t->value() ) {
				if ( next ) {
					str.append( ", " );
				}
				next = true;
				str.append( string_representation( thread_, v, cycleTracker_, position_ ) );
			}
			if ( t->value().get_size() == 1 ) {
				str.append( "," );
			}
			str.append( ")" );
		} break;
		case ( HHuginn::TYPE::LIST ): {
			HHuginn::HList const* l( static_cast<HHuginn::HList const*>( value_.raw() ) );
			str = "[";
			bool next( false );
			for ( HHuginn::value_t const& v : l->value() ) {
				if ( next ) {
					str.append( ", " );
				}
				next = true;
				str.append( string_representation( thread_, v, cycleTracker_, position_ ) );
			}
			str.append( "]" );
		} break;
		case ( HHuginn::TYPE::DEQUE ): {
			HHuginn::HDeque const* l( static_cast<HHuginn::HDeque const*>( value_.raw() ) );
			str = "deque(";
			bool next( false );
			for ( HHuginn::value_t const& v : l->value() ) {
				if ( next ) {
					str.append( ", " );
				}
				next = true;
				str.append( string_representation( thread_, v, cycleTracker_, position_ ) );
			}
			str.append( ")" );
		} break;
		case ( HHuginn::TYPE::DICT ): {
			HHuginn::HDict::values_t const& d( static_cast<HHuginn::HDict const*>( value_.raw() )->value() );
			if ( d.is_empty() ) {
				str = "dict()";
				break;
			}
			str = "[";
			bool next( false );
			for ( HHuginn::HDict::values_t::value_type const& v : d ) {
				if ( next ) {
					str.append( ", " );
				}
				next = true;
				str.append( string_representation( thread_, v.first, cycleTracker_, position_ ) ).append( ": " ).append( string_representation( thread_, v.second, cycleTracker_, position_ ) );
			}
			str.append( "]" );
		} break;
		case ( HHuginn::TYPE::LOOKUP ): {
			HHuginn::HLookup::values_t const& lk( static_cast<HHuginn::HLookup const*>( value_.raw() )->value() );
			str = "{";
			bool next( false );
			for ( HHuginn::HLookup::values_t::value_type const& v : lk ) {
				if ( next ) {
					str.append( ", " );
				}
				next = true;
				str.append( string_representation( thread_, v.first, cycleTracker_, position_ ) ).append( ": " ).append( string_representation( thread_, v.second, cycleTracker_, position_ ) );
			}
			str.append( "}" );
		} break;
		case ( HHuginn::TYPE::SET ): {
			HHuginn::HSet::values_t const& s( static_cast<HHuginn::HSet const*>( value_.raw() )->value() );
			if ( s.is_empty() ) {
				str = "set()";
				break;
			}
			str = "{";
			bool next( false );
			for ( HHuginn::value_t const& v : s ) {
				if ( next ) {
					str.append( ", " );
				}
				next = true;
				str.append( string_representation( thread_, v, cycleTracker_, position_ ) );
			}
			str.append( "}" );
		} break;
		case ( HHuginn::TYPE::ORDER ): {
			HHuginn::HOrder const* l( static_cast<HHuginn::HOrder const*>( value_.raw() ) );
			str = "order(";
			bool next( false );
			for ( HHuginn::value_t const& v : l->value() ) {
				if ( next ) {
					str.append( ", " );
				}
				next = true;
				str.append( string_representation( thread_, v, cycleTracker_, position_ ) );
			}
			str.append( ")" );
		} break;
		default: {
			if ( thread_ ) {
				HHuginn::value_t v( fallback_conversion( type_id( HHuginn::TYPE::STRING ), thread_, IDENTIFIER::INTERFACE::TO_STRING, value_, position_ ) );
				str = get_string( v );
			} else {
				str = value_->get_class()->name();
			}
		}
	}
	cycleTracker_.done( value_ );
	return ( str );
}

HHuginn::value_t string( HThread* thread_, HHuginn::value_t const& v_, int position_ ) {
	HHuginn::value_t res;
	HHuginn::type_id_t typeId( v_->type_id() );
	switch ( typeId.get() ) {
		case ( static_cast<int>( HHuginn::TYPE::STRING ) ): {
			res = v_;
		} break;
		case ( static_cast<int>( HHuginn::TYPE::NUMBER ) ): {
			res = thread_->object_factory().create_string( static_cast<HHuginn::HNumber const*>( v_.raw() )->value().to_string() );
		} break;
		case ( static_cast<int>( HHuginn::TYPE::REAL ) ): {
			res = thread_->object_factory().create_string( static_cast<HHuginn::HReal const*>( v_.raw() )->value() );
		} break;
		case ( static_cast<int>( HHuginn::TYPE::CHARACTER ) ): {
			res = thread_->object_factory().create_string( static_cast<HHuginn::HCharacter const*>( v_.raw() )->value() );
		} break;
		case ( static_cast<int>( HHuginn::TYPE::INTEGER ) ): {
			res = thread_->object_factory().create_string( static_cast<HHuginn::HInteger const*>( v_.raw() )->value() );
		} break;
		case ( static_cast<int>( HHuginn::TYPE::BOOLEAN ) ): {
			res = thread_->object_factory().create_string( static_cast<HHuginn::HBoolean const*>( v_.raw() )->value() ? KEYWORD::TRUE : KEYWORD::FALSE );
		} break;
		case ( static_cast<int>( HHuginn::TYPE::NONE ) ): {
			res = thread_->object_factory().create_string( KEYWORD::NONE );
		} break;
		default: {
			res = fallback_string_conversion( thread_, v_, position_ );
		}
	}
	return ( res );
}

HHuginn::value_t boolean( HThread* thread_, HHuginn::value_t const& v_, int position_ ) {
	HHuginn::value_t res;
	if ( v_->type_id() == HHuginn::TYPE::BOOLEAN ) {
		res = v_;
	} else {
		res = fallback_conversion( type_id( HHuginn::TYPE::BOOLEAN ), thread_, IDENTIFIER::INTERFACE::TO_BOOLEAN, v_, position_ );
	}
	return ( res );
}

HHuginn::value_t integer( HThread* thread_, HHuginn::value_t const& v_, int position_ ) {
	HHuginn::value_t res( thread_->runtime().none_value() );
	HHuginn::type_id_t typeId( v_->type_id() );
	if ( typeId == HHuginn::TYPE::STRING ) {
		int long long v( 0 );
		try {
			v = lexical_cast<int long long>( static_cast<HHuginn::HString const*>( v_.raw() )->value() );
		} catch ( HException const& e ) {
			thread_->raise( thread_->object_factory().conversion_exception_class(), e.what(), position_ );
		}
		res = thread_->object_factory().create_integer( v );
	} else if ( typeId == HHuginn::TYPE::NUMBER ) {
		int long long v( 0 );
		try {
			v = static_cast<HHuginn::HNumber const*>( v_.raw() )->value().to_integer();
		} catch ( HException const& e ) {
			thread_->raise( thread_->object_factory().conversion_exception_class(), e.what(), position_ );
		}
		res = thread_->object_factory().create_integer( v );
	} else if ( typeId == HHuginn::TYPE::REAL ) {
		res = thread_->object_factory().create_integer( static_cast<int long long>( static_cast<HHuginn::HReal const*>( v_.raw() )->value() ) );
	} else if ( typeId == HHuginn::TYPE::CHARACTER ) {
		res = thread_->object_factory().create_integer( static_cast<int long long>( static_cast<HHuginn::HCharacter const*>( v_.raw() )->value().get() ) );
	} else if ( typeId == HHuginn::TYPE::INTEGER ) {
		res = v_;
	} else {
		res = fallback_conversion( type_id( HHuginn::TYPE::INTEGER ), thread_, IDENTIFIER::INTERFACE::TO_INTEGER, v_, position_ );
	}
	return ( res );
}

HHuginn::value_t real( HThread* thread_, HHuginn::value_t const& v_, int position_ ) {
	HHuginn::value_t res;
	HHuginn::type_id_t typeId( v_->type_id() );
	if ( typeId == HHuginn::TYPE::STRING ) {
		double long v( 0 );
		try {
			v = lexical_cast<double long>( static_cast<HHuginn::HString const*>( v_.raw() )->value() );
		} catch ( HException const& e ) {
			thread_->raise( thread_->object_factory().conversion_exception_class(), e.what(), position_ );
		}
		res = thread_->object_factory().create_real( v );
	} else if ( typeId == HHuginn::TYPE::NUMBER ) {
		res = thread_->object_factory().create_real( static_cast<HHuginn::HNumber const*>( v_.raw() )->value().to_floating_point() );
	} else if ( typeId == HHuginn::TYPE::REAL ) {
		res = v_;
	} else if ( typeId == HHuginn::TYPE::INTEGER ) {
		res = thread_->object_factory().create_real( static_cast<double long>( static_cast<HHuginn::HInteger const*>( v_.raw() )->value() ) );
	} else {
		res = fallback_conversion( type_id( HHuginn::TYPE::REAL ), thread_, IDENTIFIER::INTERFACE::TO_REAL, v_, position_ );
	}
	return ( res );
}

HHuginn::value_t character( HThread* thread_, HHuginn::value_t const& v_, int position_ ) {
	HHuginn::value_t res;
	HHuginn::type_id_t typeId( v_->type_id() );
	if ( typeId == HHuginn::TYPE::CHARACTER ) {
		res = v_;
	} else if ( typeId == HHuginn::TYPE::INTEGER ) {
		res = thread_->object_factory().create_character( static_cast<HHuginn::HCharacter const*>( v_.raw() )->value() );
	} else {
		res = fallback_conversion( type_id( HHuginn::TYPE::CHARACTER ), thread_, IDENTIFIER::INTERFACE::TO_CHARACTER, v_, position_ );
	}
	return ( res );
}

HHuginn::value_t number( HThread* thread_, HHuginn::value_t const& v_, int position_ ) {
	HHuginn::value_t res( thread_->runtime().none_value() );
	HHuginn::type_id_t typeId( v_->type_id() );
	if ( typeId == HHuginn::TYPE::STRING ) {
		try {
			res = thread_->object_factory().create_number( static_cast<HHuginn::HString const*>( v_.raw() )->value() );
		} catch ( HNumberException const& ) {
			thread_->raise( thread_->object_factory().conversion_exception_class(), "Not a number.", position_ );
		}
	} else if ( typeId == HHuginn::TYPE::NUMBER ) {
		res = v_;
	} else if ( typeId == HHuginn::TYPE::REAL ) {
		try {
			res = thread_->object_factory().create_number( static_cast<HHuginn::HReal const*>( v_.raw() )->value() );
		} catch ( HNumberException const& ) {
			thread_->raise( thread_->object_factory().conversion_exception_class(), "Not a number.", position_ );
		}
	} else if ( typeId == HHuginn::TYPE::INTEGER ) {
		res = thread_->object_factory().create_number( static_cast<HHuginn::HInteger const*>( v_.raw() )->value() );
	} else {
		res = fallback_conversion( type_id( HHuginn::TYPE::NUMBER ), thread_, IDENTIFIER::INTERFACE::TO_NUMBER, v_, position_ );
	}
	return ( res );
}

}

}

}

}

