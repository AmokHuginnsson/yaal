/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cmath>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "expression.hxx"
#include "runtime.hxx"
#include "frame.hxx"
#include "thread.hxx"
#include "instruction.hxx"
#include "booleanevaluator.hxx"
#include "ternaryevaluator.hxx"
#include "helper.hxx"
#include "keyword.hxx"
#include "objectfactory.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace huginn {

template<OPERATOR>
struct operator_dispatcher {
template<typename T>
static void self( T&, T const& );
};

template<>
template<typename T>
void operator_dispatcher<OPERATOR::PLUS_ASSIGN>::self( T& self_, T const& other_ ) {
	self_ += other_;
}

template<>
template<typename T>
void operator_dispatcher<OPERATOR::MINUS_ASSIGN>::self( T& self_, T const& other_ ) {
	self_ -= other_;
}

template<>
template<typename T>
void operator_dispatcher<OPERATOR::MULTIPLY_ASSIGN>::self( T& self_, T const& other_ ) {
	self_ *= other_;
}

template<>
template<typename T>
void operator_dispatcher<OPERATOR::DIVIDE_ASSIGN>::self( T& self_, T const& other_ ) {
	self_ /= other_;
}

template<>
template<typename T>
void operator_dispatcher<OPERATOR::MODULO_ASSIGN>::self( T& self_, T const& other_ ) {
	self_ %= other_;
}

template<>
template<>
void operator_dispatcher<OPERATOR::MODULO_ASSIGN>::self( double long& self_, double long const& other_ ) {
	self_ = fmodl( self_, other_ );
}

HExpression::OExecutionStep::OExecutionStep( void )
	: _expression( nullptr )
	, _action( nullptr )
	, _position( INVALID_POSITION )
	, _access( HFrame::ACCESS::VALUE )
	, _index( -1 )
	, _identifierId( IDENTIFIER::INVALID )
	, _value()
	, _valueRef()
	, _real( 0.0L )
	, _integer( 0 )
	, _string()
	, _number()
	, _character( 0 )
	, _literalType( HHuginn::TYPE::UNKNOWN )
	, _unboundIndexes() {
}

HExpression::OExecutionStep::OExecutionStep( HExpression* expression_, action_t action_, int position_ )
	: _expression( expression_ )
	, _action( action_ )
	, _position( position_ )
	, _access( HFrame::ACCESS::VALUE )
	, _index( -1 )
	, _identifierId( IDENTIFIER::INVALID )
	, _value()
	, _valueRef()
	, _real( 0.0L )
	, _integer( 0 )
	, _string()
	, _number()
	, _character( 0 )
	, _literalType( HHuginn::TYPE::UNKNOWN )
	, _unboundIndexes() {
}

HExpression::OExecutionStep::OExecutionStep( HExpression* expression_, action_t action_, int position_, HFrame::ACCESS access_ )
	: _expression( expression_ )
	, _action( action_ )
	, _position( position_ )
	, _access( access_ )
	, _index( -1 )
	, _identifierId( IDENTIFIER::INVALID )
	, _value()
	, _valueRef()
	, _real( 0.0L )
	, _integer( 0 )
	, _string()
	, _number()
	, _character( 0 )
	, _literalType( HHuginn::TYPE::UNKNOWN )
	, _unboundIndexes() {
}

HExpression::OExecutionStep::OExecutionStep( HExpression* expression_, action_t action_, int position_, HFrame::ACCESS access_, int index_ )
	: _expression( expression_ )
	, _action( action_ )
	, _position( position_ )
	, _access( access_ )
	, _index( index_ )
	, _identifierId( IDENTIFIER::INVALID )
	, _value()
	, _valueRef()
	, _real( 0.0L )
	, _integer( 0 )
	, _string()
	, _number()
	, _character( 0 )
	, _literalType( HHuginn::TYPE::UNKNOWN )
	, _unboundIndexes() {
}

HExpression::OExecutionStep::OExecutionStep( HExpression* expression_, action_t action_, int position_, HFrame::ACCESS access_, HHuginn::identifier_id_t identifierId_ )
	: _expression( expression_ )
	, _action( action_ )
	, _position( position_ )
	, _access( access_ )
	, _index( -1 )
	, _identifierId( identifierId_ )
	, _value()
	, _valueRef()
	, _real( 0.0L )
	, _integer( 0 )
	, _string()
	, _number()
	, _character( 0 )
	, _literalType( HHuginn::TYPE::UNKNOWN )
	, _unboundIndexes() {
}

HExpression::OExecutionStep::OExecutionStep( HExpression* expression_, action_t action_, int position_, HFrame::ACCESS access_, int index_, HHuginn::identifier_id_t identifierId_ )
	: _expression( expression_ )
	, _action( action_ )
	, _position( position_ )
	, _access( access_ )
	, _index( index_ )
	, _identifierId( identifierId_ )
	, _value()
	, _valueRef()
	, _real( 0.0L )
	, _integer( 0 )
	, _string()
	, _number()
	, _character( 0 )
	, _literalType( HHuginn::TYPE::UNKNOWN )
	, _unboundIndexes() {
}

HExpression::OExecutionStep::OExecutionStep( HExpression* expression_, action_t action_, int position_, HHuginn::value_t const* valueRef_ )
	: _expression( expression_ )
	, _action( action_ )
	, _position( position_ )
	, _access( HFrame::ACCESS::VALUE )
	, _index( -1 )
	, _identifierId( IDENTIFIER::INVALID )
	, _value()
	, _valueRef( valueRef_ )
	, _real( 0.0L )
	, _integer( 0 )
	, _string()
	, _number()
	, _character( 0 )
	, _literalType( HHuginn::TYPE::UNKNOWN )
	, _unboundIndexes() {
}

HExpression::OExecutionStep::OExecutionStep( HExpression* expression_, action_t action_, int position_, HHuginn::value_t const& value_, HHuginn::TYPE type_ )
	: _expression( expression_ )
	, _action( action_ )
	, _position( position_ )
	, _access( HFrame::ACCESS::VALUE )
	, _index( -1 )
	, _identifierId( IDENTIFIER::INVALID )
	, _value( value_ )
	, _valueRef()
	, _real( 0.0L )
	, _integer( 0 )
	, _string()
	, _number()
	, _character( 0 )
	, _literalType( type_ )
	, _unboundIndexes() {
}

HExpression::OExecutionStep::OExecutionStep( HExpression* expression_, action_t action_, int position_, double long doubleLong_ )
	: _expression( expression_ )
	, _action( action_ )
	, _position( position_ )
	, _access( HFrame::ACCESS::VALUE )
	, _index( -1 )
	, _identifierId( IDENTIFIER::INVALID )
	, _value()
	, _valueRef()
	, _real( doubleLong_ )
	, _integer( 0 )
	, _string()
	, _number()
	, _character( 0 )
	, _literalType( HHuginn::TYPE::REAL )
	, _unboundIndexes() {
}

HExpression::OExecutionStep::OExecutionStep( HExpression* expression_, action_t action_, int position_, int long long integer_ )
	: _expression( expression_ )
	, _action( action_ )
	, _position( position_ )
	, _access( HFrame::ACCESS::VALUE )
	, _index( -1 )
	, _identifierId( IDENTIFIER::INVALID )
	, _value()
	, _valueRef()
	, _real( 0.0L )
	, _integer( integer_ )
	, _string()
	, _number()
	, _character( 0 )
	, _literalType( HHuginn::TYPE::INTEGER )
	, _unboundIndexes() {
}

HExpression::OExecutionStep::OExecutionStep( HExpression* expression_, action_t action_, int position_, yaal::hcore::HString const& string_ )
	: _expression( expression_ )
	, _action( action_ )
	, _position( position_ )
	, _access( HFrame::ACCESS::VALUE )
	, _index( -1 )
	, _identifierId( IDENTIFIER::INVALID )
	, _value()
	, _valueRef()
	, _real( 0.0L )
	, _integer( 0 )
	, _string( string_ )
	, _number()
	, _character( 0 )
	, _literalType( HHuginn::TYPE::STRING )
	, _unboundIndexes() {
}

HExpression::OExecutionStep::OExecutionStep( HExpression* expression_, action_t action_, int position_, yaal::hcore::HNumber const& number_ )
	: _expression( expression_ )
	, _action( action_ )
	, _position( position_ )
	, _access( HFrame::ACCESS::VALUE )
	, _index( -1 )
	, _identifierId( IDENTIFIER::INVALID )
	, _value()
	, _valueRef()
	, _real( 0.0L )
	, _integer( 0 )
	, _string()
	, _number( number_ )
	, _character( 0 )
	, _literalType( HHuginn::TYPE::STRING )
	, _unboundIndexes() {
}

HExpression::OExecutionStep::OExecutionStep( HExpression* expression_, action_t action_, int position_, code_point_t character_ )
	: _expression( expression_ )
	, _action( action_ )
	, _position( position_ )
	, _access( HFrame::ACCESS::VALUE )
	, _index( -1 )
	, _identifierId( IDENTIFIER::INVALID )
	, _value()
	, _valueRef()
	, _real( 0.0L )
	, _integer( 0 )
	, _string()
	, _number()
	, _character( character_ )
	, _literalType( HHuginn::TYPE::CHARACTER )
	, _unboundIndexes() {
}

HExpression::OExecutionStep::OExecutionStep(
	HExpression* expression_, action_t action_, int position_, HPartial::unbound_indexes_t const& unboundIndexes_, int argumentCount_
) : _expression( expression_ )
	, _action( action_ )
	, _position( position_ )
	, _access( HFrame::ACCESS::VALUE )
	, _index( -1 )
	, _identifierId( IDENTIFIER::INVALID )
	, _value()
	, _valueRef()
	, _real( 0.0L )
	, _integer( argumentCount_ )
	, _string()
	, _number()
	, _character( 0 )
	, _literalType( HHuginn::TYPE::UNKNOWN )
	, _unboundIndexes( unboundIndexes_ ) {
}

HExpression::OExecutionStep& HExpression::OExecutionStep::operator += ( OExecutionStep const& other_ ) {
	M_PROLOG
	M_ASSERT( ( _literalType == other_._literalType ) && ( _literalType != HHuginn::TYPE::UNKNOWN ) );
	if ( !! _value ) {
		if ( !! other_._value ) {
			switch ( _literalType ) {
				case ( HHuginn::TYPE::INTEGER ): { static_cast<HInteger*>( _value.raw() )->value() += static_cast<HInteger const*>( other_._value.raw() )->value(); } break;
				case ( HHuginn::TYPE::REAL ):    { static_cast<HReal*>( _value.raw() )->value()    += static_cast<HReal const*>( other_._value.raw() )->value();    } break;
				case ( HHuginn::TYPE::NUMBER ):  { static_cast<HNumber*>( _value.raw() )->value()  += static_cast<HNumber const*>( other_._value.raw() )->value();  } break;
				case ( HHuginn::TYPE::STRING ):  { static_cast<HString*>( _value.raw() )->value()  += static_cast<HString const*>( other_._value.raw() )->value();  } break;
				default: {}
			}
		} else {
			switch ( _literalType ) {
				case ( HHuginn::TYPE::INTEGER ): { static_cast<HInteger*>( _value.raw() )->value() += other_._integer; } break;
				case ( HHuginn::TYPE::REAL ):    { static_cast<HReal*>( _value.raw() )->value()    += other_._real;    } break;
				case ( HHuginn::TYPE::NUMBER ):  { static_cast<HNumber*>( _value.raw() )->value()  += other_._number;  } break;
				case ( HHuginn::TYPE::STRING ):  { static_cast<HString*>( _value.raw() )->value()  += other_._string;  } break;
				default: {}
			}
		}
	} else {
		if ( !! other_._value ) {
			switch ( _literalType ) {
				case ( HHuginn::TYPE::INTEGER ): { _integer += static_cast<HInteger const*>( other_._value.raw() )->value(); } break;
				case ( HHuginn::TYPE::REAL ):    { _real    += static_cast<HReal const*>( other_._value.raw() )->value();    } break;
				case ( HHuginn::TYPE::NUMBER ):  { _number  += static_cast<HNumber const*>( other_._value.raw() )->value();  } break;
				case ( HHuginn::TYPE::STRING ):  { _string  += static_cast<HString const*>( other_._value.raw() )->value();  } break;
				default: {}
			}
		} else {
			switch ( _literalType ) {
				case ( HHuginn::TYPE::INTEGER ): { _integer += other_._integer; } break;
				case ( HHuginn::TYPE::REAL ):    { _real    += other_._real;    } break;
				case ( HHuginn::TYPE::NUMBER ):  { _number  += other_._number;  } break;
				case ( HHuginn::TYPE::STRING ):  { _string  += other_._string;  } break;
				default: {}
			}
		}
	}
	return ( *this );
	M_EPILOG
}

HExpression::OExecutionStep& HExpression::OExecutionStep::operator -= ( OExecutionStep const& other_ ) {
	M_PROLOG
	M_ASSERT( ( _literalType == other_._literalType ) && ( _literalType != HHuginn::TYPE::UNKNOWN ) );
	if ( !! _value ) {
		if ( !! other_._value ) {
			switch ( _literalType ) {
				case ( HHuginn::TYPE::INTEGER ): { static_cast<HInteger*>( _value.raw() )->value() -= static_cast<HInteger const*>( other_._value.raw() )->value(); } break;
				case ( HHuginn::TYPE::REAL ):    { static_cast<HReal*>( _value.raw() )->value()    -= static_cast<HReal const*>( other_._value.raw() )->value();    } break;
				case ( HHuginn::TYPE::NUMBER ):  { static_cast<HNumber*>( _value.raw() )->value()  -= static_cast<HNumber const*>( other_._value.raw() )->value();  } break;
				default: {}
			}
		} else {
			switch ( _literalType ) {
				case ( HHuginn::TYPE::INTEGER ): { static_cast<HInteger*>( _value.raw() )->value() -= other_._integer; } break;
				case ( HHuginn::TYPE::REAL ):    { static_cast<HReal*>( _value.raw() )->value()    -= other_._real;    } break;
				case ( HHuginn::TYPE::NUMBER ):  { static_cast<HNumber*>( _value.raw() )->value()  -= other_._number;  } break;
				default: {}
			}
		}
	} else {
		if ( !! other_._value ) {
			switch ( _literalType ) {
				case ( HHuginn::TYPE::INTEGER ): { _integer -= static_cast<HInteger const*>( other_._value.raw() )->value(); } break;
				case ( HHuginn::TYPE::REAL ):    { _real    -= static_cast<HReal const*>( other_._value.raw() )->value();    } break;
				case ( HHuginn::TYPE::NUMBER ):  { _number  -= static_cast<HNumber const*>( other_._value.raw() )->value();  } break;
				default: {}
			}
		} else {
			switch ( _literalType ) {
				case ( HHuginn::TYPE::INTEGER ): { _integer -= other_._integer; } break;
				case ( HHuginn::TYPE::REAL ):    { _real    -= other_._real;    } break;
				case ( HHuginn::TYPE::NUMBER ):  { _number  -= other_._number;  } break;
				default: {}
			}
		}
	}
	return ( *this );
	M_EPILOG
}

HExpression::OExecutionStep& HExpression::OExecutionStep::operator *= ( OExecutionStep const& other_ ) {
	M_PROLOG
	M_ASSERT( ( _literalType == other_._literalType ) && ( _literalType != HHuginn::TYPE::UNKNOWN ) );
	if ( !! _value ) {
		if ( !! other_._value ) {
			switch ( _literalType ) {
				case ( HHuginn::TYPE::INTEGER ): { static_cast<HInteger*>( _value.raw() )->value() *= static_cast<HInteger const*>( other_._value.raw() )->value(); } break;
				case ( HHuginn::TYPE::REAL ):    { static_cast<HReal*>( _value.raw() )->value()    *= static_cast<HReal const*>( other_._value.raw() )->value();    } break;
				case ( HHuginn::TYPE::NUMBER ):  { static_cast<HNumber*>( _value.raw() )->value()  *= static_cast<HNumber const*>( other_._value.raw() )->value();  } break;
				default: {}
			}
		} else {
			switch ( _literalType ) {
				case ( HHuginn::TYPE::INTEGER ): { static_cast<HInteger*>( _value.raw() )->value() *= other_._integer; } break;
				case ( HHuginn::TYPE::REAL ):    { static_cast<HReal*>( _value.raw() )->value()    *= other_._real;    } break;
				case ( HHuginn::TYPE::NUMBER ):  { static_cast<HNumber*>( _value.raw() )->value()  *= other_._number;  } break;
				default: {}
			}
		}
	} else {
		if ( !! other_._value ) {
			switch ( _literalType ) {
				case ( HHuginn::TYPE::INTEGER ): { _integer *= static_cast<HInteger const*>( other_._value.raw() )->value(); } break;
				case ( HHuginn::TYPE::REAL ):    { _real    *= static_cast<HReal const*>( other_._value.raw() )->value();    } break;
				case ( HHuginn::TYPE::NUMBER ):  { _number  *= static_cast<HNumber const*>( other_._value.raw() )->value();  } break;
				default: {}
			}
		} else {
			switch ( _literalType ) {
				case ( HHuginn::TYPE::INTEGER ): { _integer *= other_._integer; } break;
				case ( HHuginn::TYPE::REAL ):    { _real    *= other_._real;    } break;
				case ( HHuginn::TYPE::NUMBER ):  { _number  *= other_._number;  } break;
				default: {}
			}
		}
	}
	return ( *this );
	M_EPILOG
}

namespace {

void do_divide( HReal::value_type& value_, HReal::value_type const& denominator_, int fileId_, int position_ ) {
	if ( denominator_ != 0.0L ) {
		value_ /= denominator_;
	} else {
		throw HHuginn::HHuginnRuntimeException( "Division by zero.", fileId_, position_ );
	}
}

void do_divide( HInteger::value_type& value_, HInteger::value_type const& denominator_, int fileId_, int position_ ) {
	if ( ( denominator_ != 0L ) && ( ( value_ != meta::min_signed<huginn::HInteger::value_type>::value ) || ( denominator_ != -1 ) ) ) {
		value_ /= denominator_;
	} else {
		throw HHuginn::HHuginnRuntimeException( denominator_ ? "Division overflow." : "Division by zero.", fileId_, position_ );
	}
}

void do_divide( HNumber::value_type& value_, HNumber::value_type const& denominator_, int fileId_, int position_ ) {
	if ( denominator_ != number::N0 ) {
		value_ /= denominator_;
	} else {
		throw HHuginn::HHuginnRuntimeException( "Division by zero.", fileId_, position_ );
	}
}

}

HExpression::OExecutionStep& HExpression::OExecutionStep::divide( OExecutionStep const& other_, int fileId_, int position_ ) {
	M_PROLOG
	M_ASSERT( ( _literalType == other_._literalType ) && ( _literalType != HHuginn::TYPE::UNKNOWN ) );
	if ( !! _value ) {
		if ( !! other_._value ) {
			switch ( _literalType ) {
				case ( HHuginn::TYPE::INTEGER ): { do_divide( static_cast<HInteger*>( _value.raw() )->value(), static_cast<HInteger const*>( other_._value.raw() )->value(), fileId_, position_ ); } break;
				case ( HHuginn::TYPE::REAL ):    { do_divide( static_cast<HReal*>( _value.raw() )->value(),    static_cast<HReal const*>( other_._value.raw() )->value(),    fileId_, position_ ); } break;
				case ( HHuginn::TYPE::NUMBER ):  { do_divide( static_cast<HNumber*>( _value.raw() )->value(),  static_cast<HNumber const*>( other_._value.raw() )->value(),  fileId_, position_ ); } break;
				default: {}
			}
		} else {
			switch ( _literalType ) {
				case ( HHuginn::TYPE::INTEGER ): { do_divide( static_cast<HInteger*>( _value.raw() )->value(), other_._integer, fileId_, position_ ); } break;
				case ( HHuginn::TYPE::REAL ):    { do_divide( static_cast<HReal*>( _value.raw() )->value(),    other_._real,    fileId_, position_ ); } break;
				case ( HHuginn::TYPE::NUMBER ):  { do_divide( static_cast<HNumber*>( _value.raw() )->value(),  other_._number,  fileId_, position_ ); } break;
				default: {}
			}
		}
	} else {
		if ( !! other_._value ) {
			switch ( _literalType ) {
				case ( HHuginn::TYPE::INTEGER ): { do_divide( _integer, static_cast<HInteger const*>( other_._value.raw() )->value(), fileId_, position_ ); } break;
				case ( HHuginn::TYPE::REAL ):    { do_divide( _real,    static_cast<HReal const*>( other_._value.raw() )->value(),    fileId_, position_ ); } break;
				case ( HHuginn::TYPE::NUMBER ):  { do_divide( _number,  static_cast<HNumber const*>( other_._value.raw() )->value(),  fileId_, position_ ); } break;
				default: {}
			}
		} else {
			switch ( _literalType ) {
				case ( HHuginn::TYPE::INTEGER ): { do_divide( _integer, other_._integer, fileId_, position_ ); } break;
				case ( HHuginn::TYPE::REAL ):    { do_divide( _real,    other_._real,    fileId_, position_ ); } break;
				case ( HHuginn::TYPE::NUMBER ):  { do_divide( _number,  other_._number,  fileId_, position_ ); } break;
				default: {}
			}
		}
	}
	return ( *this );
	M_EPILOG
}

namespace {

void do_modulo( HReal::value_type& value_, HReal::value_type const& denominator_, int fileId_, int position_ ) {
	if ( denominator_ != 0.0L ) {
		value_ = fmodl( value_, denominator_ );
	} else {
		throw HHuginn::HHuginnRuntimeException( "Division by zero.", fileId_, position_ );
	}
}

void do_modulo( HInteger::value_type& value_, HInteger::value_type const& denominator_, int fileId_, int position_ ) {
	if ( ( denominator_ != 0L ) && ( ( value_ != meta::min_signed<huginn::HInteger::value_type>::value ) || ( denominator_ != -1 ) ) ) {
		value_ %= denominator_;
	} else {
		throw HHuginn::HHuginnRuntimeException( denominator_ ? "Division overflow." : "Division by zero.", fileId_, position_ );
	}
}

void do_modulo( HNumber::value_type& value_, HNumber::value_type const& denominator_, int fileId_, int position_ ) {
	if ( denominator_ != number::N0 ) {
		value_ %= denominator_;
	} else {
		throw HHuginn::HHuginnRuntimeException( "Division by zero.", fileId_, position_ );
	}
}

}

HExpression::OExecutionStep& HExpression::OExecutionStep::modulo( OExecutionStep const& other_, int fileId_, int position_ ) {
	M_PROLOG
	M_ASSERT( ( _literalType == other_._literalType ) && ( _literalType != HHuginn::TYPE::UNKNOWN ) );
	if ( !! _value ) {
		if ( !! other_._value ) {
			switch ( _literalType ) {
				case ( HHuginn::TYPE::INTEGER ): { do_modulo( static_cast<HInteger*>( _value.raw() )->value(), static_cast<HInteger const*>( other_._value.raw() )->value(), fileId_, position_ ); } break;
				case ( HHuginn::TYPE::REAL ):    { do_modulo( static_cast<HReal*>( _value.raw() )->value(),    static_cast<HReal const*>( other_._value.raw() )->value(),    fileId_, position_ ); } break;
				case ( HHuginn::TYPE::NUMBER ):  { do_modulo( static_cast<HNumber*>( _value.raw() )->value(),  static_cast<HNumber const*>( other_._value.raw() )->value(),  fileId_, position_ ); } break;
				default: {}
			}
		} else {
			switch ( _literalType ) {
				case ( HHuginn::TYPE::INTEGER ): { do_modulo( static_cast<HInteger*>( _value.raw() )->value(), other_._integer, fileId_, position_ ); } break;
				case ( HHuginn::TYPE::REAL ):    { do_modulo( static_cast<HReal*>( _value.raw() )->value(),    other_._real,    fileId_, position_ ); } break;
				case ( HHuginn::TYPE::NUMBER ):  { do_modulo( static_cast<HNumber*>( _value.raw() )->value(),  other_._number,  fileId_, position_ ); } break;
				default: {}
			}
		}
	} else {
		if ( !! other_._value ) {
			switch ( _literalType ) {
				case ( HHuginn::TYPE::INTEGER ): { do_modulo( _integer, static_cast<HInteger const*>( other_._value.raw() )->value(), fileId_, position_ ); } break;
				case ( HHuginn::TYPE::REAL ):    { do_modulo( _real,    static_cast<HReal const*>( other_._value.raw() )->value(),    fileId_, position_ ); } break;
				case ( HHuginn::TYPE::NUMBER ):  { do_modulo( _number,  static_cast<HNumber const*>( other_._value.raw() )->value(),  fileId_, position_ ); } break;
				default: {}
			}
		} else {
			switch ( _literalType ) {
				case ( HHuginn::TYPE::INTEGER ): { do_modulo( _integer, other_._integer, fileId_, position_ ); } break;
				case ( HHuginn::TYPE::REAL ):    { do_modulo( _real,    other_._real,    fileId_, position_ ); } break;
				case ( HHuginn::TYPE::NUMBER ):  { do_modulo( _number,  other_._number,  fileId_, position_ ); } break;
				default: {}
			}
		}
	}
	return ( *this );
	M_EPILOG
}

namespace {

void do_power( HReal::value_type& value_, HReal::value_type const& exponent_, int fileId_, int position_ ) {
	if ( ( value_ != 0.L ) || ( exponent_ != 0.L ) ) {
		value_ = math::power( value_, exponent_ );
	} else {
		throw HHuginn::HHuginnRuntimeException( "indeterminate form 0^0", fileId_, position_ );
	}
}

void do_power( HNumber::value_type& value_, HNumber::value_type const& exponent_, int fileId_, int position_ ) {
	try {
		value_ = math::power( value_, exponent_ );
	} catch ( HNumberException const& ex ) {
		throw HHuginn::HHuginnRuntimeException( ex.what(), fileId_, position_ );
	}
}

}

HExpression::OExecutionStep& HExpression::OExecutionStep::power( OExecutionStep const& other_, int fileId_, int position_ ) {
	M_PROLOG
	M_ASSERT( ( _literalType == other_._literalType ) && ( _literalType != HHuginn::TYPE::UNKNOWN ) );
	if ( !! _value ) {
		if ( !! other_._value ) {
			switch ( _literalType ) {
				case ( HHuginn::TYPE::REAL ):    { do_power( static_cast<HReal*>( _value.raw() )->value(),    static_cast<HReal const*>( other_._value.raw() )->value(),    fileId_, position_ ); } break;
				case ( HHuginn::TYPE::NUMBER ):  { do_power( static_cast<HNumber*>( _value.raw() )->value(),  static_cast<HNumber const*>( other_._value.raw() )->value(),  fileId_, position_ ); } break;
				default: {}
			}
		} else {
			switch ( _literalType ) {
				case ( HHuginn::TYPE::REAL ):    { do_power( static_cast<HReal*>( _value.raw() )->value(),    other_._real,    fileId_, position_ ); } break;
				case ( HHuginn::TYPE::NUMBER ):  { do_power( static_cast<HNumber*>( _value.raw() )->value(),  other_._number,  fileId_, position_ ); } break;
				default: {}
			}
		}
	} else {
		if ( !! other_._value ) {
			switch ( _literalType ) {
				case ( HHuginn::TYPE::REAL ):    { do_power( _real,    static_cast<HReal const*>( other_._value.raw() )->value(),    fileId_, position_ ); } break;
				case ( HHuginn::TYPE::NUMBER ):  { do_power( _number,  static_cast<HNumber const*>( other_._value.raw() )->value(),  fileId_, position_ ); } break;
				default: {}
			}
		} else {
			switch ( _literalType ) {
				case ( HHuginn::TYPE::REAL ):    { do_power( _real,    other_._real,    fileId_, position_ ); } break;
				case ( HHuginn::TYPE::NUMBER ):  { do_power( _number,  other_._number,  fileId_, position_ ); } break;
				default: {}
			}
		}
	}
	return ( *this );
	M_EPILOG
}

HExpression::OExecutionStep& HExpression::OExecutionStep::negate( int fileId_, int position_ ) {
	M_PROLOG
	M_ASSERT( _literalType != HHuginn::TYPE::UNKNOWN );
	if ( !! _value ) {
		switch ( _literalType ) {
			case ( HHuginn::TYPE::INTEGER ): {
				HInteger::value_type& value( static_cast<HInteger*>( _value.raw() )->value() );
				if ( value == meta::min_signed<huginn::HInteger::value_type>::value ) {
					throw HHuginn::HHuginnRuntimeException( "Integer overflow.", fileId_, position_ );
				}
				value = -value;
			} break;
			case ( HHuginn::TYPE::REAL ):    { static_cast<HReal*>( _value.raw() )->value()    = -static_cast<HReal*>( _value.raw() )->value();   } break;
			case ( HHuginn::TYPE::NUMBER ):  { static_cast<HNumber*>( _value.raw() )->value()  = -static_cast<HNumber*>( _value.raw() )->value(); } break;
			default: {}
		}
	} else {
		switch ( _literalType ) {
			case ( HHuginn::TYPE::INTEGER ): {
				if ( _integer == meta::min_signed<huginn::HInteger::value_type>::value ) {
					throw HHuginn::HHuginnRuntimeException( "Integer overflow.", fileId_, position_ );
				}
				_integer = -_integer;
			} break;
			case ( HHuginn::TYPE::REAL ):    { _real    = -_real;   } break;
			case ( HHuginn::TYPE::NUMBER ):  { _number  = -_number; } break;
			default: {}
		}
	}
	return ( *this );
	M_EPILOG
}

HExpression::OExecutionStep& HExpression::OExecutionStep::modulus( int fileId_, int position_ ) {
	M_PROLOG
	M_ASSERT( _literalType != HHuginn::TYPE::UNKNOWN );
	if ( !! _value ) {
		switch ( _literalType ) {
			case ( HHuginn::TYPE::INTEGER ): {
				HInteger::value_type& value( static_cast<HInteger*>( _value.raw() )->value() );
				if ( value == meta::min_signed<huginn::HInteger::value_type>::value ) {
					throw HHuginn::HHuginnRuntimeException( "Integer overflow.", fileId_, position_ );
				}
				value = math::abs( value );
			} break;
			case ( HHuginn::TYPE::REAL ):    { static_cast<HReal*>( _value.raw() )->value()    = math::abs( static_cast<HReal*>( _value.raw() )->value() );   } break;
			case ( HHuginn::TYPE::NUMBER ):  { static_cast<HNumber*>( _value.raw() )->value()  = math::abs( static_cast<HNumber*>( _value.raw() )->value() ); } break;
			default: {}
		}
	} else {
		switch ( _literalType ) {
			case ( HHuginn::TYPE::INTEGER ): {
				if ( _integer == meta::min_signed<huginn::HInteger::value_type>::value ) {
					throw HHuginn::HHuginnRuntimeException( "Integer overflow.", fileId_, position_ );
				}
				_integer = math::abs( _integer );
			} break;
			case ( HHuginn::TYPE::REAL ):    { _real    = math::abs( _real );   } break;
			case ( HHuginn::TYPE::NUMBER ):  { _number  = math::abs( _number ); } break;
			default: {}
		}
	}
	return ( *this );
	M_EPILOG
}

namespace {

void do_factorial( HNumber::value_type& value_, int fileId_, int position_ ) {
	if ( value_ < hcore::number::N0 ) {
		throw HHuginn::HHuginnRuntimeException( "Factorial from negative.", fileId_, position_ );
	} else if ( ! value_.is_integral() ) {
		throw HHuginn::HHuginnRuntimeException( "Factorial from fraction.", fileId_, position_ );
	} else {
		value_ = hcore::number::factorial( value_.to_integer() );
	}
}

}

HExpression::OExecutionStep& HExpression::OExecutionStep::factorial( int fileId_, int position_ ) {
	M_PROLOG
	M_ASSERT( _literalType != HHuginn::TYPE::UNKNOWN );
	if ( !! _value ) {
		do_factorial( static_cast<HNumber*>( _value.raw() )->value(), fileId_, position_ );
	} else {
		do_factorial( _number, fileId_, position_ );
	}
	return ( *this );
	M_EPILOG
}

HExpression::HExpression( int fileId_, executing_parser::range_t range_ )
	: HStatement( INVALID_STATEMENT_IDENTIFIER, fileId_, range_ )
	, _executionSteps()
	, _instructions()
	, _operations()
	, _final( false ) {
	return;
}

int HExpression::execution_step_count( void ) const {
	M_PROLOG
	return ( static_cast<int>( _executionSteps.get_size() ) );
	M_EPILOG
}

int HExpression::add_execution_step( OExecutionStep const& executionStep_ ) {
	M_PROLOG
	_executionSteps.push_back( executionStep_ );
	return ( static_cast<int>( _executionSteps.get_size() - 1 ) );
	M_EPILOG
}

void HExpression::replace_execution_step( int index_, OExecutionStep const& executionStep_ ) {
	M_PROLOG
	OExecutionStep& es( _executionSteps[index_] );
	bool optimize( es._literalType != HHuginn::TYPE::UNKNOWN );
	OExecutionStep::action_t action( es._action );
	es = executionStep_;
	if ( optimize ) {
		if ( es._action == &HExpression::get_variable_reference ) {
			es._action = &HExpression::get_variable_value;
		} else if ( es._action == &HExpression::get_field_ref_direct ) {
			_executionSteps[index_ + 1]._action = action;
		}
	}
	return;
	M_EPILOG
}

void HExpression::pop_execution_step( void ) {
	M_PROLOG
	_executionSteps.pop_back();
	return;
	M_EPILOG
}

HExpression::OExecutionStep& HExpression::execution_step( int idx_ ) {
	M_PROLOG
	return ( _executionSteps[idx_] );
	M_EPILOG
}

void HExpression::merge( HExpression& expression_ ) {
	M_PROLOG
	for ( OExecutionStep& es : expression_._executionSteps ) {
		es._expression = this;
	}
	move( expression_._executionSteps.begin(), expression_._executionSteps.end(), back_insert_iterator( _executionSteps ) );
	move( expression_._instructions.begin(), expression_._instructions.end(), back_insert_iterator( _instructions ) );
	expression_._executionSteps.clear();
	expression_._instructions.clear();
	operations_t operationsStore;
	while ( ! expression_._operations.is_empty() ) {
		operationsStore.push( expression_._operations.top() );
		expression_._operations.pop();
	}
	while ( ! operationsStore.is_empty() ) {
		_operations.push( operationsStore.top() );
		operationsStore.pop();
	}
	return;
	M_EPILOG
}

bool HExpression::is_empty( void ) const {
	M_PROLOG
	return ( _executionSteps.is_empty() );
	M_EPILOG
}

void HExpression::oper( OPERATOR operator_, int position_ ) {
	M_PROLOG
	_operations.emplace( operator_, position_ );
	return;
	M_EPILOG
}

void HExpression::commit_oper( OPERATOR operator_, int fileId_, int position_ ) {
	M_PROLOG
	M_ASSERT( ! _operations.is_empty() );
	switch ( operator_ ) {
		case ( OPERATOR::EQUALS ):
		case ( OPERATOR::NOT_EQUALS ):
		case ( OPERATOR::LESS ):
		case ( OPERATOR::GREATER ):
		case ( OPERATOR::LESS_OR_EQUAL ):
		case ( OPERATOR::GREATER_OR_EQUAL ):
		case ( OPERATOR::IS_ELEMENT_OF ):
		case ( OPERATOR::IS_NOT_ELEMENT_OF ):
		case ( OPERATOR::PLUS ):
		case ( OPERATOR::MINUS ):
		case ( OPERATOR::MULTIPLY ):
		case ( OPERATOR::DIVIDE ):
		case ( OPERATOR::MODULO ):
		case ( OPERATOR::NEGATE ):
		case ( OPERATOR::FACTORIAL ):
		case ( OPERATOR::BOOLEAN_AND ):
		case ( OPERATOR::BOOLEAN_OR ):
		case ( OPERATOR::BOOLEAN_XOR ):
		case ( OPERATOR::BOOLEAN_NOT ):
		case ( OPERATOR::TERNARY ):
		case ( OPERATOR::MEMBER_ACCESS ):
		case ( OPERATOR::MODULUS ):
		case ( OPERATOR::PARENTHESIS ): {
			M_ASSERT( _operations.top()._operator == operator_ );
			_instructions.push_back( _operations.top() );
			_operations.pop();
		} break;
		case ( OPERATOR::POWER_TERM ): {
			M_DEBUG_CODE( bool hasPowerOperator( false ); );
			while ( ! _operations.is_empty() && ( _operations.top()._operator == OPERATOR::POWER ) ) {
				_instructions.push_back( _operations.top() );
				_operations.pop();
				M_DEBUG_CODE( hasPowerOperator = true; );
			}
			M_ASSERT( hasPowerOperator );
			_instructions.push_back( OPositionedOperator( operator_, 0 ) );
		} break;
		case ( OPERATOR::ASSIGN_TERM ): {
			M_DEBUG_CODE( bool hasAssignOperator( false ); );
			while ( ! _operations.is_empty() && ( _operations.top()._operator >= OPERATOR::ASSIGN ) && ( _operations.top()._operator <= OPERATOR::POWER_ASSIGN ) ) {
				_instructions.push_back( _operations.top() );
				_operations.pop();
				M_DEBUG_CODE( hasAssignOperator = true; );
			}
			M_ASSERT( hasAssignOperator );
			_instructions.push_back( OPositionedOperator( operator_, 0 ) );
		} break;
		case ( OPERATOR::FUNCTION_CALL ): {
			while ( _operations.top()._operator == OPERATOR::FUNCTION_ARGUMENT ) {
				_instructions.push_back( _operations.top() );
				_operations.pop();
			}
			M_ASSERT( _operations.top()._operator == operator_ );
			_instructions.push_back( _operations.top() );
			_operations.pop();
		} break;
		case ( OPERATOR::SUBSCRIPT ): {
			M_ASSERT( _operations.top()._operator == OPERATOR::FUNCTION_ARGUMENT );
			_instructions.push_back( _operations.top() );
			_operations.pop();
			M_ASSERT( _operations.top()._operator == operator_ );
			_instructions.push_back( _operations.top() );
			_operations.pop();
		} break;
		case ( OPERATOR::RANGE ): {
			while ( ( _operations.top()._operator == OPERATOR::FUNCTION_ARGUMENT ) || ( _operations.top()._operator == OPERATOR::SUBSCRIPT_ARGUMENT ) ) {
				_instructions.push_back( _operations.top() );
				_operations.pop();
			}
			M_ASSERT( _operations.top()._operator == OPERATOR::SUBSCRIPT );
			_instructions.push_back( _operations.top() );
			_operations.pop();
		} break;
		default: {
			M_ASSERT( ! "Invalid code path."[0] );
		}
	}
	try_collape( fileId_, position_ );
	return;
	M_EPILOG
}

void HExpression::try_collape( int fileId_, int position_ ) {
	M_PROLOG
	if ( _instructions.is_empty() ) {
		return;
	}
	int stepCount( static_cast<int>( _executionSteps.get_size() ) );
	OPERATOR op( _instructions.back()._operator );
	switch ( op ) {
		case ( OPERATOR::PLUS ):
		case ( OPERATOR::MINUS ):
		case ( OPERATOR::MULTIPLY ):
		case ( OPERATOR::DIVIDE ):
		case ( OPERATOR::MODULO ): {
			if (
				( _executionSteps[stepCount - 2]._literalType != HHuginn::TYPE::UNKNOWN )
				&& ( _executionSteps[stepCount - 2]._literalType == _executionSteps[stepCount - 3]._literalType )
			) {
				_executionSteps.pop_back();
				switch ( op ) {
					case ( OPERATOR::PLUS ):     { _executionSteps[stepCount - 3] += _executionSteps.back(); } break;
					case ( OPERATOR::MINUS ):    { _executionSteps[stepCount - 3] -= _executionSteps.back(); } break;
					case ( OPERATOR::MULTIPLY ): { _executionSteps[stepCount - 3] *= _executionSteps.back(); } break;
					case ( OPERATOR::DIVIDE ):   { _executionSteps[stepCount - 3].divide( _executionSteps.back(), fileId_, position_ ); } break;
					case ( OPERATOR::MODULO ):   { _executionSteps[stepCount - 3].modulo( _executionSteps.back(), fileId_, position_ ); } break;
					default: {}
				}
				_executionSteps.pop_back();
				_instructions.pop_back();
			}
		} break;
		case ( OPERATOR::POWER_TERM ): {
			_instructions.pop_back();
			int exponentIdx( stepCount - 2 );
			while (
				! _instructions.is_empty()
				&& ( _instructions.back()._operator == OPERATOR::POWER )
				&& (
					( _executionSteps[exponentIdx]._literalType == HHuginn::TYPE::REAL )
					|| ( _executionSteps[exponentIdx]._literalType == HHuginn::TYPE::NUMBER )
				)
				&& ( _executionSteps[exponentIdx]._literalType == _executionSteps[exponentIdx - 1]._literalType )
			) {
				_executionSteps[exponentIdx - 1].power( _executionSteps[exponentIdx], fileId_, position_ );
				-- exponentIdx;
				_instructions.pop_back();
			}
			if ( ! _instructions.is_empty() && ( _instructions.back()._operator == OPERATOR::POWER ) ) {
				++ exponentIdx;
				_executionSteps[exponentIdx] = _executionSteps.back();
				_instructions.push_back( OPositionedOperator( OPERATOR::POWER_TERM, 0 ) );
			}
			++ exponentIdx;
			while ( exponentIdx < stepCount ) {
				_executionSteps.pop_back();
				++ exponentIdx;
			}
		} break;
		case ( OPERATOR::NEGATE ): {
			if ( _executionSteps[stepCount - 2]._literalType != HHuginn::TYPE::UNKNOWN ) {
				_executionSteps.pop_back();
				_executionSteps.back().negate( fileId_, position_ );
				_instructions.pop_back();
			}
		} break;
		case ( OPERATOR::MODULUS ): {
			if ( _executionSteps[stepCount - 2]._literalType != HHuginn::TYPE::UNKNOWN ) {
				_executionSteps.pop_back();
				_executionSteps.back().modulus( fileId_, position_ );
				_instructions.pop_back();
			}
		} break;
		case ( OPERATOR::FACTORIAL ): {
			if ( _executionSteps[stepCount - 2]._literalType != HHuginn::TYPE::UNKNOWN ) {
				_executionSteps.pop_back();
				_executionSteps.back().factorial( fileId_, position_ );
				_instructions.pop_back();
			}
		} break;
		case ( OPERATOR::PARENTHESIS ): {
			if ( _executionSteps[stepCount - 2]._literalType != HHuginn::TYPE::UNKNOWN ) {
				_executionSteps.pop_back();
				_instructions.pop_back();
			}
		} break;
		default: {
			break;
		}
	}
	if ( _instructions.is_empty() ) {
		return;
	}
	op = _instructions.back()._operator;
	if ( op == OPERATOR::ASSIGN_TERM ) {
		try_collape_assign( fileId_, position_ );
		return;
	}
	return;
	M_EPILOG
}

void HExpression::try_collape_assign( int, int ) {
	M_PROLOG
	int instructionCount( static_cast<int>( _instructions.get_size() ) );
	int stepCount( static_cast<int>( _executionSteps.get_size() ) );
	if ( ( instructionCount < 2 ) || ( stepCount < 2 ) ) {
		return;
	}
	OExecutionStep& es( _executionSteps[stepCount - 2] );
	switch ( es._literalType ) {
		case ( HHuginn::TYPE::INTEGER ): {
			try_collape_assign_integer();
			break;
		}
		case ( HHuginn::TYPE::REAL ): {
			try_collape_assign_real();
			break;
		}
		default: {
		}
	}
	return;
	M_EPILOG
}

void HExpression::try_collape_assign_integer( void ) {
	M_PROLOG
	int instructionCount( static_cast<int>( _instructions.get_size() ) );
	switch ( _instructions[instructionCount - 2]._operator ) {
		case ( OPERATOR::PLUS_ASSIGN ): {
			try_collape_assign_action( HHuginn::TYPE::INTEGER, &HExpression::oper_assign_integer_ref<OPERATOR::PLUS_ASSIGN>, &HExpression::oper_assign_integer_val<OPERATOR::PLUS_ASSIGN> );
			break;
		}
		case ( OPERATOR::MINUS_ASSIGN ): {
			try_collape_assign_action( HHuginn::TYPE::INTEGER, &HExpression::oper_assign_integer_ref<OPERATOR::MINUS_ASSIGN>, &HExpression::oper_assign_integer_val<OPERATOR::MINUS_ASSIGN> );
			break;
		}
		case ( OPERATOR::MULTIPLY_ASSIGN ): {
			try_collape_assign_action( HHuginn::TYPE::INTEGER, &HExpression::oper_assign_integer_ref<OPERATOR::MULTIPLY_ASSIGN>, &HExpression::oper_assign_integer_val<OPERATOR::MULTIPLY_ASSIGN> );
			break;
		}
		case ( OPERATOR::DIVIDE_ASSIGN ): {
			try_collape_assign_action( HHuginn::TYPE::INTEGER, &HExpression::oper_assign_integer_ref<OPERATOR::DIVIDE_ASSIGN>, &HExpression::oper_assign_integer_val<OPERATOR::DIVIDE_ASSIGN> );
			break;
		}
		case ( OPERATOR::MODULO_ASSIGN ): {
			try_collape_assign_action( HHuginn::TYPE::INTEGER, &HExpression::oper_assign_integer_ref<OPERATOR::MODULO_ASSIGN>, &HExpression::oper_assign_integer_val<OPERATOR::MODULO_ASSIGN> );
			break;
		}
		default: {
			break;
		}
	}
	return;
	M_EPILOG
}

void HExpression::try_collape_assign_real( void ) {
	M_PROLOG
	int instructionCount( static_cast<int>( _instructions.get_size() ) );
	switch ( _instructions[instructionCount - 2]._operator ) {
		case ( OPERATOR::PLUS_ASSIGN ): {
			try_collape_assign_action( HHuginn::TYPE::REAL, &HExpression::oper_assign_real_ref<OPERATOR::PLUS_ASSIGN>, &HExpression::oper_assign_real_val<OPERATOR::PLUS_ASSIGN> );
			break;
		}
		case ( OPERATOR::MINUS_ASSIGN ): {
			try_collape_assign_action( HHuginn::TYPE::REAL, &HExpression::oper_assign_real_ref<OPERATOR::MINUS_ASSIGN>, &HExpression::oper_assign_real_val<OPERATOR::MINUS_ASSIGN> );
			break;
		}
		case ( OPERATOR::MULTIPLY_ASSIGN ): {
			try_collape_assign_action( HHuginn::TYPE::REAL, &HExpression::oper_assign_real_ref<OPERATOR::MULTIPLY_ASSIGN>, &HExpression::oper_assign_real_val<OPERATOR::MULTIPLY_ASSIGN> );
			break;
		}
		case ( OPERATOR::DIVIDE_ASSIGN ): {
			try_collape_assign_action( HHuginn::TYPE::REAL, &HExpression::oper_assign_real_ref<OPERATOR::DIVIDE_ASSIGN>, &HExpression::oper_assign_real_val<OPERATOR::DIVIDE_ASSIGN> );
			break;
		}
		case ( OPERATOR::MODULO_ASSIGN ): {
			try_collape_assign_action( HHuginn::TYPE::REAL, &HExpression::oper_assign_real_ref<OPERATOR::MODULO_ASSIGN>, &HExpression::oper_assign_real_val<OPERATOR::MODULO_ASSIGN> );
			break;
		}
		default: {
			break;
		}
	}
	return;
	M_EPILOG
}

void HExpression::try_collape_assign_action( HHuginn::TYPE type_, OExecutionStep::action_t refAction_, OExecutionStep::action_t valAction_ ) {
	M_PROLOG
	int stepCount( static_cast<int>( _executionSteps.get_size() ) );
	OExecutionStep& es( _executionSteps[stepCount - 2] );
	M_ASSERT( !! es._value && ( es._value->type_id() == type_ ) );
	if (
		( ( stepCount == 3 ) && ( _executionSteps[0]._action == nullptr ) )
		|| (
			( stepCount > 3 )
			&& ( _executionSteps[stepCount - 3]._action != &HExpression::subscript )
			&& ( _executionSteps[stepCount - 3]._action != &HExpression::get_field )
		)
	) {
		_executionSteps[stepCount - 3]._action = refAction_;
		_executionSteps[stepCount - 3]._literalType = HHuginn::TYPE::NONE;
		es._action = valAction_;
	} else {
		es._action = refAction_;
	}
	_instructions.pop_back();
	_executionSteps.pop_back();
	switch ( type_ ) {
		case ( HHuginn::TYPE::INTEGER ): {
			es._integer = get_integer( es._value );
			break;
		}
		case ( HHuginn::TYPE::REAL ): {
			es._real = get_real( es._value );
			break;
		}
		default: {
			M_ASSERT( !"Invalid code path!"[0] );
		}
	}
	es._value.reset();
	return;
	M_EPILOG
}

void HExpression::close_parenthesis( OExecutionStep const& executionStep_, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( ! frame_->values().is_empty() );
	OPERATOR o( _instructions[frame_->ip()]._operator );
	M_ASSERT( ( o == OPERATOR::MODULUS ) || ( o == OPERATOR::PARENTHESIS ) );
	++ frame_->ip();
	if ( o == OPERATOR::MODULUS ) {
		HHuginn::value_t v( yaal::move( frame_->values().top() ) );
		frame_->values().pop();
		frame_->values().push( v->operator_modulus( frame_->thread(), v, executionStep_._position ) );
	}
	return;
	M_EPILOG
}

void HExpression::get_field_direct( OExecutionStep const& executionStep_, huginn::HFrame* frame_ ) {
	M_PROLOG
	frame_->values().emplace( frame_->get_field( executionStep_._index ) );
	return;
	M_EPILOG
}

void HExpression::get_field_ref_direct( OExecutionStep const& executionStep_, huginn::HFrame* frame_ ) {
	M_PROLOG
	frame_->values().emplace( frame_->thread()->object_factory().create_reference( frame_->get_field_ref( executionStep_._index ) ) );
	return;
	M_EPILOG
}

void HExpression::get_variable_value( OExecutionStep const& executionStep_, huginn::HFrame* frame_ ) {
	M_PROLOG
	frame_->values().emplace( frame_->get_variable_value( executionStep_._index ) );
	return;
	M_EPILOG
}

void HExpression::get_variable_reference( OExecutionStep const& executionStep_, huginn::HFrame* frame_ ) {
	M_PROLOG
	frame_->values().emplace( frame_->thread()->object_factory().create_reference( frame_->get_variable_reference( executionStep_._index ) ) );
	return;
	M_EPILOG
}

void HExpression::make_variable( OExecutionStep const& executionStep_, huginn::HFrame* frame_ ) {
	M_PROLOG
	frame_->values().emplace( frame_->thread()->object_factory().create_reference( frame_->make_variable( executionStep_._index ) ) );
	return;
	M_EPILOG
}

void HExpression::get_this( OExecutionStep const&, huginn::HFrame* frame_ ) {
	M_PROLOG
	frame_->values().emplace( frame_->get_this() );
	return;
	M_EPILOG
}

void HExpression::get_super( OExecutionStep const& executionStep_, huginn::HFrame* frame_ ) {
	M_PROLOG
	frame_->values().emplace( frame_->get_super( executionStep_._position ) );
	return;
	M_EPILOG
}

void HExpression::get_field( OExecutionStep const& executionStep_,huginn::HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::MEMBER_ACCESS );
	int p( _instructions[frame_->ip()]._position );
	++ frame_->ip();
	HFrame::values_t& values( frame_->values() );
	HHuginn::value_t v( yaal::move( values.top() ) );
	values.pop();
	HThread* t( frame_->thread() );
	if ( executionStep_._access != HFrame::ACCESS::BOUND_CALL ) {
		values.push( instruction::member( t, executionStep_._access, v, executionStep_._identifierId, p ) );
	} else {
		HHuginn::value_t f( instruction::member( t, executionStep_._access, v, executionStep_._identifierId, p ) );
		if ( f->type_id() == HHuginn::TYPE::METHOD ) {
			values.push( yaal::move( v ) );
		}
		values.push( yaal::move( f ) );
	}
	return;
	M_EPILOG
}

void HExpression::set_variable( OExecutionStep const& es_, HFrame* frame_ ) {
	M_PROLOG
	int& ip( frame_->ip() );
	HFrame::values_t& values( frame_->values() );
	HThread* t( frame_->thread() );
	while ( ip < static_cast<int>( _instructions.get_size() ) ) {
		instructions_t::value_type& operation( _instructions[ip] );
		if ( ( operation._operator < OPERATOR::ASSIGN ) || ( operation._operator > OPERATOR::POWER_ASSIGN ) ) {
			break;
		}
		int p( operation._position );
		++ ip;
		HHuginn::value_t src( yaal::move( values.top() ) );
		values.pop();
		if ( operation._operator == OPERATOR::ASSIGN_PACK ) {
			if ( src->type_id() != HHuginn::TYPE::TUPLE ) {
				throw HHuginn::HHuginnRuntimeException( "Assigner is not a `tuple` object.", file_id(), p );
			}
			huginn::HTuple::values_t& srcData( static_cast<huginn::HTuple*>( src.raw() )->value() );
			int ds( static_cast<int>( srcData.get_size() ) );
			if ( ds != es_._index ) {
				throw HHuginn::HHuginnRuntimeException(
					hcore::to_string( ds > es_._index ? "Too many" : "Not enough" )
						.append( " values to unpack, expected: " ).append( es_._index )
						.append( ", got: " ).append( ds ).append( "." ),
					file_id(),
					p
				);
			}
			huginn::HTuple::values_t data( es_._index );
			for ( int i( ds - 1 ); i >= 0; -- i ) {
				HHuginn::value_t dst( yaal::move( values.top() ) );
				values.pop();
				M_ASSERT( dst->type_id() == HHuginn::TYPE::REFERENCE );
				HReference& ref( *static_cast<HReference*>( dst.raw() ) );
				data[i] = srcData[i];
				ref.set( t, yaal::move( srcData[i] ), p );
			}
			values.push( t->object_factory().create_tuple( yaal::move( data ) ) );
		} else {
			HHuginn::value_t dst( yaal::move( values.top() ) );
			values.pop();
			M_ASSERT( dst->type_id() == HHuginn::TYPE::REFERENCE );
			HReference& ref( *static_cast<HReference*>( dst.raw() ) );
			if ( operation._operator == OPERATOR::ASSIGN ) {
				ref.set( t, yaal::move( src ), p );
			} else {
				HHuginn::value_t v( ref.get( t, p ) );
				HClass const* refClass( v->get_class() );
				HClass const* srcClass( src->get_class() );
				if ( refClass != srcClass ) {
					operands_type_mismatch( op_to_str( operation._operator ), refClass, srcClass, file_id(), p );
				}
				switch ( operation._operator ) {
					case ( OPERATOR::PLUS_ASSIGN ):     { v->operator_add( t, v, src, p ); } break;
					case ( OPERATOR::MINUS_ASSIGN ):    { v->operator_subtract( t, v, src, p ); } break;
					case ( OPERATOR::MULTIPLY_ASSIGN ): { v->operator_multiply( t, v, src, p ); } break;
					case ( OPERATOR::DIVIDE_ASSIGN ):   { v->operator_divide( t, v, src, p ); } break;
					case ( OPERATOR::MODULO_ASSIGN ):   { v->operator_modulo( t, v, src, p ); } break;
					case ( OPERATOR::POWER_ASSIGN ):    { v->operator_power( t, v, src, p ); } break;
					default: {
						M_ASSERT( ! "bad code path"[0] );
					} break;
				}
				ref.set( t, yaal::move( v ), p );
			}
			values.push( ref.get( t, p ) );
		}
	}
	M_ASSERT( ( ip < static_cast<int>( _instructions.get_size() ) ) && (  _instructions[ip]._operator == OPERATOR::ASSIGN_TERM ) );
	++ ip;
	return;
	M_EPILOG
}

void HExpression::grab_args( HFrame* frame_, HArguments& args_ ) {
	M_PROLOG
	int& ip( frame_->ip() );
	M_ASSERT( ip < static_cast<int>( _instructions.get_size() ) );
	HFrame::values_t& values( frame_->values() );
	M_ASSERT( ! values.is_empty() );
	HHuginn::values_t& arguments( args_ );
	while ( _instructions[ip]._operator == OPERATOR::FUNCTION_ARGUMENT ) {
		++ ip;
		M_ASSERT( ip < static_cast<int>( _instructions.get_size() ) );
		HHuginn::value_t& v( values.top() );
		if ( v->type_id() != HHuginn::TYPE::VARIADIC_PARAMETERS ) {
			arguments.push_back( yaal::move( v ) );
		} else {
			HHuginn::value_t& vp( static_cast<HTaggedValue*>( v.raw() )->value() );
			HHuginn::values_t& p( static_cast<huginn::HTuple*>( vp.raw() )->value() );
			arguments.insert( arguments.end(), p.rbegin(), p.rend() );
		}
		values.pop();
	}
	M_ASSERT( _instructions[ip]._operator == OPERATOR::FUNCTION_CALL );
	reverse( arguments.begin(), arguments.end() );
	return;
	M_EPILOG
}

void HExpression::repack_named_parameters( OExecutionStep const& executionStep_, huginn::HFrame* frame_ ) {
	M_PROLOG
	HFrame::values_t& values( frame_->values() );
	HHuginn::value_t v( yaal::move( values.top() ) );
	values.pop();
	if ( v->type_id() != HHuginn::TYPE::LOOKUP ) {
		throw HHuginn::HHuginnRuntimeException(
			"Packed parameter is "_ys.append( a_type_name( v->get_class() ) ).append( " instead of a `lookup`." ),
			file_id(),
			executionStep_._position
		);
	}
	HHuginn::value_t* pv( ! values.is_empty() ? &values.top() : nullptr );
	if ( pv && ( (*pv)->type_id() == HHuginn::TYPE::NAMED_PARAMETERS ) ) {
		HTaggedValue& tv( *static_cast<HTaggedValue*>( pv->raw() ) );
		HHuginn::value_t& l( tv.value() );
		l->operator_add( frame_->thread(), l, v, executionStep_._position );
		int& ip( frame_->ip() );
		M_ASSERT( _instructions[ip]._operator == OPERATOR::FUNCTION_ARGUMENT );
		++ ip;
	} else {
		v = frame_->thread()->object_factory().create_tagged_value( v, frame_->thread()->object_factory().named_parameters_class() );
		values.push( yaal::move( v ) );
	}
	return;
	M_EPILOG
}

void HExpression::unpack_variadic_parameters( OExecutionStep const& executionStep_, huginn::HFrame* frame_ ) {
	M_PROLOG
	HFrame::values_t& values( frame_->values() );
	HHuginn::value_t& vr( values.top() );
	if ( vr->type_id() == HHuginn::TYPE::TUPLE ) {
		vr = frame_->thread()->object_factory().create_tagged_value( vr, frame_->thread()->object_factory().variadic_parameters_class() );
	} else {
		throw HHuginn::HHuginnRuntimeException(
			"Unpacked parameter is "_ys.append( a_type_name( vr->get_class() ) ).append( " instead of a `tuple`." ),
			file_id(),
			executionStep_._position
		);
	}
	return;
	M_EPILOG
}

void HExpression::function_call( OExecutionStep const& executionStep_, HFrame* frame_ ) {
	M_PROLOG
	HArguments args( frame_ );
	grab_args( frame_, args );
	M_ASSERT( ! frame_->values().is_empty() );
	int& ip( frame_->ip() );
	int p( _instructions[ip]._position );
	++ ip;
	HFrame::values_t& values( frame_->values() );
	HHuginn::value_t f( yaal::move( values.top() ) );
	values.pop();
	HClass const* c( f->get_class() );
	HHuginn::type_id_t t( c->type_id() );
	HThread* thread( frame_->thread() );
	frame_->set_position( p );
	if ( t == HHuginn::TYPE::METHOD ) {
		HHuginn::value_t v( yaal::move( values.top() ) );
		values.pop();
		HClass::HMethod* m( static_cast<HClass::HMethod*>( f.raw() ) );
		values.push( m->fast_call( thread, &v, args, executionStep_._position ) );
	} else {
		values.push( f->operator_call( thread, f, args, executionStep_._position ) );
	}
	return;
	M_EPILOG
}

HHuginn::values_t HExpression::grab_args( HFrame* frame_, HPartial::unbound_indexes_t& unboundIndexes_ ) {
	M_PROLOG
	int& ip( frame_->ip() );
	M_ASSERT( ip < static_cast<int>( _instructions.get_size() ) );
	HFrame::values_t& values( frame_->values() );
	M_ASSERT( ! values.is_empty() );
	HHuginn::values_t argumentsRaw;
	while ( _instructions[ip]._operator == OPERATOR::FUNCTION_ARGUMENT ) {
		++ ip;
		M_ASSERT( ip < static_cast<int>( _instructions.get_size() ) );
		argumentsRaw.push_back( yaal::move( values.top() ) );
		values.pop();
	}
	M_ASSERT( _instructions[ip]._operator == OPERATOR::FUNCTION_CALL );
	reverse( argumentsRaw.begin(), argumentsRaw.end() );
	HHuginn::values_t arguments;
	int offset( 0 );
	int index( 0 );
	int unboundIndex( 0 );
	for ( HHuginn::value_t const& v : argumentsRaw ) {
		while ( ( unboundIndex < unboundIndexes_.get_size() ) && ( index == unboundIndexes_[unboundIndex].internal_index() ) ) {
			arguments.push_back( HHuginn::value_t() );
			unboundIndexes_[unboundIndex].shift( offset );
			++ unboundIndex;
			++ index;
		}
		if ( v->type_id() != HHuginn::TYPE::VARIADIC_PARAMETERS ) {
			arguments.push_back( v );
		} else {
			HHuginn::value_t const& vp( static_cast<HTaggedValue const*>( v.raw() )->value() );
			HHuginn::values_t const& p( static_cast<huginn::HTuple const*>( vp.raw() )->value() );
			arguments.insert( arguments.end(), p.begin(), p.end() );
			offset += static_cast<int>( p.get_size() - 1 );
		}
		++ index;
	}
	while ( unboundIndex < unboundIndexes_.get_size() ) {
		arguments.push_back( HHuginn::value_t() );
		unboundIndexes_[unboundIndex].shift( offset );
		++ unboundIndex;
	}
	return ( arguments );
	M_EPILOG
}

void HExpression::make_partial( OExecutionStep const& executionStep_, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( executionStep_._integer > 0 );
	M_ASSERT( ! executionStep_._unboundIndexes.is_empty() );
	M_ASSERT( executionStep_._integer <= executionStep_._unboundIndexes.get_size() );
	HPartial::unbound_indexes_t unboundIndexes( executionStep_._unboundIndexes );
	HHuginn::values_t arguments( grab_args( frame_, unboundIndexes ) );
	M_ASSERT( ! frame_->values().is_empty() );
	int& ip( frame_->ip() );
	++ ip;
	HFrame::values_t& values( frame_->values() );
	HHuginn::value_t f( yaal::move( values.top() ) );
	values.pop();
	HClass const* c( f->get_class() );
	HHuginn::type_id_t t( c->type_id() );
	HObjectFactory& of( frame_->thread()->object_factory() );
	if ( t == HHuginn::TYPE::METHOD ) {
		HHuginn::value_t v( yaal::move( values.top() ) );
		values.pop();
		HClass::HMethod* m( static_cast<HClass::HMethod*>( f.raw() ) );
		f = of.create_bound_method( m->function(), v );
	}
	values.push( of.create_partial( f, yaal::move( arguments ), unboundIndexes, static_cast<int>( executionStep_._integer ) ) );
	return;
	M_EPILOG
}

void HExpression::create_closure( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	HFrame::values_t& values( frame_->values() );
	HHuginn::value_t f( yaal::move( values.top() ) );
	values.pop();
	HArguments args( frame_ );
	grab_args( frame_, args );
	++ frame_->ip();
	M_ASSERT( f->type_id() == HHuginn::TYPE::FUNCTION_REFERENCE );
	HObjectFactory& of( frame_->thread()->object_factory() );
	HHuginn::value_t closure( of.create_object( of.none_class(), args ) );
	values.push( of.create_bound_method( static_cast<huginn::HFunctionReference*>( f.raw() )->function(), closure ) );
	return;
	M_EPILOG
}

namespace {

struct ValuePosition {
	HHuginn::value_t _value;
	int _position;
	ValuePosition( HHuginn::value_t const& val_, int pos_ )
		: _value( val_ ), _position( pos_ ) {
	}
};
typedef yaal::hcore::HArray<ValuePosition> positioned_values_t;

template <typename assoc_t>
inline HHuginn::value_t fill_assoc( HThread* thread_, HHuginn::value_t&& assoc_, positioned_values_t const& values_ ) {
	assoc_t* dict( static_cast<assoc_t*>( assoc_.raw() ) );
	for ( int i( 0 ), S( static_cast<int>( values_.get_size() ) ); i < S; i += 2 ) {
		dict->insert( thread_, values_[i]._value, values_[i + 1]._value, values_[i]._position );
	}
	return ( yaal::move( assoc_ ) );
}

}

void HExpression::make_assoc( OPERATOR op_, HFrame* frame_, int ) {
	M_PROLOG
	int& ip( frame_->ip() );
	M_ASSERT( ip < static_cast<int>( _instructions.get_size() ) );
	positioned_values_t values;
	while ( _instructions[ip]._operator == OPERATOR::FUNCTION_ARGUMENT ) {
		values.emplace_back( yaal::move( frame_->values().top() ), _instructions[ip]._position );
		++ ip;
		M_ASSERT( ip < static_cast<int>( _instructions.get_size() ) );
		frame_->values().pop();
	}
	M_ASSERT( ( values.get_size() % 2 ) == 0 );
	M_ASSERT( _instructions[ip]._operator == OPERATOR::FUNCTION_CALL );
	++ ip;
	reverse( values.begin(), values.end() );
	HThread* t( frame_->thread() );
	HHuginn::value_t v(
		op_ == OPERATOR::MAKE_DICT
		? fill_assoc<huginn::HDict>( t, t->object_factory().create_dict(), values )
		: fill_assoc<huginn::HLookup>( t, t->object_factory().create_lookup(), values )
	);
	frame_->values().push( v );
	return;
	M_EPILOG
}

void HExpression::plus( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::PLUS );
	int p( _instructions[frame_->ip()]._position );
	++ frame_->ip();
	HHuginn::value_t v2( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	HHuginn::value_t& v1( frame_->values().top() );
	HClass const* c1( v1->get_class() );
	HClass const* c2( v2->get_class() );
	if ( c1 != c2 ) {
		operands_type_mismatch( op_to_str( OPERATOR::PLUS ), c1, c2, file_id(), p );
	}
	if ( ! v1.unique() ) {
		v1 = v1->clone( frame_->thread(), &v1, p );
	}
	v1->operator_add( frame_->thread(), v1, v2, p );
	return;
	M_EPILOG
}

template<typename huginn_type, typename operator_type>
void HExpression::oper_assign_ref(
	HFrame* frame_,
	OPERATOR op_,
	HHuginn::TYPE type_,
	class_getter_t classGetter_,
	operator_type operator_,
	typename huginn_type::value_type const& other_
) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == op_ );
	int p( _instructions[frame_->ip()]._position );
	++ frame_->ip();
	HFrame::values_t& values( frame_->values() );
	HHuginn::value_t refVal( yaal::move( values.top() ) );
	values.pop();
	M_ASSERT( refVal->type_id() == HHuginn::TYPE::REFERENCE );
	HReference& ref( *static_cast<HReference*>( refVal.raw() ) );
	HThread* t( frame_->thread() );
	HHuginn::value_t v( ref.get( t, p ) );
	HClass const* c( v->get_class() );
	if ( c->type_id() != type_ ) {
		operands_type_mismatch( op_to_str( op_ ), c, (t->object_factory().*classGetter_)(), file_id(), p );
	}
	operator_( static_cast<huginn_type*>( v.raw() )->value(), other_ );
	ref.set( t, yaal::move( v ), p );
	values.push( ref.get( t, p ) );
	return;
	M_EPILOG
}

template<typename huginn_type, typename operator_type>
void HExpression::oper_assign_val(
	HFrame* frame_,
	OPERATOR op_,
	HHuginn::TYPE type_,
	class_getter_t classGetter_,
	operator_type operator_,
	typename huginn_type::value_type const& other_
) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == op_ );
	int p( _instructions[frame_->ip()]._position );
	++ frame_->ip();
	HHuginn::value_t& v( frame_->values().top() );
	HClass const* c( v->get_class() );
	if ( c->type_id() != type_ ) {
		operands_type_mismatch( op_to_str( op_ ), c, (frame_->thread()->object_factory().*classGetter_)(), file_id(), p );
	}
	operator_( static_cast<huginn_type*>( v.raw() )->value(), other_ );
	return;
	M_EPILOG
}

template<OPERATOR oper>
void HExpression::oper_assign_integer_ref( OExecutionStep const& es_, HFrame* frame_ ) {
	M_PROLOG
	typedef operator_dispatcher<oper> operator_dispatcher_type;
	oper_assign_ref<HInteger>(
		frame_,
		oper,
		HHuginn::TYPE::INTEGER,
		&HObjectFactory::integer_class,
		&operator_dispatcher_type::template self<HInteger::value_type>,
		es_._integer
	);
	return;
	M_EPILOG
}

template<OPERATOR oper>
void HExpression::oper_assign_integer_val( OExecutionStep const& es_, HFrame* frame_ ) {
	M_PROLOG
	typedef operator_dispatcher<oper> operator_dispatcher_type;
	oper_assign_val<HInteger>(
		frame_,
		oper,
		HHuginn::TYPE::INTEGER,
		&HObjectFactory::integer_class,
		&operator_dispatcher_type::template self<HInteger::value_type>,
		es_._integer
	);
	return;
	M_EPILOG
}

template<OPERATOR oper>
void HExpression::oper_assign_real_ref( OExecutionStep const& es_, HFrame* frame_ ) {
	M_PROLOG
	typedef operator_dispatcher<oper> operator_dispatcher_type;
	oper_assign_ref<HReal>(
		frame_,
		oper,
		HHuginn::TYPE::REAL,
		&HObjectFactory::real_class,
		&operator_dispatcher_type::template self<HReal::value_type>,
		es_._real
	);
	return;
	M_EPILOG
}

template<OPERATOR oper>
void HExpression::oper_assign_real_val( OExecutionStep const& es_, HFrame* frame_ ) {
	M_PROLOG
	typedef operator_dispatcher<oper> operator_dispatcher_type;
	oper_assign_val<HReal>(
		frame_,
		oper,
		HHuginn::TYPE::REAL,
		&HObjectFactory::real_class,
		&operator_dispatcher_type::template self<HReal::value_type>,
		es_._real
	);
	return;
	M_EPILOG
}

void HExpression::plus_assign_number( OExecutionStep const&, HFrame* ) {
	M_PROLOG
	return;
	M_EPILOG
}

void HExpression::plus_assign_string( OExecutionStep const&, HFrame* ) {
	M_PROLOG
	return;
	M_EPILOG
}

void HExpression::plus_integer( OExecutionStep const&, HFrame* ) {
	M_PROLOG
	return;
	M_EPILOG
}

void HExpression::plus_real( OExecutionStep const&, HFrame* ) {
	M_PROLOG
	return;
	M_EPILOG
}

void HExpression::plus_number( OExecutionStep const&, HFrame* ) {
	M_PROLOG
	return;
	M_EPILOG
}

void HExpression::plus_string( OExecutionStep const&, HFrame* ) {
	M_PROLOG
	return;
	M_EPILOG
}

void HExpression::minus( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::MINUS );
	int p( _instructions[frame_->ip()]._position );
	++ frame_->ip();
	HHuginn::value_t v2( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	HHuginn::value_t& v1( frame_->values().top() );
	HClass const* c1( v1->get_class() );
	HClass const* c2( v2->get_class() );
	if ( c1 != c2 ) {
		operands_type_mismatch( op_to_str( OPERATOR::MINUS ), c1, c2, file_id(), p );
	}
	if ( ! v1.unique() ) {
		v1 = v1->clone( frame_->thread(), &v1, p );
	}
	v1->operator_subtract( frame_->thread(), v1, v2, p );
	return;
	M_EPILOG
}

void HExpression::mul( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::MULTIPLY );
	int p( _instructions[frame_->ip()]._position );
	++ frame_->ip();
	HHuginn::value_t v2( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	HHuginn::value_t& v1( frame_->values().top() );
	HClass const* c1( v1->get_class() );
	HClass const* c2( v2->get_class() );
	if ( c1 != c2 ) {
		operands_type_mismatch( op_to_str( OPERATOR::MULTIPLY ), c1, c2, file_id(), p );
	}
	if ( ! v1.unique() ) {
		v1 = v1->clone( frame_->thread(), &v1, p );
	}
	v1->operator_multiply( frame_->thread(), v1, v2, p );
	return;
	M_EPILOG
}

void HExpression::div( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::DIVIDE );
	int p( _instructions[frame_->ip()]._position );
	++ frame_->ip();
	HHuginn::value_t v2( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	HHuginn::value_t& v1( frame_->values().top() );
	HClass const* c1( v1->get_class() );
	HClass const* c2( v2->get_class() );
	if ( c1 != c2 ) {
		operands_type_mismatch( op_to_str( OPERATOR::DIVIDE ), c1, c2, file_id(), p );
	}
	if ( ! v1.unique() ) {
		v1 = v1->clone( frame_->thread(), &v1, p );
	}
	v1->operator_divide( frame_->thread(), v1, v2, p );
	return;
	M_EPILOG
}

void HExpression::mod( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::MODULO );
	int p( _instructions[frame_->ip()]._position );
	++ frame_->ip();
	HHuginn::value_t v2( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	HHuginn::value_t& v1( frame_->values().top() );
	HClass const* c1( v1->get_class() );
	HClass const* c2( v2->get_class() );
	if ( c1 != c2 ) {
		operands_type_mismatch( op_to_str( OPERATOR::MODULO ), c1, c2, file_id(), p );
	}
	if ( ! v1.unique() ) {
		v1 = v1->clone( frame_->thread(), &v1, p );
	}
	v1->operator_modulo( frame_->thread(), v1, v2, p );
	return;
	M_EPILOG
}

void HExpression::factorial( OExecutionStep const&, huginn::HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::FACTORIAL );
	int p( _instructions[frame_->ip()]._position );
	++ frame_->ip();
	M_ASSERT( ! frame_->values().is_empty() );
	HHuginn::value_t v( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	frame_->values().push( v->operator_factorial( frame_->thread(), v, p ) );
	return;
	M_EPILOG
}

void HExpression::negate( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::NEGATE );
	int p( _instructions[frame_->ip()]._position );
	++ frame_->ip();
	M_ASSERT( ! frame_->values().is_empty() );
	HHuginn::value_t v( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	frame_->values().push( v->operator_negate( frame_->thread(), v, p ) );
	return;
	M_EPILOG
}

void HExpression::power( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	int& ip( frame_->ip() );
	while ( ( ip < static_cast<int>( _instructions.get_size() ) ) && ( _instructions[ip]._operator == OPERATOR::POWER ) ) {
		int p( _instructions[ip]._position );
		++ ip;
		HHuginn::value_t v2( yaal::move( frame_->values().top() ) );
		frame_->values().pop();
		HHuginn::value_t& v1( frame_->values().top() );
		HClass const* c1( v1->get_class() );
		HClass const* c2( v2->get_class() );
		if ( c1 != c2 ) {
			operands_type_mismatch( op_to_str( OPERATOR::POWER ), c1, c2, file_id(), p );
		}
		if ( ! v1.unique() ) {
			v1 = v1->clone( frame_->thread(), &v1, p );
		}
		v1->operator_power( frame_->thread(), v1, v2, p );
	}
	M_ASSERT( ( ip < static_cast<int>( _instructions.get_size() ) ) && (  _instructions[ip]._operator == OPERATOR::POWER_TERM ) );
	++ ip;
	return;
	M_EPILOG
}

void HExpression::subscript( OExecutionStep const& executionStep_, HFrame* frame_ ) {
	M_PROLOG
	int& ip( frame_->ip() );
	M_ASSERT( ip < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[ip]._operator == OPERATOR::FUNCTION_ARGUMENT );
	++ ip;
	HFrame::values_t& values( frame_->values() );
	HHuginn::value_t index( yaal::move( values.top() ) );
	values.pop();
	M_ASSERT( ip < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[ip]._operator == OPERATOR::SUBSCRIPT );
	int p( _instructions[ip]._position );
	++ ip;
	HHuginn::value_t base( yaal::move( values.top() ) );
	values.pop();
	if ( executionStep_._access == HFrame::ACCESS::VALUE ) {
		values.push( base->operator_subscript( frame_->thread(), base, index, p ) );
	} else {
		HObjectFactory& of( frame_->thread()->object_factory() );
		values.push( of.create<HSubscriptReference>( of.reference_class(), yaal::move( base ), yaal::move( index ), p ) );
	}
	return;
	M_EPILOG
}

void HExpression::range( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	int& ip( frame_->ip() );
	M_ASSERT( ip < static_cast<int>( _instructions.get_size() ) );
	HHuginn::value_t from;
	HHuginn::value_t to;
	HHuginn::value_t step;
	HHuginn::value_t* v( &step );
	OPERATOR op( _instructions[ip]._operator );
	M_ASSERT( ( op == OPERATOR::FUNCTION_ARGUMENT ) || ( op == OPERATOR::SUBSCRIPT_ARGUMENT ) );
	int rangeOpCount( 0 );
	while ( ( op == OPERATOR::FUNCTION_ARGUMENT ) || ( op == OPERATOR::SUBSCRIPT_ARGUMENT ) ) {
		if ( op == OPERATOR::FUNCTION_ARGUMENT ) {
			*v = yaal::move( frame_->values().top() );
			frame_->values().pop();
		} else {
			if ( rangeOpCount == 0 ) {
				v = &to;
			} else {
				v = &from;
			}
			++ rangeOpCount;
		}
		++ ip;
		M_ASSERT( ip < static_cast<int>( _instructions.get_size() ) );
		op = _instructions[ip]._operator;
	}
	M_ASSERT( rangeOpCount <= 2 );
	M_ASSERT( op == OPERATOR::SUBSCRIPT );
	int p( _instructions[ip]._position );
	++ ip;
	bool select( ( !! from ) || ( !! to ) || ( !! step ) );
	HHuginn::value_t base( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	if ( ! select && ( rangeOpCount > 0 ) ) {
		HHuginn::type_id_t t( base->type_id() );
		if ( ( t != HHuginn::TYPE::TUPLE ) && ( t != HHuginn::TYPE::LIST ) && ( t != HHuginn::TYPE::DEQUE ) && ( t != HHuginn::TYPE::STRING ) ) {
			throw HHuginn::HHuginnRuntimeException( "Range operator not supported on `"_ys.append( base->get_class()->name() ).append( "`." ), file_id(), p );
		}
		frame_->values().push( yaal::move( base ) );
	} else {
		if ( rangeOpCount == 1 ) {
			from = yaal::move( to );
			to = yaal::move( step );
		}
		frame_->values().push( base->operator_range( frame_->thread(), base, from, to, step, p ) );
	}
	return;
	M_EPILOG
}

void HExpression::equals( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::EQUALS );
	int p( _instructions[frame_->ip()]._position );
	++ frame_->ip();
	HHuginn::value_t v2( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	HHuginn::value_t& v1( frame_->values().top() );
	HClass const* c1( v1->get_class() );
	HClass const* c2( v2->get_class() );
	if ( ( c1 != c2 ) && ( c1->type_id() != HHuginn::TYPE::NONE ) && ( c2->type_id() != HHuginn::TYPE::NONE ) ) {
		operands_type_mismatch( op_to_str( OPERATOR::EQUALS ), c1, c2, file_id(), p );
	}
	v1 = frame_->thread()->runtime().boolean_value( v1->operator_equals( frame_->thread(), v1, v2, p ) );
	return;
	M_EPILOG
}

void HExpression::not_equals( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::NOT_EQUALS );
	int p( _instructions[frame_->ip()]._position );
	++ frame_->ip();
	HHuginn::value_t v2( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	HHuginn::value_t& v1( frame_->values().top() );
	HClass const* c1( v1->get_class() );
	HClass const* c2( v2->get_class() );
	if ( ( c1 != c2 ) && ( c1->type_id() != HHuginn::TYPE::NONE ) && ( c2->type_id() != HHuginn::TYPE::NONE ) ) {
		operands_type_mismatch( op_to_str( OPERATOR::NOT_EQUALS ), c1, c2, file_id(), p );
	}
	v1 = frame_->thread()->runtime().boolean_value( ! v1->operator_equals( frame_->thread(), v1, v2, p ) );
	return;
	M_EPILOG
}

void HExpression::less( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::LESS );
	int p( _instructions[frame_->ip()]._position );
	++ frame_->ip();
	HHuginn::value_t v2( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	HHuginn::value_t& v1( frame_->values().top() );
	HClass const* c1( v1->get_class() );
	HClass const* c2( v2->get_class() );
	if ( c1 != c2 ) {
		operands_type_mismatch( op_to_str( OPERATOR::LESS ), c1, c2, file_id(), p );
	}
	v1 = frame_->thread()->runtime().boolean_value( v1->operator_less( frame_->thread(), v1, v2, p ) );
	return;
	M_EPILOG
}

void HExpression::greater( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::GREATER );
	int p( _instructions[frame_->ip()]._position );
	++ frame_->ip();
	HHuginn::value_t v2( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	HHuginn::value_t& v1( frame_->values().top() );
	HClass const* c1( v1->get_class() );
	HClass const* c2( v2->get_class() );
	if ( c1 != c2 ) {
		operands_type_mismatch( op_to_str( OPERATOR::GREATER ), c1, c2, file_id(), p );
	}
	v1 = frame_->thread()->runtime().boolean_value( v1->operator_greater( frame_->thread(), v1, v2, p ) );
	return;
	M_EPILOG
}

void HExpression::less_or_equal( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::LESS_OR_EQUAL );
	int p( _instructions[frame_->ip()]._position );
	++ frame_->ip();
	HHuginn::value_t v2( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	HHuginn::value_t& v1( frame_->values().top() );
	HClass const* c1( v1->get_class() );
	HClass const* c2( v2->get_class() );
	if ( c1 != c2 ) {
		operands_type_mismatch( op_to_str( OPERATOR::LESS_OR_EQUAL ), c1, c2, file_id(), p );
	}
	v1 = frame_->thread()->runtime().boolean_value( v1->operator_less_or_equal( frame_->thread(), v1, v2, p ) );
	return;
	M_EPILOG
}

void HExpression::greater_or_equal( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::GREATER_OR_EQUAL );
	int p( _instructions[frame_->ip()]._position );
	++ frame_->ip();
	HHuginn::value_t v2( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	HHuginn::value_t& v1( frame_->values().top() );
	HClass const* c1( v1->get_class() );
	HClass const* c2( v2->get_class() );
	if ( c1 != c2 ) {
		operands_type_mismatch( op_to_str( OPERATOR::GREATER_OR_EQUAL ), c1, c2, file_id(), p );
	}
	v1 = frame_->thread()->runtime().boolean_value( v1->operator_greater_or_equal( frame_->thread(), v1, v2, p ) );
	return;
	M_EPILOG
}

void HExpression::is_element_of( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::IS_ELEMENT_OF );
	int p( _instructions[frame_->ip()]._position );
	++ frame_->ip();
	HHuginn::value_t v2( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	HHuginn::value_t& v1( frame_->values().top() );
	HClass const* c2( v2->get_class() );
	if ( ( c2->type_id() <= type_id( HHuginn::TYPE::UNKNOWN ) ) && ! is_collection_like( c2 ) ) {
		throw HHuginn::HHuginnRuntimeException( hcore::to_string( _errMsgHHuginn_[ERR_CODE::OP_NOT_COLL] ).append( a_type_name( c2 ) ), file_id(), p );
	}
	v1 = frame_->thread()->runtime().boolean_value( v2->operator_contains( frame_->thread(), v2, v1, p ) );
	return;
	M_EPILOG
}

void HExpression::is_not_element_of( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::IS_NOT_ELEMENT_OF );
	int p( _instructions[frame_->ip()]._position );
	++ frame_->ip();
	HHuginn::value_t v2( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	HHuginn::value_t& v1( frame_->values().top() );
	HClass const* c2( v2->get_class() );
	if ( ( c2->type_id() <= type_id( HHuginn::TYPE::UNKNOWN ) ) && ! is_collection_like( c2 ) ) {
		throw HHuginn::HHuginnRuntimeException( hcore::to_string( _errMsgHHuginn_[ERR_CODE::OP_NOT_COLL] ).append( a_type_name( c2 ) ), file_id(), p );
	}
	v1 = frame_->thread()->runtime().boolean_value( ! v2->operator_contains( frame_->thread(), v2, v1, p ) );
	return;
	M_EPILOG
}

void HExpression::boolean_and( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::BOOLEAN_AND );
	++ frame_->ip();
	HHuginn::value_t v( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	M_ASSERT( dynamic_cast<HBooleanEvaluator*>( v.raw() ) );
	frame_->values().push( frame_->thread()->runtime().boolean_value( static_cast<HBooleanEvaluator*>( v.raw() )->execute( frame_->thread() ) ) );
	return;
	M_EPILOG
}

void HExpression::boolean_or( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::BOOLEAN_OR );
	++ frame_->ip();
	HHuginn::value_t v( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	M_ASSERT( dynamic_cast<HBooleanEvaluator*>( v.raw() ) );
	frame_->values().push( frame_->thread()->runtime().boolean_value( static_cast<HBooleanEvaluator*>( v.raw() )->execute( frame_->thread() ) ) );
	return;
	M_EPILOG
}

void HExpression::boolean_xor( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::BOOLEAN_XOR );
	int p( _instructions[frame_->ip()]._position );
	++ frame_->ip();
	HHuginn::value_t v2( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	HHuginn::value_t v1( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	if ( ( v1->type_id() != HHuginn::TYPE::BOOLEAN ) || ( v2->type_id() != HHuginn::TYPE::BOOLEAN ) ) {
		throw HHuginn::HHuginnRuntimeException(
			hcore::to_string( _errMsgHHuginn_[ERR_CODE::OPS_NOT_BOOL] )
				.append( a_type_name( v1->get_class() ) )
				.append( ", " )
				.append( a_type_name( v2->get_class() ) ),
			file_id(),
			p
		);
	}
	frame_->values().push( instruction::boolean_xor( frame_->thread(), v1, v2, p ) );
	return;
	M_EPILOG
}

void HExpression::boolean_not( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::BOOLEAN_NOT );
	int p( _instructions[frame_->ip()]._position );
	++ frame_->ip();
	HHuginn::value_t& v( frame_->values().top() );
	if ( v->type_id() != HHuginn::TYPE::BOOLEAN ) {
		throw HHuginn::HHuginnRuntimeException( hcore::to_string( _errMsgHHuginn_[ERR_CODE::OP_NOT_BOOL] ).append( v->get_class()->name() ), file_id(), p );
	}
	v = instruction::boolean_not( frame_->thread(), v, p );
	return;
	M_EPILOG
}

void HExpression::ternary( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::TERNARY );
	++ frame_->ip();
	HHuginn::value_t v( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	M_ASSERT( dynamic_cast<HTernaryEvaluator*>( v.raw() ) );
	frame_->values().push( static_cast<HTernaryEvaluator*>( v.raw() )->execute( frame_->thread() ) );
	return;
	M_EPILOG
}

void HExpression::store_direct( OExecutionStep const& executionStep_, HFrame* frame_ ) {
	M_PROLOG
	frame_->values().push( executionStep_._value );
	return;
	M_EPILOG
}

void HExpression::store_external_reference( OExecutionStep const& executionStep_, huginn::HFrame* frame_ ) {
	M_PROLOG
	frame_->values().push( *executionStep_._valueRef );
	return;
	M_EPILOG
}

void HExpression::store_real( OExecutionStep const& executionStep_, HFrame* frame_ ) {
	M_PROLOG
	frame_->values().push( frame_->thread()->object_factory().create_real( executionStep_._real ) );
	return;
	M_EPILOG
}

void HExpression::store_integer( OExecutionStep const& executionStep_, HFrame* frame_ ) {
	M_PROLOG
	frame_->values().push( frame_->thread()->object_factory().create_integer( executionStep_._integer ) );
	return;
	M_EPILOG
}

void HExpression::store_string( OExecutionStep const& executionStep_, HFrame* frame_ ) {
	M_PROLOG
	frame_->values().push( frame_->thread()->runtime().object_factory()->create_string( executionStep_._string ) );
	return;
	M_EPILOG
}

void HExpression::store_number( OExecutionStep const& executionStep_, HFrame* frame_ ) {
	M_PROLOG
	frame_->values().push( frame_->thread()->object_factory().create_number( executionStep_._number ) );
	return;
	M_EPILOG
}

void HExpression::store_character( OExecutionStep const& executionStep_, HFrame* frame_ ) {
	M_PROLOG
	frame_->values().push( frame_->thread()->object_factory().create_character( executionStep_._character ) );
	return;
	M_EPILOG
}

HHuginn::value_t HExpression::evaluate( huginn::HThread* thread_ ) const {
	M_PROLOG
		M_ASSERT( _operations.is_empty() );
		HFrame* f( thread_->current_frame() );
		f->start_expression();
		try {
			for ( OExecutionStep const& e : _executionSteps ) {
				( e._expression->*e._action )( e, f );
				if ( ! thread_->can_continue() ) {
					break;
				}
			}
		} catch ( instruction::Interrupt const& ) {
		}
		HHuginn::value_t result;
		if ( thread_->state() < HThread::STATE::EXCEPTION ) {
			result = yaal::move( f->values().top() );
			M_ASSERT( f->ip() == static_cast<int>( _instructions.get_size() ) );
			f->values().pop();
		} else {
			f->values().clear();
		}
		f->end_expression();
		return ( result );
	} catch ( HHuginn::HHuginnRuntimeException const& e ) {
		if ( e.position() <= 0 ) {
			throw HHuginn::HHuginnRuntimeException( e.message(), file_id(), position() );
		} else {
			throw;
		}
	M_EPILOG
}

void HExpression::do_execute( huginn::HThread* thread_ ) const {
	M_PROLOG
	HHuginn::value_t result( evaluate( thread_ ) );
	if ( _final && ( thread_->state() < HThread::STATE::EXCEPTION ) ) {
		HFrame* f( thread_->current_frame() );
		f->set_result( yaal::move( result ) );
	}
	return;
	M_EPILOG
}

void HExpression::commit( huginn::HThread* thread_, HHuginn::value_t&& value_ ) const {
	M_PROLOG
	HHuginn::value_t ref( evaluate( thread_ ) );
	M_ASSERT( ref->type_id() == HHuginn::TYPE::REFERENCE );
	static_cast<HReference*>( ref.raw() )->set( thread_, yaal::move( value_ ), position() );
	return;
	M_EPILOG
}

void HExpression::mark_final( void ) {
	_final = true;
}

}

}

}

