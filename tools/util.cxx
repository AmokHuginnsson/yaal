/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

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
		{ "", "e", "y" },
		{ "y ", "e ", "ych " },
		{ "±c ", "±ce ", "êcy " },
		{ " ", "y ", "ów " },
		{ " ", "y ", "ów " },
		{ " ", "y ", "ów " },
		{ " ", "y ", "ów " },
		{ " ", "y ", "ów " },
		{ " ", "y ", "ów " }
	};

HString n_oLastErrorMessage;

HString kwota_slownie( double a_dKwota )
	{
	M_PROLOG
	int l_iCtr = 0, l_iLength = 0, l_iForma = 0, l_iSub = 0;
	char l_cCyfra = 0;
	HString l_oSlownie;
	HString l_oString;
	HString l_oPrzypadek;
	l_oString.format ( "%.2f", a_dKwota );
	l_iLength = l_oString.get_length();
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
		l_cCyfra = static_cast<char>( l_oString[ ( l_iLength - l_iCtr ) - 1 ] - '0' );
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
		return ( l_oAnalyser.count() );
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
	while ( l_oTmpNumber.get_length() > D_STEP_LENGTH )
		{
		l_oTmpString = l_oTmpNumber.mid ( l_iStep * D_STEP_LENGTH, D_STEP_LENGTH );
		l_iTmpLength = l_oTmpString.get_length();
		l_iNumber = strtol ( l_oTmpString, NULL, 10 );
		l_iNumber %= a_iModulo;
		l_oTmpString.format ( "%d", l_iNumber );
		l_oTmpNumber.shift_left ( l_iTmpLength - l_oTmpString.get_length() );
		l_iTmpLength = l_oTmpString.get_length();
		for ( l_iCtr = 0; l_iCtr < l_iTmpLength; l_iCtr ++ )
			l_oTmpNumber.set_at( l_iCtr, l_oTmpString[ l_iCtr ] );
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
	l_iLength = l_oIBAN.get_length();
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
	l_oIBAN.lower();
	for ( l_iCtr = 0; l_iCtr < l_iLength; l_iCtr ++ )
		{
		if ( isalpha ( l_oIBAN [ l_iCtr ] ) )
			{
			l_oTmpString.format ( "%02d", ( l_oIBAN [ l_iCtr ] - 'a' ) + 10 );
			l_pcPattern [ 0 ] = l_oIBAN [ l_iCtr ];
			l_oIBAN.replace ( l_pcPattern, l_oTmpString );
			l_iLength = l_oIBAN.get_length();
			}
		}
/*	M_LOG ( l_oIBAN ); */
	l_iCtr = modulo_ASCII ( l_oIBAN, 97 );
	if ( l_iCtr == 1 )
		return ( false );
	n_oLastErrorMessage.format ( "IBAN: bad checksum: %d", l_iCtr );
	return ( true );
	}

char const* get_last_error ( void )
	{
	if ( n_oLastErrorMessage )
		return ( n_oLastErrorMessage );
	return ( "" );
	}

void show_help( OOption* a_psOptions, int a_iCount, char const* const a_pcProgramName, char const* const a_pcIntro, char const* const a_pcNotes )
	{
	::printf(
"Usage: %s [OPTION]... [FILE]...\n"
"%s - %s\n\n"
"Mandatory arguments to long options are mandatory for short options too.\n"
"Options:\n",
			a_pcProgramName, a_pcProgramName, a_pcIntro );
	size_t l_iLongestLongLength = 0;
	size_t l_iLongestShortLength = 0;
	for ( int i = 0; i < a_iCount; ++ i )
		{
		OOption& o = a_psOptions[ i ];
		size_t tmp = ( o.f_pcName ? ::strlen( o.f_pcName ) + 2 : 0 ) + ( o.f_pcArgument ? ::strlen( o.f_pcArgument ) + 1 : 0 ) + ( o.f_eSwitchType == OOption::D_OPTIONAL ? 2 : 1 );
		if ( tmp > l_iLongestLongLength )
			l_iLongestLongLength = tmp;
		tmp = 0;
		if ( a_psOptions[ i ].f_pcShortForm && ( ( tmp = ::strlen( a_psOptions[ i ].f_pcShortForm ) + 1 ) > l_iLongestShortLength ) )
			l_iLongestShortLength = tmp;
		}
	HString desc;
	char const* description = NULL;
	/* display each option description */
	for ( int i = 0; i < a_iCount; ++ i )
		{
		OOption& o = a_psOptions[ i ];
		if ( ! ( o.f_pcShortForm || o.f_pcName ) )
			continue;
		HString sf;
		/* if short form char exist, build full form of short form */
		if ( o.f_pcShortForm )
			{
			sf = "-";
			sf += o.f_pcShortForm;
			}
		char const* coma = o.f_pcShortForm && o.f_pcName ? "," : " ";
		if ( ! description )
			description = o.f_pcDescription;
		/* if long form word exist, build full form of long form */
		HString lf;
		if ( o.f_pcName )
			{
			lf = "--";
			lf += o.f_pcName;
			}
		if ( o.f_pcArgument )
			{
			if ( o.f_eSwitchType == OOption::D_OPTIONAL )
				lf += "[";
			( lf += "=" ) += o.f_pcArgument;
			if ( o.f_eSwitchType == OOption::D_OPTIONAL )
				lf += "]";
			}
		if ( i > 0 ) /* subsequent options */
			{
			OOption& p = a_psOptions[ i - 1 ];
			if ( o.f_pcName && p.f_pcName && ( ! ::strcmp( o.f_pcName, p.f_pcName ) ) )
				{
				lf = "", coma = " ";
				if ( description == o.f_pcDescription )
					description = "";
				}
			if ( o.f_pcShortForm && p.f_pcShortForm && ( ! ::strcmp( o.f_pcShortForm, p.f_pcShortForm ) ) )
				{
				sf = "", coma = " ";
				if ( description == o.f_pcDescription )
					description = "";
				}
			}
		printf( "  %*s%s %-*s ",
				static_cast<int>( l_iLongestShortLength ), sf.raw() ? sf.raw() : "", coma,
				static_cast<int>( l_iLongestLongLength ), lf.raw() ? lf.raw() : "" );
		int cols = 80 - ( l_iLongestLongLength + l_iLongestShortLength + 7 );
		desc = description;
		bool loop = true;
		do
			{
			int eol = 0;
			while ( ( eol < cols ) && ( eol >= 0 ) )
				{
				eol = desc.find_one_of( n_pcWhiteSpace, eol );
				if ( ( eol < 0 ) || ( eol > cols ) )
					break;
				eol = desc.find_other_than( n_pcWhiteSpace, eol );
				}
			if ( eol >= cols )
				{
				printf( "%.*s\n", eol, desc.raw() );
				desc.shift_left( eol );
				desc.trim_left();
				desc.insert( 0, 2, "  " );
				if ( i < ( a_iCount - 1 ) )
					{
					OOption& n = a_psOptions[ i + 1 ];
					if ( ( o.f_pcName && n.f_pcName && ( ! ::strcmp( o.f_pcName, n.f_pcName ) ) )
							|| ( o.f_pcShortForm && n.f_pcShortForm && ( ! ::strcmp( o.f_pcShortForm, n.f_pcShortForm ) ) ) )
						{
						description = desc.raw();
						break;
						}
					}
				printf( "     %*s", static_cast<int>( l_iLongestLongLength + l_iLongestShortLength ), "" );
				}
			else
				{
				printf( "%s\n", desc.raw() );
				description = NULL;
				loop = false;
				}
			}
		while ( loop );
		}
	if ( a_pcNotes )
		::printf( "\n%s\n", a_pcNotes );
	return;
	}

void dump_configuration( OOption* a_psOptions, int a_iCount, char const* const a_pcProgramName, char const* const a_pcIntro, char const* const a_pcNotes )
	{
	M_PROLOG
	a_pcProgramName && ::printf( "# this is configuration file for: `%s' programme\n", a_pcProgramName );
	a_pcIntro && ::printf( "# %s\n", a_pcIntro );
	if ( a_pcProgramName || a_pcIntro )
		::printf( "\n" );
	::printf(
"# comments begin with `#' char and continues until end of line\n"
"# option names are case insensitive\n"
"# in most cases option values are case insensitive also\n"
"# syntax for settings is:\n"
"# ^{option}{white}(['\"]){value1 value2 ... valuen}\\1{white}# comment$\n"
"# value may be surrounded by apostrophes or quotation marks\n"
"# one level of this surrounding is stripped\n"
"# we currently distinguish between four kind of value types:\n"
"# bool   - (true|yes|on|false|no|off)\n"
"# char   - [a-z]\n"
"# int    - [0-1]+\n"
"# string - .+\n"
"#\n"
"# example:\n"
"# log_path ${HOME}/var/log/programme.log\n\n"
	);
	HString desc;
	char const* description = NULL;
	for ( int i = 0; i < a_iCount; ++ i )
		{
		OOption& o = a_psOptions[ i ];
		if ( ! o.f_pcName )
			continue;
		if ( i > 0 ) /* subsequent options */
			{
			OOption& p = a_psOptions[ i - 1 ];
			if ( o.f_pcName && p.f_pcName
					&& ( ! ::strcmp( o.f_pcName, p.f_pcName ) )
					&& ( o.f_pcDescription == description ) )
				description = "";
			if ( o.f_pcShortForm && p.f_pcShortForm
					&& ( ! ::strcmp( o.f_pcShortForm, p.f_pcShortForm ) )
					&& ( o.f_pcDescription == description ) )
				description = "";
			}
		static int const D_MAXIMUM_LINE_LENGTH = 72;
		::printf( "# %s, type: ", o.f_pcName );
		switch( o.f_eValueType )
			{
			case ( D_BOOL ): ::printf( "boolean\n" ); break;
			case ( D_INT ): case ( D_INT_SHORT ): case ( D_INT_LONG ): ::printf( "integer\n" ); break;
			case ( D_FLOAT ): case ( D_DOUBLE ): case ( D_DOUBLE_LONG ): ::printf( "floating point\n" ); break;
			case ( D_CHAR_PTR ): case ( D_HSTRING ): ::printf( "character string\n" ); break;
			default: ::printf( "special\n" ); break;
			}
		if ( ! description )
			description = o.f_pcDescription;
		desc = description;
		bool loop = true;
		do
			{
			int eol = 0;
			while ( ( eol < D_MAXIMUM_LINE_LENGTH ) && ( eol >= 0 ) )
				{
				eol = desc.find_one_of( n_pcWhiteSpace, eol );
				if ( ( eol < 0 ) || ( eol > D_MAXIMUM_LINE_LENGTH ) )
					break;
				eol = desc.find_other_than( n_pcWhiteSpace, eol );
				}
			if ( eol >= D_MAXIMUM_LINE_LENGTH )
				{
				printf( "# %.*s\n", eol, desc.raw() );
				desc.shift_left( eol );
				desc.trim_left();
				description = desc.raw();
				}
			else
				{
				printf( "# %s\n", desc.raw() );
				description = NULL;
				loop = false;
				}
			}
		while ( loop );
		if ( o.f_pvValue )
			{
			switch ( o.f_eValueType )
				{
				case ( D_BOOL ):
					::printf( "%s %s\n", o.f_pcName, *static_cast<bool*>( o.f_pvValue ) ? "true" : "false" );
				break;
				case ( D_HSTRING ):
					{
					HString& s = *static_cast<HString*>( o.f_pvValue );
					::printf( "%s%s %s\n", ! s.is_empty() ? "" : "# ", o.f_pcName, s.raw() ? s.raw() : "" );
					}
				break;
				case ( D_CHAR_PTR ):
					{
					char* ptr = static_cast<char*>( o.f_pvValue );
					::printf( "%s%s %s\n", ptr && ptr[ 0 ] ? "" : "# ", o.f_pcName, ptr );
					}
				break;
				case ( D_INT ):
					::printf( "%s %d\n", o.f_pcName, *static_cast<int*>( o.f_pvValue ) );
				break;
				case ( D_INT_LONG ):
					::printf( "%s %ld", o.f_pcName, *static_cast<int long*>( o.f_pvValue ) );
				break;
				default:
					;
				}
			}
		::printf( "\n" );
		}
	a_pcNotes && ::printf( "# %s\n\n", a_pcNotes );
	::printf( "# vim: ft=conf\n" );
	return;
	M_EPILOG
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

namespace sleep
{

int sleep_real( timeval& a_xTime, bool a_bIgnoreInterrrupt )
	{
	int err = 0;
	while ( ( ( err = ::select( 0, NULL, NULL, NULL, &a_xTime ) ) == -1 ) && ( errno == EINTR ) && a_bIgnoreInterrrupt )
		;
	return ( err );
	}

int milisecond( int a_iQuantity, bool a_bIgnoreInterrrupt )
	{
	timeval wait;
	wait.tv_sec = 0;
	wait.tv_usec = a_iQuantity * 1000;
	return ( sleep_real( wait, a_bIgnoreInterrrupt ) );
	}

int second( int a_iQuantity, bool a_bIgnoreInterrrupt )
	{
	timeval wait;
	wait.tv_sec = a_iQuantity;
	wait.tv_usec = 0;
	return ( sleep_real( wait, a_bIgnoreInterrrupt ) );
	}

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
	int l_iLengthOne = ::strlen( a_pcOne );
	int l_iLengthTwo = ::strlen( a_pcTwo );
	int** l_ppiDistanceMatrix = NULL;
	if ( ! l_iLengthTwo )
		return ( l_iLengthOne );
	if ( ! l_iLengthOne )
		return ( l_iLengthTwo );
	l_iLengthOne ++;
	l_iLengthTwo ++;
	HPointer<int*, HPointerArray> l_oDistanceMatrixHolder( new int*[ l_iLengthOne ] );
	HPointer<int, HPointerArray> l_oDistanceMatrix( new int[ l_iLengthOne * l_iLengthTwo ] );
	l_ppiDistanceMatrix = l_oDistanceMatrixHolder.raw();
	for ( l_iIndexOne = 0; l_iIndexOne < l_iLengthOne; ++ l_iIndexOne )
		l_ppiDistanceMatrix[ l_iIndexOne ] = l_oDistanceMatrix.raw() + l_iIndexOne * l_iLengthTwo;
	for ( l_iIndexOne = 0; l_iIndexOne < l_iLengthOne; ++ l_iIndexOne )
		l_ppiDistanceMatrix[ l_iIndexOne ][ 0 ] = l_iIndexOne;
	for ( l_iIndexTwo = 0; l_iIndexTwo < l_iLengthTwo; ++ l_iIndexTwo )
		l_ppiDistanceMatrix[ 0 ][ l_iIndexTwo ] = l_iIndexTwo;
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
			l_ppiDistanceMatrix[ l_iIndexOne + 1 ][ l_iIndexTwo + 1 ] = min3(
					l_ppiDistanceMatrix[ l_iIndexOne ][ l_iIndexTwo + 1 ] + 1,
					l_ppiDistanceMatrix[ l_iIndexOne + 1 ][ l_iIndexTwo ] + 1,
					l_ppiDistanceMatrix[ l_iIndexOne ][ l_iIndexTwo ] + l_iCost );
			if ( a_bDamerau && ( l_iIndexOne > 0 ) && ( l_iIndexTwo > 0 )
					&& ( a_pcOne[ l_iIndexOne - 1 ] == a_pcTwo[ l_iIndexTwo ] )
					&& ( a_pcOne[ l_iIndexOne ] == a_pcTwo[ l_iIndexTwo - 1 ] ) )
				l_ppiDistanceMatrix[ l_iIndexOne + 1 ][ l_iIndexTwo + 1 ] = min(
						l_ppiDistanceMatrix[ l_iIndexOne + 1 ][ l_iIndexTwo + 1 ],
						l_ppiDistanceMatrix[ l_iIndexOne - 1 ][ l_iIndexTwo - 1 ] + l_iCost );
			}
		}
	return ( l_ppiDistanceMatrix[ l_iLengthOne ][ l_iLengthTwo ] );
	}

}

}

}

}

