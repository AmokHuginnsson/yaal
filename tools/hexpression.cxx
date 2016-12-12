/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

  analyser.cxx - this file is integral part of `yaal' project.

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
#include <cstring>
#include <cstdlib>
#include <limits>
#include <libintl.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hcore/htree.hxx"
#include "hcore/hcore.hxx"
#include "hcore/math.hxx"
#include "hexpression.hxx"
#include "xmath.hxx"

using namespace yaal;
using namespace yaal::math;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

struct OPERATOR {
	static int const ADD;
	static int const SUBTRACT;
	static int const MULTIPLY;
	static int const DIVIDE;
	static int const MODULO;
	static int const FACTORIAL;
};

int const OPERATOR::ADD = '+';
int const OPERATOR::SUBTRACT = '-';
int const OPERATOR::MULTIPLY = '*';
int const OPERATOR::DIVIDE = '/';
int const OPERATOR::MODULO = '%';
int const OPERATOR::FACTORIAL = '!';

struct FUNCTION {
	static int const FUNCTIONS;
	static int const SIN;
	static int const SINH;
	static int const COS;
	static int const COSH;
	static int const TG;
	static int const TGH;
	static int const CTG;
	static int const CTGH;
	static int const ARCSIN;
	static int const ARCCOS;
	static int const ARCTG;
	static int const ARCCTG;
	static int const EXP;
	static int const SQRT;
	static int const LN;
	static int const LOG;
	static int const ABS;
	static int const ADDITION;
	static int const MULTIPLICATION;
	static int const POWER;
	static int const SIGNUM;
	static int const BRACKET;
};

int const FUNCTION::FUNCTIONS = 0;
int const FUNCTION::SIN = 2;
int const FUNCTION::SINH = 1;
int const FUNCTION::COS = 4;
int const FUNCTION::COSH = 3;
int const FUNCTION::TG = 6;
int const FUNCTION::TGH = 5;
int const FUNCTION::CTG = 8;
int const FUNCTION::CTGH = 7;
int const FUNCTION::ARCSIN = 9;
int const FUNCTION::ARCCOS = 10;
int const FUNCTION::ARCTG = 11;
int const FUNCTION::ARCCTG = 12;
int const FUNCTION::EXP = 13;
int const FUNCTION::SQRT = 14;
int const FUNCTION::LN = 15;
int const FUNCTION::LOG = 16;
int const FUNCTION::ABS = 17;
int const FUNCTION::ADDITION = 1;
int const FUNCTION::MULTIPLICATION = 2;
int const FUNCTION::POWER = 3;
int const FUNCTION::SIGNUM = 4;
int const FUNCTION::BRACKET = 5;


char const* const _syntaxError_ = _( "syntax error" );
char _functionsMnemonics_[][ 8 ] = {
	"sinh\0\0\0", "sin\0\0\0\0",
	"cosh\0\0\0", "cos\0\0\0\0",
	"tgh\0\0\0\0", "tg\0\0\0\0\0",
	"ctgh\0\0\0", "ctg\0\0\0\0",
	"arcsin\0", "arccos\0",
	"arctg\0\0", "arcctg\0",
	"exp\0\0\0\0", "sqrt\0\0\0",
	"ln\0\0\0\0\0", "log\0\0\0\0"
};

int _functionMnemonicsLength_[ 16 ] = {
	4, 3, 4, 3, 3, 2, 4, 3, 6, 6, 5, 6, 3, 4, 2, 3
};

HExpression::HExpression( void )
	: _index( 0 )
	, _length( 0 )
	, _error( OK )
	, _variables()
	, _constantsPool()
	, _terminalIndexes()
	, _formula()
	, _equationTree() {
	M_PROLOG
	return;
	M_EPILOG
}

HExpression::HExpression( HString const& formula )
	: _index( 0 )
	, _length( 0 )
	, _error( OK )
	, _variables()
	, _constantsPool()
	, _terminalIndexes()
	, _formula()
	, _equationTree() {
	M_PROLOG
	compile( formula );
	return;
	M_EPILOG
}

HExpression::~HExpression( void ) {
	M_PROLOG
	return;
	M_DESTRUCTOR_EPILOG
}

HExpression::HExpression( HExpression const& ex )
	: _index( ex._index )
	, _length( ex._length )
	, _error( ex._error )
	, _variables()
	, _constantsPool( ex._constantsPool )
	, _terminalIndexes( ex._terminalIndexes )
	, _formula( ex._formula )
	, _equationTree( ex._equationTree ) {
	M_PROLOG
	yaal::copy( ex._variables, ex._variables + MAX_VARIABLE_COUNT, _variables );
	return;
	M_EPILOG
}

HExpression& HExpression::operator = ( HExpression const& ex ) {
	M_PROLOG
	if ( &ex != this ) {
		HExpression tmp( ex );
		swap( tmp );
	}
	return ( *this );
	M_EPILOG
}

void HExpression::swap( HExpression& ex ) {
	M_PROLOG
	if ( &ex != this ) {
		using yaal::swap;
		swap( _index, ex._index );
		swap( _length, ex._length );
		swap( _error, ex._error );
		yaal::swap_ranges( _variables, _variables + MAX_VARIABLE_COUNT, ex._variables );
		_constantsPool.swap( ex._constantsPool );
		_terminalIndexes.swap( ex._terminalIndexes );
		_formula.swap( ex._formula );
		_equationTree.swap( ex._equationTree );
	}
	return;
	M_EPILOG
}

double long HExpression::count_branch( tree_t::const_node_t node_ ) {
	M_PROLOG
	double long value( 0 );
	if ( node_->has_childs() ) {
		value = ( this->*( (*node_)->METHOD ) )( node_ );
	} else {
		int index( (*node_)->_variables.head() );
		if ( index >= 0 ) {
			value = _variables[ index ];
		} else {
			value = _constantsPool[ -1 - index ];
		}
	}
	return ( value );
	M_EPILOG
}

double long HExpression::functions( tree_t::const_node_t node_ ) {
	M_PROLOG
	int function = (*node_)->_variables.tail();
	double long leftValue = 0;
	leftValue = count_branch( &*node_->begin() );
	double long retVal = 0;
	switch ( function ) {
		case ( FUNCTION::SIN ):
			retVal = sinus( leftValue );
		break;
		case ( FUNCTION::SINH ):
			retVal = hyperbolic_sinus( leftValue );
		break;
		case ( FUNCTION::COS ):
			retVal = cosinus( leftValue );
		break;
		case ( FUNCTION::COSH ):
			retVal = hyperbolic_cosinus( leftValue );
		break;
		case ( FUNCTION::TG	): {
			if ( eq( ::std::floor( leftValue / PI + .5 ), ( leftValue / PI + .5 ) ) ) {
				throw HExpressionException( _( "tg: argument not in domain" ) );
			}
			retVal = tangens( leftValue );
		}
		break;
		case ( FUNCTION::TGH ):
			retVal = hyperbolic_tangens( leftValue );
		break;
		case ( FUNCTION::CTG ): {
			if ( eq( ::std::floor( leftValue / PI ), ( leftValue / PI ) ) ) {
				throw HExpressionException( _( "ctg: argument not in domain" ) );
			}
			if ( leftValue == 0 ) {
				leftValue =  std::numeric_limits<double long>::epsilon();
			}
			retVal = cotangens( leftValue );
		}
		break;
		case ( FUNCTION::CTGH ): {
			leftValue = hyperbolic_tangens( leftValue );
			if ( leftValue == 0 ) {
				leftValue = std::numeric_limits<double long>::epsilon();
			}
			retVal = 1. / leftValue;
		}
		break;
		case ( FUNCTION::ARCSIN ): {
			if ( ( leftValue < - PI / 2 ) || ( leftValue > PI / 2 ) ) {
				throw HExpressionException( _( "arcsin: argument not in domain" ) );
			}
			retVal = arcus_sinus( leftValue );
		}
		break;
		case ( FUNCTION::ARCCOS ): {
			if ( ( leftValue < - PI / 2 ) || ( leftValue > PI / 2 ) ) {
				throw HExpressionException( _( "arccos: argument not in domain" ) );
			}
			retVal = arcus_cosinus( leftValue );
		}
		break;
		case ( FUNCTION::ARCTG ):
			retVal = arcus_tangens( leftValue );
		break;
		case ( FUNCTION::ARCCTG ):
			retVal = arcus_cotangens( leftValue );
		break;
		case ( FUNCTION::EXP ):
			retVal = natural_exponential( leftValue );
		break;
		case ( FUNCTION::SQRT ): {
			if ( leftValue < 0 ) {
				throw HExpressionException( _( "square_root: argument not in domain" ) );
			}
			retVal = square_root( leftValue );
		}
		break;
		case ( FUNCTION::LN ): {
			if ( leftValue <= 0 ) {
				throw HExpressionException( _( "ln: argument not in domain" ) );
			}
			retVal = natural_logarithm( leftValue );
		}
		break;
		case ( FUNCTION::LOG ): {
			if ( leftValue <= 0 )
				throw HExpressionException( _( "log: argument not in domain" ) );
			retVal = ::std::log10( leftValue );
		}
		break;
		case ( FUNCTION::ABS ):
			retVal = ( leftValue < 0 ) ? -leftValue : leftValue;
		break;
		default:
			M_THROW( _( "unknown function type" ), function );
		break;
	}
	return ( retVal );
	M_EPILOG
}

double long HExpression::addition( tree_t::const_node_t node_ ) {
	M_PROLOG
	double long leftValue = 0, rightValue = 0;
	tree_t::HNode::const_iterator it = node_->begin();
	int_list_t::const_iterator var = (*node_)->_variables.begin();
	leftValue = count_branch( &*it );
	while ( ( ++ it ) != node_->end() ) {
		int op( *var );
		++ var;
		rightValue = count_branch( &*it );
		switch ( op ) {
			case ( OPERATOR::ADD ) :
				leftValue += rightValue;
			break;
			case ( OPERATOR::SUBTRACT ) :
				leftValue -= rightValue;
			break;
			default:
				M_THROW( _( "unknown addition operator" ), op );
			break;
		}
	}
	return ( leftValue );
	M_EPILOG
}

double long HExpression::multiplication( tree_t::const_node_t node_ ) {
	M_PROLOG
	double long leftValue = 0, rightValue = 0;
	tree_t::HNode::const_iterator it = node_->begin();
	int_list_t::const_iterator var = (*node_)->_variables.begin();
	leftValue = count_branch( &*it );
	while ( ( ++ it ) != node_->end() ) {
		int op =  *var;
		++ var;
		rightValue = count_branch( &*it );
		switch ( op ) {
			case ( OPERATOR::MULTIPLY ) :
				leftValue *= rightValue;
			break;
			case ( OPERATOR::DIVIDE ) :
				if ( rightValue == 0.0l )
					throw HExpressionException( _( "divide by 0" ) );
				leftValue /= rightValue;
			break;
			case ( OPERATOR::MODULO ) :
				if ( ! static_cast<int long>( rightValue ) )
					throw HExpressionException( _( "modulo by 0" ) );
				leftValue = static_cast<double long>( static_cast<int long>( leftValue ) % static_cast<int long>( rightValue ) );
			break;
			default:
				M_THROW( _( "unknown multiplication operator" ), op );
			break;
		}
	}
	return ( leftValue );
	M_EPILOG
}

double long HExpression::power( tree_t::const_node_t node_ ) {
	M_PROLOG
	double long leftValue, rightValue;
	tree_t::HNode::const_iterator it = node_->begin();
	leftValue = count_branch ( &*it );
	rightValue = count_branch ( &*( ++ it ) );
	if ( ( leftValue < 0 )
			&& ( ! eq( rightValue, ::floorl( rightValue ) ) ) )
		return ( 0 );
	return ( ::powl( leftValue, rightValue ) );
	M_EPILOG
}

double long HExpression::signum( tree_t::const_node_t node_ ) {
	M_PROLOG
	double long leftValue;
	leftValue = count_branch( &*node_->begin() );
	return ( -leftValue );
	M_EPILOG
}

double long HExpression::factorial( tree_t::const_node_t node_ ) {
	M_PROLOG
	double long leftValue;
	leftValue = count_branch( &*node_->begin() );
	return ( static_cast<double long>( xmath::factorial( static_cast<int long long unsigned>( leftValue ) ) ) );
	M_EPILOG
}

double long HExpression::bracket( tree_t::const_node_t node_ ) {
	M_PROLOG
	double long leftValue;
	leftValue = count_branch( &*node_->begin() );
	return ( leftValue );
	M_EPILOG
}

bool HExpression::translate( HString const& formula_ ) {
	M_PROLOG
	int index = 0, realIndex = 0, ctr = 0;
	int long length = formula_.get_length();
	_formula.reserve( length );
	_formula.fillz( '\0', 0, length );
	_terminalIndexes.resize( length + 1 );
	while ( index < length ) {
		_terminalIndexes[ realIndex ] = index;
		if ( ( formula_[ index ] >= 'a' )
				&& ( formula_[ index ] <= 'z' ) ) {
			for ( ctr = 0; ctr < 16; ctr++ ) {
				if (
					strncmp(
						formula_.raw() + index,
						_functionsMnemonics_[ ctr ],
						static_cast<size_t>( _functionMnemonicsLength_[ ctr ] )
					) == 0
				) {
					_formula.set_at( realIndex, static_cast<char>( ctr + 1 ) );
					break;
				}
			}
			if ( ctr < 16 ) {
				index += _functionMnemonicsLength_[ ctr ];
				realIndex ++;
			} else {
				_error = UNKNOWN_MNEMONIC;
				_index = realIndex;
				return ( false );
			}
		} else {
			_formula.set_at( realIndex, formula_[ index ] );
			index ++;
			realIndex ++;
		}
		_length = realIndex;
	}
	_terminalIndexes[ realIndex ] = index;
	return ( true );
	M_EPILOG
}

bool HExpression::addition_production( tree_t::node_t node_ ) {
	M_PROLOG
	M_ASSERT ( node_ );
	if ( ! multiplication_production( &*node_->add_node() ) ) {
		return ( false );
	}
	if ( _index > _length ) {
		_error = UNEXPECTED_TERMINATION;
		return ( false );
	}
	(**node_).METHOD = &HExpression::addition;
	if ( ( _formula[ _index ] != OPERATOR::ADD )
			&& ( _formula[ _index ] != OPERATOR::SUBTRACT ) ) {
		shorten_the_branch( node_ );
		return ( true );
	}
	while ( ( _formula[ _index ] == OPERATOR::ADD )
			|| ( _formula[ _index ] == OPERATOR::SUBTRACT ) ) {
		(**node_)._variables.push_back( _formula[ _index ++ ] );
		if ( ! multiplication_production( &*node_->add_node() ) ) {
			return ( false );
		}
	}
	return ( true );
	M_EPILOG
}

bool HExpression::multiplication_production( tree_t::node_t node_ ) {
	M_PROLOG
	if ( ! power_production( &*node_->add_node() ) ) {
		return ( false );
	}
	if ( _index > _length ) {
		_error = UNEXPECTED_TERMINATION;
		return ( false );
	}
	(**node_).METHOD = &HExpression::multiplication;
	if ( ( _formula[ _index ] != OPERATOR::MULTIPLY )
			&& ( _formula[ _index ] != OPERATOR::DIVIDE )
			&& ( _formula[ _index ] != OPERATOR::MODULO ) ) {
		shorten_the_branch( node_ );
		return ( true );
	}
	while ( ( _formula[ _index ] == OPERATOR::MULTIPLY )
			|| ( _formula[ _index ] == OPERATOR::DIVIDE )
			|| ( _formula[ _index ] == OPERATOR::MODULO ) ) {
		(**node_)._variables.push_back( _formula[ _index ++ ] );
		if ( ! power_production( &*node_->add_node() ) ) {
			return ( false );
		}
	}
	return ( true );
	M_EPILOG
}

void HExpression::shorten_the_branch( tree_t::node_t node_ ) {
	M_PROLOG
	tree_t::node_t parent = node_->get_parent();
	if ( parent ) {
		M_ASSERT( node_->child_count() == 1 );
		tree_t::HNode::iterator it;
		for ( it = parent->begin(); it != parent->end(); ++ it ) {
			if ( &*it == node_ )
				break;
		}
		M_ASSERT( it != parent->end() );
		parent->replace_node( it, &*node_->begin() );
	}
	return;
	M_EPILOG
}

bool HExpression::power_production( tree_t::node_t node_ ) {
	M_PROLOG
	if ( ! signum_production( &*node_->add_node() ) ) {
		return ( false );
	}
	if ( _index > _length ) {
		_error = UNEXPECTED_TERMINATION;
		return ( false );
	}
	(**node_).METHOD = &HExpression::bracket;
	if ( _formula[ _index ] == '^' ) {
		++ _index;
		if ( ! power_production( &*node_->add_node() ) ) {
			return ( false );
		}
		(**node_).METHOD = &HExpression::power;
	} else {
		shorten_the_branch( node_ );
	}
	return ( true );
	M_EPILOG
}

bool HExpression::signum_production( tree_t::node_t node_ ) {
	M_PROLOG
	if ( _index > _length ) {
		_error = UNEXPECTED_TERMINATION;
		return ( false );
	}
	if ( _formula[ _index ] == '-' ) {
		++ _index;
		if ( ! factorial_production( &*node_->add_node() ) ) {
			return ( false );
		}
		(**node_).METHOD = &HExpression::signum;
	} else if ( ! factorial_production( node_ ) ) {
		return ( false );
	}
	return ( true );
	M_EPILOG
}

bool HExpression::factorial_production( tree_t::node_t node_ ) {
	M_PROLOG
	if ( ! terminal_production( &*node_->add_node() ) )
		return ( false );
	if ( _index > _length ) {
		_error = UNEXPECTED_TERMINATION;
		return ( false );
	} else if ( ( _index < _length ) && ( _formula[ _index ] == OPERATOR::FACTORIAL ) ) {
		++ _index;
		(**node_).METHOD = &HExpression::factorial;
	} else {
		shorten_the_branch( node_ );
	}
	return ( true );
	M_EPILOG
}

bool HExpression::terminal_production( tree_t::node_t node_ ) {
	M_PROLOG
	if ( _index > _length ) {
		_error = UNEXPECTED_TERMINATION;
		return ( false );
	}
	switch ( _formula[ _index ] ) {
		case '(' : {
			++ _index;
			if ( ! addition_production( node_ ) ) {
				return ( false );
			}
			if ( _formula [ _index ] != ')' ) {
				_error = CLOSING_BRACKET_EXPECTED;
				return ( false );
			} else {
				++ _index;
			}
			return ( true );
		}
		case '|' : {
			++ _index;
			if ( ! addition_production( &*node_->add_node() ) ) {
				return ( false );
			}
			(**node_).METHOD = &HExpression::functions;
			(**node_)._variables.push_back( FUNCTION::ABS );
			if ( _formula[ _index ] != '|' ) {
				_error = CLOSING_ABSOLUTE_EXPECTED;
				return ( false );
			} else {
				++ _index;
			}
			return ( true );
		}
		default:
			break;
	}
	if ( ( _formula[ _index ] >= 'A' ) && ( _formula[ _index ] <= 'Z' ) ) {
		(**node_)._variables.push_back( _formula[ _index ++ ] - 'A' );
		return ( true );
	}
	if ( ( _formula[ _index ] > FUNCTION::FUNCTIONS ) && ( _formula[ _index ] < FUNCTION::ABS ) ) {
		++ _index;
		if ( _formula[ _index ] == '(' ) {
			++ _index;
			(**node_).METHOD = &HExpression::functions;
			(**node_)._variables.push_back ( static_cast<int>( _formula[ _index - 2 ] ) );
			if ( ! addition_production ( &*node_->add_node() ) ) {
				return ( false );
			}
			if ( _formula[ _index ] != ')' ) {
				_error = CLOSING_FUNCTION_BRACKET_EXPECTED;
				return ( false );
			} else {
				++ _index;
			}
		} else {
			_error = OPENING_FUNCTION_BRACKET_EXPECTED;
			return ( false );
		}
		return ( true );
	}
	int digits( 0 );
	int offset( _index );
	while ( ( _index < _length ) && ( _formula[ _index ] >= '0' ) && ( _formula[ _index ] <= '9' ) ) {
		++ _index;
		++ digits;
	}
	if ( ( _index < _length ) && ( _formula[ _index ] == '.' ) ) {
		++ _index;
	}
	while ( ( _index < _length ) && ( _formula[ _index ] >= '0' ) && ( _formula[ _index ] <= '9' ) ) {
		++ _index;
		++ digits;
	}
	if ( digits > 0 ) {
		double long value( ::strtold( _formula.raw() + offset, nullptr ) );
		_constantsPool.push_back( value );
		/* We save variables as positive indexes and constants as negative
		 * indexes, positive and negative 0 index would conflict so
		 * we shift negative indexes by 1, so 0 becomes -1, 1 becomes -2,
		 * 2 becomes -3, and so on and so forth.
		 * HArray::size() returns current access/addition peak for revelant pool,
		 * so to get index of lately added value we need to decrement size by 1. */
		(**node_)._variables.push_back( static_cast<int>( - ( _constantsPool.size() - 1 ) - 1 ) );
		return ( true );
	} else if ( _index != offset ) {
		/* Just one alone dot found. */
		_error = DIGIT_EXPECTED;
		return ( false );
	}
	_error = UNEXPECTED_TOKEN;
	return ( false );
	M_EPILOG
}

double long* HExpression::compile( HString const& formula_ ) {
	M_PROLOG
	int long length = 0;
	_index = 0;
	_error = OK;
	length = formula_.get_length();
	if ( length == 0 ) {
		_error = PREMATURE_TERMINATION;
		throw HExpressionException( _syntaxError_ );
	}
	if ( ! translate( formula_ ) ) {
		throw HExpressionException( _syntaxError_ );
	}
	_constantsPool.clear();
	tree_t::node_t root = _equationTree.create_new_root();
	if ( addition_production( root ) ) {
		if ( ( _index < _length ) && ( _error == OK ) ) {
			_error = UNEXPECTED_TOKEN;
		}
	} else if ( _index >= _length ) {
		_error = UNEXPECTED_TERMINATION;
	}
	if ( _error != OK ) {
		_equationTree.clear();
		_constantsPool.clear();
		throw HExpressionException( _syntaxError_ );
	}
	return ( _variables );
	M_EPILOG
}

double long* HExpression::variables( void ) {
	M_PROLOG
	M_ASSERT( _error == OK );
	return ( _variables );
	M_EPILOG
}

double long& HExpression::operator[]( int index_ ) {
	M_PROLOG
	if ( ( index_ >= 'a' ) && ( index_ <= 'z' ) )
		index_ = ( index_ - 'a' ) + 'A';
	double long* val = nullptr;
	if ( ( index_ >= 0 ) && ( index_ < MAX_VARIABLE_COUNT ) ) {
		val = &_variables[ index_ ];
	} else if ( ( index_ >= 'A' ) && ( index_ <= 'Z' ) ) {
		val = &_variables[ index_ - 'A' ];
	} else {
		M_THROW( "index out of range", index_ );
	}
	return ( *val );
	M_EPILOG
}

double long HExpression::evaluate( void ) {
	M_PROLOG
	tree_t::const_node_t root = _equationTree.get_root();
	if ( ! root ) {
		M_THROW( "logic tree is not compiled", _error );
	}
	return ( ( this->*( (*root)->METHOD ) )( root ) );
	M_EPILOG
}

char const* HExpression::get_error( void ) const {
	M_PROLOG
	switch ( _error ) {
		case ( OK ):
			return ( _( "successful" ) );
		case ( UNKNOWN_MNEMONIC ):
			return ( _( "unknown mnemonic" ) );
		case ( UNEXPECTED_TERMINATION ):
			return ( _( "unexpected termination" ) );
		case ( CLOSING_BRACKET_EXPECTED ):
			return ( _( "closing bracket expected" ) );
		case ( CLOSING_ABSOLUTE_EXPECTED ):
			return ( _( "closing absolute bracket expected" ) );
		case ( CLOSING_FUNCTION_BRACKET_EXPECTED ):
			return ( _( "closing function bracket expected" ) );
		case ( OPENING_FUNCTION_BRACKET_EXPECTED ):
			return ( _( "opening function bracket expected" ) );
		case ( DIGIT_EXPECTED ):
			return ( _( "digit expected" ) );
		case ( UNEXPECTED_TOKEN ):
			return ( _( "unexpected token" ) );
		case ( PREMATURE_TERMINATION ):
			return ( _( "premature termination" ) );
		default :
			M_THROW ( _( "unknown error code" ), _error );
		break;
	}
	return ( nullptr );
	M_EPILOG
}

int HExpression::get_error_token( void ) const {
	M_PROLOG
	if ( _length > _index ) {
		return ( _terminalIndexes [ _index ] );
	} else if ( _index >= 0 ) {
		return ( _length );
	}
	return ( 0 );
	M_EPILOG
}

void swap( yaal::tools::HExpression& a, yaal::tools::HExpression& b )
	{ a.swap( b ); }

}

}

