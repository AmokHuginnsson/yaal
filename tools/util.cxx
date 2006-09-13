/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	util.cxx - this file is integral part of `yaal' project.

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

#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cstdarg>

#include "hcore/hexception.h"
M_VCSID ( "$Id$" )
#include "hcore/hpointer.h"
#include "util.h"
#include "hanalyser.h"
#include "hcore/hlog.h"

using namespace yaal::hcore;

namespace yaal
{

namespace tools
{

namespace util
{

namespace
	{
	static int const D_MIN_IBAN_LENGTH = 20;
	static int const D_STEP_LENGTH = 4;
	}

char n_pcTransTableStripPL [ 256 ];

char n_ppcJednNastki [ ] [ 16 ] =
	{
	"zero ",
	"jeden ",
	"dwa ",
	"trzy ",
	"cztery ",
	"piêæ ",
	"sze¶æ ",
	"siedem ",
	"osiem ",
	"dziewiêæ ",
	"dziesiêæ ",
	"jedena¶cie ",
	"dwana¶cie ",
	"trzyna¶cie ",
	"czterna¶cie ",
	"piêtna¶cie ",
	"szesna¶cie ",
	"siedemna¶cie ",
	"osiemna¶cie ",
	"dziewiêtna¶cie ",
	"dwadzie¶cia "
	};

char n_ppcDzies [ ] [ 24 ] =
	{
	"dziesiêæ-zero ",
	"dziesiêæ ",
	"dwadzie¶cia ",
	"trzydzie¶ci ",
	"czterdzie¶ci ",
	"piêædziesi±t ",
	"sze¶ædziesi±t ",
	"siedemdziesi±t ",
	"osiemdziesi±t ",
	"dziewiêædziesi±t ",
	"sto "
	};

char n_ppcSetki [ ] [ 16 ] =
	{
	"sto-zero ",
	"sto ",
	"dwie¶cie ",
	"trzysta ",
	"czterysta ",
	"piêæset ",
	"sze¶æset ",
	"siedemset ",
	"osiemset ",
	"dziewiêæset ",
	"tysi±c "
	};

char n_ppcTemat [ ] [ 12 ] =
	{
	"grosz",
	"z³ot",
	"tysi",
	"milion",
	"miliard",
	"bilion",
	"biliard",
	"trylion",
	"tryliard"
	};

char n_pppcKoncowka [ ] [ 3 ] [ 6 ] =
	{
		{
		"",
		"e",
		"y"
		},
		{
		"y ",
		"e ",
		"ych "
		},
		{
		"±c ",
		"±ce ",
		"êcy "
		},
		{
		" ",
		"y ",
		"ów "
		},
		{
		" ",
		"y ",
		"ów "
		},
		{
		" ",
		"y ",
		"ów "
		},
		{
		" ",
		"y ",
		"ów "
		},
		{
		" ",
		"y ",
		"ów "
		},
		{
		" ",
		"y ",
		"ów "
		}
	};

HString n_oLastErrorMessage;

HString kwota_slownie ( double a_dKwota )
	{
	M_PROLOG
	int l_iCtr = 0, l_iLength = 0, l_iForma = 0, l_iSub = 0;
	char l_cCyfra = 0;
	HString l_oSlownie;
	HString l_oString;
	HString l_oPrzypadek;
	l_oString.format ( "%.2f", a_dKwota );
	l_iLength = l_oString.get_length ( );
	for ( l_iCtr = 0; l_iCtr < l_iLength; l_iCtr ++ )
		{
		if ( ( l_iCtr % 3 ) == 0 )
			{
			l_iSub = ( ( l_iLength - l_iCtr ) > 1 ? 2 : 1 );
			l_iForma = atoi ( l_oString.mid ( ( l_iLength - l_iCtr ) - l_iSub, l_iSub ) );
			if ( ( l_iCtr > 5 ) && ( ( l_iLength - l_iCtr ) > 2 ) &&
					! strncmp ( static_cast < char const * const > ( l_oString ) + ( l_iLength - l_iCtr ) - 3, "000", 3 ) )
				continue;
			}
		if ( l_iCtr == 2 )
			continue;
		l_cCyfra = l_oString [ ( l_iLength - l_iCtr ) - 1 ] - '0';
		switch ( l_iCtr % 3 )
			{
			case ( 0 ) :
				{
				l_oPrzypadek = n_ppcTemat [ l_iCtr / 3 ];
				if ( l_iForma == 1 )
					l_oPrzypadek += n_pppcKoncowka [ l_iCtr / 3 ] [ 0 ];
				else if ( ( ( ( l_iForma % 10 ) > 1 ) && ( ( l_iForma % 10 ) < 5 ) )
						&& ( ( l_iForma < 10 ) || ( l_iForma > 20 ) ) )
					l_oPrzypadek += n_pppcKoncowka [ l_iCtr / 3 ] [ 1 ];
				else
					l_oPrzypadek += n_pppcKoncowka [ l_iCtr / 3 ] [ 2 ];
				l_oSlownie = l_oPrzypadek + l_oSlownie;
				if ( ( l_iForma < 20 ) &&  ( l_iForma > 9 ) )
					l_oSlownie = n_ppcJednNastki [ l_iForma ] + l_oSlownie;
				else if ( l_cCyfra )
					l_oSlownie = n_ppcJednNastki [ static_cast < int > ( l_cCyfra ) ] + l_oSlownie;
				else if ( ! l_iForma && ( ( l_iCtr < 3 ) || ( a_dKwota < 1 ) ) )
					l_oSlownie = n_ppcJednNastki [ 0 ] + l_oSlownie;
				}
			break;
			case ( 1 ) :
				if ( l_iForma > 19 )
					l_oSlownie = n_ppcDzies [ static_cast < int > ( l_cCyfra ) ] + l_oSlownie;
			break;
			case ( 2 ) :
				if ( l_cCyfra )
					l_oSlownie = n_ppcSetki [ static_cast < int > ( l_cCyfra ) ] + l_oSlownie;
			break;
			default:
			break;
			}
		}
	return ( l_oSlownie );
	M_EPILOG
	}

void usun_ogonki ( char * a_pcString )
	{
	int l_iCtr = 0;
	while ( a_pcString [ l_iCtr ] )
		{
		a_pcString [ l_iCtr ] = n_pcTransTableStripPL [ static_cast < char unsigned > ( a_pcString [ l_iCtr ] ) ];
		l_iCtr ++;
		}
	return;
	}

double atof_ex ( char const * a_pcString, bool a_bParse )
	{
	static HAnalyser l_oAnalyser;
	HString l_oStr = a_pcString;
	l_oStr.replace ( ",", "." );
	l_oStr.replace ( " ", "" );
	l_oStr.replace ( "\t", "" );
	if ( a_bParse && l_oAnalyser.analyse ( l_oStr ) )
		return ( l_oAnalyser.count ( ) );
	return ( strtod ( l_oStr, NULL ) );
	}

int modulo_ASCII ( char const * const a_pcASCIINumber, int a_iModulo )
	{
	int l_iCtr = 0, l_iNumber = 0, l_iStep = 0, l_iTmpLength = 0;
	int l_iLength = strlen ( a_pcASCIINumber );
	HString l_oTmpString, l_oTmpNumber = a_pcASCIINumber;
	if ( l_iLength < 0 )
		M_THROW ( "bad ASCII number length", l_iLength );
	if ( ! a_iModulo )
		M_THROW ( "zero denominatior", a_iModulo );
	while ( l_oTmpNumber.get_length ( ) > D_STEP_LENGTH )
		{
		l_oTmpString = l_oTmpNumber.mid ( l_iStep * D_STEP_LENGTH, D_STEP_LENGTH );
		l_iTmpLength = l_oTmpString.get_length ( );
		l_iNumber = strtol ( l_oTmpString, NULL, 10 );
		l_iNumber %= a_iModulo;
		l_oTmpString.format ( "%d", l_iNumber );
		l_oTmpNumber.shift_left ( l_iTmpLength - l_oTmpString.get_length ( ) );
		l_iTmpLength = l_oTmpString.get_length ( );
		for ( l_iCtr = 0; l_iCtr < l_iTmpLength; l_iCtr ++ )
			l_oTmpNumber [ l_iCtr ] = l_oTmpString [ l_iCtr ];
/*		M_LOG ( l_oTmpNumber ); */
		}
	return ( strtol ( l_oTmpNumber, NULL, 10 ) % a_iModulo );
	}

bool verify_IBAN ( char const * a_pcIBAN )
	{
	int l_iCtr = 0, l_iLength = strlen ( a_pcIBAN );
	char l_pcPattern [ 2 ] = "\0";
	HString l_oIBAN, l_oTmpString;
	if ( l_iLength < D_MIN_IBAN_LENGTH )
		{
		n_oLastErrorMessage.format ( "IBAN: Number too short (%d).", l_iLength );
		return ( true );
		}
	l_oIBAN.hs_realloc ( l_iLength );
	for ( l_iCtr = 0; l_iCtr < l_iLength; l_iCtr ++ )
		if ( isalnum ( a_pcIBAN [ l_iCtr ] ) )
			l_oIBAN += a_pcIBAN [ l_iCtr ];
	l_iLength = l_oIBAN.get_length ( );
	if ( l_iLength < D_MIN_IBAN_LENGTH )
		{
		n_oLastErrorMessage.format ( "IBAN: Number too short (%d).", l_iLength );
		return ( true );
		}
	if ( ! ( isalpha ( l_oIBAN [ 0 ] ) && isalpha ( l_oIBAN [ 1 ] ) ) )
		{
		n_oLastErrorMessage = "IBAN: No country code present.";
		return ( true );
		}
	l_oTmpString = l_oIBAN.left ( 4 );
	l_oIBAN.shift_left ( 4 );
	l_oIBAN += l_oTmpString;
/*	M_LOG ( l_oIBAN ); */
	l_oIBAN.lower ( );
	for ( l_iCtr = 0; l_iCtr < l_iLength; l_iCtr ++ )
		{
		if ( isalpha ( l_oIBAN [ l_iCtr ] ) )
			{
			l_oTmpString.format ( "%02d", ( l_oIBAN [ l_iCtr ] - 'a' ) + 10 );
			l_pcPattern [ 0 ] = l_oIBAN [ l_iCtr ];
			l_oIBAN.replace ( l_pcPattern, l_oTmpString );
			l_iLength = l_oIBAN.get_length ( );
			}
		}
/*	M_LOG ( l_oIBAN ); */
	l_iCtr = modulo_ASCII ( l_oIBAN, 97 );
	if ( l_iCtr == 1 )
		return ( false );
	n_oLastErrorMessage.format ( "IBAN: bad checksum: %d", l_iCtr );
	return ( true );
	}

char const * const get_last_error ( void )
	{
	if ( n_oLastErrorMessage )
		return ( n_oLastErrorMessage );
	return ( "" );
	}

void failure ( int a_iExitStatus, char const * const a_pcFormat, ... )
	{
	va_list l_xAp;
	va_start ( l_xAp, a_pcFormat );
	vfprintf ( stderr, a_pcFormat, l_xAp );
	log << "failure: ";
	log ( a_pcFormat, l_xAp );
	va_end ( l_xAp );
	throw ( a_iExitStatus );
	}

namespace
{
int min3 ( int a, int b, int c )
	{
	int ret = c;
	if ( a < ret )
		ret = a;
	if ( b < ret )
		ret = b;
	return ( ret );
	}
int min ( int a, int b )
	{
	return ( a < b ? a : b );
	}
}

namespace distance
{

int levenshtein_damerau ( char const * const a_pcOne, char const * const a_pcTwo, bool a_bDamerau )
	{
	int l_iCost = 0;
	int l_iIndexOne = 0, l_iIndexTwo = 0;
	int l_iLengthOne = strlen ( a_pcOne );
	int l_iLengthTwo = strlen ( a_pcTwo );
	int * * l_ppiDistanceMatrix = NULL;
	if ( ! l_iLengthTwo )
		return ( l_iLengthOne );
	if ( ! l_iLengthOne )
		return ( l_iLengthTwo );
	l_iLengthOne ++;
	l_iLengthTwo ++;
	HPointer < int *, HPointerArray, HPointerRelaxed > l_oDistanceMatrixHolder ( new int * [ l_iLengthOne ] );
	HPointer < int, HPointerArray, HPointerRelaxed > l_oDistanceMatrix ( new int [ l_iLengthOne * l_iLengthTwo ] );
	l_ppiDistanceMatrix = l_oDistanceMatrixHolder.raw ( );
	for ( l_iIndexOne = 0; l_iIndexOne < l_iLengthOne; ++ l_iIndexOne )
		l_ppiDistanceMatrix [ l_iIndexOne ] = l_oDistanceMatrix.raw ( ) + l_iIndexOne * l_iLengthTwo;
	for ( l_iIndexOne = 0; l_iIndexOne < l_iLengthOne; ++ l_iIndexOne )
		l_ppiDistanceMatrix [ l_iIndexOne ] [ 0 ] = l_iIndexOne;
	for ( l_iIndexTwo = 0; l_iIndexTwo < l_iLengthTwo; ++ l_iIndexTwo )
		l_ppiDistanceMatrix [ 0 ] [ l_iIndexTwo ] = l_iIndexTwo;
	l_iLengthTwo --;
	l_iLengthOne --;
	/* real magic starts here */
	for ( l_iIndexOne = 0; l_iIndexOne < l_iLengthOne; ++ l_iIndexOne )
		{
		for ( l_iIndexTwo = 0; l_iIndexTwo < l_iLengthTwo; ++ l_iIndexTwo )
			{
			l_iCost = 0;
			if ( a_pcOne [ l_iIndexOne ] != a_pcTwo [ l_iIndexTwo ] )
				l_iCost = 1;
			l_ppiDistanceMatrix [ l_iIndexOne + 1 ] [ l_iIndexTwo + 1 ] = min3 (
					l_ppiDistanceMatrix [ l_iIndexOne ] [ l_iIndexTwo + 1 ] + 1,
					l_ppiDistanceMatrix [ l_iIndexOne + 1 ] [ l_iIndexTwo ] + 1,
					l_ppiDistanceMatrix [ l_iIndexOne ] [ l_iIndexTwo ] + l_iCost );
			if ( a_bDamerau && ( l_iIndexOne > 0 ) && ( l_iIndexTwo > 0 )
					&& ( a_pcOne [ l_iIndexOne - 1 ] == a_pcTwo [ l_iIndexTwo ] )
					&& ( a_pcOne [ l_iIndexOne ] == a_pcTwo [ l_iIndexTwo - 1 ] ) )
				l_ppiDistanceMatrix [ l_iIndexOne + 1 ] [ l_iIndexTwo + 1 ] = min ( l_ppiDistanceMatrix [ l_iIndexOne + 1 ] [ l_iIndexTwo + 1 ], l_ppiDistanceMatrix [ l_iIndexOne - 1 ] [ l_iIndexTwo - 1 ] + l_iCost );
			}
		}
	return ( l_ppiDistanceMatrix [ l_iLengthOne ] [ l_iLengthTwo ] );
	}

}

}

}

}

