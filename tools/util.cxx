/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	util.cxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cstdarg>
#include <sys/time.h> /* timeval (for FreeBSD) */

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hcore/hpointer.hxx"
#include "hcore/htokenizer.hxx"
#include "util.hxx"
#include "hexpression.hxx"
#include "hcore/hlog.hxx"
#include "hdes.hxx"
#include "xmath.hxx"
#include "hcore/hrandomizer.hxx"

using namespace yaal::hcore;

namespace yaal
{

namespace tools
{

namespace util
{

namespace
	{
	static int const MIN_IBAN_LENGTH = 20;
	static int const STEP_LENGTH = 4;
	}

char _transTableStripPL_ [ 256 ];

char _jednNastki_ [ ] [ 16 ] =
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

char _dzies_ [ ] [ 24 ] =
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

char _setki_ [ ] [ 16 ] =
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

char _temat_ [ ] [ 12 ] =
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

char _koncowka_ [ ] [ 3 ] [ 6 ] =
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

HString _lastErrorMessage_;

HString kwota_slownie( double kwota_ )
	{
	M_PROLOG
	int ctr = 0;
	int long length = 0;
	int forma = 0, sub = 0;
	char cyfra = 0;
	HString slownie;
	HString string;
	HString przypadek;
	string.format( "%.2f", kwota_ );
	length = string.get_length();
	for ( ctr = 0; ctr < length; ctr ++ )
		{
		if ( ( ctr % 3 ) == 0 )
			{
			sub = ( ( length - ctr ) > 1 ? 2 : 1 );
			forma = lexical_cast<int>( string.mid( ( length - ctr ) - sub, sub ) );
			if ( ( ctr > 5 ) && ( ( length - ctr ) > 2 ) &&
					! ::strncmp( string.raw() + ( length - ctr ) - 3, "000", 3 ) )
				continue;
			}
		if ( ctr == 2 )
			continue;
		cyfra = static_cast<char>( string[ ( length - ctr ) - 1 ] - '0' );
		switch ( ctr % 3 )
			{
			case ( 0 ) :
				{
				przypadek = _temat_[ ctr / 3 ];
				if ( forma == 1 )
					przypadek += _koncowka_[ ctr / 3 ] [ 0 ];
				else if ( ( ( ( forma % 10 ) > 1 ) && ( ( forma % 10 ) < 5 ) )
						&& ( ( forma < 10 ) || ( forma > 20 ) ) )
					przypadek += _koncowka_[ ctr / 3 ] [ 1 ];
				else
					przypadek += _koncowka_[ ctr / 3 ] [ 2 ];
				slownie = przypadek + slownie;
				if ( ( forma < 20 ) &&  ( forma > 9 ) )
					slownie = _jednNastki_[ forma ] + slownie;
				else if ( cyfra )
					slownie = _jednNastki_[ static_cast<int>( cyfra ) ] + slownie;
				else if ( ! forma && ( ( ctr < 3 ) || ( kwota_ < 1 ) ) )
					slownie = _jednNastki_[ 0 ] + slownie;
				}
			break;
			case ( 1 ) :
				if ( forma > 19 )
					slownie = _dzies_[ static_cast<int>( cyfra ) ] + slownie;
			break;
			case ( 2 ) :
				if ( cyfra )
					slownie = _setki_[ static_cast<int>( cyfra ) ] + slownie;
			break;
			default:
			break;
			}
		}
	return ( slownie );
	M_EPILOG
	}

void usun_ogonki ( char * string_ )
	{
	M_PROLOG
	int ctr = 0;
	while ( string_ [ ctr ] )
		{
		string_ [ ctr ] = _transTableStripPL_ [ static_cast < char unsigned > ( string_ [ ctr ] ) ];
		ctr ++;
		}
	return;
	M_EPILOG
	}

double long atof_ex( HString const& string_, bool parse_ )
	{
	M_PROLOG
	double long value( 0 );
	HString str = string_;
	str.replace ( ",", "." ).replace ( " ", "" ).replace ( "\t", "" );
	if ( parse_ )
		{
		HExpression analyser;
		try
			{
			if ( analyser.compile( str ) )
				value = analyser.evaluate();
			else
				throw HExpressionException( HString( analyser.get_error() ) + " for: " + string_ + ", at: " + analyser.get_error_token() );
			}
		catch ( HExpressionException const& e )
			{
			throw HExpressionException( HString( e.what() ) + " - " + analyser.get_error() + " for: " + string_ + ", at: " + analyser.get_error_token() );
			}
		}
	else
		value = lexical_cast<double long>( str );
	return ( value );
	M_EPILOG
	}

int modulo_ASCII( HString const& aSCIINumber_, int modulo_ )
	{
	M_PROLOG
	int ctr = 0, number = 0, step = 0;
	int long tmpLength = 0;
	int long length = aSCIINumber_.get_length();
	HString tmpString, tmpNumber = aSCIINumber_;
	if ( length < 0 )
		M_THROW( "bad ASCII number length", length );
	if ( ! modulo_ )
		M_THROW( "zero denominatior", modulo_ );
	while ( tmpNumber.get_length() > STEP_LENGTH )
		{
		tmpString = tmpNumber.mid( step * STEP_LENGTH, STEP_LENGTH );
		tmpLength = tmpString.get_length();
		number = lexical_cast<int>( tmpString );
		number %= modulo_;
		tmpString.format ( "%d", number );
		tmpNumber.shift_left ( tmpLength - tmpString.get_length() );
		tmpLength = tmpString.get_length();
		for ( ctr = 0; ctr < tmpLength; ctr ++ )
			tmpNumber.set_at( ctr, tmpString[ ctr ] );
/*		M_LOG ( tmpNumber ); */
		}
	return ( lexical_cast<int>( tmpNumber ) % modulo_ );
	M_EPILOG
	}

bool verify_IBAN( HString const& iBAN_ )
	{
	M_PROLOG
	int ctr = 0;
	int long length = iBAN_.get_length();
	char pattern [ 2 ] = "\0";
	HString iBAN, tmpString;
	if ( length < MIN_IBAN_LENGTH )
		{
		_lastErrorMessage_.format ( "IBAN: Number too short (%d).", length );
		return ( true );
		}
	iBAN.hs_realloc( length );
	for ( ctr = 0; ctr < length; ctr ++ )
		if ( isalnum( iBAN_[ ctr ] ) )
			iBAN += iBAN_[ ctr ];
	length = iBAN.get_length();
	if ( length < MIN_IBAN_LENGTH )
		{
		_lastErrorMessage_.format ( "IBAN: Number too short (%d).", length );
		return ( true );
		}
	if ( ! ( isalpha ( iBAN [ 0 ] ) && isalpha ( iBAN [ 1 ] ) ) )
		{
		_lastErrorMessage_ = "IBAN: No country code present.";
		return ( true );
		}
	tmpString = iBAN.left ( 4 );
	iBAN.shift_left ( 4 );
	iBAN += tmpString;
/*	M_LOG ( iBAN ); */
	iBAN.lower();
	for ( ctr = 0; ctr < length; ctr ++ )
		{
		if ( isalpha ( iBAN [ ctr ] ) )
			{
			tmpString.format ( "%02d", ( iBAN [ ctr ] - 'a' ) + 10 );
			pattern [ 0 ] = iBAN [ ctr ];
			iBAN.replace ( pattern, tmpString );
			length = iBAN.get_length();
			}
		}
/*	M_LOG ( iBAN ); */
	ctr = modulo_ASCII( iBAN, 97 );
	if ( ctr == 1 )
		return ( false );
	_lastErrorMessage_.format ( "IBAN: bad checksum: %d", ctr );
	return ( true );
	M_EPILOG
	}

char const* get_last_error( void )
	{
	if ( ! _lastErrorMessage_.is_empty() )
		return ( _lastErrorMessage_.raw() );
	return ( "" );
	}

HString get_token( yaal::hcore::HString const& s, yaal::hcore::HString const& d, int long i )
	{
	M_PROLOG
	HTokenizer t( s, d );
	return ( t[ i ] );
	M_EPILOG
	}

bool is_byte( int value )
	{
	return ( value <= static_cast<int>( meta::max_unsigned<char unsigned>::value ) );
	}

void show_help( void* arg )
	{
	M_PROLOG
	OOptionInfo& info = *static_cast<OOptionInfo*>( arg );
	::printf(
"Usage: %s [OPTION]... [FILE]...\n"
"%s - %s\n\n"
"Mandatory arguments to long options are mandatory for short options too.\n"
"Options:\n",
			info._name, info._name, info._intro );
	size_t longestLongLength( 0 );
	size_t longestShortLength( 0 );
	HProgramOptionsHandler::options_t const& opts( info._opt.get_options() );
	for ( HProgramOptionsHandler::options_t::const_iterator it( opts.begin() ), end( opts.end() );
			it != end; ++ it )
		{
		HProgramOptionsHandler::OOption const& o = *it;
		/* + 2 for --, + 1 for =, 2 for [] */
		size_t tmp = ( o._name ? ::strlen( o._name ) + 2 : 0 ) + ( o._argument ? ::strlen( o._argument ) + 1 : 0 ) + ( o._switchType == HProgramOptionsHandler::OOption::TYPE::OPTIONAL ? 2 : 0 );
		if ( tmp > longestLongLength )
			longestLongLength = tmp;
		if ( is_byte( it->_shortForm ) )
			longestShortLength = 2;
		}
	HString desc;
	char const* description = NULL;
	char const* envColumns( ::getenv( "COLUMNS" ) );
	int const columns( envColumns ? xmath::clip( 80, lexical_cast<int>( envColumns ), 128 ): 80 );
	int cols( static_cast<int>( columns - ( longestLongLength + longestShortLength + 2 + 2 + 2 ) ) );
	/* display each option description */
	int const COUNT = static_cast<int>( opts.size() );
	for ( int i( 0 ); i < COUNT; ++ i )
		{
		HProgramOptionsHandler::OOption const& o = opts[ i ];
		if ( ! ( is_byte( o._shortForm ) || o._name ) )
			continue;
		HString sf;
		/* if short form char exist, build full form of short form */
		if ( is_byte( o._shortForm ) )
			{
			sf = "-";
			sf += static_cast<char>( o._shortForm );
			}
		char const* coma = is_byte( o._shortForm ) && o._name ? "," : " ";
		if ( ! description )
			description = o._description;
		/* if long form word exist, build full form of long form */
		HString lf;
		if ( o._name )
			{
			lf = "--";
			lf += o._name;
			}
		if ( o._argument )
			{
			if ( o._switchType == HProgramOptionsHandler::OOption::TYPE::OPTIONAL )
				lf += "[";
			( lf += "=" ) += o._argument;
			if ( o._switchType == HProgramOptionsHandler::OOption::TYPE::OPTIONAL )
				lf += "]";
			}
		if ( i > 0 ) /* subsequent options */
			{
			HProgramOptionsHandler::OOption const& p = opts[ i - 1 ];
			if ( o._name && p._name && ( ! ::strcmp( o._name, p._name ) ) )
				{
				lf = "", coma = " ";
				if ( description == o._description )
					description = "";
				}
			if ( is_byte( o._shortForm ) && is_byte( p._shortForm ) && (  o._shortForm == p._shortForm ) )
				{
				sf = "", coma = " ";
				if ( description == o._description )
					description = "";
				}
			}
		printf( "  %*s%s %-*s  ",
				static_cast<int>( longestShortLength ), sf.raw() ? sf.raw() : "", coma,
				static_cast<int>( longestLongLength ), lf.raw() ? lf.raw() : "" );
		/* + 2 for two prefixing spaces, + 2 for 2 spaces separating options from descriptions, + 2 for coma and space */
		desc = description;
		bool loop( true );
		do
			{
			int eol( 0 );
			int ws( 0 );
			while ( ( ws < cols ) && ( ws >= 0 ) )
				{
				eol = ws;
				ws = static_cast<int>( desc.find_one_of( _whiteSpace_, ws ) );
				if ( ( ws < 0 ) || ( ws > cols ) )
					break;
				eol = ws;
				ws = static_cast<int>( desc.find_other_than( _whiteSpace_, ws ) );
				}
			if ( ( ws >= cols ) || ( desc.get_length() > cols ) )
				{
				printf( "%.*s\n", eol, desc.raw() );
				desc.shift_left( eol );
				desc.trim_left();
				desc.insert( 0, 2, "  " );
				if ( i < ( COUNT - 1 ) )
					{
					HProgramOptionsHandler::OOption const& n = opts[ i + 1 ];
					if ( ( o._name && n._name && ( ! ::strcmp( o._name, n._name ) ) )
							|| ( is_byte( o._shortForm ) && is_byte( n._shortForm ) && ( o._shortForm == n._shortForm ) ) )
						{
						description = desc.raw();
						break;
						}
					}
				printf( "%*s", static_cast<int>( longestLongLength + longestShortLength + 2 + 2 + 2 ), "" );
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
	if ( info._note )
		::printf( "\n%s\n", info._note );
	return;
	M_EPILOG
	}

void dump_configuration( void* arg )
	{
	M_PROLOG
	OOptionInfo& info = *static_cast<OOptionInfo*>( arg );
	info._name && ::printf( "# this is configuration file for: `%s' program\n", info._name );
	info._intro && ::printf( "# %s\n", info._intro );
	if ( info._name || info._intro )
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
"# int    - [0-9]+\n"
"# string - [^ ].*\n"
"#\n"
"# example:\n"
"# log_path ${HOME}/var/log/program.log\n\n"
	);
	HString desc;
	char const* description = NULL;
	HProgramOptionsHandler::options_t const& opts = info._opt.get_options();
	int const COUNT = static_cast<int>( opts.size() );
	for ( int i = 0; i < COUNT; ++ i )
		{
		HProgramOptionsHandler::OOption const& o = opts[ i ];
		if ( ! o._name )
			continue;
		if ( i > 0 ) /* subsequent options */
			{
			HProgramOptionsHandler::OOption const& p = opts[ i - 1 ];
			if ( o._name && p._name
					&& ( ! ::strcmp( o._name, p._name ) )
					&& ( o._description == description ) )
				description = "";
			if ( is_byte( o._shortForm ) && is_byte( p._shortForm )
					&& ( o._shortForm == p._shortForm )
					&& ( o._description == description ) )
				description = "";
			}
		static int const MAXIMUM_LINE_LENGTH = 72;
		::printf( "# %s, type: ", o._name );
		if ( !! o._value )
			{
			switch ( o._value->get_type().value() )
				{
				case ( TYPE::BOOL ): ::printf( "boolean\n" ); break;
				case ( TYPE::INT ): case ( TYPE::INT_SHORT ): case ( TYPE::INT_LONG ): ::printf( "integer\n" ); break;
				case ( TYPE::FLOAT ): case ( TYPE::DOUBLE ): case ( TYPE::DOUBLE_LONG ): ::printf( "floating point\n" ); break;
				case ( TYPE::CHAR_PTR ): case ( TYPE::HSTRING ): ::printf( "character string\n" ); break;
				default: ::printf( "special\n" ); break;
				}
			}
		else
			::printf( "boolean\n" );
		if ( ! description )
			description = o._description;
		desc = description;
		bool loop = true;
		do
			{
			int eol = 0;
			while ( ( eol < MAXIMUM_LINE_LENGTH ) && ( eol >= 0 ) )
				{
				eol = static_cast<int>( desc.find_one_of( _whiteSpace_, eol ) );
				if ( ( eol < 0 ) || ( eol > MAXIMUM_LINE_LENGTH ) )
					break;
				eol = static_cast<int>( desc.find_other_than( _whiteSpace_, eol ) );
				}
			if ( eol >= MAXIMUM_LINE_LENGTH )
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
		if ( !! o._value )
			{
			switch ( o._value->get_type().value() )
				{
				case ( TYPE::BOOL ):
					::printf( "%s %s\n", o._name, o._value->get<bool>() ? "true" : "false" );
				break;
				case ( TYPE::HSTRING ):
					{
					HString const& s = o._value->get<HString>();
					if ( ! s.is_empty() )
						::printf( "%s \"%s\"\n", o._name, s.raw() );
					else
						::printf( "# %s\n", o._name );
					}
				break;
				case ( TYPE::CHAR_PTR ):
					{
					char const* ptr = o._value->get<char const*>();
					if ( ptr && ptr[0] )
						::printf( "%s %s\n", o._name, ptr );
					else
						::printf( "# %s\n", o._name );
					}
				break;
				case ( TYPE::INT ):
					::printf( "%s %d\n", o._name, o._value->get<int>() );
				break;
				case ( TYPE::INT_LONG ):
					::printf( "%s %ld\n", o._name, o._value->get<int long>() );
				break;
				case ( TYPE::DOUBLE_LONG ):
					::printf( "%s %Lf\n", o._name, o._value->get<double long>() );
				break;
				case ( TYPE::DOUBLE ):
					::printf( "%s %f\n", o._name, o._value->get<double>() );
				break;
				default:
					;
				}
			}
		::printf( "\n" );
		}
	info._note && ::printf( "# %s\n\n", info._note );
	::printf( "# vim: ft=conf\n" );
	return;
	M_EPILOG
	}

void failure( int exitStatus_, char const* const format_, ... )
	{
	M_PROLOG
	HString msg;
	va_list ap;
	va_start( ap, format_ );
	vfprintf( stderr, format_, ap );
	msg.vformat( format_, ap );
	log << "failure: " << msg;
	va_end( ap );
	throw ( exitStatus_ );
	M_EPILOG
	}

namespace sleep
{

bool sleep_real( timeval& time_, bool ignoreInterrrupt_ )
	{
	int err( 0 );
	while ( ( ( err = ::select( 0, NULL, NULL, NULL, &time_ ) ) == -1 ) && ( errno == EINTR ) && ignoreInterrrupt_ )
		;
	return ( ( err != 0 ) && ! ( ( errno == EINTR ) && ignoreInterrrupt_ ) );
	}

bool milisecond( int quantity_, bool ignoreInterrrupt_ )
	{
	timeval wait;
	wait.tv_sec = quantity_ / 1000;
	wait.tv_usec = ( quantity_ %  1000 ) * 1000;
	return ( sleep_real( wait, ignoreInterrrupt_ ) );
	}

bool second( int quantity_, bool ignoreInterrrupt_ )
	{
	timeval wait;
	wait.tv_sec = quantity_;
	wait.tv_usec = 0;
	return ( sleep_real( wait, ignoreInterrrupt_ ) );
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

int levenshtein_damerau( yaal::hcore::HString const& one_, yaal::hcore::HString const& two_, bool damerau_ )
	{
	M_PROLOG
	int cost = 0;
	int indexOne = 0, indexTwo = 0;
	int lengthOne = static_cast<int>( one_.length() );
	int lengthTwo = static_cast<int>( two_.length() );
	int** distanceMatrix = NULL;
	if ( ! lengthTwo )
		return ( lengthOne );
	if ( ! lengthOne )
		return ( lengthTwo );
	lengthOne ++;
	lengthTwo ++;
	HPointer<int*, HPointerArray> distanceMatrixHolder( new int*[ lengthOne ] );
	HPointer<int, HPointerArray> distanceMatrixBuffer( new int[ lengthOne * lengthTwo ] );
	distanceMatrix = distanceMatrixHolder.raw();
	for ( indexOne = 0; indexOne < lengthOne; ++ indexOne )
		distanceMatrix[ indexOne ] = distanceMatrixBuffer.raw() + indexOne * lengthTwo;
	for ( indexOne = 0; indexOne < lengthOne; ++ indexOne )
		distanceMatrix[ indexOne ][ 0 ] = indexOne;
	for ( indexTwo = 0; indexTwo < lengthTwo; ++ indexTwo )
		distanceMatrix[ 0 ][ indexTwo ] = indexTwo;
	lengthTwo --;
	lengthOne --;
	/* real magic starts here */
	for ( indexOne = 0; indexOne < lengthOne; ++ indexOne )
		{
		for ( indexTwo = 0; indexTwo < lengthTwo; ++ indexTwo )
			{
			cost = 0;
			if ( one_ [ indexOne ] != two_ [ indexTwo ] )
				cost = 1;
			distanceMatrix[ indexOne + 1 ][ indexTwo + 1 ] = min3(
					distanceMatrix[ indexOne ][ indexTwo + 1 ] + 1,
					distanceMatrix[ indexOne + 1 ][ indexTwo ] + 1,
					distanceMatrix[ indexOne ][ indexTwo ] + cost );
			if ( damerau_ && ( indexOne > 0 ) && ( indexTwo > 0 )
					&& ( one_[ indexOne - 1 ] == two_[ indexTwo ] )
					&& ( one_[ indexOne ] == two_[ indexTwo - 1 ] ) )
				distanceMatrix[ indexOne + 1 ][ indexTwo + 1 ] = min(
						distanceMatrix[ indexOne + 1 ][ indexTwo + 1 ],
						distanceMatrix[ indexOne - 1 ][ indexTwo - 1 ] + cost );
			}
		}
	return ( distanceMatrix[ lengthOne ][ lengthTwo ] );
	M_EPILOG
	}

}

namespace crypto
{

void crypt_3des( yaal::hcore::HStreamInterface::ptr_t src_, yaal::hcore::HStreamInterface::ptr_t dst_, HString const& key_ )
	{
	M_PROLOG
	crypt_3des( *src_, *dst_, key_ );
	return;
	M_EPILOG
	}

void crypt_3des( yaal::hcore::HStreamInterface& src_, yaal::hcore::HStreamInterface& dst_, HString const& key_ )
	{
	M_PROLOG
	static int const BUF_SIZE( 128 );
	HChunk buf( BUF_SIZE );
	HDes des( key_ );
	HRandomizer r;
	int long const toRead( buf.get_size() );
	M_ASSERT( toRead == BUF_SIZE );
	char gap( 0 );
	int long nRead( 0 );
	while ( ( nRead = src_.read( buf.raw(), toRead ) ) > 0 )
		{
		gap = static_cast<char>( nRead % 8 );
		if ( gap )
			{
			gap = static_cast<char>( 8 - gap );
			generate_n( buf.raw() + nRead, gap, randomizer_helper::make_randomizer( 255 ) );
			}
		int long toWrite( nRead + gap );
		M_ASSERT( toWrite <= toRead );
		des.crypt( buf.get<u8_t>(), toWrite, HDes::CRYPT );
		dst_.write( buf.raw(), toWrite );
		}
	dst_.write( &gap, 1 );
	return;
	M_EPILOG
	}

void decrypt_3des( yaal::hcore::HStreamInterface::ptr_t src_, yaal::hcore::HStreamInterface::ptr_t dst_, yaal::hcore::HString const& key_ )
	{
	M_PROLOG
	decrypt_3des( *src_, *dst_, key_ );
	return;
	M_EPILOG
	}

void decrypt_3des( yaal::hcore::HStreamInterface& src_, yaal::hcore::HStreamInterface& dst_, yaal::hcore::HString const& key_ )
	{
	M_PROLOG
	static int const BUF_SIZE( 128 );
	HChunk bufA( BUF_SIZE );
	HChunk bufB( BUF_SIZE );
	HDes des( key_ );
	int long const toRead( bufA.get_size() );
	M_ASSERT( toRead == BUF_SIZE );
	M_ASSERT( bufB.get_size() == BUF_SIZE );
	char gap( 0 );
	int long nRead( 0 );
	int long toWrite( 0 );
	char* readBuf( bufA.raw() );
	char* writeBuf( bufB.raw() );
	while ( ( ( nRead = src_.read( readBuf, toRead ) ) > 0 ) || ( toWrite > 0 ) )
		{
		/* There are 3 cases for non-malformed encrypted data:
		 * 1 - we read only full blocks
		 * 2 - we read only gap
		 * 3 - we read both data (some full blocks) and the gap
		 */
		if ( nRead > 0 )
			{
			gap = static_cast<char>( nRead % 8 );
			if ( gap )
				{
				M_ENSURE_EX( gap == 1, "malformed encrypted packet" );
				-- nRead;
				gap = readBuf[ nRead ];
				M_ENSURE_EX( ( gap >= 0 ) && ( gap < 8 ) && ( toWrite > 0 ? ( gap <= toWrite ) : ( gap <= nRead ) ), "malformed gap" );
				}
			toWrite -= ( nRead > 0 ? 0 /* full blocks from previous read */ : gap /* only gap has been read */ );
			if ( toWrite > 0 )
				dst_.write( writeBuf, toWrite );
			toWrite = nRead;
			M_ASSERT( ! ( toWrite % 8 ) );
			if ( toWrite > 0 )
				des.crypt( reinterpret_cast<u8_t*>( readBuf ), toWrite, HDes::DECRYPT );
			toWrite -= gap;
			swap( readBuf, writeBuf );
			}
		else if ( toWrite > 0 )
			{
			dst_.write( writeBuf, toWrite );
			toWrite = 0;
			}
		}
	return;
	M_EPILOG
	}

}

}

}

}

