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

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace huginn {

namespace value_builtin {

HHuginn::value_t subscript( HExpression::ACCESS subscript_, HHuginn::value_t& base_, HHuginn::value_t const& index_, int position_ ) {
	HHuginn::type_t baseType( base_->type() );
	HHuginn::value_t res;
	if ( ( baseType == HHuginn::TYPE::LIST ) || ( baseType == HHuginn::TYPE::STRING ) ) {
		if ( index_->type() != HHuginn::TYPE::INTEGER ) {
			throw HHuginn::HHuginnRuntimeException( _errMsgHHuginn_[ERR_CODE::IDX_NOT_INT], position_ );
		}
		HHuginn::HInteger const* i( static_cast<HHuginn::HInteger const*>( index_.raw() ) );
		int long long index( i->value() );
		int long size(
			baseType == HHuginn::TYPE::LIST
				? static_cast<HHuginn::HList*>( base_.raw() )->size()
				: static_cast<HHuginn::HString*>( base_.raw() )->value().get_length()
		);
		if ( ( index < -size ) || ( index >= size ) ) {
			throw HHuginn::HHuginnRuntimeException( "Bad index.", position_ );
		}
		if ( index < 0 ) {
			index += size;
		}
		if ( baseType == HHuginn::TYPE::LIST ) {
			HHuginn::HList* l( static_cast<HHuginn::HList*>( base_.raw() ) );
			res = ( subscript_ == HExpression::ACCESS::VALUE ? l->get( index ) : l->get_ref( index ) );
		} else {
			M_ASSERT( baseType == HHuginn::TYPE::STRING );
			HHuginn::HString* s( static_cast<HHuginn::HString*>( base_.raw() ) );
			res = make_pointer<HHuginn::HHuginn::HCharacter>( s->value()[static_cast<int>( index )] );
		}
	} else if ( baseType == HHuginn::TYPE::MAP ) {
		HHuginn::HMap* m( static_cast<HHuginn::HMap*>( base_.raw() ) );
		res = ( subscript_ == HExpression::ACCESS::VALUE ? m->get( index_, position_ ) : m->get_ref( index_, position_ ) );
	} else {
		throw HHuginn::HHuginnRuntimeException( "Subscript is not supported on `"_ys.append( baseType->name() ).append( "'." ), position_ );
	}
	return ( res );
}

HHuginn::value_t range(
	HHuginn::value_t& base_,
	HHuginn::value_t const& from_,
	HHuginn::value_t const& to_,
	HHuginn::value_t const& step_,
	int position_
) {
	HHuginn::type_t baseType( base_->type() );
	HHuginn::value_t res;
	if ( ( baseType == HHuginn::TYPE::LIST ) || ( baseType == HHuginn::TYPE::STRING ) ) {
		if ( ( from_->type() != HHuginn::TYPE::INTEGER ) && ( from_->type() != HHuginn::TYPE::NONE ) ) {
			throw HHuginn::HHuginnRuntimeException( "Range operand `from' is not an integer.", position_ );
		}
		if ( ( to_->type() != HHuginn::TYPE::INTEGER ) && ( to_->type() != HHuginn::TYPE::NONE ) ) {
			throw HHuginn::HHuginnRuntimeException( "Range operand `to' is not an integer.", position_ );
		}
		if ( ( step_->type() != HHuginn::TYPE::INTEGER ) && ( step_->type() != HHuginn::TYPE::NONE ) ) {
			throw HHuginn::HHuginnRuntimeException( "Range operand `step' is not an integer.", position_ );
		}
		int long size( baseType == HHuginn::TYPE::LIST ? static_cast<HHuginn::HList*>( base_.raw() )->size() : static_cast<HHuginn::HString*>( base_.raw() )->value().get_length() );
		HHuginn::HInteger const* integer( step_->type() == HHuginn::TYPE::INTEGER ? static_cast<HHuginn::HInteger const*>( step_.raw() ) : nullptr );
		int long step( integer ? static_cast<int long>( integer->value() ) : 1 );
		if ( step == 0 ) {
			throw HHuginn::HHuginnRuntimeException( "Range step cannot be zero.", position_ );
		}
		HHuginn::HInteger const* integerFrom = from_->type() == HHuginn::TYPE::INTEGER ? static_cast<HHuginn::HInteger const*>( from_.raw() ) : nullptr;
		HHuginn::HInteger const* integerTo = to_->type() == HHuginn::TYPE::INTEGER ? static_cast<HHuginn::HInteger const*>( to_.raw() ) : nullptr;
		int long from( integerFrom ? static_cast<int long>( integerFrom->value() ) : ( step > 0 ? 0 : size ) );
		int long to( integerTo ? static_cast<int long>( integerTo->value() ) : ( step > 0 ? size : -1 ) );
		res = ( baseType == HHuginn::TYPE::LIST ) ? pointer_static_cast<HHuginn::HValue>( make_pointer<HHuginn::HList>() ) : pointer_static_cast<HHuginn::HValue>( make_pointer<HHuginn::HString>( "" ) );

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
			} else {
				M_ASSERT( baseType == HHuginn::TYPE::STRING );
				HHuginn::HString* s( static_cast<HHuginn::HString*>( base_.raw() ) );
				HHuginn::HString* r( static_cast<HHuginn::HString*>( res.raw() ) );
				if ( step > 0 ) {
					for ( int long i( from ); i < to; i += step ) {
						r->value().push_back( s->value()[ i ] );
					}
				} else {
					for ( int long i( from ); i > to; i += step ) {
						r->value().push_back( s->value()[ i ] );
					}
				}
			}
		} while ( false );
	} else {
		throw HHuginn::HHuginnRuntimeException( "Range operator is not supported on `"_ys.append( baseType->name() ).append( "'." ), position_ );
	}
	return ( res );
}

HHuginn::value_t add( HHuginn::value_t const& v1_, HHuginn::value_t const& v2_, int position_ ) {
	M_ASSERT( v1_->type() == v2_->type() );
	HHuginn::value_t res;
	HHuginn::type_t typeId( v1_->type() );
	if ( typeId == HHuginn::TYPE::INTEGER ) {
		res = make_pointer<HHuginn::HInteger>( static_cast<HHuginn::HInteger const*>( v1_.raw() )->value() + static_cast<HHuginn::HInteger const*>( v2_.raw() )->value() );
	} else if ( typeId == HHuginn::TYPE::REAL ) {
		res = make_pointer<HHuginn::HReal>( static_cast<HHuginn::HReal const*>( v1_.raw() )->value() + static_cast<HHuginn::HReal const*>( v2_.raw() )->value() );
	} else if ( typeId == HHuginn::TYPE::STRING ) {
		res = make_pointer<HHuginn::HString>( static_cast<HHuginn::HString const*>( v1_.raw() )->value() + static_cast<HHuginn::HString const*>( v2_.raw() )->value() );
	} else if ( typeId == HHuginn::TYPE::NUMBER ) {
		res = make_pointer<HHuginn::HNumber>( static_cast<HHuginn::HNumber const*>( v1_.raw() )->value() + static_cast<HHuginn::HNumber const*>( v2_.raw() )->value() );
	} else {
		throw HHuginn::HHuginnRuntimeException( "There is no `+' operator for `"_ys.append( v1_->type()->name() ).append( "'." ), position_ );
	}
	return ( res );
}

HHuginn::value_t sub( HHuginn::value_t const& v1_, HHuginn::value_t const& v2_, int position_ ) {
	M_ASSERT( v1_->type() == v2_->type() );
	HHuginn::value_t res;
	HHuginn::type_t typeId( v1_->type() );
	if ( typeId == HHuginn::TYPE::INTEGER ) {
		res = make_pointer<HHuginn::HInteger>( static_cast<HHuginn::HInteger const*>( v1_.raw() )->value() - static_cast<HHuginn::HInteger const*>( v2_.raw() )->value() );
	} else if ( typeId == HHuginn::TYPE::REAL ) {
		res = make_pointer<HHuginn::HReal>( static_cast<HHuginn::HReal const*>( v1_.raw() )->value() - static_cast<HHuginn::HReal const*>( v2_.raw() )->value() );
	} else if ( typeId == HHuginn::TYPE::NUMBER ) {
		res = make_pointer<HHuginn::HNumber>( static_cast<HHuginn::HNumber const*>( v1_.raw() )->value() - static_cast<HHuginn::HNumber const*>( v2_.raw() )->value() );
	} else {
		throw HHuginn::HHuginnRuntimeException( "There is no `-' operator for `"_ys.append( v1_->type()->name() ).append( "'." ), position_ );
	}
	return ( res );
}

HHuginn::value_t mul( HHuginn::value_t const& v1_, HHuginn::value_t const& v2_, int position_ ) {
	M_ASSERT( v1_->type() == v2_->type() );
	HHuginn::value_t res;
	HHuginn::type_t typeId( v1_->type() );
	if ( typeId == HHuginn::TYPE::INTEGER ) {
		res = make_pointer<HHuginn::HInteger>( static_cast<HHuginn::HInteger const*>( v1_.raw() )->value() * static_cast<HHuginn::HInteger const*>( v2_.raw() )->value() );
	} else if ( typeId == HHuginn::TYPE::REAL ) {
		res = make_pointer<HHuginn::HReal>( static_cast<HHuginn::HReal const*>( v1_.raw() )->value() * static_cast<HHuginn::HReal const*>( v2_.raw() )->value() );
	} else if ( typeId == HHuginn::TYPE::NUMBER ) {
		res = make_pointer<HHuginn::HNumber>( static_cast<HHuginn::HNumber const*>( v1_.raw() )->value() * static_cast<HHuginn::HNumber const*>( v2_.raw() )->value() );
	} else {
		throw HHuginn::HHuginnRuntimeException( "There is no `*' operator for `"_ys.append( v1_->type()->name() ).append( "'." ), position_ );
	}
	return ( res );
}

HHuginn::value_t div( HHuginn::value_t const& v1_, HHuginn::value_t const& v2_, int position_ ) {
	M_ASSERT( v1_->type() == v2_->type() );
	HHuginn::value_t res;
	HHuginn::type_t typeId( v1_->type() );
	if ( typeId == HHuginn::TYPE::INTEGER ) {
		res = make_pointer<HHuginn::HInteger>( static_cast<HHuginn::HInteger const*>( v1_.raw() )->value() / static_cast<HHuginn::HInteger const*>( v2_.raw() )->value() );
	} else if ( typeId == HHuginn::TYPE::REAL ) {
		res = make_pointer<HHuginn::HReal>( static_cast<HHuginn::HReal const*>( v1_.raw() )->value() / static_cast<HHuginn::HReal const*>( v2_.raw() )->value() );
	} else if ( typeId == HHuginn::TYPE::NUMBER ) {
		res = make_pointer<HHuginn::HNumber>( static_cast<HHuginn::HNumber const*>( v1_.raw() )->value() / static_cast<HHuginn::HNumber const*>( v2_.raw() )->value() );
	} else {
		throw HHuginn::HHuginnRuntimeException( "There is no `/' operator for `"_ys.append( v1_->type()->name() ).append( "'." ), position_ );
	}
	return ( res );
}

HHuginn::value_t mod( HHuginn::value_t const& v1_, HHuginn::value_t const& v2_, int position_ ) {
	M_ASSERT( v1_->type() == v2_->type() );
	HHuginn::value_t res;
	if ( v1_->type() == HHuginn::TYPE::INTEGER ) {
		res = make_pointer<HHuginn::HInteger>( static_cast<HHuginn::HInteger const*>( v1_.raw() )->value() % static_cast<HHuginn::HInteger const*>( v2_.raw() )->value() );
	} else {
		throw HHuginn::HHuginnRuntimeException( "There is no `%' operator for `"_ys.append( v1_->type()->name() ).append( "'." ), position_ );
	}
	return ( res );
}

HHuginn::value_t pow( HHuginn::value_t const& v1_, HHuginn::value_t const& v2_, int position_ ) {
	M_ASSERT( v1_->type() == v2_->type() );
	HHuginn::value_t res;
	HHuginn::type_t typeId( v1_->type() );
	if ( typeId == HHuginn::TYPE::REAL ) {
		res = make_pointer<HHuginn::HReal>( ::powl( static_cast<HHuginn::HReal const*>( v1_.raw() )->value(), static_cast<HHuginn::HReal const*>( v2_.raw() )->value() ) );
	} else if ( typeId == HHuginn::TYPE::NUMBER ) {
		res = make_pointer<HHuginn::HNumber>( static_cast<HHuginn::HNumber const*>( v1_.raw() )->value() ^ static_cast<HHuginn::HNumber const*>( v2_.raw() )->value().to_integer() );
	} else {
		throw HHuginn::HHuginnRuntimeException( "There is no `^' operator for `"_ys.append( v1_->type()->name() ).append( "'." ), position_ );
	}
	return ( res );
}

HHuginn::value_t abs( HHuginn::value_t const& v_, int position_ ) {
	HHuginn::value_t res;
	HHuginn::type_t typeId( v_->type() );
	if ( typeId == HHuginn::TYPE::INTEGER ) {
		int long long v( static_cast<HHuginn::HInteger const*>( v_.raw() )->value() );
		if ( v >= 0 ) {
			res = v_;
		} else {
			res = make_pointer<HHuginn::HInteger>( -v );
		}
	} else if ( typeId == HHuginn::TYPE::REAL ) {
		double long v( static_cast<HHuginn::HReal const*>( v_.raw() )->value() );
		if ( v >= 0 ) {
			res = v_;
		} else {
			res = make_pointer<HHuginn::HReal>( -v );
		}
	} else if ( typeId == HHuginn::TYPE::NUMBER ) {
		yaal::hcore::HNumber const& v( static_cast<HHuginn::HNumber const*>( v_.raw() )->value() );
		if ( v >= 0 ) {
			res = v_;
		} else {
			res = make_pointer<HHuginn::HNumber>( -v );
		}
	} else {
		throw HHuginn::HHuginnRuntimeException( "There is no |.| operator for `"_ys.append( v_->type()->name() ).append( "'." ), position_ );
	}
	return ( res );
}

HHuginn::value_t neg( HHuginn::value_t const& v_, int position_ ) {
	HHuginn::value_t res;
	HHuginn::type_t typeId( v_->type() );
	if ( typeId == HHuginn::TYPE::INTEGER ) {
		res = make_pointer<HHuginn::HInteger>( -static_cast<HHuginn::HInteger const*>( v_.raw() )->value() );
	} else if ( typeId == HHuginn::TYPE::REAL ) {
		res = make_pointer<HHuginn::HReal>( -static_cast<HHuginn::HReal const*>( v_.raw() )->value() );
	} else if ( typeId == HHuginn::TYPE::NUMBER ) {
		res = make_pointer<HHuginn::HNumber>( -static_cast<HHuginn::HNumber const*>( v_.raw() )->value() );
	} else {
		throw HHuginn::HHuginnRuntimeException( "There is no `negate` operator for `"_ys.append( v_->type()->name() ).append( "'." ), position_ );
	}
	return ( res );
}

bool equals( HHuginn::value_t const& v1_, HHuginn::value_t const& v2_, int position_ ) {
	bool noneOperand( ( v1_->type() == HHuginn::TYPE::NONE ) || ( v2_->type() == HHuginn::TYPE::NONE ) );
	M_ASSERT( noneOperand || ( v1_->type() == v2_->type() ) );
	bool res( false );
	if ( ! noneOperand ) {
		HHuginn::type_t typeId( v1_->type() );
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
			throw HHuginn::HHuginnRuntimeException( "There is no `==' operator for `"_ys.append( v1_->type()->name() ).append( "'." ), position_ );
		}
	} else {
		res = v1_->type() == v2_->type();
	}
	return ( res );
}

bool less_low( HHuginn::value_t const& v1_, HHuginn::value_t const& v2_ ) {
	return ( less( v1_, v2_, 0 ) );
}

bool less( HHuginn::value_t const& v1_, HHuginn::value_t const& v2_, int position_ ) {
	M_ASSERT( v1_->type() == v2_->type() );
	bool res( false );
	HHuginn::type_t typeId( v1_->type() );
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
		throw HHuginn::HHuginnRuntimeException( "There is no `<' operator for `"_ys.append( type_name( v1_->type() ) ).append( "'." ), position_ );
	}
	return ( res );
}

bool greater( HHuginn::value_t const& v1_, HHuginn::value_t const& v2_, int position_ ) {
	M_ASSERT( v1_->type() == v2_->type() );
	bool res( false );
	HHuginn::type_t typeId( v1_->type() );
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
		throw HHuginn::HHuginnRuntimeException( "There is no `>' operator for `"_ys.append( type_name( v1_->type() ) ).append( "'." ), position_ );
	}
	return ( res );
}

bool less_or_equal( HHuginn::value_t const& v1_, HHuginn::value_t const& v2_, int position_ ) {
	M_ASSERT( v1_->type() == v2_->type() );
	bool res( false );
	HHuginn::type_t typeId( v1_->type() );
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
		throw HHuginn::HHuginnRuntimeException( "There is no `<=' operator for `"_ys.append( type_name( v1_->type() ) ).append( "'." ), position_ );
	}
	return ( res );
}

bool greater_or_equal( HHuginn::value_t const& v1_, HHuginn::value_t const& v2_, int position_ ) {
	M_ASSERT( v1_->type() == v2_->type() );
	bool res( false );
	HHuginn::type_t typeId( v1_->type() );
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
		throw HHuginn::HHuginnRuntimeException( "There is no `>=' operator for `"_ys.append( type_name( v1_->type() ) ).append( "'." ), position_ );
	}
	return ( res );
}

HHuginn::value_t boolean_xor( HHuginn::value_t const& v1_, HHuginn::value_t const& v2_, int ) {
	M_ASSERT( ( v1_->type() == HHuginn::TYPE::BOOLEAN ) && ( v2_->type() == HHuginn::TYPE::BOOLEAN ) );
	bool v1( static_cast<HHuginn::HBoolean const*>( v1_.raw() )->value() );
	bool v2( static_cast<HHuginn::HBoolean const*>( v2_.raw() )->value() );
	return ( make_pointer<HHuginn::HBoolean>( ( v1 && ! v2 ) || ( ! v1 && v2 ) ) );
}

HHuginn::value_t boolean_not( HHuginn::value_t const& v_, int ) {
	M_ASSERT( v_->type() == HHuginn::TYPE::BOOLEAN );
	return ( make_pointer<HHuginn::HBoolean>( ! static_cast<HHuginn::HBoolean const*>( v_.raw() )->value() ) );
}

HHuginn::value_t string( HHuginn::value_t const& v_, int position_ ) {
	HHuginn::value_t res;
	HHuginn::type_t typeId( v_->type() );
	if ( typeId == HHuginn::TYPE::STRING ) {
		res = v_;
	} else if ( typeId == HHuginn::TYPE::NUMBER ) {
		res = make_pointer<HHuginn::HString>( static_cast<HHuginn::HNumber const*>( v_.raw() )->value().to_string() );
	} else if ( typeId == HHuginn::TYPE::REAL ) {
		res = make_pointer<HHuginn::HString>( static_cast<HHuginn::HReal const*>( v_.raw() )->value() );
	} else if ( typeId == HHuginn::TYPE::CHARACTER ) {
		res = make_pointer<HHuginn::HString>( static_cast<HHuginn::HCharacter const*>( v_.raw() )->value() );
	} else if ( typeId == HHuginn::TYPE::INTEGER ) {
		res = make_pointer<HHuginn::HString>( static_cast<HHuginn::HInteger const*>( v_.raw() )->value() );
	} else {
		throw HHuginn::HHuginnRuntimeException( "Conversion from `"_ys
			.append( v_->type()->name() )
			.append( "' to `string' is not supported." ),
			position_
		);
	}
	return ( res );
}

HHuginn::value_t boolean( HHuginn::value_t const& v_, int position_ ) {
	HHuginn::value_t res;
	if ( v_->type() == HHuginn::TYPE::BOOLEAN ) {
		res = v_;
	} else {
		throw HHuginn::HHuginnRuntimeException( "Conversion from `"_ys
			.append( v_->type()->name() )
			.append( "' to `boolean' is not supported." ),
			position_
		);
	}
	return ( res );
}

HHuginn::value_t integer( HHuginn::value_t const& v_, int position_ ) {
	HHuginn::value_t res;
	HHuginn::type_t typeId( v_->type() );
	if ( typeId == HHuginn::TYPE::STRING ) {
		res = make_pointer<HHuginn::HInteger>( lexical_cast<int long long>( static_cast<HHuginn::HString const*>( v_.raw() )->value() ) );
	} else if ( typeId == HHuginn::TYPE::NUMBER ) {
		res = make_pointer<HHuginn::HInteger>( static_cast<HHuginn::HNumber const*>( v_.raw() )->value().to_integer() );
	} else if ( typeId == HHuginn::TYPE::REAL ) {
		res = make_pointer<HHuginn::HInteger>( static_cast<int long long>( static_cast<HHuginn::HReal const*>( v_.raw() )->value() ) );
	} else if ( typeId == HHuginn::TYPE::CHARACTER ) {
		res = make_pointer<HHuginn::HInteger>( static_cast<int long long>( static_cast<HHuginn::HCharacter const*>( v_.raw() )->value() ) );
	} else if ( typeId == HHuginn::TYPE::INTEGER ) {
		res = v_;
	} else {
		throw HHuginn::HHuginnRuntimeException( "Conversion from `"_ys
			.append( type_name( v_->type() ) )
			.append( "' to `integer' is not supported." ),
			position_
		);
	}
	return ( res );
}

HHuginn::value_t real( HHuginn::value_t const& v_, int position_ ) {
	HHuginn::value_t res;
	HHuginn::type_t typeId( v_->type() );
	if ( typeId == HHuginn::TYPE::STRING ) {
		res = make_pointer<HHuginn::HReal>( lexical_cast<double long>( static_cast<HHuginn::HString const*>( v_.raw() )->value() ) );
	} else if ( typeId == HHuginn::TYPE::NUMBER ) {
		res = make_pointer<HHuginn::HReal>( static_cast<HHuginn::HNumber const*>( v_.raw() )->value().to_floating_point() );
	} else if ( typeId == HHuginn::TYPE::REAL ) {
		res = v_;
	} else if ( typeId == HHuginn::TYPE::INTEGER ) {
		res = make_pointer<HHuginn::HReal>( static_cast<double long>( static_cast<HHuginn::HInteger const*>( v_.raw() )->value() ) );
	} else {
		throw HHuginn::HHuginnRuntimeException( "Conversion from `"_ys
			.append( type_name( v_->type() ) )
			.append( "' to `real' is not supported." ),
			position_
		);
	}
	return ( res );
}

HHuginn::value_t character( HHuginn::value_t const& v_, int position_ ) {
	HHuginn::value_t res;
	HHuginn::type_t typeId( v_->type() );
	if ( typeId == HHuginn::TYPE::CHARACTER ) {
		res = v_;
	} else if ( typeId == HHuginn::TYPE::INTEGER ) {
		res = make_pointer<HHuginn::HCharacter>( static_cast<char>( static_cast<HHuginn::HCharacter const*>( v_.raw() )->value() ) );
	} else {
		throw HHuginn::HHuginnRuntimeException( "Conversion from `"_ys
			.append( type_name( v_->type() ) )
			.append( "' to `character' is not supported." ),
			position_
		);
	}
	return ( res );
}

HHuginn::value_t number( HHuginn::value_t const& v_, int position_ ) {
	HHuginn::value_t res;
	HHuginn::type_t typeId( v_->type() );
	if ( typeId == HHuginn::TYPE::STRING ) {
		res = make_pointer<HHuginn::HNumber>( static_cast<HHuginn::HString const*>( v_.raw() )->value() );
	} else if ( typeId == HHuginn::TYPE::NUMBER ) {
		res = v_;
	} else if ( typeId == HHuginn::TYPE::REAL ) {
		res = make_pointer<HHuginn::HNumber>( static_cast<HHuginn::HReal const*>( v_.raw() )->value() );
	} else if ( typeId == HHuginn::TYPE::INTEGER ) {
		res = make_pointer<HHuginn::HNumber>( static_cast<HHuginn::HInteger const*>( v_.raw() )->value() );
	} else {
		throw HHuginn::HHuginnRuntimeException( "Conversion from `"_ys
			.append( type_name( v_->type() ) )
			.append( "' to `number' is not supported." ),
			position_
		);
	}
	return ( res );
}

}


}

}

}

