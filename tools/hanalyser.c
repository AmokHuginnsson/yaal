/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	analyser.c - this file is integral part of `stdhapi' project.

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

#ifdef __DEBUGGER_BABUNI__
#define log libc_math_log
#endif /* __DEBUGGER_BABUNI__ */
#include <math.h>
#ifdef __DEBUGGER_BABUNI__
#undef log
#endif /* __DEBUGGER_BABUNI__ */
#include <string.h>
#include <stdlib.h>
#include <libintl.h>

#include "hcore/hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "hcore/xalloc.h"
#include "hcore/htree.h"
#include "hcore/hcore.h"
#include "hanalyser.h"

#ifdef __DEBUGGER_BABUNI__
#include "hcore/hlog.h"
#endif /* __DEBUGGER_BABUNI__ */

using namespace stdhapi::hcore;

namespace stdhapi
{

namespace tools
{

#define D_FUNCTIONS 0
#define D_SIN				2
#define D_SINH			1
#define D_COS				4
#define D_COSH			3
#define D_TG				6
#define D_TGH				5
#define D_CTG				8
#define D_CTGH			7
#define D_ARCSIN		9
#define D_ARCCOS		10
#define D_ARCTG			11
#define D_ARCCTG		12
#define D_EXP				13
#define D_SQRT			14
#define D_LN				15
#define	D_LOG				16
#define D_ABS				17
#define D_ADDITION	1
#define D_MULTIPLICATION 2
#define D_POWER			3
#define D_SIGNUM		4
#define D_BRACKET		5
	
#define M_COUNT_BRANCH( arg ) count_branch ( dynamic_cast < HAnalyserNode * > ( arg ) )

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

HAnalyser::HAnalyserNode::HAnalyserNode ( HAnalyserNode * a_poNode )
				 : HTree < double_ptr_list_t >::HNode ( a_poNode ), METHOD ( NULL )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HAnalyser::HAnalyserNode * HAnalyser::HAnalyserNode::grow_up_branch ( int a_iFlag )
	{
	M_PROLOG
	HAnalyserNode * l_poNode = NULL;
	if ( a_iFlag )
		l_poNode = new HAnalyserNode ( this );
	else
		l_poNode = 0;
	f_oBranch.add_tail ( reinterpret_cast < ANALYZER_NODE_PTR_t * > ( & l_poNode ) );
	return ( l_poNode );
	M_EPILOG
	}

HAnalyser::HAnalyser ( void ) : f_iIndex ( 0 ), f_iLength ( 0 ),
			f_eError ( E_OK ), f_oConstantsPool ( 0, HPool < double >::D_AUTO_GROW ),
			f_oTerminalIndexes ( 0, HPool < int >::D_AUTO_GROW ), f_oFormula ( )
	{
	M_PROLOG
	int l_iCtr = 0;
	for ( l_iCtr = 0; l_iCtr < 26; l_iCtr ++ )
		f_pdVariables [ l_iCtr ] = 0;
	return;
	M_EPILOG
	}

HAnalyser::~HAnalyser ( void )
	{
	M_PROLOG
	HAnalyserNode * l_poNode = NULL;
	if ( f_poRoot )
		{
		l_poNode = dynamic_cast < HAnalyserNode * > ( f_poRoot );
		M_ASSERT ( l_poNode );
		delete l_poNode;
		}
	f_poRoot = NULL;
	return;
	M_EPILOG
	}

double HAnalyser::count_branch ( HAnalyserNode * a_poNode )
	{
	M_PROLOG
	double l_dValue = 0;
	if ( ( a_poNode->f_oBranch.quantity ( ) )
			&& ( a_poNode->f_oBranch [ 0 ] != 0 ) )
		l_dValue = ( this->* ( a_poNode->METHOD ) ) ( a_poNode );
	else
		l_dValue = * static_cast < double * > ( a_poNode->f_tLeaf.head ( ) );
	return ( l_dValue );
	M_EPILOG
	}

double HAnalyser::functions ( HAnalyserNode * a_poNode )
	{
	M_PROLOG
	int l_iFunction = reinterpret_cast < int > ( a_poNode->f_tLeaf.tail ( ) );
	double l_dLeftValue = 0;
	l_dLeftValue = M_COUNT_BRANCH ( a_poNode->f_oBranch [ 0 ] );
	switch ( l_iFunction )
		{
		case ( D_SIN ):
			return ( sin ( l_dLeftValue ) );
		case ( D_SINH ):
			return ( sinh ( l_dLeftValue ) );
		case ( D_COS ):
			return ( cos ( l_dLeftValue ) );
		case ( D_COSH ):
			return ( cosh ( l_dLeftValue ) );
		case ( D_TG	):
			{
			if ( eq ( floor ( l_dLeftValue / M_PI + .5 ),
						( l_dLeftValue / M_PI + .5 ) ) )
				return ( 0 );
			return ( tan ( l_dLeftValue ) );
			}
		case ( D_TGH ):
			return ( tanh ( l_dLeftValue ) );
		case ( D_CTG ):
			{
			if ( eq ( floor( l_dLeftValue / M_PI ),
						( l_dLeftValue / M_PI ) ) )
				return ( 0 );
			l_dLeftValue = tan ( l_dLeftValue );
			if ( l_dLeftValue == 0 )
				return ( 0 );
			return ( 1. / l_dLeftValue );
			}
		case ( D_CTGH ):
			{
			l_dLeftValue = tanh( l_dLeftValue );
			if ( l_dLeftValue == 0 )
				return ( 0 );
			return ( 1. / l_dLeftValue );
			}
		case ( D_ARCSIN ):
			{
			if ( ( l_dLeftValue < - M_PI / 2 ) || ( l_dLeftValue > M_PI / 2 ) )
				return ( 0 );
			return ( asin ( l_dLeftValue ) );
			}
		case ( D_ARCCOS ):
			{
			if ( ( l_dLeftValue < - M_PI / 2 ) || ( l_dLeftValue > M_PI / 2 ) )
				return ( 0 );
			return ( acos ( l_dLeftValue ) );
			}
		case ( D_ARCTG ):
			return ( atan ( l_dLeftValue ) );
		case ( D_ARCCTG ):
			return ( M_PI - atan ( l_dLeftValue ) );
		case ( D_EXP ):
			return ( exp ( l_dLeftValue ) );
		case ( D_SQRT ):
			{
			if ( l_dLeftValue < 0 )
				return ( sqrt ( - l_dLeftValue ) );
			return ( sqrt ( l_dLeftValue ) );
			}
		case ( D_LN ):
			{
			if ( l_dLeftValue == 0 )
				return ( -1000 );
			if ( l_dLeftValue < 0 )
				return ( ::log ( -l_dLeftValue ) );
			return ( ::log ( l_dLeftValue ) );
			}
		case ( D_LOG ):
			{
			if ( l_dLeftValue == 0 )
				return ( - 1000 );
			if ( l_dLeftValue < 0 )
				return ( log10 ( - l_dLeftValue ) );
			return ( log10 ( l_dLeftValue ) );
			}
		case ( D_ABS ):
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

double HAnalyser::addition ( HAnalyserNode * a_poNode )
	{
	M_PROLOG
	int l_iOperator = 0;
	int l_iIndex = 1, l_iQuantity = a_poNode->f_oBranch.quantity ( );
	double l_dLeftValue = 0, l_dRightValue = 0;
	l_dLeftValue = M_COUNT_BRANCH ( a_poNode->f_oBranch [ 0 ] );
	while ( l_iIndex < l_iQuantity )
		{
		l_iOperator = reinterpret_cast < int > ( a_poNode->f_tLeaf [ l_iIndex - 1 ] );
		l_dRightValue = M_COUNT_BRANCH ( a_poNode->f_oBranch [ l_iIndex ] );
		switch ( l_iOperator )
			{
			case 0 :
				{
				l_dLeftValue += l_dRightValue;
				break;
				}
			case 1 :
				{
				l_dLeftValue -= l_dRightValue;
				break;
				}
			default:
				M_THROW ( _ ( "unknown addition operator" ), l_iOperator );
			}
		l_iIndex++;
		}
#ifdef __DEBUGGER_BABUNI__
	::log << "index: " << l_iIndex << endl;
#endif /* __DEBUGGER_BABUNI__ */
	return ( l_dLeftValue );
	M_EPILOG
	}

double HAnalyser::multiplication ( HAnalyserNode * a_poNode )
	{
	M_PROLOG
	int l_iOperator = 0;
	int l_iIndex = 1, l_iQuantity = a_poNode->f_oBranch.quantity ( );
	double l_dLeftValue = 0, l_dRightValue = 0;
	l_dLeftValue = M_COUNT_BRANCH ( a_poNode->f_oBranch [ 0 ] );
	while ( l_iIndex < l_iQuantity )
		{
		l_iOperator = reinterpret_cast < int > ( a_poNode->f_tLeaf [ l_iIndex - 1 ] );
		l_dRightValue = M_COUNT_BRANCH ( a_poNode->f_oBranch [ l_iIndex ] );
		switch ( l_iOperator )
			{
			case 0 :
				{
				l_dLeftValue *= l_dRightValue;
				break;
				}
			case 1 :
				{
				if ( l_dRightValue )
					l_dLeftValue /= l_dRightValue;
				break;
				}
			default:
				M_THROW ( _ ( "unknown multiplication operator" ), l_iOperator );
			}
		l_iIndex++;
		}
	return ( l_dLeftValue );
	M_EPILOG
	}

double HAnalyser::power ( HAnalyserNode * a_poNode )
	{
	M_PROLOG
	double l_dLeftValue, l_dRightValue;
	l_dLeftValue = M_COUNT_BRANCH ( a_poNode->f_oBranch [ 0 ] );
	l_dRightValue = M_COUNT_BRANCH ( a_poNode->f_oBranch [ 1 ] );
	if ( ( l_dLeftValue < 0 )
			&& ( ! eq ( l_dRightValue, floor( l_dRightValue ) ) ) )
		return ( 0 );
	return ( pow ( l_dLeftValue, l_dRightValue ) );
	M_EPILOG
	}

double HAnalyser::signum ( HAnalyserNode * a_poNode )
	{
	M_PROLOG
	double l_dLeftValue;
	l_dLeftValue = M_COUNT_BRANCH ( a_poNode->f_oBranch [ 0 ] );
	return ( - l_dLeftValue );
	M_EPILOG
	}

double HAnalyser::bracket ( HAnalyserNode * a_poNode )
	{
	M_PROLOG
	double l_dLeftValue;
	l_dLeftValue = M_COUNT_BRANCH ( a_poNode->f_oBranch [ 0 ] );
	return ( l_dLeftValue );
	M_EPILOG
	}

bool HAnalyser::translate ( char const * a_pcFormula )
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
							n_ppcFunctionsMnemonics [ l_iCtr ],
							n_piFunctionMnemonicsLength [ l_iCtr ] ) == 0 )
				{
				f_oFormula [ l_iRealIndex ] = static_cast < char > ( l_iCtr + 1 );
				break;
				}
			if ( l_iCtr < 16 )
				{
				l_iIndex += n_piFunctionMnemonicsLength [ l_iCtr ];
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
			f_oFormula [ l_iRealIndex ] = a_pcFormula [ l_iIndex ];
			l_iIndex ++;
			l_iRealIndex ++;
			}
		f_iLength = l_iRealIndex;
		}
	f_oTerminalIndexes [ l_iRealIndex ] = l_iIndex;
	f_oFormula [ l_iRealIndex ] = 0;
	return ( false );
	M_EPILOG
	}

bool HAnalyser::addition_production ( HAnalyserNode * a_poNode )
	{
	M_PROLOG
	int l_iCtr = 0;
	HAnalyserNode * l_poTrunk = NULL;
	M_ASSERT ( a_poNode );
	if ( multiplication_production ( a_poNode->grow_up_branch ( ) ) )
		return ( true );
	if ( f_iIndex > f_iLength )
		{
		f_eError = E_UNEXPECTED_TRMINATION;
		return ( true );
		}
	a_poNode->METHOD = & HAnalyser::addition;
	if ( ( f_oFormula [ f_iIndex ] != '+' ) && ( f_oFormula[ f_iIndex ] != '-' ) )
		{
		if ( ! a_poNode->f_poTrunk )
			return ( false );
		l_poTrunk = dynamic_cast < HAnalyserNode * > ( a_poNode->f_poTrunk );
		M_ASSERT ( l_poTrunk );
		while ( l_poTrunk->f_oBranch [ l_iCtr ] != a_poNode )
			l_iCtr ++;
		l_poTrunk->f_oBranch [ l_iCtr ] = a_poNode->f_oBranch [ 0 ];
		a_poNode->f_oBranch [ 0 ] = 0;
		if ( a_poNode->f_oBranch.quantity ( ) > 1 )
			a_poNode->f_oBranch [ 1 ] = 0;
		if ( a_poNode )
			delete a_poNode;
		return ( false );
		}
	while ( ( f_oFormula [ f_iIndex ] == '+' )
			|| ( f_oFormula [ f_iIndex ] == '-' ) )
		{
		l_iCtr = ( f_oFormula [ f_iIndex ++ ] == '+' ) ? 0 : 1;
		a_poNode->f_tLeaf.add_tail ( ) = reinterpret_cast < double * > ( l_iCtr );
		if ( multiplication_production ( a_poNode->grow_up_branch ( ) ) )
			return ( true );
		}
	return ( false );
	M_EPILOG
	}

bool HAnalyser::multiplication_production ( HAnalyserNode * a_poNode )
	{
	M_PROLOG
	int l_iCtr = 0;
	HAnalyserNode * l_poTrunk = NULL;
	if ( power_production ( static_cast < HAnalyserNode * > ( a_poNode->grow_up_branch ( ) ) ) )
		return ( true );
	if ( f_iIndex > f_iLength )
		{
		f_eError = E_UNEXPECTED_TRMINATION;
		return ( true );
		}
	a_poNode->METHOD = & HAnalyser::multiplication;
	if ( ( f_oFormula [ f_iIndex ] != '*' ) && ( f_oFormula[ f_iIndex ] != '/' ) )
		{
		if ( ! a_poNode->f_poTrunk )
			return ( false );
		l_poTrunk = dynamic_cast < HAnalyserNode * > ( a_poNode->f_poTrunk );
		M_ASSERT ( l_poTrunk );
		while ( l_poTrunk->f_oBranch [ l_iCtr ] != a_poNode )
			l_iCtr ++;
		l_poTrunk->f_oBranch [ l_iCtr ] = a_poNode->f_oBranch [ 0 ];
		a_poNode->f_oBranch [ 0 ] = 0;
		if ( a_poNode->f_oBranch.quantity ( ) > 1 )
			a_poNode->f_oBranch [ 1 ] = 0;
		if ( a_poNode )
			delete a_poNode;
		return ( false );
		}
	while ( ( f_oFormula [ f_iIndex ] == '*' )
			|| ( f_oFormula [ f_iIndex ] == '/' ) )
		{
		l_iCtr = ( f_oFormula [ f_iIndex ++ ] == '*' ) ? 0 : 1;
		a_poNode->f_tLeaf.add_tail ( ) = reinterpret_cast < double * > ( l_iCtr );
		if ( power_production( a_poNode->grow_up_branch ( ) ) )
			return ( true );
		}
	return ( false );
	M_EPILOG
	}

bool HAnalyser::power_production ( HAnalyserNode * a_poNode )
	{
	M_PROLOG
	int l_iCtr = 0;
	HAnalyserNode * l_poTrunk;
	if ( signum_production( a_poNode->grow_up_branch ( ) ) )
		return ( true );
	if ( f_iIndex > f_iLength )
		{
		f_eError = E_UNEXPECTED_TRMINATION;
		return ( true );
		}
	a_poNode->METHOD = & HAnalyser::bracket;
	if ( f_oFormula [ f_iIndex ] == '^' )
		{
		f_iIndex ++;
		if ( power_production( a_poNode->grow_up_branch ( ) ) )
			return ( true );
		a_poNode->METHOD = & HAnalyser::power;
		}
	else
		{
		if ( ! a_poNode->f_poTrunk )
			return ( false );
		l_poTrunk = dynamic_cast < HAnalyserNode * > ( a_poNode->f_poTrunk );
		M_ASSERT ( l_poTrunk );
		while ( l_poTrunk->f_oBranch [ l_iCtr ] != a_poNode )
			l_iCtr ++;
		l_poTrunk->f_oBranch [ l_iCtr ] = a_poNode->f_oBranch [ 0 ];
		a_poNode->f_oBranch [ 0 ] = 0;
		if ( a_poNode->f_oBranch.quantity ( ) > 1 )
			a_poNode->f_oBranch [ 1 ] = 0;
		if ( a_poNode )
			delete a_poNode;
		}
	return ( false );
	M_EPILOG
	}

bool HAnalyser::signum_production ( HAnalyserNode * a_poNode )
	{
	M_PROLOG
	if ( f_iIndex > f_iLength )
		{
		f_eError = E_UNEXPECTED_TRMINATION;
		return ( true );
		}
	if ( f_oFormula [ f_iIndex ] == '-' )
		{
		f_iIndex ++;
		if ( terminal_production ( a_poNode->grow_up_branch ( ) ) )
			return ( true );
		a_poNode->METHOD = & HAnalyser::signum;
		}
	else
		if ( terminal_production ( a_poNode ) )
			return ( true );
	return ( false );
	M_EPILOG
	}

bool HAnalyser::terminal_production ( HAnalyserNode * a_poNode )
	{
	M_PROLOG
	if ( f_iIndex > f_iLength )
		{
		f_eError = E_UNEXPECTED_TRMINATION;
		return ( true );
		}
	switch ( f_oFormula [ f_iIndex ] )
		{
		case '(' :
			{
			f_iIndex ++;
			if ( addition_production ( a_poNode ) )
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
			if ( addition_production ( a_poNode->grow_up_branch ( ) ) )
				return ( true );
			a_poNode->METHOD = & HAnalyser::functions;
			a_poNode->f_tLeaf.add_tail ( ) = reinterpret_cast < double * > ( D_ABS );
			if ( f_oFormula [ f_iIndex ] != '|' )
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
	if ( ( f_oFormula [ f_iIndex ] >= 'A' )
			&& ( f_oFormula [ f_iIndex ] <= 'Z' ) )
		{
		a_poNode->f_tLeaf.add_tail ( ) = f_pdVariables + f_oFormula [ f_iIndex ++ ] - 'A';
		return ( false );
		}
	if ( ( f_oFormula [ f_iIndex ] > D_FUNCTIONS )
			&& ( f_oFormula [ f_iIndex ] < D_ABS ) )
		{
		f_iIndex ++;
		if ( f_oFormula [ f_iIndex ] == '(' )
			{
			f_iIndex ++;
			a_poNode->METHOD = & HAnalyser::functions;
			a_poNode->f_tLeaf.add_tail ( ) = reinterpret_cast < double * > ( static_cast < int > ( f_oFormula [ f_iIndex - 2 ] ) );
			if ( addition_production ( a_poNode->grow_up_branch ( ) ) )
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
		if ( f_iIndex >= f_iLength )
			{
			f_eError = E_UNEXPECTED_TRMINATION;
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
		a_poNode->f_tLeaf.add_tail ( ) = & f_oConstantsPool.add ( l_dValue );
		return ( false );
		}
	f_eError = E_UNEXPECTED_TOKEN;
	return ( true );
	M_EPILOG
	}

double * HAnalyser::analyse ( char const * a_pcFormula )
	{
	M_PROLOG
	int l_iLength = 0;
	HAnalyserNode * l_poNode = NULL;
	f_iIndex = 0;
	f_eError = E_OK;
	l_iLength = strlen ( a_pcFormula );
	if ( l_iLength == 0 )
		return ( NULL );
	f_oFormula.hs_realloc ( l_iLength + 1 ); /* + 1 for trailing null */
	if ( translate ( a_pcFormula ) )
		return ( NULL );
	if ( f_poRoot )
		{
		l_poNode = dynamic_cast < HAnalyserNode * > ( f_poRoot );
		M_ASSERT ( l_poNode );
		delete l_poNode;
		}
	f_oConstantsPool.reset ( );
	f_poRoot = 0;
	f_poRoot = l_poNode = new HAnalyserNode ( 0 );
	l_poNode->f_poTree = this;
	if ( ! addition_production ( dynamic_cast < HAnalyserNode * > ( f_poRoot ) ) )
		if ( f_iIndex < f_iLength )
			f_eError = E_PREMATURE_TRMINATION;
	if ( f_eError != E_OK )
		return ( NULL );
	return ( f_pdVariables );
	M_EPILOG
	}

double & HAnalyser::operator [ ] ( int a_iIndex )
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

double HAnalyser::count ( void )
	{
	M_PROLOG
	HAnalyserNode * l_poRoot = NULL;
	if ( ! f_poRoot )
		M_THROW ( "logic tree is not compiled", static_cast < int > ( f_eError ) );
	l_poRoot = dynamic_cast < HAnalyserNode * > ( f_poRoot );
	M_ASSERT ( l_poRoot );
	return ( ( this->* ( l_poRoot->METHOD ) ) ( l_poRoot ) );
	M_EPILOG
	}

char const * HAnalyser::get_error ( void ) const
	{
	M_PROLOG
	switch ( f_eError )
		{
		case ( E_OK ):
			return ( _ ( "succesful" ) );
		case ( E_UNKNOWN_MNEMONIC ):
			return ( _ ( "unknown mnemonic" ) );
		case ( E_UNEXPECTED_TRMINATION ):
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
		case ( E_PREMATURE_TRMINATION ):
			return ( _ ( "premature termination" ) );
		default :
			M_THROW ( _ ( "enknown error code" ), static_cast < int > ( f_eError ) );
		}
	M_EPILOG
	}

int HAnalyser::get_error_token ( void ) const
	{
	M_PROLOG
	return ( f_oTerminalIndexes [ f_iIndex ] );
	M_EPILOG
	}

}

}

