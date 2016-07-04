/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  value_builtin.cxx - this file is integral part of `yaal' project.

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

#include <cmath>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "value_builtin.hxx"
#include "runtime.hxx"
#include "thread.hxx"
#include "helper.hxx"
#include "exception.hxx"
#include "objectfactory.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace huginn {

namespace value_builtin {

HHuginn::value_t subscript(
	HThread* thread_,
	HExpression::ACCESS subscript_,
	HHuginn::value_t& base_,
	HHuginn::value_t const& index_,
	int position_
) {
	HHuginn::type_id_t baseType( base_->type_id() );
	HHuginn::value_t res;
	HObjectFactory& of( *thread_->runtime().object_factory() );
	if ( ( baseType == HHuginn::TYPE::LIST ) || ( baseType == HHuginn::TYPE::DEQUE ) || ( baseType == HHuginn::TYPE::STRING ) ) {
		if ( index_->type_id() != HHuginn::TYPE::INTEGER ) {
			throw HHuginn::HHuginnRuntimeException( _errMsgHHuginn_[ERR_CODE::IDX_NOT_INT], position_ );
		}
		HHuginn::HInteger const* i( static_cast<HHuginn::HInteger const*>( index_.raw() ) );
		int long long index( i->value() );
		int long size( static_cast<HHuginn::HIterable*>( base_.raw() )->size() );
		if ( ( index < -size ) || ( index >= size ) ) {
			throw HHuginn::HHuginnRuntimeException( "Bad index.", position_ );
		}
		if ( index < 0 ) {
			index += size;
		}
		if ( baseType == HHuginn::TYPE::LIST ) {
			HHuginn::HList* l( static_cast<HHuginn::HList*>( base_.raw() ) );
			res = ( subscript_ == HExpression::ACCESS::VALUE ? l->get( index ) : of.create_reference( l->get_ref( index ) ) );
		} else if ( baseType == HHuginn::TYPE::DEQUE ) {
			HHuginn::HDeque* d( static_cast<HHuginn::HDeque*>( base_.raw() ) );
			res = ( subscript_ == HExpression::ACCESS::VALUE ? d->get( index ) : of.create_reference( d->get_ref( index ) ) );
		} else {
			M_ASSERT( baseType == HHuginn::TYPE::STRING );
			HHuginn::HString* s( static_cast<HHuginn::HString*>( base_.raw() ) );
			res = thread_->object_factory().create_character( s->value()[static_cast<int>( index )] );
		}
	} else if ( baseType == HHuginn::TYPE::DICT ) {
		HHuginn::HDict* d( static_cast<HHuginn::HDict*>( base_.raw() ) );
		res = ( subscript_ == HExpression::ACCESS::VALUE ? d->get( index_, position_ ) : of.create_reference( d->get_ref( index_, position_ ) ) );
	} else if ( baseType == HHuginn::TYPE::LOOKUP ) {
		HHuginn::HLookup* l( static_cast<HHuginn::HLookup*>( base_.raw() ) );
		res = ( subscript_ == HExpression::ACCESS::VALUE ? l->get( index_, position_ ) : of.create_reference( l->get_ref( index_ ) ) );
	} else {
		throw HHuginn::HHuginnRuntimeException( "Subscript is not supported on `"_ys.append( base_->get_class()->name() ).append( "'." ), position_ );
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
	if ( ( baseType == HHuginn::TYPE::LIST ) || ( baseType == HHuginn::TYPE::DEQUE ) || ( baseType == HHuginn::TYPE::STRING ) ) {
		if ( !! from_ && ( from_->type_id() != HHuginn::TYPE::INTEGER ) ) {
			throw HHuginn::HHuginnRuntimeException( "Range operand `from' is not an integer.", position_ );
		}
		if ( !! to_ && ( to_->type_id() != HHuginn::TYPE::INTEGER ) ) {
			throw HHuginn::HHuginnRuntimeException( "Range operand `to' is not an integer.", position_ );
		}
		if ( !! step_ && ( step_->type_id() != HHuginn::TYPE::INTEGER ) ) {
			throw HHuginn::HHuginnRuntimeException( "Range operand `step' is not an integer.", position_ );
		}
		int long size( static_cast<HHuginn::HIterable*>( base_.raw() )->size() );
		if ( baseType == HHuginn::TYPE::LIST ) {
			res = thread_->object_factory().create_list();
		} else if ( baseType == HHuginn::TYPE::DEQUE ) {
			res = thread_->object_factory().create_deque();
		} else {
			res = thread_->object_factory().create_string();
		}
		HHuginn::HInteger const* integer( static_cast<HHuginn::HInteger const*>( step_.raw() ) );
		int long step( integer ? static_cast<int long>( integer->value() ) : 1 );
		if ( step == 0 ) {
			throw HHuginn::HHuginnRuntimeException( "Range step cannot be zero.", position_ );
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
			if ( baseType == HHuginn::TYPE::LIST ) {
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
					r.assign( s.raw() + from, to - from );
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
		throw HHuginn::HHuginnRuntimeException( "Range operator is not supported on `"_ys.append( base_->get_class()->name() ).append( "'." ), position_ );
	}
	return ( res );
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
		if ( HHuginn::HObject const* o = dynamic_cast<HHuginn::HObject const*>( v1_.raw() ) ) {
			o->call_method( thread_, v1_, "add", { v2_ }, position_ );
		} else {
			throw HHuginn::HHuginnRuntimeException( "There is no `+' operator for `"_ys.append( v1_->get_class()->name() ).append( "'." ), position_ );
		}
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
		if ( HHuginn::HObject const* o = dynamic_cast<HHuginn::HObject const*>( v1_.raw() ) ) {
			o->call_method( thread_, v1_, "substract", { v2_ }, position_ );
		} else {
			throw HHuginn::HHuginnRuntimeException( "There is no `-' operator for `"_ys.append( v1_->get_class()->name() ).append( "'." ), position_ );
		}
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
		if ( HHuginn::HObject const* o = dynamic_cast<HHuginn::HObject const*>( v1_.raw() ) ) {
			o->call_method( thread_, v1_, "multiply", { v2_ }, position_ );
		} else {
			throw HHuginn::HHuginnRuntimeException( "There is no `*' operator for `"_ys.append( v1_->get_class()->name() ).append( "'." ), position_ );
		}
	}
	return;
}

void div( HThread* thread_, HHuginn::value_t& v1_, HHuginn::value_t const& v2_, int position_ ) {
	M_ASSERT( v1_->type_id() == v2_->type_id() );
	HHuginn::type_id_t typeId( v1_->type_id() );
	if ( typeId == HHuginn::TYPE::INTEGER ) {
		HHuginn::HInteger::value_type denominator( static_cast<HHuginn::HInteger const*>( v2_.raw() )->value() );
		if ( denominator != 0 ) {
			static_cast<HHuginn::HInteger*>( v1_.raw() )->value() /= denominator;
		} else {
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
		if ( HHuginn::HObject const* o = dynamic_cast<HHuginn::HObject const*>( v1_.raw() ) ) {
			o->call_method( thread_, v1_, "divide", { v2_ }, position_ );
		} else {
			v1_ = thread_->runtime().none_value();
			throw HHuginn::HHuginnRuntimeException( "There is no `/' operator for `"_ys.append( v1_->get_class()->name() ).append( "'." ), position_ );
		}
	}
	HRuntime& rt( thread_->runtime() );
	if ( v1_ == rt.none_value() ) {
		thread_->raise( rt.object_factory()->arithmetic_exception_class(), "Division by zero.", position_ );
	}
}

void mod( HThread* thread_, HHuginn::value_t& v1_, HHuginn::value_t const& v2_, int position_ ) {
	M_ASSERT( v1_->type_id() == v2_->type_id() );
	HHuginn::type_id_t typeId( v1_->type_id() );
	if ( typeId == HHuginn::TYPE::INTEGER ) {
		HHuginn::HInteger::value_type denominator( static_cast<HHuginn::HInteger const*>( v2_.raw() )->value() );
		if ( denominator != 0 ) {
			static_cast<HHuginn::HInteger*>( v1_.raw() )->value() %= denominator;
		} else {
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
		if ( HHuginn::HObject const* o = dynamic_cast<HHuginn::HObject const*>( v1_.raw() ) ) {
			o->call_method( thread_, v1_, "modulo", { v2_ }, position_ );
		} else {
			v1_ = thread_->runtime().none_value();
			throw HHuginn::HHuginnRuntimeException( "There is no `%' operator for `"_ys.append( v1_->get_class()->name() ).append( "'." ), position_ );
		}
	}
	HRuntime& rt( thread_->runtime() );
	if ( v1_ == rt.none_value() ) {
		thread_->raise( rt.object_factory()->arithmetic_exception_class(), "Division by zero.", position_ );
	}
	return;
}

void pow( HHuginn::value_t& v1_, HHuginn::value_t const& v2_, int position_ ) {
	M_ASSERT( v1_->type_id() == v2_->type_id() );
	HHuginn::value_t res;
	HHuginn::type_id_t typeId( v1_->type_id() );
	if ( typeId == HHuginn::TYPE::REAL ) {
		HHuginn::HReal::value_type& val( static_cast<HHuginn::HReal*>( v1_.raw() )->value() );
		val = ::powl( val, static_cast<HHuginn::HReal const*>( v2_.raw() )->value() );
	} else if ( typeId == HHuginn::TYPE::NUMBER ) {
		static_cast<HHuginn::HNumber*>( v1_.raw() )->value() ^= static_cast<HHuginn::HNumber const*>( v2_.raw() )->value().to_integer();
	} else {
		throw HHuginn::HHuginnRuntimeException( "There is no `^' operator for `"_ys.append( v1_->get_class()->name() ).append( "'." ), position_ );
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
		}
		if ( ! n.is_integral() ) {
			thread_->raise( rt.object_factory()->arithmetic_exception_class(), "Factorial from fraction.", position_ );
		}
		res = thread_->object_factory().create_number( number::factorial( n.to_integer() ) );
	} else {
		throw HHuginn::HHuginnRuntimeException( "There is no `!` operator for `"_ys.append( v_->get_class()->name() ).append( "'." ), position_ );
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
		} else {
			res = thread_->object_factory().create_integer( -v );
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
		throw HHuginn::HHuginnRuntimeException( "There is no |.| operator for `"_ys.append( v_->get_class()->name() ).append( "'." ), position_ );
	}
	return ( res );
}

HHuginn::value_t neg( HThread* thread_, HHuginn::value_t const& v_, int position_ ) {
	HHuginn::value_t res;
	HHuginn::type_id_t typeId( v_->type_id() );
	if ( typeId == HHuginn::TYPE::INTEGER ) {
		res = thread_->object_factory().create_integer( -static_cast<HHuginn::HInteger const*>( v_.raw() )->value() );
	} else if ( typeId == HHuginn::TYPE::REAL ) {
		res = thread_->object_factory().create_real( -static_cast<HHuginn::HReal const*>( v_.raw() )->value() );
	} else if ( typeId == HHuginn::TYPE::NUMBER ) {
		res = thread_->object_factory().create_number( -static_cast<HHuginn::HNumber const*>( v_.raw() )->value() );
	} else {
		if ( HHuginn::HObject const* o = dynamic_cast<HHuginn::HObject const*>( v_.raw() ) ) {
			o->call_method( thread_, v_, "negate", {}, position_ );
		} else {
			throw HHuginn::HHuginnRuntimeException( "There is no `negate` operator for `"_ys.append( v_->get_class()->name() ).append( "'." ), position_ );
		}
	}
	return ( res );
}

int long hash( HHuginn::value_t const& v_ ) {
	int long rt( 0 );
	HHuginn::type_id_t typeId( v_->type_id() );
	if ( typeId == HHuginn::TYPE::INTEGER ) {
		rt = hcore::hash<int long long>()( static_cast<HHuginn::HInteger const*>( v_.raw() )->value() );
	} else if ( typeId == HHuginn::TYPE::REAL ) {
		rt = hcore::hash<double long>()( static_cast<HHuginn::HReal const*>( v_.raw() )->value() );
	} else if ( typeId == HHuginn::TYPE::STRING ) {
		rt = hcore::hash<hcore::HString>()( static_cast<HHuginn::HString const*>( v_.raw() )->value() );
	} else if ( typeId == HHuginn::TYPE::NUMBER ) {
		rt = hcore::hash<double long>()( static_cast<HHuginn::HNumber const*>( v_.raw() )->value().to_floating_point() );
	} else if ( typeId == HHuginn::TYPE::CHARACTER ) {
		rt = hcore::hash<char>()( static_cast<HHuginn::HCharacter const*>( v_.raw() )->value() );
	} else if ( typeId == HHuginn::TYPE::BOOLEAN ) {
		rt = hcore::hash<bool>()( static_cast<HHuginn::HBoolean const*>( v_.raw() )->value() );
	} else if ( typeId != HHuginn::TYPE::NONE ) {
		throw HHuginn::HHuginnRuntimeException( "There is no `hash' operator for `"_ys.append( v_->get_class()->name() ).append( "'." ), 0 );
	}
	return ( rt );
}


bool equals( HHuginn::value_t const& v1_, HHuginn::value_t const& v2_, int position_ ) {
	bool noneOperand( ( v1_->type_id() == HHuginn::TYPE::NONE ) || ( v2_->type_id() == HHuginn::TYPE::NONE ) );
	M_ASSERT( noneOperand || ( v1_->type_id() == v2_->type_id() ) );
	bool res( false );
	if ( ! noneOperand ) {
		HHuginn::type_id_t typeId( v1_->type_id() );
		if ( typeId == HHuginn::TYPE::INTEGER ) {
			res = static_cast<HHuginn::HInteger const*>( v1_.raw() )->value() == static_cast<HHuginn::HInteger const*>( v2_.raw() )->value();
		} else if ( typeId == HHuginn::TYPE::REAL ) {
			res = static_cast<HHuginn::HReal const*>( v1_.raw() )->value() == static_cast<HHuginn::HReal const*>( v2_.raw() )->value();
		} else if ( typeId == HHuginn::TYPE::STRING ) {
			res = static_cast<HHuginn::HString const*>( v1_.raw() )->value() == static_cast<HHuginn::HString const*>( v2_.raw() )->value();
		} else if ( typeId == HHuginn::TYPE::NUMBER ) {
			res = static_cast<HHuginn::HNumber const*>( v1_.raw() )->value() == static_cast<HHuginn::HNumber const*>( v2_.raw() )->value();
		} else if ( typeId == HHuginn::TYPE::CHARACTER ) {
			res = static_cast<HHuginn::HCharacter const*>( v1_.raw() )->value() == static_cast<HHuginn::HCharacter const*>( v2_.raw() )->value();
		} else if ( typeId == HHuginn::TYPE::BOOLEAN ) {
			res = static_cast<HHuginn::HBoolean const*>( v1_.raw() )->value() == static_cast<HHuginn::HBoolean const*>( v2_.raw() )->value();
		} else {
			throw HHuginn::HHuginnRuntimeException( "There is no `==' operator for `"_ys.append( v1_->get_class()->name() ).append( "'." ), position_ );
		}
	} else {
		res = v1_->type_id() == v2_->type_id();
	}
	return ( res );
}

bool key_equals( HHuginn::value_t const& v1_, HHuginn::value_t const& v2_ ) {
	return ( ( v1_->type_id() == v2_->type_id() ) && equals( v1_, v2_, 0 ) );
}

bool less_low( HHuginn::value_t const& v1_, HHuginn::value_t const& v2_ ) {
	return ( less( v1_, v2_, 0 ) );
}

bool less( HHuginn::value_t const& v1_, HHuginn::value_t const& v2_, int position_ ) {
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
		res = static_cast<int unsigned>( static_cast<HHuginn::HCharacter const*>( v1_.raw() )->value() ) < static_cast<int unsigned>( static_cast<HHuginn::HCharacter const*>( v2_.raw() )->value() );
	} else {
		throw HHuginn::HHuginnRuntimeException( "There is no `<' operator for `"_ys.append( type_name( v1_->type_id() ) ).append( "'." ), position_ );
	}
	return ( res );
}

bool greater( HHuginn::value_t const& v1_, HHuginn::value_t const& v2_, int position_ ) {
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
		res = static_cast<int unsigned>( static_cast<HHuginn::HCharacter const*>( v1_.raw() )->value() ) > static_cast<int unsigned>( static_cast<HHuginn::HCharacter const*>( v2_.raw() )->value() );
	} else {
		throw HHuginn::HHuginnRuntimeException( "There is no `>' operator for `"_ys.append( type_name( v1_->type_id() ) ).append( "'." ), position_ );
	}
	return ( res );
}

bool less_or_equal( HHuginn::value_t const& v1_, HHuginn::value_t const& v2_, int position_ ) {
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
		res = static_cast<int unsigned>( static_cast<HHuginn::HCharacter const*>( v1_.raw() )->value() ) <= static_cast<int unsigned>( static_cast<HHuginn::HCharacter const*>( v2_.raw() )->value() );
	} else {
		throw HHuginn::HHuginnRuntimeException( "There is no `<=' operator for `"_ys.append( type_name( v1_->type_id() ) ).append( "'." ), position_ );
	}
	return ( res );
}

bool greater_or_equal( HHuginn::value_t const& v1_, HHuginn::value_t const& v2_, int position_ ) {
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
		res = static_cast<int unsigned>( static_cast<HHuginn::HCharacter const*>( v1_.raw() )->value() ) >= static_cast<int unsigned>( static_cast<HHuginn::HCharacter const*>( v2_.raw() )->value() );
	} else {
		throw HHuginn::HHuginnRuntimeException( "There is no `>=' operator for `"_ys.append( type_name( v1_->type_id() ) ).append( "'." ), position_ );
	}
	return ( res );
}

HHuginn::value_t boolean_xor( HThread* thread_, HHuginn::value_t const& v1_, HHuginn::value_t const& v2_, int ) {
	M_ASSERT( ( v1_->type_id() == HHuginn::TYPE::BOOLEAN ) && ( v2_->type_id() == HHuginn::TYPE::BOOLEAN ) );
	bool v1( static_cast<HHuginn::HBoolean const*>( v1_.raw() )->value() );
	bool v2( static_cast<HHuginn::HBoolean const*>( v2_.raw() )->value() );
	return ( thread_->object_factory().create_boolean( ( v1 && ! v2 ) || ( ! v1 && v2 ) ) );
}

HHuginn::value_t boolean_not( HThread* thread_, HHuginn::value_t const& v_, int ) {
	M_ASSERT( v_->type_id() == HHuginn::TYPE::BOOLEAN );
	return ( thread_->object_factory().create_boolean( ! static_cast<HHuginn::HBoolean const*>( v_.raw() )->value() ) );
}

namespace {

HHuginn::value_t fallback_conversion( HHuginn::type_id_t type_, HThread* thread_, HHuginn::value_t const& v_, int position_ ) {
	HHuginn::value_t res;
	if ( HHuginn::HObject const* o = dynamic_cast<HHuginn::HObject const*>( v_.raw() ) ) {
		res = o->call_method( thread_, v_, "to_"_ys.append( type_name( type_ ) ), HHuginn::values_t(), position_ );
	} else {
		throw HHuginn::HHuginnRuntimeException(
			"Conversion from `"_ys
			.append( v_->get_class()->name() )
			.append( "' to `" )
			.append( type_name( type_ ) )
			.append( "' is not supported." ),
			position_
		);
	}
	return ( res );
}

}

HHuginn::value_t string( HThread* thread_, HHuginn::value_t const& v_, int position_ ) {
	HHuginn::value_t res;
	HHuginn::type_id_t typeId( v_->type_id() );
	if ( typeId == HHuginn::TYPE::STRING ) {
		res = v_;
	} else if ( typeId == HHuginn::TYPE::NUMBER ) {
		res = thread_->object_factory().create_string( static_cast<HHuginn::HNumber const*>( v_.raw() )->value().to_string() );
	} else if ( typeId == HHuginn::TYPE::REAL ) {
		res = thread_->object_factory().create_string( static_cast<HHuginn::HReal const*>( v_.raw() )->value() );
	} else if ( typeId == HHuginn::TYPE::CHARACTER ) {
		res = thread_->object_factory().create_string( static_cast<HHuginn::HCharacter const*>( v_.raw() )->value() );
	} else if ( typeId == HHuginn::TYPE::INTEGER ) {
		res = thread_->object_factory().create_string( static_cast<HHuginn::HInteger const*>( v_.raw() )->value() );
	} else {
		res = fallback_conversion( type_id( HHuginn::TYPE::STRING ), thread_, v_, position_ );
	}
	return ( res );
}

HHuginn::value_t boolean( HThread* thread_, HHuginn::value_t const& v_, int position_ ) {
	HHuginn::value_t res;
	if ( v_->type_id() == HHuginn::TYPE::BOOLEAN ) {
		res = v_;
	} else {
		res = fallback_conversion( type_id( HHuginn::TYPE::BOOLEAN ), thread_, v_, position_ );
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
		res = thread_->object_factory().create_integer( static_cast<HHuginn::HNumber const*>( v_.raw() )->value().to_integer() );
	} else if ( typeId == HHuginn::TYPE::REAL ) {
		res = thread_->object_factory().create_integer( static_cast<int long long>( static_cast<HHuginn::HReal const*>( v_.raw() )->value() ) );
	} else if ( typeId == HHuginn::TYPE::CHARACTER ) {
		res = thread_->object_factory().create_integer( static_cast<int long long>( static_cast<HHuginn::HCharacter const*>( v_.raw() )->value() ) );
	} else if ( typeId == HHuginn::TYPE::INTEGER ) {
		res = v_;
	} else {
		res = fallback_conversion( type_id( HHuginn::TYPE::INTEGER ), thread_, v_, position_ );
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
		res = fallback_conversion( type_id( HHuginn::TYPE::REAL ), thread_, v_, position_ );
	}
	return ( res );
}

HHuginn::value_t character( HThread* thread_, HHuginn::value_t const& v_, int position_ ) {
	HHuginn::value_t res;
	HHuginn::type_id_t typeId( v_->type_id() );
	if ( typeId == HHuginn::TYPE::CHARACTER ) {
		res = v_;
	} else if ( typeId == HHuginn::TYPE::INTEGER ) {
		res = thread_->object_factory().create_character( static_cast<char>( static_cast<HHuginn::HCharacter const*>( v_.raw() )->value() ) );
	} else {
		res = fallback_conversion( type_id( HHuginn::TYPE::CHARACTER ), thread_, v_, position_ );
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
		res = fallback_conversion( type_id( HHuginn::TYPE::NUMBER ), thread_, v_, position_ );
	}
	return ( res );
}

}


}

}

}

