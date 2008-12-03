/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	analyser.cxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <cmath>
#include <cstring>
#include <cstdlib>
#include <libintl.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "hcore/xalloc.hxx"
#include "hcore/htree.hxx"
#include "hcore/hcore.hxx"
#include "hexpression.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal
{

namespace tools
{

struct OPERATOR
	{
	static int const D_ADD;
	static int const D_SUBSTRACT;
	static int const D_MULTIPLY;
	static int const D_DIVIDE;
	static int const D_MODULO;
	};

int const OPERATOR::D_ADD = '+';
int const OPERATOR::D_SUBSTRACT = '-';
int const OPERATOR::D_MULTIPLY = '*';
int const OPERATOR::D_DIVIDE = '/';
int const OPERATOR::D_MODULO = '%';

struct FUNCTION
	{
	static int const D_FUNCTIONS;
	static int const D_SIN;
	static int const D_SINH;
	static int const D_COS;
	static int const D_COSH;
	static int const D_TG;
	static int const D_TGH;
	static int const D_CTG;
	static int const D_CTGH;
	static int const D_ARCSIN;
	static int const D_ARCCOS;
	static int const D_ARCTG;
	static int const D_ARCCTG;
	static int const D_EXP;
	static int const D_SQRT;
	static int const D_LN;
	static int const D_LOG;
	static int const D_ABS;
	static int const D_ADDITION;
	static int const D_MULTIPLICATION;
	static int const D_POWER;
	static int const D_SIGNUM;
	static int const D_BRACKET;
	};

int const FUNCTION::D_FUNCTIONS = 0;
int const FUNCTION::D_SIN = 2;
int const FUNCTION::D_SINH = 1;
int const FUNCTION::D_COS = 4;
int const FUNCTION::D_COSH = 3;
int const FUNCTION::D_TG = 6;
int const FUNCTION::D_TGH = 5;
int const FUNCTION::D_CTG = 8;
int const FUNCTION::D_CTGH = 7;
int const FUNCTION::D_ARCSIN = 9;
int const FUNCTION::D_ARCCOS = 10;
int const FUNCTION::D_ARCTG = 11;
int const FUNCTION::D_ARCCTG = 12;
int const FUNCTION::D_EXP = 13;
int const FUNCTION::D_SQRT = 14;
int const FUNCTION::D_LN = 15;
int const FUNCTION::D_LOG = 16;
int const FUNCTION::D_ABS = 17;
int const FUNCTION::D_ADDITION = 1;
int const FUNCTION::D_MULTIPLICATION = 2;
int const FUNCTION::D_POWER = 3;
int const FUNCTION::D_SIGNUM = 4;
int const FUNCTION::D_BRACKET = 5;
	

char const* const n_pcSyntaxError = _( "syntax error" );
char n_ppcFunctionsMnemonics [ ] [ 8 ] = 
	{
	"sinh\0\0\0", "sin\0\0\0\0", "cosh\0\0\0", "cos\0\0\0\0",
	"tgh\0\0\0\0", "tg\0\0\0\0\0", "ctgh\0\0\0", "ctg\0\0\0\0",
	"arcsin\0", "arccos\0", "arctg\0\0", "arcctg\0", "exp\0\0\0\0",
	"sqrt\0\0\0", "ln\0\0\0\0\0", "log\0\0\0\0"
	};

int n_piFunctionMnemonicsLength [ 16 ] = 
	{
	4, 3, 4, 3, 3, 2, 4, 3, 6, 6, 5, 6, 3, 4, 2, 3
	};

static double long D_PI = 3.141592653589793238462643383279502884197069399375105820974944592307816406286208998628034825342117067982148086513282306647093844;

HExpression::HExpression( void )
	: f_iIndex( 0 ), f_iLength( 0 ), f_eError( E_OK ),
	f_oConstantsPool( 0, HPool<double long>::D_AUTO_GROW ),
	f_oTerminalIndexes( 0, HPool<int>::D_AUTO_GROW ), f_oFormula(),
	f_oEquationTree()
	{
	M_PROLOG
	yaal::fill( f_pdVariables, f_pdVariables + 26, 0 );
	return;
	M_EPILOG
	}

HExpression::HExpression( HString const& formula )
	: f_iIndex( 0 ), f_iLength( 0 ), f_eError( E_OK ),
	f_oConstantsPool( 0, HPool<double long>::D_AUTO_GROW ),
	f_oTerminalIndexes( 0, HPool<int>::D_AUTO_GROW ), f_oFormula(),
	f_oEquationTree()
	{
	M_PROLOG
	yaal::fill( f_pdVariables, f_pdVariables + 26, 0 );
	compile( formula );
	return;
	M_EPILOG
	}

HExpression::~HExpression( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HExpression::HExpression( HExpression const& ex )
	: f_iIndex( ex.f_iIndex ), f_iLength( ex.f_iLength ), f_eError( ex.f_eError ),
	f_oConstantsPool( ex.f_oConstantsPool ),
	f_oTerminalIndexes( ex.f_oTerminalIndexes ), f_oFormula( ex.f_oFormula ),
	f_oEquationTree( ex.f_oEquationTree )
	{
	M_PROLOG
	yaal::copy( ex.f_pdVariables, ex.f_pdVariables + 26, f_pdVariables );
	return;
	M_EPILOG
	}

HExpression& HExpression::operator = ( HExpression const& ex )
	{
	M_PROLOG
	if ( &ex != this )
		{
		HExpression tmp( ex );
		swap( tmp );
		}
	return ( *this );
	M_EPILOG
	}

void HExpression::swap( HExpression& ex )
	{
	M_PROLOG
	if ( &ex != this )
		{
		using yaal::swap;
		swap( f_iIndex, ex.f_iIndex );
		swap( f_iLength, ex.f_iLength );
		swap( f_eError, ex.f_eError );
		f_oConstantsPool.swap( ex.f_oConstantsPool );
		f_oTerminalIndexes.swap( ex.f_oTerminalIndexes );
		f_oFormula.swap( ex.f_oFormula );
		f_oEquationTree.swap( ex.f_oEquationTree );
		yaal::swap_ranges( f_pdVariables, f_pdVariables + 26, ex.f_pdVariables );
		}
	return;
	M_EPILOG
	}

double long HExpression::count_branch( tree_t::const_node_t a_roNode )
	{
	M_PROLOG
	int l_iIndex = 0;
	double long l_dValue = 0;
	if ( a_roNode->has_childs() )
		l_dValue = ( this->*( (*a_roNode)->METHOD ) )( a_roNode );
	else
		{
		l_iIndex = (*a_roNode)->f_oVariables.head();
		if ( l_iIndex >= 0 )
			l_dValue = f_pdVariables[ l_iIndex ];
		else
			l_dValue = f_oConstantsPool[ - 1 - l_iIndex ];
		}
	return ( l_dValue );
	M_EPILOG
	}

double long HExpression::functions( tree_t::const_node_t a_roNode )
	{
	M_PROLOG
	int l_iFunction = (*a_roNode)->f_oVariables.tail();
	double long l_dLeftValue = 0;
	l_dLeftValue = count_branch( &*a_roNode->begin() );
	double long retVal = 0;
	switch ( l_iFunction )
		{
		case ( FUNCTION::D_SIN ):
			retVal = ::std::sin( l_dLeftValue );
		break;
		case ( FUNCTION::D_SINH ):
			retVal = ::std::sinh( l_dLeftValue );
		break;
		case ( FUNCTION::D_COS ):
			retVal = ::std::cos( l_dLeftValue );
		break;
		case ( FUNCTION::D_COSH ):
			retVal = ::std::cosh( l_dLeftValue );
		break;
		case ( FUNCTION::D_TG	):
			{
			if ( eq( ::std::floor( l_dLeftValue / D_PI + .5 ),
						( l_dLeftValue / D_PI + .5 ) ) )
				throw HExpressionException( _( "tg: argument not in domain" ) );
			retVal = ::std::tan( l_dLeftValue );
			}
		break;
		case ( FUNCTION::D_TGH ):
			retVal = ::std::tanh( l_dLeftValue );
		break;
		case ( FUNCTION::D_CTG ):
			{
			if ( eq( ::std::floor( l_dLeftValue / D_PI ),
						( l_dLeftValue / D_PI ) ) )
				throw HExpressionException( _( "ctg: argument not in domain" ) );
			l_dLeftValue = ::std::tan( l_dLeftValue );
			if ( l_dLeftValue == 0 )
				retVal = 0;
			retVal = 1. / l_dLeftValue;
			}
		break;
		case ( FUNCTION::D_CTGH ):
			{
			l_dLeftValue = ::std::tanh( l_dLeftValue );
			if ( l_dLeftValue == 0 )
				retVal = 0;
			retVal = 1. / l_dLeftValue;
			}
		break;
		case ( FUNCTION::D_ARCSIN ):
			{
			if ( ( l_dLeftValue < - D_PI / 2 ) || ( l_dLeftValue > D_PI / 2 ) )
				throw HExpressionException( _( "arcsin: argument not in domain" ) );
			retVal = ::std::asin( l_dLeftValue );
			}
		break;
		case ( FUNCTION::D_ARCCOS ):
			{
			if ( ( l_dLeftValue < - D_PI / 2 ) || ( l_dLeftValue > D_PI / 2 ) )
				throw HExpressionException( _( "arccos: argument not in domain" ) );
			retVal = ::std::acos( l_dLeftValue );
			}
		break;
		case ( FUNCTION::D_ARCTG ):
			retVal = ::std::atan( l_dLeftValue );
		break;
		case ( FUNCTION::D_ARCCTG ):
			retVal = D_PI - std::atan( l_dLeftValue );
		break;
		case ( FUNCTION::D_EXP ):
			retVal = ::std::exp( l_dLeftValue );
		break;
		case ( FUNCTION::D_SQRT ):
			{
			if ( l_dLeftValue < 0 )
				throw HExpressionException( _( "sqrt: argument not in domain" ) );
			retVal = ::std::sqrt( l_dLeftValue );
			}
		break;
		case ( FUNCTION::D_LN ):
			{
			if ( l_dLeftValue <= 0 )
				throw HExpressionException( _( "ln: argument not in domain" ) );
			retVal = ::std::log( l_dLeftValue );
			}
		break;
		case ( FUNCTION::D_LOG ):
			{
			if ( l_dLeftValue <= 0 )
				throw HExpressionException( _( "log: argument not in domain" ) );
			retVal = ::std::log10( l_dLeftValue );
			}
		break;
		case ( FUNCTION::D_ABS ):
			retVal = ( l_dLeftValue < 0 ) ? -l_dLeftValue : l_dLeftValue;
		break;
		default:
			M_THROW( _( "unknown function type" ), l_iFunction );
		}
	return ( retVal );
	M_EPILOG
	}

double long HExpression::addition( tree_t::const_node_t a_roNode )
	{
	M_PROLOG
	int l_iOperator = 0;
	double long l_dLeftValue = 0, l_dRightValue = 0;
	tree_t::const_iterator it = a_roNode->begin();
	int_list_t::const_iterator var = (*a_roNode)->f_oVariables.begin();
	l_dLeftValue = count_branch( &*it );
	while ( ( ++ it ) != a_roNode->end() )
		{
		l_iOperator = *var;
		++ var;
		l_dRightValue = count_branch( &*it );
		switch ( l_iOperator )
			{
			case ( OPERATOR::D_ADD ) :
				l_dLeftValue += l_dRightValue;
			break;
			case ( OPERATOR::D_SUBSTRACT ) :
				l_dLeftValue -= l_dRightValue;
			break;
			default:
				M_THROW( _( "unknown addition operator" ), l_iOperator );
			}
		}
	return ( l_dLeftValue );
	M_EPILOG
	}

double long HExpression::multiplication( tree_t::const_node_t a_roNode )
	{
	M_PROLOG
	int l_iOperator = 0;
	double long l_dLeftValue = 0, l_dRightValue = 0;
	tree_t::const_iterator it = a_roNode->begin();
	int_list_t::const_iterator var = (*a_roNode)->f_oVariables.begin();
	l_dLeftValue = count_branch( &*it );
	while ( ( ++ it ) != a_roNode->end() )
		{
		l_iOperator =  *var;
		++ var;
		l_dRightValue = count_branch( &*it );
		switch ( l_iOperator )
			{
			case ( OPERATOR::D_MULTIPLY ) :
				l_dLeftValue *= l_dRightValue;
			break;
			case ( OPERATOR::D_DIVIDE ) :
				if ( ! l_dRightValue )
					throw HExpressionException( _( "divide by 0" ) );
				l_dLeftValue /= l_dRightValue;
			break;
			case ( OPERATOR::D_MODULO ) :
				if ( ! static_cast<int long>( l_dRightValue ) )
					throw HExpressionException( _( "modulo by 0" ) );
				l_dLeftValue = static_cast<double long>( static_cast<int long>( l_dLeftValue ) % static_cast<int long>( l_dRightValue ) );
			break;
			default:
				M_THROW( _( "unknown multiplication operator" ), l_iOperator );
			}
		}
	return ( l_dLeftValue );
	M_EPILOG
	}

double long HExpression::power( tree_t::const_node_t a_roNode )
	{
	M_PROLOG
	double long l_dLeftValue, l_dRightValue;
	tree_t::const_iterator it = a_roNode->begin();
	l_dLeftValue = count_branch ( &*it );
	l_dRightValue = count_branch ( &*( ++ it ) );
	if ( ( l_dLeftValue < 0 )
			&& ( ! eq( l_dRightValue, ::floorl( l_dRightValue ) ) ) )
		return ( 0 );
	return ( ::powl( l_dLeftValue, l_dRightValue ) );
	M_EPILOG
	}

double long HExpression::signum( tree_t::const_node_t a_roNode )
	{
	M_PROLOG
	double long l_dLeftValue;
	l_dLeftValue = count_branch( &*a_roNode->begin() );
	return ( -l_dLeftValue );
	M_EPILOG
	}

double long HExpression::bracket( tree_t::const_node_t a_roNode )
	{
	M_PROLOG
	double long l_dLeftValue;
	l_dLeftValue = count_branch( &*a_roNode->begin() );
	return ( l_dLeftValue );
	M_EPILOG
	}

bool HExpression::translate( HString const& a_oFormula )
	{
	M_PROLOG
	int l_iIndex = 0, l_iRealIndex = 0, l_iCtr = 0;
	int long l_iLength = a_oFormula.get_length();
	f_oFormula.hs_realloc( l_iLength + 1 ); /* + 1 for trailing null */
	f_oFormula.fillz( '\0', l_iLength );
	f_oTerminalIndexes.pool_realloc( l_iLength + 1 );
	while ( l_iIndex < l_iLength )
		{
		f_oTerminalIndexes[ l_iRealIndex ] = l_iIndex;
		if ( ( a_oFormula[ l_iIndex ] >= 'a' )
				&& ( a_oFormula[ l_iIndex ] <= 'z' ) )
			{
			for ( l_iCtr = 0; l_iCtr < 16; l_iCtr++ )
				if ( strncmp( a_oFormula.raw() + l_iIndex,
							n_ppcFunctionsMnemonics[ l_iCtr ],
							n_piFunctionMnemonicsLength[ l_iCtr ] ) == 0 )
				{
				f_oFormula.set_at( l_iRealIndex, static_cast<char>( l_iCtr + 1 ) );
				break;
				}
			if ( l_iCtr < 16 )
				{
				l_iIndex += n_piFunctionMnemonicsLength[ l_iCtr ];
				l_iRealIndex ++;
				}
			else
				{
				f_eError = E_UNKNOWN_MNEMONIC;
				f_iIndex = l_iRealIndex;
				return ( true );
				}
			}
		else
			{
			f_oFormula.set_at( l_iRealIndex, a_oFormula[ l_iIndex ] );
			l_iIndex ++;
			l_iRealIndex ++;
			}
		f_iLength = l_iRealIndex;
		}
	f_oTerminalIndexes[ l_iRealIndex ] = l_iIndex;
	return ( false );
	M_EPILOG
	}

bool HExpression::addition_production( tree_t::node_t a_roNode )
	{
	M_PROLOG
	M_ASSERT ( a_roNode );
	if ( multiplication_production( &*a_roNode->add_node() ) )
		return ( true );
	if ( f_iIndex > f_iLength )
		{
		f_eError = E_UNEXPECTED_TERMINATION;
		return ( true );
		}
	(**a_roNode).METHOD = &HExpression::addition;
	if ( ( f_oFormula[ f_iIndex ] != OPERATOR::D_ADD )
			&& ( f_oFormula[ f_iIndex ] != OPERATOR::D_SUBSTRACT ) )
		{
		shorten_the_branch( a_roNode );
		return ( false );
		}
	while ( ( f_oFormula [ f_iIndex ] == OPERATOR::D_ADD )
			|| ( f_oFormula [ f_iIndex ] == OPERATOR::D_SUBSTRACT ) )
		{
		(**a_roNode).f_oVariables.push_back( f_oFormula[ f_iIndex ++ ] );
		if ( multiplication_production( &*a_roNode->add_node() ) )
			return ( true );
		}
	return ( false );
	M_EPILOG
	}

bool HExpression::multiplication_production( tree_t::node_t a_roNode )
	{
	M_PROLOG
	if ( power_production( &*a_roNode->add_node() ) )
		return ( true );
	if ( f_iIndex > f_iLength )
		{
		f_eError = E_UNEXPECTED_TERMINATION;
		return ( true );
		}
	(**a_roNode).METHOD = &HExpression::multiplication;
	if ( ( f_oFormula[ f_iIndex ] != OPERATOR::D_MULTIPLY )
			&& ( f_oFormula[ f_iIndex ] != OPERATOR::D_DIVIDE )
			&& ( f_oFormula[ f_iIndex ] != OPERATOR::D_MODULO ) )
		{
		shorten_the_branch( a_roNode );
		return ( false );
		}
	while ( ( f_oFormula[ f_iIndex ] == OPERATOR::D_MULTIPLY )
			|| ( f_oFormula[ f_iIndex ] == OPERATOR::D_DIVIDE )
			|| ( f_oFormula[ f_iIndex ] == OPERATOR::D_MODULO ) )
		{
		(**a_roNode).f_oVariables.push_back( f_oFormula[ f_iIndex ++ ] );
		if ( power_production( &*a_roNode->add_node() ) )
			return ( true );
		}
	return ( false );
	M_EPILOG
	}

void HExpression::shorten_the_branch( tree_t::node_t a_poNode )
	{
	M_PROLOG
	tree_t::node_t parent = a_poNode->get_parent();
	if ( parent )
		{
		M_ASSERT( a_poNode->child_count() == 1 );
		tree_t::iterator it;
		for ( it = parent->begin(); it != parent->end(); ++ it )
			{
			if ( &*it == a_poNode )
				break;
			}
		M_ASSERT( it != parent->end() );
		parent->replace_node( it, &*a_poNode->begin() );
		}
	return;
	M_EPILOG
	}

bool HExpression::power_production( tree_t::node_t a_roNode )
	{
	M_PROLOG
	if ( signum_production( &*a_roNode->add_node() ) )
		return ( true );
	if ( f_iIndex > f_iLength )
		{
		f_eError = E_UNEXPECTED_TERMINATION;
		return ( true );
		}
	(**a_roNode).METHOD = &HExpression::bracket;
	if ( f_oFormula[ f_iIndex ] == '^' )
		{
		f_iIndex ++;
		if ( power_production( &*a_roNode->add_node() ) )
			return ( true );
		(**a_roNode).METHOD = &HExpression::power;
		}
	else
		shorten_the_branch( a_roNode );
	return ( false );
	M_EPILOG
	}

bool HExpression::signum_production( tree_t::node_t a_roNode )
	{
	M_PROLOG
	if ( f_iIndex > f_iLength )
		{
		f_eError = E_UNEXPECTED_TERMINATION;
		return ( true );
		}
	if ( f_oFormula[ f_iIndex ] == '-' )
		{
		f_iIndex ++;
		if ( terminal_production( &*a_roNode->add_node() ) )
			return ( true );
		(**a_roNode).METHOD = &HExpression::signum;
		}
	else
		if ( terminal_production( a_roNode ) )
			return ( true );
	return ( false );
	M_EPILOG
	}

bool HExpression::terminal_production( tree_t::node_t a_roNode )
	{
	M_PROLOG
	if ( f_iIndex > f_iLength )
		{
		f_eError = E_UNEXPECTED_TERMINATION;
		return ( true );
		}
	switch ( f_oFormula [ f_iIndex ] )
		{
		case '(' :
			{
			f_iIndex ++;
			if ( addition_production ( a_roNode ) )
				return ( true );
			if ( f_oFormula [ f_iIndex ] != ')' )
				{
				f_eError = E_CLOSING_BRACKET_EXPECTED;
				return ( true );
				}
			else
				f_iIndex ++;
			return ( false );
			}
		case '|' :
			{
			f_iIndex ++;
			if ( addition_production ( &*a_roNode->add_node() ) )
				return ( true );
			(**a_roNode).METHOD = &HExpression::functions;
			(**a_roNode).f_oVariables.push_back( FUNCTION::D_ABS );
			if ( f_oFormula[ f_iIndex ] != '|' )
				{
				f_eError = E_CLOSING_ABSOLUTE_EXPECTED;
				return ( true );
				}
			else
				f_iIndex ++;
			return ( false );
			}
		default:
			break;
		}
	if ( ( f_oFormula[ f_iIndex ] >= 'A' )
			&& ( f_oFormula[ f_iIndex ] <= 'Z' ) )
		{
		(**a_roNode).f_oVariables.push_back( f_oFormula[ f_iIndex ++ ] - 'A' );
		return ( false );
		}
	if ( ( f_oFormula[ f_iIndex ] > FUNCTION::D_FUNCTIONS )
			&& ( f_oFormula[ f_iIndex ] < FUNCTION::D_ABS ) )
		{
		f_iIndex ++;
		if ( f_oFormula[ f_iIndex ] == '(' )
			{
			f_iIndex ++;
			(**a_roNode).METHOD = &HExpression::functions;
			(**a_roNode).f_oVariables.push_back ( static_cast<int>( f_oFormula[ f_iIndex - 2 ] ) );
			if ( addition_production ( &*a_roNode->add_node() ) )
				return ( true );
			if ( f_oFormula [ f_iIndex ] != ')' )
				{
				f_eError = E_CLOSING_FUNCTION_BRACKET_EXPECTED;
				return ( true );
				}
			else
				f_iIndex ++;
			}
		else
			{
			f_eError = E_OPENING_FUNCTION_BRACKET_EXPECTED;
			return ( true );
			}
		return ( false );
		}
	if ( ( f_oFormula [ f_iIndex ] >= '0' )
			&& ( f_oFormula [ f_iIndex ] <= '9' ) )
		{
		double long l_dValue = 0;
		int l_iOffset = f_iIndex;
		if ( f_iIndex > f_iLength )
			{
			f_eError = E_UNEXPECTED_TERMINATION;
			return ( true );
			}
		while ( ( f_oFormula [ f_iIndex ] >= '0' )
				&& ( f_oFormula [ f_iIndex ] <= '9' ) )
			f_iIndex ++;
		if ( f_oFormula [ f_iIndex ] == '.' )
			{
			f_iIndex ++;
			if ( ( f_oFormula [ f_iIndex ] >= '0' )
					&& ( f_oFormula [ f_iIndex ] <= '9' ) )
				while ( ( f_oFormula [ f_iIndex ] >= '0' )
						&& ( f_oFormula [ f_iIndex ] <= '9' ) )
					f_iIndex ++;
			else
				{
				f_eError = E_DIGIT_EXPECTED;
				return ( true );
				}
			}
		l_dValue = ::strtod( f_oFormula.raw() + l_iOffset, NULL );
		f_oConstantsPool.push_back( l_dValue );
		/* We save variables as positive indexes and constants as negative
		 * indexes, positive and negative 0 index would conflict so
		 * we shift negative indexes by 1, so 0 becomes -1, 1 becomes -2,
		 * 2 becomes -3, and so on and so forth.
		 * HPool::size() returns current access/addition peak for revelant pool,
		 * so to get index of lately added value we need to decrement size by 1. */
		(**a_roNode).f_oVariables.push_back( static_cast<int>( - ( f_oConstantsPool.size() - 1 ) - 1 ) );
		return ( false );
		}
	f_eError = E_UNEXPECTED_TOKEN;
	return ( true );
	M_EPILOG
	}

double long* HExpression::compile( HString const& a_oFormula )
	{
	M_PROLOG
	int long l_iLength = 0;
	f_iIndex = 0;
	f_eError = E_OK;
	l_iLength = a_oFormula.get_length();
	if ( l_iLength == 0 )
		{
		f_eError = E_PREMATURE_TERMINATION;
		throw HExpressionException( n_pcSyntaxError );
		}
	if ( translate( a_oFormula ) )
		throw HExpressionException( n_pcSyntaxError );
	f_oConstantsPool.reset();
	tree_t::node_t root = f_oEquationTree.create_new_root();
	if ( ! addition_production( root ) )
		{
		if ( ( f_iIndex < f_iLength ) && ( f_eError == E_OK ) )
			f_eError = E_UNEXPECTED_TOKEN;
		}
	else if ( f_iIndex >= f_iLength )
		f_eError = E_UNEXPECTED_TERMINATION;
	if ( f_eError != E_OK )
		throw HExpressionException( n_pcSyntaxError );
	return ( f_pdVariables );
	M_EPILOG
	}

double long* HExpression::variables( void )
	{
	M_PROLOG
	M_ASSERT( f_eError == E_OK );
	return ( f_pdVariables );
	M_EPILOG
	}

double long& HExpression::operator[]( int a_iIndex )
	{
	M_PROLOG
	if ( ( a_iIndex >= 'a' ) && ( a_iIndex <= 'a' ) )
		a_iIndex = ( a_iIndex - 'a' ) + 'A';
	double long* val = NULL;
	if ( ( a_iIndex >= 0 ) && ( a_iIndex < 26 ) )
		val = &f_pdVariables[ a_iIndex ];
	else if ( ( a_iIndex >= 'A' ) && ( a_iIndex <= 'Z' ) )
		val = &f_pdVariables[ a_iIndex - 'A' ];
	else
		M_THROW( "index out of range", a_iIndex );
	return ( *val );
	M_EPILOG
	}

double long HExpression::evaluate( void )
	{
	M_PROLOG
	tree_t::const_node_t root = f_oEquationTree.get_root();
	if ( ! root )
		M_THROW( "logic tree is not compiled", f_eError );
	return ( ( this->* ( (*root)->METHOD ) ) ( root ) );
	M_EPILOG
	}

char const* HExpression::get_error( void ) const
	{
	M_PROLOG
	switch ( f_eError )
		{
		case ( E_OK ):
			return ( _( "succesful" ) );
		case ( E_UNKNOWN_MNEMONIC ):
			return ( _( "unknown mnemonic" ) );
		case ( E_UNEXPECTED_TERMINATION ):
			return ( _( "unexpected termination" ) );
		case ( E_CLOSING_BRACKET_EXPECTED ):
			return ( _( "closing bracket expected" ) );
		case ( E_CLOSING_ABSOLUTE_EXPECTED ):
			return ( _( "closing absolute bracket expected" ) );
		case ( E_CLOSING_FUNCTION_BRACKET_EXPECTED ):
			return ( _( "closing function bracket expected" ) );
		case ( E_OPENING_FUNCTION_BRACKET_EXPECTED ):
			return ( _( "opening function bracket expected" ) );
		case ( E_DIGIT_EXPECTED ):
			return ( _( "digit expected" ) );
		case ( E_UNEXPECTED_TOKEN ):
			return ( _( "unexpected token" ) );
		case ( E_PREMATURE_TERMINATION ):
			return ( _( "premature termination" ) );
		default :
			M_THROW ( _( "enknown error code" ), f_eError );
		}
	return ( NULL );
	M_EPILOG
	}

int HExpression::get_error_token( void ) const
	{
	M_PROLOG
	if ( f_iLength > f_iIndex )
		return ( f_oTerminalIndexes [ f_iIndex ] );
	else if ( f_iIndex >= 0 )
		return ( f_iLength );
	return ( 0 );
	M_EPILOG
	}

}

void swap( yaal::tools::HExpression& a, yaal::tools::HExpression& b )
	{ a.swap( b ); }

}

