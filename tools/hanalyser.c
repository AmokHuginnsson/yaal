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

#include "hcore/hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "hcore/xalloc.h"
#include "hcore/htree.h"
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

#define M_COUNT_BRANCH( arg ) count_branch ( static_cast < HAnalyserNode * > ( arg ) )

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
				 : HTree < HList < double * > >::HNode ( a_poNode )
	{
	M_PROLOG
	METHOD = NULL;
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

HAnalyser::HAnalyser ( void ) : f_oConstantsPool ( 0, D_HPOOL_AUTO_GROW )
	{
	M_PROLOG
	int l_iCtr = 0;
	f_iError = 0;
	f_iLength = 0;
	f_iIndex = 0;
	for ( l_iCtr = 0; l_iCtr < 26; l_iCtr ++ )
		f_pdVariables [ l_iCtr ] = 0;
	return;
	M_EPILOG
	}

HAnalyser::~HAnalyser ( void )
	{
	M_PROLOG
	HAnalyserNode * l_poNode = static_cast < HAnalyserNode * > ( f_poRoot );
	if ( l_poNode )
		delete l_poNode;
	f_poRoot = NULL;
	return;
	M_EPILOG
	}

double HAnalyser::count_branch( HAnalyserNode * a_poNode )
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

double HAnalyser::functions( HAnalyserNode * a_poNode )
	{
	M_PROLOG
	double l_dLeftValue;
	l_dLeftValue = M_COUNT_BRANCH ( a_poNode->f_oBranch [ 0 ] );
	switch ( reinterpret_cast < int > ( a_poNode->f_tLeaf.tail ( ) ) )
		{
		case D_SIN :
			return ( sin ( l_dLeftValue ) );
		case D_SINH :
			return ( sinh ( l_dLeftValue ) );
		case D_COS :
			return ( cos ( l_dLeftValue ) );
		case D_COSH :
			return ( cosh ( l_dLeftValue ) );
		case D_TG	:
			{
			if ( floor( l_dLeftValue / M_PI + .5 ) == ( l_dLeftValue / M_PI + .5 ) )
				return ( 0 );
			return ( tan ( l_dLeftValue ) );
			}
		case D_TGH	:
			return ( tanh ( l_dLeftValue ) );
		case D_CTG	:
			{
			if ( floor( l_dLeftValue / M_PI ) == ( l_dLeftValue / M_PI ) )
				return ( 0 );
			l_dLeftValue = tan ( l_dLeftValue );
			if ( l_dLeftValue == 0 )
				return ( 0 );
			return ( 1. / l_dLeftValue );
			}
		case D_CTGH :
			{
			l_dLeftValue = tanh( l_dLeftValue );
			if ( l_dLeftValue == 0 )
				return ( 0 );
			return ( 1. / l_dLeftValue );
			}
		case D_ARCSIN :
			{
			if ( ( l_dLeftValue < -M_PI / 2 ) || ( l_dLeftValue > M_PI / 2 ) )
				return ( 0 );
			return ( asin ( l_dLeftValue ) );
			}
		case D_ARCCOS :
			{
			if ( ( l_dLeftValue < -M_PI / 2 ) || ( l_dLeftValue > M_PI / 2 ) )
				return ( 0 );
			return ( acos ( l_dLeftValue ) );
			}
		case D_ARCTG :
			return ( atan ( l_dLeftValue ) );
		case D_ARCCTG :
			return ( M_PI -atan ( l_dLeftValue ) );
		case D_EXP :
			return ( exp ( l_dLeftValue ) );
		case D_SQRT :
			{
			if ( l_dLeftValue < 0 )
				return ( sqrt ( -l_dLeftValue ) );
			return ( sqrt ( l_dLeftValue ) );
			}
#ifndef __DEBUGGER_BABUNI__
		case D_LN :
			{
			if ( l_dLeftValue == 0 )
				return ( -1000 );
			if ( l_dLeftValue < 0 )
				return ( log ( -l_dLeftValue ) );
			return ( log ( l_dLeftValue ) );
			}
#endif /* not __DEBUGGER_BABUNI__ */
		case D_LOG :
			{
			if ( l_dLeftValue == 0 )
				return ( -1000 );
			if ( l_dLeftValue < 0 )
				return ( log10 ( -l_dLeftValue ) );
			return ( log10 ( l_dLeftValue ) );
			}
		case D_ABS :
			{
			if ( l_dLeftValue < 0 )
				return ( -l_dLeftValue );
			return ( l_dLeftValue );
			}
		}
	return ( 0 );
	M_EPILOG
	}

double HAnalyser::addition( HAnalyserNode * a_poNode )
	{
	M_PROLOG
	int l_iIndex = 1, l_iQuantity = a_poNode->f_oBranch.quantity ( );
	double l_dLeftValue, l_dRightValue;
	l_dLeftValue = M_COUNT_BRANCH ( a_poNode->f_oBranch [ 0 ] );
	while ( l_iIndex < l_iQuantity )
		{
		l_dRightValue = M_COUNT_BRANCH ( a_poNode->f_oBranch [ l_iIndex ] );
		switch ( reinterpret_cast < int > ( a_poNode->f_tLeaf [ l_iIndex - 1 ] ) )
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
			}
		l_iIndex++;
		}
#ifdef __DEBUGGER_BABUNI__
	::log << "index: " << l_iIndex << endl;
#endif /* __DEBUGGER_BABUNI__ */
	return ( l_dLeftValue );
	M_EPILOG
	}

double HAnalyser::multiplication( HAnalyserNode * a_poNode )
	{
	M_PROLOG
	int l_iIndex = 1, l_iQuantity = a_poNode->f_oBranch.quantity ( );
	double l_dLeftValue, l_dRightValue;
	l_dLeftValue = M_COUNT_BRANCH ( a_poNode->f_oBranch [ 0 ] );
	while ( l_iIndex < l_iQuantity )
		{
		l_dRightValue = M_COUNT_BRANCH ( a_poNode->f_oBranch [ l_iIndex ] );
		switch ( reinterpret_cast < int > ( a_poNode->f_tLeaf [ l_iIndex - 1 ] ) )
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
			}
		l_iIndex++;
		}
	return ( l_dLeftValue );
	M_EPILOG
	}

double HAnalyser::power( HAnalyserNode * a_poNode )
	{
	M_PROLOG
	double l_dLeftValue, l_dRightValue;
	l_dLeftValue = M_COUNT_BRANCH ( a_poNode->f_oBranch [ 0 ] );
	l_dRightValue = M_COUNT_BRANCH ( a_poNode->f_oBranch [ 1 ] );
	if ( ( l_dLeftValue < 0 ) && ( l_dRightValue != floor( l_dRightValue ) ) )
		return ( 0 );
	return ( pow ( l_dLeftValue, l_dRightValue ) );
	M_EPILOG
	}

double HAnalyser::signum( HAnalyserNode * a_poNode )
	{
	M_PROLOG
	double l_dLeftValue;
	l_dLeftValue = M_COUNT_BRANCH ( a_poNode->f_oBranch [ 0 ] );
	return ( - l_dLeftValue );
	M_EPILOG
	}

double HAnalyser::bracket( HAnalyserNode * a_poNode )
	{
	M_PROLOG
	double l_dLeftValue;
	l_dLeftValue = M_COUNT_BRANCH ( a_poNode->f_oBranch [ 0 ] );
	return ( l_dLeftValue );
	M_EPILOG
	}

int HAnalyser::translate( char const * a_pcFormula )
	{
	M_PROLOG
	int l_iError = 0, l_iIndex = 0, l_iRealIndex = 0, l_iCtr = 0, l_iLength = 0;
	l_iLength = strlen ( a_pcFormula );
	while ( l_iIndex < l_iLength )
		{
		if ( ( a_pcFormula [ l_iIndex ] >= 'a' )
				&& ( a_pcFormula [ l_iIndex ] <= 'z' ) )
			{
			for ( l_iCtr = 0; l_iCtr < 16; l_iCtr++ )
				if ( strncmp( a_pcFormula + l_iIndex,
							n_ppcFunctionsMnemonics [ l_iCtr ],
							n_piFunctionMnemonicsLength [ l_iCtr ] ) == 0 )
				{
				f_oFormula [ l_iRealIndex ] = l_iCtr + 1;
				break;
				}
			if ( l_iCtr < 16 )
				{
				l_iIndex += n_piFunctionMnemonicsLength [ l_iCtr ];
				l_iRealIndex ++;
				}
			else
				{
				l_iError ++;
				l_iIndex ++;
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
	f_oFormula [ l_iRealIndex ] = 0;
	f_iError += l_iError;
	return ( l_iError );
	M_EPILOG
	}

void HAnalyser::addition_production ( HAnalyserNode * a_poNode )
	{
	M_PROLOG
	int l_iCtr = 0;
	HAnalyserNode * l_poTrunk = NULL;
	multiplication_production ( a_poNode->grow_up_branch ( ) );
	if ( f_iIndex > f_iLength )
		{
		f_iError++;
		return ;
		}
	a_poNode->METHOD = & HAnalyser::addition;
	if ( ( f_oFormula [ f_iIndex ] != '+' ) && ( f_oFormula[ f_iIndex ] != '-' ) )
		{
		l_poTrunk = static_cast < HAnalyserNode * > ( a_poNode->f_poTrunk );
		if ( ! l_poTrunk )
			return ;
		while ( 1 )
			{
			if ( l_poTrunk->f_oBranch [ l_iCtr ] == a_poNode )
				{
				l_poTrunk->f_oBranch [ l_iCtr ] = a_poNode->f_oBranch [ 0 ];
				break;
				}
			l_iCtr++;
			}
		a_poNode->f_oBranch [ 0 ] = 0;
		if ( a_poNode->f_oBranch.quantity ( ) > 1 )
			a_poNode->f_oBranch [ 1 ] = 0;
		if ( a_poNode )
			delete a_poNode;
		return ;
		}
	while ( ( f_oFormula [ f_iIndex ] == '+' )
			|| ( f_oFormula [ f_iIndex ] == '-' ) )
		{
		l_iCtr = ( f_oFormula [ f_iIndex ++ ] == '+' ) ? 0 : 1;
		a_poNode->f_tLeaf.add_tail ( ) = reinterpret_cast < double * > ( l_iCtr );
		multiplication_production ( a_poNode->grow_up_branch ( ) );
		}
	return;
	M_EPILOG
	}

void HAnalyser::multiplication_production ( HAnalyserNode * a_poNode )
	{
	M_PROLOG
	int l_iCtr = 0;
	HAnalyserNode * l_poTrunk = NULL;
	power_production ( static_cast < HAnalyserNode * > ( a_poNode->grow_up_branch ( ) ) );
	if ( f_iIndex > f_iLength )
		{
		f_iError ++;
		return;
		}
	a_poNode->METHOD = & HAnalyser::multiplication;
	if ( ( f_oFormula [ f_iIndex ] != '*' ) && ( f_oFormula[ f_iIndex ] != '/' ) )
		{
		l_poTrunk = static_cast < HAnalyserNode * > ( a_poNode->f_poTrunk );
		if ( ! l_poTrunk )
			return ;
		while ( 1 )
			{
			if ( l_poTrunk->f_oBranch [ l_iCtr ] == a_poNode )
				{
				l_poTrunk->f_oBranch [ l_iCtr ] = a_poNode->f_oBranch [ 0 ];
				break;
				}
			l_iCtr++;
			}
		a_poNode->f_oBranch [ 0 ] = 0;
		if ( a_poNode->f_oBranch.quantity ( ) > 1 )
			a_poNode->f_oBranch [ 1 ] = 0;
		if ( a_poNode )
			delete a_poNode;
		return ;
		}
	while ( ( f_oFormula [ f_iIndex ] == '*' )
			|| ( f_oFormula [ f_iIndex ] == '/' ) )
		{
		l_iCtr = ( f_oFormula [ f_iIndex ++ ] == '*' ) ? 0 : 1;
		a_poNode->f_tLeaf.add_tail ( ) = reinterpret_cast < double * > ( l_iCtr );
		power_production( a_poNode->grow_up_branch ( ) );
		}
	return ;
	M_EPILOG
	}

void HAnalyser::power_production( HAnalyserNode * a_poNode )
	{
	M_PROLOG
	int l_iCtr = 0;
	HAnalyserNode * l_poTrunk;
	signum_production( a_poNode->grow_up_branch ( ) );
	if ( f_iIndex > f_iLength )
		{
		f_iError++;
		return ;
		}
	a_poNode->METHOD = & HAnalyser::bracket;
	if ( f_oFormula [ f_iIndex ] == '^' )
		{
		f_iIndex++;
		power_production( a_poNode->grow_up_branch ( ) );
		a_poNode->METHOD = & HAnalyser::power;
		}
	else
		{
		l_poTrunk = static_cast < HAnalyserNode * > ( a_poNode->f_poTrunk );
		if ( ! l_poTrunk )
			return;
		while ( 1 )
			{
			if ( l_poTrunk->f_oBranch [ l_iCtr ] == a_poNode )
				{
				l_poTrunk->f_oBranch [ l_iCtr ] = a_poNode->f_oBranch [ 0 ];
				break;
				}
			l_iCtr++;
			}
		a_poNode->f_oBranch [ 0 ] = 0;
		if ( a_poNode->f_oBranch.quantity ( ) > 1 )
			a_poNode->f_oBranch [ 1 ] = 0;
		if ( a_poNode )
			delete a_poNode;
		}
	return ;
	M_EPILOG
	}

void HAnalyser::signum_production( HAnalyserNode * a_poNode )
	{
	M_PROLOG
	if ( f_iIndex > f_iLength )
		{
		f_iError++;
		return ;
		}
	if ( f_oFormula [ f_iIndex ] == '-' )
		{
		f_iIndex++;
		terminal_production ( a_poNode->grow_up_branch ( ) );
		a_poNode->METHOD = & HAnalyser::signum;
		}
	else
		terminal_production ( a_poNode );
	return ;
	M_EPILOG
	}

void HAnalyser::terminal_production ( HAnalyserNode * a_poNode )
	{
	M_PROLOG
	if ( f_iIndex > f_iLength )
		{
		f_iError++;
		return ;
		}
	switch ( f_oFormula [ f_iIndex ] )
		{
		case '(' :
			{
			f_iIndex++;
			addition_production ( a_poNode );
			if ( f_oFormula [ f_iIndex ] != ')' )
				f_iError++;
			else
				f_iIndex++;
			return ;
			}
		case '|' :
			{
			f_iIndex++;
			addition_production ( a_poNode->grow_up_branch ( ) );
			a_poNode->METHOD = & HAnalyser::functions;
			a_poNode->f_tLeaf.add_tail ( ) = reinterpret_cast < double * > ( D_ABS );
			if ( f_oFormula [ f_iIndex ] != '|' )
				f_iError++;
			else
				f_iIndex++;
			return ;
			}
		}
	if ( ( f_oFormula[ f_iIndex ] >= 'A' ) && ( f_oFormula[ f_iIndex ] <= 'Z' ) )
		{
		a_poNode->f_tLeaf.add_tail ( ) = f_pdVariables + f_oFormula[ f_iIndex++] - 'A';
		return;
		}
	if ( ( f_oFormula [ f_iIndex ] > D_FUNCTIONS )
			&& ( f_oFormula [ f_iIndex ] < D_ABS ) )
		{
		f_iIndex++;
		if ( f_oFormula [ f_iIndex ] == '(' )
			{
			f_iIndex++;
			a_poNode->METHOD = & HAnalyser::functions;
			a_poNode->f_tLeaf.add_tail ( ) = reinterpret_cast < double * > ( static_cast < int > ( f_oFormula [ f_iIndex - 2 ] ) );
			addition_production ( a_poNode->grow_up_branch ( ) );
			if ( f_oFormula [ f_iIndex ] != ')' )
				f_iError++;
			else
				f_iIndex++;
			}
		else
			f_iError++;
		return ;
		}
	if ( ( f_oFormula [ f_iIndex ] >= '0' ) && ( f_oFormula[ f_iIndex ] <= '9' ) )
		{
		double l_dValue = 0;
		int l_iOffset = f_iIndex;
		if ( f_iIndex >= f_iLength )
			{
			f_iError++;
			return ;
			}
		while ( ( f_oFormula [ f_iIndex ] >= '0' )
				&& ( f_oFormula [ f_iIndex ] <= '9' ) )f_iIndex++;
		if ( f_oFormula [ f_iIndex ] == '.' )
			{
			f_iIndex++;
			if ( ( f_oFormula [ f_iIndex ] >= '0' )
					&& ( f_oFormula [ f_iIndex ] <= '9' ) )
				while ( ( f_oFormula [ f_iIndex ] >= '0' )
						&& ( f_oFormula [ f_iIndex ] <= '9' ) ) f_iIndex++;
			else
				f_iError++;
			}
		if ( f_iError == 0 )
			{
			l_dValue = strtod ( static_cast < char * > ( f_oFormula ) + l_iOffset, NULL );
			a_poNode->f_tLeaf.add_tail ( ) = & f_oConstantsPool.add ( l_dValue );
			}
		return ;
		}
	f_iError++;
	return ;
	M_EPILOG
	}

double * HAnalyser::analyse( char const * a_pcFormula )
	{
	M_PROLOG
	int l_iLength = 0;
	HAnalyserNode * l_poNode = static_cast < HAnalyserNode * > ( f_poRoot );
	f_iIndex = 0;
	f_iError = 0;
	l_iLength = strlen ( a_pcFormula );
	if ( l_iLength == 0 )
		return ( NULL );
	f_oFormula.hs_realloc ( l_iLength + 1 ); /* + 1 for trailing null */
	if ( translate ( a_pcFormula ) > 0 )
		return ( NULL );
	if ( l_poNode )
		delete l_poNode;
	f_oConstantsPool.reset ( );
	f_poRoot = 0;
	f_poRoot = l_poNode = new HAnalyserNode ( 0 );
	l_poNode->f_poTree = this;
	addition_production( static_cast < HAnalyserNode * > ( f_poRoot ) );
	if ( f_iIndex < f_iLength )
		f_iError++;
	if ( f_iError > 0 )
		return ( NULL );
	return ( f_pdVariables );
	M_EPILOG
	}

double & HAnalyser::operator [ ] ( int a_iIndex )
	{
	M_PROLOG
	if ( ( a_iIndex >= 'a' ) && ( a_iIndex <= 'a' ) )
		a_iIndex = a_iIndex - 'a' + 'A';
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
	HAnalyserNode * l_poRoot = static_cast < HAnalyserNode * > ( f_poRoot );
	if ( ! l_poRoot )
		M_THROW ( "logic tree is not compiled", f_iError );
	return ( ( this->* ( l_poRoot->METHOD ) ) ( l_poRoot ) );
	M_EPILOG
	}

}

}

