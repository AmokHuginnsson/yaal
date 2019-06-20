/* Read yaal/LICENSE.md file for copyright and licensing information. */

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
#include "reference.hxx"
#include "objectreference.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace huginn {

namespace instruction {

void no_such_member( HThread*, yaal::hcore::HString const&, HHuginn::identifier_id_t, int, HClass const* ) __attribute__(( noreturn ));
inline void no_such_member(
	HThread* thread_,
	yaal::hcore::HString const& name_,
	HHuginn::identifier_id_t memberId_,
	int position_,
	HClass const* context_ = nullptr
) {
	hcore::HString message;
	HRuntime& rt( thread_->runtime() );
	message
		.append( "`" )
		.append( name_ )
		.append( "` does not have `" )
		.append( rt.identifier_name( memberId_ ) )
		.append( "` member (did you mean `" );
	if ( context_ ) {
		message.append( rt.suggestion( context_, memberId_ ) );
	} else {
		message.append( rt.suggestion( memberId_ ) );
	}
	message.append( "`?)." ),
	throw HHuginn::HHuginnRuntimeException( message, thread_->current_frame()->file_id(), position_ );
}

HHuginn::value_t member_value( HThread* thread_, HHuginn::value_t const& object_, HHuginn::identifier_id_t memberId_, int position_ ) {
	M_PROLOG
	HHuginn::value_t m;
	HRuntime& rt( thread_->runtime() );
	HClass const* cls( object_->get_class() );
	int idx( cls->field_index( IDENTIFIER::INTERFACE::MEMBER ) );
	HHuginn::value_t s( rt.object_factory()->create_string( rt.identifier_name( memberId_ ) ) );
	if ( idx < 0 ) {
		no_such_member( thread_, cls->name(), memberId_, position_, cls );
	} else if ( HObject const* o = dynamic_cast<HObject const*>( object_.raw() ) ) {
		m = o->call_method( thread_, object_, IDENTIFIER::INTERFACE::MEMBER, HArguments( thread_, s ), position_ );
	} else {
		HClass::HMethod const& method( *static_cast<HClass::HMethod const*>( cls->field( idx ).raw() ) );
		m = method.function()( thread_, const_cast<HHuginn::value_t*>( &object_ ), HArguments( thread_, s ), position_ );
	}
	M_ASSERT( !! m );
	if ( ! thread_->can_continue() ) {
		throw Interrupt();
	}
	return ( m );
	M_EPILOG
}

void member_assign( HThread* thread_, HHuginn::value_t& object_, HHuginn::identifier_id_t memberId_, HHuginn::value_t const& value_, int position_ ) {
	M_PROLOG
	HRuntime& rt( thread_->runtime() );
	HHuginn::value_t s( rt.object_factory()->create_string( rt.identifier_name( memberId_ ) ) );
	HClass const* cls( object_->get_class() );
	int idx( cls->field_index( IDENTIFIER::INTERFACE::SET_MEMBER ) );
	if ( idx < 0 ) {
		no_such_member( thread_, cls->name(), memberId_, position_, cls );
	} else if ( HObject const* o = dynamic_cast<HObject const*>( object_.raw() ) ) {
		o->call_method( thread_, object_, IDENTIFIER::INTERFACE::SET_MEMBER, HArguments( thread_, value_, s ), position_ );
	} else {
		HClass::HMethod const& m( *static_cast<HClass::HMethod const*>( cls->field( idx ).raw() ) );
		m.function()( thread_, const_cast<HHuginn::value_t*>( &object_ ), HArguments( thread_, value_, s ), position_ );
	}
	if ( ! thread_->can_continue() ) {
		throw Interrupt();
	}
	return;
	M_EPILOG
}

HHuginn::value_t member( HThread* thread_, HFrame::ACCESS access_, HHuginn::value_t& v_, HHuginn::identifier_id_t memberId_, int position_ ) {
	M_PROLOG
	HRuntime& rt( thread_->runtime() );
	HHuginn::value_t m;
	HClass const* cls( v_->get_class() );
	if ( cls->is_complex() ) {
		int fi( v_->field_index( memberId_ ) );
		if ( fi >= 0 ) {
			if ( access_ == HFrame::ACCESS::BOUND_CALL ) {
				m = v_->field( fi );
			} else if ( access_ == HFrame::ACCESS::VALUE ) {
				m = v_->field( v_, fi );
			} else if ( ! v_.unique() ) {
				if ( HObject* o = dynamic_cast<HObject*>( v_.raw() ) ) {
					m = rt.object_factory()->create_reference( o->field_ref( fi ) );
				} else {
					throw HHuginn::HHuginnRuntimeException( "Assignment to read-only location.", thread_->current_frame()->file_id(), position_ );
				}
			} else {
				throw HHuginn::HHuginnRuntimeException( "Assignment to temporary.", thread_->current_frame()->file_id(), position_ );
			}
		} else {
			fi = cls->field_index( memberId_, HClass::MEMBER_TYPE::STATIC );
			if ( fi >= 0 ) {
				throw HHuginn::HHuginnRuntimeException(
					"`"_ys
						.append( rt.identifier_name( memberId_ ) )
						.append( "` member of `" )
						.append( cls->name() )
						.append( "` must be accessed from static context." ),
					thread_->current_frame()->file_id(),
					position_
				);
			}
			if ( access_ != HFrame::ACCESS::REFERENCE ) {
				m = member_value( thread_, v_, memberId_, position_ );
			} else {
				m = rt.object_factory()->create<HMemberReference>( rt.object_factory()->reference_class(), v_, memberId_ );
			}
		}
	} else if ( HObjectReference* oref = dynamic_cast<HObjectReference*>( v_.raw() ) ) { /* Handle `super` keyword. */
		if ( access_ == HFrame::ACCESS::REFERENCE ) {
			throw HHuginn::HHuginnRuntimeException( "Changing upcasted reference.", thread_->current_frame()->file_id(), position_ );
		}
		int fi( oref->field_index( memberId_ ) );
		if ( fi >= 0 ) {
			m = oref->field( thread_, fi, position_ );
		} else if ( ( memberId_ == IDENTIFIER::KEYWORD::CONSTRUCTOR ) && ( oref->reference_class()->type() == HClass::TYPE::BUILTIN ) ) {
			m = rt.object_factory()->create_bound_method(
				hcore::call(
					&HObject::init_base, _1, _2, _3, _4
				),
				oref->value()
			);
		} else {
			no_such_member( thread_, oref->reference_class()->name(), memberId_, position_ );
		}
	} else if ( v_->type_id() == HHuginn::TYPE::FUNCTION_REFERENCE ) {
		huginn::HFunctionReference* fr( static_cast<huginn::HFunctionReference*>( v_.raw() ) );
		HHuginn::identifier_id_t funcId( fr->identifier_id() );
		HClass const* c( fr->juncture() );
		if ( ! c ) {
			c = rt.get_class( funcId ).raw();
		}
		if ( ! c ) {
			throw HHuginn::HHuginnRuntimeException(
				"`"_ys.append( rt.identifier_name( funcId ) ).append( "` is not a compound object." ),
				thread_->current_frame()->file_id(),
				position_
			);
		}
		int fi( c->field_index( memberId_, HClass::MEMBER_TYPE::STATIC ) );
		if ( fi < 0 ) {
			no_such_member( thread_, c->name(), memberId_, position_ );
		}
		if ( access_ == HFrame::ACCESS::REFERENCE ) {
			throw HHuginn::HHuginnRuntimeException( "Assignment to read-only location.", thread_->current_frame()->file_id(), position_ );
		}
		HHuginn::value_t const& f( c->field( fi ) );
		m = f->type_id() == HHuginn::TYPE::METHOD
			? rt.object_factory()->create_unbound_method( c, static_cast<HClass::HMethod const*>( f.raw() )->function() )
			: f;
	} else {
		throw HHuginn::HHuginnRuntimeException( "`"_ys.append( cls->name() ).append( "` is not a compound object." ), thread_->current_frame()->file_id(), position_ );
	}
	M_ASSERT( !! m );
	return ( m );
	M_EPILOG
}

bool less( HThread* thread_, HHuginn::value_t const& v1_, HHuginn::value_t const& v2_, int position_ ) {
	return ( v1_->operator_less( thread_, v1_, v2_, position_ ) );
}

bool checked_less( HThread* thread_, HHuginn::value_t const& v1_, HHuginn::value_t const& v2_, int position_ ) {
	HClass const* c1( v1_->get_class() );
	HClass const* c2( v2_->get_class() );
	if ( c1 != c2 ) {
		operands_type_mismatch( op_to_str( OPERATOR::LESS ), c1, c2, thread_->current_frame()->file_id(), position_ );
	}
	return ( v1_->operator_less( thread_, v1_, v2_, position_ ) );
}

HHuginn::value_t boolean_xor( HThread* thread_, HHuginn::value_t const& v1_, HHuginn::value_t const& v2_, int ) {
	M_ASSERT( ( v1_->type_id() == HHuginn::TYPE::BOOLEAN ) && ( v2_->type_id() == HHuginn::TYPE::BOOLEAN ) );
	bool v1( static_cast<HBoolean const*>( v1_.raw() )->value() );
	bool v2( static_cast<HBoolean const*>( v2_.raw() )->value() );
	return ( thread_->runtime().boolean_value( ( v1 && ! v2 ) || ( ! v1 && v2 ) ) );
}

HHuginn::value_t boolean_not( HThread* thread_, HHuginn::value_t const& v_, int ) {
	M_ASSERT( v_->type_id() == HHuginn::TYPE::BOOLEAN );
	return ( thread_->runtime().boolean_value( ! static_cast<HBoolean const*>( v_.raw() )->value() ) );
}

namespace {

HHuginn::value_t fallback_conversion( HHuginn::type_id_t type_, HThread* thread_, HHuginn::identifier_id_t methodIdentifier_, HHuginn::value_t const& v_, int position_ ) {
	HHuginn::value_t res;
	if ( HObject const* o = dynamic_cast<HObject const*>( v_.raw() ) ) {
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
		HClass const* c( v_->get_class() );
		int idx( c->field_index( methodIdentifier_ ) );
		if ( idx >= 0 ) {
			HClass::HMethod const& m( *static_cast<HClass::HMethod const*>( c->field( idx ).raw() ) );
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
			str.assign( '"' ).append( static_cast<HString const*>( value_.raw() )->value() ).append( '"' );
		} break;
		case ( HHuginn::TYPE::INTEGER ): {
			str = static_cast<huginn::HInteger const*>( value_.raw() )->value();
		} break;
		case ( HHuginn::TYPE::REAL ): {
			str = static_cast<HReal const*>( value_.raw() )->value();
		} break;
		case ( HHuginn::TYPE::NUMBER ): {
			str.assign( '$' ).append( static_cast<HNumber const*>( value_.raw() )->value().to_string() );
		} break;
		case ( HHuginn::TYPE::CHARACTER ): {
			str.assign( "'" ).append( static_cast<HCharacter const*>( value_.raw() )->value() ).append( "'" );
		} break;
		case ( HHuginn::TYPE::BOOLEAN ): {
			str = static_cast<HBoolean const*>( value_.raw() )->value() ? KEYWORD::TRUE : KEYWORD::FALSE;
		} break;
		case ( HHuginn::TYPE::NONE ): {
			str = "none";
		} break;
		case ( HHuginn::TYPE::FUNCTION_REFERENCE ): {
			if ( thread_ ) {
				huginn::HFunctionReference const& fr( *static_cast<huginn::HFunctionReference const*>( value_.raw() ) );
				HRuntime& r( thread_->runtime() );
				HClass const* c( r.get_class( fr.function().id() ) );
				hcore::HString const* originName( !!c ? r.package_name( c->origin() ) : nullptr );
				if ( originName ) {
					str.assign( *originName ).append( "." );
				}
				str.append( r.identifier_name( fr.identifier_id() ) );
			} else {
				str = type_name( HHuginn::TYPE::FUNCTION_REFERENCE );
			}
		} break;
		case ( HHuginn::TYPE::TUPLE ): {
			huginn::HTuple const* t( static_cast<huginn::HTuple const*>( value_.raw() ) );
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
			huginn::HList const* l( static_cast<huginn::HList const*>( value_.raw() ) );
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
			huginn::HDeque const* l( static_cast<huginn::HDeque const*>( value_.raw() ) );
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
			huginn::HDict::values_t const& d( static_cast<huginn::HDict const*>( value_.raw() )->value() );
			if ( d.is_empty() ) {
				str = "dict()";
				break;
			}
			str = "[";
			bool next( false );
			for ( huginn::HDict::values_t::value_type const& v : d ) {
				if ( next ) {
					str.append( ", " );
				}
				next = true;
				str.append( string_representation( thread_, v.first, cycleTracker_, position_ ) ).append( ": " ).append( string_representation( thread_, v.second, cycleTracker_, position_ ) );
			}
			str.append( "]" );
		} break;
		case ( HHuginn::TYPE::LOOKUP ): {
			huginn::HLookup::values_t const& lk( static_cast<huginn::HLookup const*>( value_.raw() )->value() );
			str = "{";
			bool next( false );
			for ( huginn::HLookup::values_t::value_type const& v : lk ) {
				if ( next ) {
					str.append( ", " );
				}
				next = true;
				str.append( string_representation( thread_, v.first, cycleTracker_, position_ ) ).append( ": " ).append( string_representation( thread_, v.second, cycleTracker_, position_ ) );
			}
			str.append( "}" );
		} break;
		case ( HHuginn::TYPE::SET ): {
			huginn::HSet::values_t const& s( static_cast<huginn::HSet const*>( value_.raw() )->value() );
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
			huginn::HOrder const* l( static_cast<huginn::HOrder const*>( value_.raw() ) );
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
			res = thread_->object_factory().create_string( static_cast<HNumber const*>( v_.raw() )->value().to_string() );
		} break;
		case ( static_cast<int>( HHuginn::TYPE::REAL ) ): {
			res = thread_->object_factory().create_string( static_cast<HReal const*>( v_.raw() )->value() );
		} break;
		case ( static_cast<int>( HHuginn::TYPE::CHARACTER ) ): {
			res = thread_->object_factory().create_string( static_cast<HCharacter const*>( v_.raw() )->value() );
		} break;
		case ( static_cast<int>( HHuginn::TYPE::INTEGER ) ): {
			res = thread_->object_factory().create_string( static_cast<huginn::HInteger const*>( v_.raw() )->value() );
		} break;
		case ( static_cast<int>( HHuginn::TYPE::BOOLEAN ) ): {
			res = thread_->object_factory().create_string( static_cast<HBoolean const*>( v_.raw() )->value() ? KEYWORD::TRUE : KEYWORD::FALSE );
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
			v = lexical_cast<int long long>( static_cast<HString const*>( v_.raw() )->value() );
		} catch ( hcore::HException const& e ) {
			thread_->raise( thread_->object_factory().conversion_exception_class(), e.what(), position_ );
		}
		res = thread_->object_factory().create_integer( v );
	} else if ( typeId == HHuginn::TYPE::NUMBER ) {
		int long long v( 0 );
		try {
			v = static_cast<HNumber const*>( v_.raw() )->value().to_integer();
		} catch ( hcore::HException const& e ) {
			thread_->raise( thread_->object_factory().conversion_exception_class(), e.what(), position_ );
		}
		res = thread_->object_factory().create_integer( v );
	} else if ( typeId == HHuginn::TYPE::REAL ) {
		res = thread_->object_factory().create_integer( static_cast<int long long>( static_cast<HReal const*>( v_.raw() )->value() ) );
	} else if ( typeId == HHuginn::TYPE::CHARACTER ) {
		res = thread_->object_factory().create_integer( static_cast<int long long>( static_cast<HCharacter const*>( v_.raw() )->value().get() ) );
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
			v = lexical_cast<double long>( static_cast<HString const*>( v_.raw() )->value() );
		} catch ( hcore::HException const& e ) {
			thread_->raise( thread_->object_factory().conversion_exception_class(), e.what(), position_ );
		}
		res = thread_->object_factory().create_real( v );
	} else if ( typeId == HHuginn::TYPE::NUMBER ) {
		res = thread_->object_factory().create_real( static_cast<HNumber const*>( v_.raw() )->value().to_floating_point() );
	} else if ( typeId == HHuginn::TYPE::REAL ) {
		res = v_;
	} else if ( typeId == HHuginn::TYPE::INTEGER ) {
		res = thread_->object_factory().create_real( static_cast<double long>( static_cast<huginn::HInteger const*>( v_.raw() )->value() ) );
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
		res = thread_->object_factory().create_character( static_cast<HCharacter const*>( v_.raw() )->value() );
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
			res = thread_->object_factory().create_number( static_cast<HString const*>( v_.raw() )->value() );
		} catch ( HNumberException const& ) {
			thread_->raise( thread_->object_factory().conversion_exception_class(), "Not a number.", position_ );
		}
	} else if ( typeId == HHuginn::TYPE::NUMBER ) {
		res = v_;
	} else if ( typeId == HHuginn::TYPE::REAL ) {
		try {
			res = thread_->object_factory().create_number( static_cast<HReal const*>( v_.raw() )->value() );
		} catch ( HNumberException const& ) {
			thread_->raise( thread_->object_factory().conversion_exception_class(), "Not a number.", position_ );
		}
	} else if ( typeId == HHuginn::TYPE::INTEGER ) {
		res = thread_->object_factory().create_number( static_cast<huginn::HInteger const*>( v_.raw() )->value() );
	} else {
		res = fallback_conversion( type_id( HHuginn::TYPE::NUMBER ), thread_, IDENTIFIER::INTERFACE::TO_NUMBER, v_, position_ );
	}
	return ( res );
}

}

}

}

}

