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

#include "hcore/hexception.h"
M_VCSID ( "$Id$" )
#include "hcore/xalloc.h"
#include "hcore/htree.h"
#include "hcore/hcore.h"
#include "hanalyser.h"

#ifdef __DEBUGGER_BABUNI__
#include "hcore/hlog.h"
#endif /* __DEBUGGER_BABUNI__ */

using namespace yaal;
using namespace yaal::hcore;

namespace yaal
{

namespace tools
{

namespace
	{
	static int const D_ADD = 0;
	static int const D_SUBSTRACT = 1;
	static int const D_MULTIPLY = 0;
	static int const D_DIVIDE = 1;
	}

struct FUNCTIONS
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

int const FUNCTIONS::D_FUNCTIONS = 0;
int const FUNCTIONS::D_SIN = 2;
int const FUNCTIONS::D_SINH = 1;
int const FUNCTIONS::D_COS = 4;
int const FUNCTIONS::D_COSH = 3;
int const FUNCTIONS::D_TG = 6;
int const FUNCTIONS::D_TGH = 5;
int const FUNCTIONS::D_CTG = 8;
int const FUNCTIONS::D_CTGH = 7;
int const FUNCTIONS::D_ARCSIN = 9;
int const FUNCTIONS::D_ARCCOS = 10;
int const FUNCTIONS::D_ARCTG = 11;
int const FUNCTIONS::D_ARCCTG = 12;
int const FUNCTIONS::D_EXP = 13;
int const FUNCTIONS::D_SQRT = 14;
int const FUNCTIONS::D_LN = 15;
int const FUNCTIONS::D_LOG = 16;
int const FUNCTIONS::D_ABS = 17;
int const FUNCTIONS::D_ADDITION = 1;
int const FUNCTIONS::D_MULTIPLICATION = 2;
int const FUNCTIONS::D_POWER = 3;
int const FUNCTIONS::D_SIGNUM = 4;
int const FUNCTIONS::D_BRACKET = 5;
	

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

/*
HAnalyser::tree_t::iterator& HAnalyser::HAnalyserNode::grow_up_branch( int a_iFlag )
	{
	M_PROLOG
	tree_t::iterator& l_poNode = NULL;
	if ( a_iFlag == FILL::D_NEW_AUTO )
		l_poNode = new HAnalyserNode( this );
	else
		l_poNode = 0;
	f_oBranch.add_tail( reinterpret_cast<ANALYZER_NODE_PTR_t*>( &l_poNode ) );
	return ( l_poNode );
	M_EPILOG
	}
*/

HAnalyser::HAnalyser( void ) : f_iIndex( 0 ), f_iLength( 0 ),
			f_eError( E_OK ), f_oConstantsPool ( 0, HPool<double>::D_AUTO_GROW ),
			f_oTerminalIndexes( 0, HPool<int>::D_AUTO_GROW ), f_oFormula(),
			f_oEquationTree()
	{
	M_PROLOG
	int l_iCtr = 0;
	for ( l_iCtr = 0; l_iCtr < 26; l_iCtr ++ )
		f_pdVariables [ l_iCtr ] = 0;
	return;
	M_EPILOG
	}

HAnalyser::~HAnalyser( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

double HAnalyser::count_branch( tree_t::const_node_t a_roNode )
	{
	M_PROLOG
	int l_iIndex = 0;
	double l_dValue = 0;
	if ( a_roNode->has_childs() )
		l_dValue = ( this->* ( (*a_roNode)->METHOD ) ) ( a_roNode );
	else
		{
		l_iIndex = (*a_roNode)->f_oVariables.head();
		if ( l_iIndex >= 0 )
			l_dValue = f_pdVariables [ l_iIndex ];
		else
			l_dValue = f_oConstantsPool [ - 1 - l_iIndex ];
		}
	return ( l_dValue );
	M_EPILOG
	}

double HAnalyser::functions( tree_t::const_node_t a_roNode )
	{
	M_PROLOG
	int l_iFunction = (*a_roNode)->f_oVariables.tail();
	double l_dLeftValue = 0;
	l_dLeftValue = count_branch( &*a_roNode->begin() );
	switch ( l_iFunction )
		{
		case ( FUNCTIONS::D_SIN ):
			return ( sin ( l_dLeftValue ) );
		case ( FUNCTIONS::D_SINH ):
			return ( sinh ( l_dLeftValue ) );
		case ( FUNCTIONS::D_COS ):
			return ( cos ( l_dLeftValue ) );
		case ( FUNCTIONS::D_COSH ):
			return ( cosh ( l_dLeftValue ) );
		case ( FUNCTIONS::D_TG	):
			{
			if ( eq ( floor ( l_dLeftValue / M_PI + .5 ),
						( l_dLeftValue / M_PI + .5 ) ) )
				return ( 0 );
			return ( tan ( l_dLeftValue ) );
			}
		case ( FUNCTIONS::D_TGH ):
			return ( tanh ( l_dLeftValue ) );
		case ( FUNCTIONS::D_CTG ):
			{
			if ( eq ( floor( l_dLeftValue / M_PI ),
						( l_dLeftValue / M_PI ) ) )
				return ( 0 );
			l_dLeftValue = tan ( l_dLeftValue );
			if ( l_dLeftValue == 0 )
				return ( 0 );
			return ( 1. / l_dLeftValue );
			}
		case ( FUNCTIONS::D_CTGH ):
			{
			l_dLeftValue = tanh( l_dLeftValue );
			if ( l_dLeftValue == 0 )
				return ( 0 );
			return ( 1. / l_dLeftValue );
			}
		case ( FUNCTIONS::D_ARCSIN ):
			{
			if ( ( l_dLeftValue < - M_PI / 2 ) || ( l_dLeftValue > M_PI / 2 ) )
				return ( 0 );
			return ( asin ( l_dLeftValue ) );
			}
		case ( FUNCTIONS::D_ARCCOS ):
			{
			if ( ( l_dLeftValue < - M_PI / 2 ) || ( l_dLeftValue > M_PI / 2 ) )
				return ( 0 );
			return ( acos ( l_dLeftValue ) );
			}
		case ( FUNCTIONS::D_ARCTG ):
			return ( atan ( l_dLeftValue ) );
		case ( FUNCTIONS::D_ARCCTG ):
			return ( M_PI - atan ( l_dLeftValue ) );
		case ( FUNCTIONS::D_EXP ):
			return ( exp ( l_dLeftValue ) );
		case ( FUNCTIONS::D_SQRT ):
			{
			if ( l_dLeftValue < 0 )
				return ( sqrt ( - l_dLeftValue ) );
			return ( sqrt ( l_dLeftValue ) );
			}
		case ( FUNCTIONS::D_LN ):
			{
			if ( l_dLeftValue == 0 )
				return ( -1000 );
			if ( l_dLeftValue < 0 )
				return ( ::log ( -l_dLeftValue ) );
			return ( ::log ( l_dLeftValue ) );
			}
		case ( FUNCTIONS::D_LOG ):
			{
			if ( l_dLeftValue == 0 )
				return ( - 1000 );
			if ( l_dLeftValue < 0 )
				return ( log10 ( - l_dLeftValue ) );
			return ( log10 ( l_dLeftValue ) );
			}
		case ( FUNCTIONS::D_ABS ):
			{
			if ( l_dLeftValue < 0 )
				return ( - l_dLeftValue );
			return ( l_dLeftValue );
			}
		default:
			M_THROW ( _ ( "unknown function type" ), l_iFunction );
		}
	M_EPILOG
	}

double HAnalyser::addition( tree_t::const_node_t a_roNode )
	{
	M_PROLOG
	int l_iOperator = 0;
	double l_dLeftValue = 0, l_dRightValue = 0;
	tree_t::iterator it = a_roNode->begin();
	int_list_t::iterator var = (*a_roNode)->f_oVariables.begin();
	l_dLeftValue = count_branch( &*it );
	while ( ( ++ it ) != a_roNode->end() )
		{
		l_iOperator = *var;
		++ var;
		l_dRightValue = count_branch( &*it );
		switch ( l_iOperator )
			{
			case ( D_ADD ) :
				l_dLeftValue += l_dRightValue;
			break;
			case ( D_SUBSTRACT ) :
				l_dLeftValue -= l_dRightValue;
			break;
			default:
				M_THROW ( _ ( "unknown addition operator" ), l_iOperator );
			}
		}
	return ( l_dLeftValue );
	M_EPILOG
	}

double HAnalyser::multiplication( tree_t::const_node_t a_roNode )
	{
	M_PROLOG
	int l_iOperator = 0;
	double l_dLeftValue = 0, l_dRightValue = 0;
	tree_t::iterator it = a_roNode->begin();
	int_list_t::iterator var = (*a_roNode)->f_oVariables.begin();
	l_dLeftValue = count_branch( &*it );
	while ( ( ++ it ) != a_roNode->end() )
		{
		l_iOperator =  *var;
		++ var;
		l_dRightValue = count_branch( &*it );
		switch ( l_iOperator )
			{
			case ( D_MULTIPLY ) :
				l_dLeftValue *= l_dRightValue;
			break;
			case ( D_DIVIDE ) :
				if ( l_dRightValue )
					l_dLeftValue /= l_dRightValue;
			break;
			default:
				M_THROW ( _ ( "unknown multiplication operator" ), l_iOperator );
			}
		}
	return ( l_dLeftValue );
	M_EPILOG
	}

double HAnalyser::power( tree_t::const_node_t a_roNode )
	{
	M_PROLOG
	double l_dLeftValue, l_dRightValue;
	tree_t::iterator it = a_roNode->begin();
	l_dLeftValue = count_branch ( &*it );
	l_dRightValue = count_branch ( &*( ++ it ) );
	if ( ( l_dLeftValue < 0 )
			&& ( ! eq ( l_dRightValue, floor( l_dRightValue ) ) ) )
		return ( 0 );
	return ( pow( l_dLeftValue, l_dRightValue ) );
	M_EPILOG
	}

double HAnalyser::signum( tree_t::const_node_t a_roNode )
	{
	M_PROLOG
	double l_dLeftValue;
	l_dLeftValue = count_branch( &*a_roNode->begin() );
	return ( -l_dLeftValue );
	M_EPILOG
	}

double HAnalyser::bracket( tree_t::const_node_t a_roNode )
	{
	M_PROLOG
	double l_dLeftValue;
	l_dLeftValue = count_branch( &*a_roNode->begin() );
	return ( l_dLeftValue );
	M_EPILOG
	}

bool HAnalyser::translate( char const* a_pcFormula )
	{
	M_PROLOG
	int l_iIndex = 0, l_iRealIndex = 0, l_iCtr = 0, l_iLength = 0;
	l_iLength = strlen ( a_pcFormula );
	f_oTerminalIndexes.pool_realloc ( l_iLength + 1 );
	while ( l_iIndex < l_iLength )
		{
		f_oTerminalIndexes [ l_iRealIndex ] = l_iIndex;
		if ( ( a_pcFormula [ l_iIndex ] >= 'a' )
				&& ( a_pcFormula [ l_iIndex ] <= 'z' ) )
			{
			for ( l_iCtr = 0; l_iCtr < 16; l_iCtr++ )
				if ( strncmp( a_pcFormula + l_iIndex,
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
			f_oFormula.set_at( l_iRealIndex, a_pcFormula[ l_iIndex ] );
			l_iIndex ++;
			l_iRealIndex ++;
			}
		f_iLength = l_iRealIndex;
		}
	f_oTerminalIndexes[ l_iRealIndex ] = l_iIndex;
	f_oFormula.set_at( l_iRealIndex, 0 );
	return ( false );
	M_EPILOG
	}

bool HAnalyser::addition_production( tree_t::node_t a_roNode )
	{
	M_PROLOG
	int l_iCtr = 0;
	M_ASSERT ( a_roNode );
	if ( multiplication_production( &*a_roNode->add_node() ) )
		return ( true );
	if ( f_iIndex > f_iLength )
		{
		f_eError = E_UNEXPECTED_TERMINATION;
		return ( true );
		}
	(**a_roNode).METHOD = &HAnalyser::addition;
	if ( ( f_oFormula[ f_iIndex ] != '+' ) && ( f_oFormula[ f_iIndex ] != '-' ) )
		{
		shorten_the_branch( a_roNode );
		return ( false );
		}
	while ( ( f_oFormula [ f_iIndex ] == '+' )
			|| ( f_oFormula [ f_iIndex ] == '-' ) )
		{
		l_iCtr = ( f_oFormula [ f_iIndex ++ ] == '+' ) ? D_ADD : D_SUBSTRACT;
		(**a_roNode).f_oVariables.add_tail( &l_iCtr );
		if ( multiplication_production( &*a_roNode->add_node() ) )
			return ( true );
		}
	return ( false );
	M_EPILOG
	}

bool HAnalyser::multiplication_production( tree_t::node_t a_roNode )
	{
	M_PROLOG
	int l_iCtr = 0;
	if ( power_production( &*a_roNode->add_node() ) )
		return ( true );
	if ( f_iIndex > f_iLength )
		{
		f_eError = E_UNEXPECTED_TERMINATION;
		return ( true );
		}
	(**a_roNode).METHOD = &HAnalyser::multiplication;
	if ( ( f_oFormula[ f_iIndex ] != '*' ) && ( f_oFormula[ f_iIndex ] != '/' ) )
		{
		shorten_the_branch( a_roNode );
		return ( false );
		}
	while ( ( f_oFormula[ f_iIndex ] == '*' )
			|| ( f_oFormula[ f_iIndex ] == '/' ) )
		{
		l_iCtr = ( f_oFormula[ f_iIndex ++ ] == '*' ) ? D_MULTIPLY : D_DIVIDE;
		(**a_roNode).f_oVariables.add_tail( &l_iCtr );
		if ( power_production( &*a_roNode->add_node() ) )
			return ( true );
		}
	return ( false );
	M_EPILOG
	}

void HAnalyser::shorten_the_branch( tree_t::node_t a_poNode )
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

bool HAnalyser::power_production( tree_t::node_t a_roNode )
	{
	M_PROLOG
	if ( signum_production( &*a_roNode->add_node() ) )
		return ( true );
	if ( f_iIndex > f_iLength )
		{
		f_eError = E_UNEXPECTED_TERMINATION;
		return ( true );
		}
	(**a_roNode).METHOD = &HAnalyser::bracket;
	if ( f_oFormula[ f_iIndex ] == '^' )
		{
		f_iIndex ++;
		if ( power_production( &*a_roNode->add_node() ) )
			return ( true );
		(**a_roNode).METHOD = &HAnalyser::power;
		}
	else
		shorten_the_branch( a_roNode );
	return ( false );
	M_EPILOG
	}

bool HAnalyser::signum_production( tree_t::node_t a_roNode )
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
		(**a_roNode).METHOD = &HAnalyser::signum;
		}
	else
		if ( terminal_production( a_roNode ) )
			return ( true );
	return ( false );
	M_EPILOG
	}

bool HAnalyser::terminal_production( tree_t::node_t a_roNode )
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
			(**a_roNode).METHOD = &HAnalyser::functions;
			(**a_roNode).f_oVariables.push_back( FUNCTIONS::D_ABS );
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
	if ( ( f_oFormula[ f_iIndex ] > FUNCTIONS::D_FUNCTIONS )
			&& ( f_oFormula[ f_iIndex ] < FUNCTIONS::D_ABS ) )
		{
		f_iIndex ++;
		if ( f_oFormula[ f_iIndex ] == '(' )
			{
			f_iIndex ++;
			(**a_roNode).METHOD = &HAnalyser::functions;
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
		double l_dValue = 0;
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
		l_dValue = strtod ( static_cast < char const * const > ( f_oFormula ) + l_iOffset, NULL );
		f_oConstantsPool.push_back( l_dValue );
		/* We save variables as positive indexes and constants as negative
		 * indexes, positive and negative 0 index would conflict so
		 * we shift negative indexes by 1, so 0 becomes -1, 1 becomes -2,
		 * 2 becomes -3, and so on and so forth.
		 * HPool::size() returns current access/addition peak for revelant pool,
		 * so to get index of lately added value we need to decrement size by 1. */
		(**a_roNode).f_oVariables.push_back( - ( f_oConstantsPool.size() - 1 ) - 1 );
		return ( false );
		}
	f_eError = E_UNEXPECTED_TOKEN;
	return ( true );
	M_EPILOG
	}

double* HAnalyser::analyse( char const* a_pcFormula )
	{
	M_PROLOG
	int l_iLength = 0;
	f_iIndex = 0;
	f_eError = E_OK;
	l_iLength = strlen ( a_pcFormula );
	if ( l_iLength == 0 )
		{
		f_eError = E_PREMATURE_TERMINATION;
		return ( NULL );
		}
	f_oFormula.hs_realloc ( l_iLength + 1 ); /* + 1 for trailing null */
	if ( translate ( a_pcFormula ) )
		return ( NULL );
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
		return ( NULL );
	return ( f_pdVariables );
	M_EPILOG
	}

double& HAnalyser::operator [ ] ( int a_iIndex )
	{
	M_PROLOG
	if ( ( a_iIndex >= 'a' ) && ( a_iIndex <= 'a' ) )
		a_iIndex = ( a_iIndex - 'a' ) + 'A';
	if ( ( a_iIndex >= 0 ) && ( a_iIndex < 26 ) )
		return ( f_pdVariables [ a_iIndex ] );
	else if ( ( a_iIndex >= 'A' ) && ( a_iIndex <= 'Z' ) )
		return ( f_pdVariables [ a_iIndex - 'A' ] );
	else
		M_THROW ( "index out of range", a_iIndex );
	M_EPILOG
	}

double HAnalyser::count( void )
	{
	M_PROLOG
	tree_t::const_node_t root = f_oEquationTree.get_root();
	if ( ! root )
		M_THROW ( "logic tree is not compiled", f_eError );
	return ( ( this->* ( (*root)->METHOD ) ) ( root ) );
	M_EPILOG
	}

char const* HAnalyser::get_error( void ) const
	{
	M_PROLOG
	switch ( f_eError )
		{
		case ( E_OK ):
			return ( _ ( "succesful" ) );
		case ( E_UNKNOWN_MNEMONIC ):
			return ( _ ( "unknown mnemonic" ) );
		case ( E_UNEXPECTED_TERMINATION ):
			return ( _ ( "unexpected termination" ) );
		case ( E_CLOSING_BRACKET_EXPECTED ):
			return ( _ ( "closing bracket expected" ) );
		case ( E_CLOSING_ABSOLUTE_EXPECTED ):
			return ( _ ( "closing absolute bracket expected" ) );
		case ( E_CLOSING_FUNCTION_BRACKET_EXPECTED ):
			return ( _ ( "closing function bracket expected" ) );
		case ( E_OPENING_FUNCTION_BRACKET_EXPECTED ):
			return ( _ ( "opening function bracket expected" ) );
		case ( E_DIGIT_EXPECTED ):
			return ( _ ( "digit expected" ) );
		case ( E_UNEXPECTED_TOKEN ):
			return ( _ ( "unexpected token" ) );
		case ( E_PREMATURE_TERMINATION ):
			return ( _ ( "premature termination" ) );
		default :
			M_THROW ( _ ( "enknown error code" ), f_eError );
		}
	M_EPILOG
	}

int HAnalyser::get_error_token( void ) const
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

}

