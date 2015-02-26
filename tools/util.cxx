/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	util.cxx - this file is integral part of `yaal' project.

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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cstdarg>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "util.hxx"
#include "hcore/memory.hxx"
#include "hcore/hpointer.hxx"
#include "hcore/htokenizer.hxx"
#include "hexpression.hxx"
#include "hcore/hlog.hxx"
#include "xmath.hxx"
#include "hcore/hrandomizer.hxx"

using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace util {

namespace {
	static int const MIN_IBAN_LENGTH = 20;
	static int const STEP_LENGTH = 4;
}

char _transTableStripPL_[ 256 ];

char _jednNastki_[][ 16 ] = {
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

char _dzies_[][ 24 ] = {
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

char _setki_[][ 16 ] = {
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

char _temat_[][ 12 ] = {
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

char _koncowka_[][ 3 ][ 6 ] = {
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

char _integer_[][ 12 ] = {
	"thousand",
	"million",
	"billion",
	"trillion",
	"quadrillion",
	"quintillion",
	"sextillion"
};

char _numbers_[][ 10 ] = {
	"zero",
	"one",
	"two",
	"three",
	"four",
	"five",
	"six",
	"seven",
	"eight",
	"nine",
	"ten",
	"eleven",
	"twelve",
	"thirteen",
	"fourteen",
	"fiveteen",
	"sixteen",
	"seventeen",
	"nineteen"
};

char const _tenths_[][ 10 ] = {
	"zero-ten",
	"ten",
	"twety",
	"thirty",
	"fourty",
	"fivety",
	"sixty",
	"seventy",
	"eighty",
	"ninety"
};

char const _houndred_[] = "houndred";

HString _lastErrorMessage_;

HString money_string( HNumber const& amount_ ) {
	M_PROLOG
	HNumber n( amount_ );
	n.round( 2 );
	HString string( n.to_string() );
	int long pos( string.reverse_find( '.' ) );
	if ( pos == HString::npos )
		string += ".00";
	else if ( pos == 1 )
		string += "0";
	return ( string );
	M_EPILOG
}

HString in_words_pl( HNumber const& kwota_ ) {
	M_PROLOG
	int forma( 0 );
	HString slownie;
	HString przypadek;
	HString string( money_string( kwota_ ) );
	int long length( string.get_length() );
	for ( int i( 0 ); i < length; i ++ ) {
		if ( ( i % 3 ) == 0 ) {
			int sub( ( length - i ) > 1 ? 2 : 1 );
			forma = lexical_cast<int>( string.mid( ( length - i ) - sub, sub ) );
			if ( ( i > 5 ) && ( ( length - i ) > 2 ) &&
					! ::strncmp( string.raw() + ( length - i ) - 3, "000", 3 ) )
				continue;
		}
		if ( i == 2 )
			continue;
		char cyfra( static_cast<char>( string[ ( length - i ) - 1 ] - '0' ) );
		switch ( i % 3 ) {
			case ( 0 ) : {
				przypadek = _temat_[ i / 3 ];
				if ( forma == 1 )
					przypadek += _koncowka_[ i / 3 ] [ 0 ];
				else if ( ( ( ( forma % 10 ) > 1 ) && ( ( forma % 10 ) < 5 ) )
						&& ( ( forma < 10 ) || ( forma > 20 ) ) )
					przypadek += _koncowka_[ i / 3 ] [ 1 ];
				else
					przypadek += _koncowka_[ i / 3 ] [ 2 ];
				slownie = przypadek + slownie;
				if ( ( forma < 20 ) &&  ( forma > 9 ) )
					slownie = _jednNastki_[ forma ] + slownie;
				else if ( cyfra )
					slownie = _jednNastki_[ static_cast<int>( cyfra ) ] + slownie;
				else if ( ! forma && ( ( i < 3 ) || ( kwota_ < 1 ) ) )
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

void usun_ogonki( HString& string_ ) {
	M_PROLOG
	for ( HString::HIterator it( string_.begin() ), end( string_.end() ); it != end; ++ it )
		*it = _transTableStripPL_[ static_cast<char unsigned>( *it ) ];
	return;
	M_EPILOG
}

double long atof_ex( HString const& string_, bool parse_ ) {
	M_PROLOG
	double long value( 0 );
	HString str = string_;
	str.replace ( ",", "." ).replace ( " ", "" ).replace ( "\t", "" );
	if ( parse_ ) {
		HExpression analyser;
		try {
			if ( analyser.compile( str ) ) {
				value = analyser.evaluate();
			} else {
				throw HExpressionException( HString( analyser.get_error() ) + " for: " + string_ + ", at: " + analyser.get_error_token() );
			}
		} catch ( HExpressionException const& e ) {
			throw HExpressionException( HString( e.what() ) + " - " + analyser.get_error() + " for: " + string_ + ", at: " + analyser.get_error_token() );
		}
	} else
		value = lexical_cast<double long>( str );
	return ( value );
	M_EPILOG
}

namespace {

int modulo_ASCII( HString const& aSCIINumber_, int modulo_ ) {
	M_PROLOG
	int ctr = 0, number = 0, step = 0;
	int long tmpLength = 0;
	int long length = aSCIINumber_.get_length();
	HString tmpString, tmpNumber = aSCIINumber_;
	if ( length < 0 )
		M_THROW( "bad ASCII number length", length );
	if ( ! modulo_ )
		M_THROW( "zero denominatior", modulo_ );
	while ( tmpNumber.get_length() > STEP_LENGTH ) {
		tmpString = tmpNumber.mid( step * STEP_LENGTH, STEP_LENGTH );
		tmpLength = tmpString.get_length();
		number = lexical_cast<int>( tmpString );
		number %= modulo_;
		tmpString.format ( "%d", number );
		tmpNumber.shift_left( tmpLength - tmpString.get_length() );
		tmpLength = tmpString.get_length();
		for ( ctr = 0; ctr < tmpLength; ctr ++ )
			tmpNumber.set_at( ctr, tmpString[ ctr ] );
/*		M_LOG ( tmpNumber ); */
	}
	return ( lexical_cast<int>( tmpNumber ) % modulo_ );
	M_EPILOG
}

}

bool verify_IBAN( HString const& IBAN_ ) {
	M_PROLOG
	int ctr = 0;
	int long length = IBAN_.get_length();
	char pattern[ 2 ] = "\0";
	HString IBAN, tmpString;
	bool valid( false );
	do {
		if ( length < MIN_IBAN_LENGTH ) {
			_lastErrorMessage_.format( "IBAN: Number too short (%d).", static_cast<int>( length ) );
			break;
		}
		IBAN.reserve( length );
		for ( ctr = 0; ctr < length; ctr ++ )
			if ( isalnum( IBAN_[ ctr ] ) )
				IBAN += IBAN_[ ctr ];
		length = IBAN.get_length();
		if ( length < MIN_IBAN_LENGTH ) {
			_lastErrorMessage_.format( "IBAN: Number too short (%d).", static_cast<int>( length ) );
			break;
		}
		if ( ! ( isalpha( IBAN[ 0 ] ) && isalpha( IBAN[ 1 ] ) ) ) {
			_lastErrorMessage_ = "IBAN: No country code present.";
			break;
		}
		tmpString = IBAN.left( 4 );
		IBAN.shift_left ( 4 );
		IBAN += tmpString;
	/*	M_LOG ( IBAN ); */
		IBAN.lower();
		for ( ctr = 0; ctr < length; ctr ++ ) {
			if ( isalpha ( IBAN[ ctr ] ) ) {
				tmpString.format( "%02d", ( IBAN[ ctr ] - 'a' ) + 10 );
				pattern[ 0 ] = IBAN[ ctr ];
				IBAN.replace( pattern, tmpString );
				length = IBAN.get_length();
			}
		}
	/*	M_LOG ( IBAN ); */
		ctr = modulo_ASCII( IBAN, 97 );
		if ( ctr != 1 ) {
			_lastErrorMessage_.format( "IBAN: bad checksum: %d", ctr );
			break;
		}
		valid = true;
	} while ( false );
	return ( valid );
	M_EPILOG
}

char const* get_last_error( void ) {
	if ( ! _lastErrorMessage_.is_empty() )
		return ( _lastErrorMessage_.raw() );
	return ( "" );
}

HString get_token( yaal::hcore::HString const& s, yaal::hcore::HString const& d, int long i ) {
	M_PROLOG
	HTokenizer t( s, d );
	return ( t[ i ] );
	M_EPILOG
}

inline bool is_byte( int value ) {
	return ( value <= static_cast<int>( meta::max_unsigned<char unsigned>::value ) );
}

void show_help( void* arg ) {
	M_PROLOG
	errno = 0;
	OOptionInfo& info = *static_cast<OOptionInfo*>( arg );
	::printf(
"Usage: %s %s\n"
"%s - %s\n\n"
"Mandatory arguments to long options are mandatory for short options too.\n"
"Options:\n",
			info._name, info._syntax ? info._syntax : "[OPTION]... [FILE]...", info._name, info._intro );
	int longestLongLength( 0 );
	int longestShortLength( 0 );
	HProgramOptionsHandler::options_t const& opts( info._opt.get_options() );
	for ( HProgramOptionsHandler::options_t::const_iterator it( opts.begin() ), end( opts.end() );
			it != end; ++ it ) {
		HProgramOptionsHandler::OOption const& o = *it;
		/* + 2 for --, + 1 for =, 2 for [] */
		int tmp( static_cast<int>( ( o._name ? ::strlen( o._name ) + 2 : 0 )
					+ ( o._argument ? ::strlen( o._argument ) + 1 : 0 )
					+ ( o._switchType == HProgramOptionsHandler::OOption::TYPE::OPTIONAL ? 2 : 0 ) ) );
		if ( tmp > longestLongLength )
			longestLongLength = tmp;
		if ( is_byte( it->_shortForm ) )
			longestShortLength = 2;
	}
	HString desc;
	char const* description( NULL );
	char const* envColumns( ::getenv( "COLUMNS" ) );
	int const columns( envColumns ? xmath::clip( 80, lexical_cast<int>( envColumns ), 128 ): 80 );
	int cols( columns - ( longestLongLength + longestShortLength + 2 + 2 + 2 ) );
	/* display each option description */
	int const COUNT = static_cast<int>( opts.size() );
	for ( int i( 0 ); i < COUNT; ++ i ) {
		HProgramOptionsHandler::OOption const& o = opts[ i ];
		if ( ! ( is_byte( o._shortForm ) || o._name ) )
			continue;
		HString sf;
		/* if short form char exist, build full form of short form */
		if ( is_byte( o._shortForm ) ) {
			sf = "-";
			sf += static_cast<char>( o._shortForm );
		}
		char const* comma( is_byte( o._shortForm ) && o._name ? "," : " " );
		if ( ! description )
			description = o._description;
		/* if long form word exist, build full form of long form */
		HString lf;
		if ( o._name ) {
			lf = "--";
			lf += o._name;
		}
		if ( o._argument ) {
			if ( o._switchType == HProgramOptionsHandler::OOption::TYPE::OPTIONAL )
				lf += "[";
			( lf += "=" ) += o._argument;
			if ( o._switchType == HProgramOptionsHandler::OOption::TYPE::OPTIONAL )
				lf += "]";
		}
		if ( i > 0 ) /* subsequent options */ {
			HProgramOptionsHandler::OOption const& p = opts[ i - 1 ];
			if ( o._name && p._name && ( ! ::strcmp( o._name, p._name ) ) ) {
				lf = "", comma = " ";
				if ( description == o._description )
					description = "";
			}
			if ( is_byte( o._shortForm ) && is_byte( p._shortForm ) && (  o._shortForm == p._shortForm ) ) {
				sf = "", comma = " ";
				if ( description == o._description )
					description = "";
			}
		}
		printf( "  %*s%s %-*s  ",
				static_cast<int>( longestShortLength ), sf.raw() ? sf.raw() : "", comma,
				static_cast<int>( longestLongLength ), lf.raw() ? lf.raw() : "" );
		/* + 2 for two prefixing spaces, + 2 for 2 spaces separating options from descriptions, + 2 for comma and space */
		desc = description;
		bool loop( true );
		do {
			int eol( 0 );
			int ws( 0 );
			int words( 0 );
			while ( ( ws < cols ) && ( ws >= 0 ) ) {
				eol = ws;
				ws = static_cast<int>( desc.find_one_of( _whiteSpace_.data(), ws ) );
				if ( ( ws < 0 ) && ( words < 2 ) ) {
					eol = static_cast<int>( desc.get_length() );
				}
				if ( ( ws < 0 ) || ( ws > cols ) )
					break;
				eol = ws;
				ws = static_cast<int>( desc.find_other_than( _whiteSpace_.data(), ws ) );
				if ( ws > 0 )
					++ words;
			}
			if ( ( ws >= cols ) || ( desc.get_length() > cols ) ) {
				printf( "%.*s\n", eol, desc.raw() );
				desc.shift_left( eol );
				desc.trim_left();
				desc.insert( 0, 2, "  " );
				if ( i < ( COUNT - 1 ) ) {
					HProgramOptionsHandler::OOption const& n = opts[ i + 1 ];
					if ( ( o._name && n._name && ( ! ::strcmp( o._name, n._name ) ) )
							|| ( is_byte( o._shortForm ) && is_byte( n._shortForm ) && ( o._shortForm == n._shortForm ) ) ) {
						description = desc.raw();
						break;
					}
				}
				printf( "%*s", static_cast<int>( longestLongLength + longestShortLength + 2 + 2 + 2 ), "" );
			} else {
				printf( "%s\n", desc.raw() );
				description = NULL;
				loop = false;
			}
		} while ( loop );
	}
	if ( info._note )
		::printf( "\n%s\n", info._note );
	return;
	M_EPILOG
}

void dump_configuration( void* arg ) {
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
	for ( int i = 0; i < COUNT; ++ i ) {
		HProgramOptionsHandler::OOption const& o = opts[ i ];
		if ( ! o._name )
			continue;
		if ( i > 0 ) /* subsequent options */ {
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
		if ( !! o._value ) {
			switch ( o._value->get_type().value() ) {
				case ( TYPE::BOOL ): ::printf( "boolean\n" ); break;
				case ( TYPE::INT ): case ( TYPE::INT_SHORT ): case ( TYPE::INT_LONG ): ::printf( "integer\n" ); break;
				case ( TYPE::FLOAT ): case ( TYPE::DOUBLE ): case ( TYPE::DOUBLE_LONG ): ::printf( "floating point\n" ); break;
				case ( TYPE::HSTRING ): ::printf( "character string\n" ); break;
				default: ::printf( "special\n" ); break;
			}
		} else
			::printf( "boolean\n" );
		if ( ! description )
			description = o._description;
		desc = description;
		bool loop = true;
		do {
			int eol = 0;
			while ( ( eol < MAXIMUM_LINE_LENGTH ) && ( eol >= 0 ) ) {
				eol = static_cast<int>( desc.find_one_of( _whiteSpace_.data(), eol ) );
				if ( ( eol < 0 ) || ( eol > MAXIMUM_LINE_LENGTH ) )
					break;
				eol = static_cast<int>( desc.find_other_than( _whiteSpace_.data(), eol ) );
			}
			if ( eol >= MAXIMUM_LINE_LENGTH ) {
				printf( "# %.*s\n", eol, desc.raw() );
				desc.shift_left( eol );
				desc.trim_left();
				description = desc.raw();
			} else {
				printf( "# %s\n", desc.raw() );
				description = NULL;
				loop = false;
			}
		} while ( loop );
		if ( !! o._value ) {
			switch ( o._value->get_type().value() ) {
				case ( TYPE::BOOL ):
					::printf( "%s %s\n", o._name, o._value->get<bool>() ? "true" : "false" );
				break;
				case ( TYPE::HSTRING ): {
					HString const& s = o._value->get<HString>();
					if ( ! s.is_empty() )
						::printf( "%s \"%s\"\n", o._name, s.raw() );
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
				break;
			}
		}
		::printf( "\n" );
	}
	info._note && ::printf( "# %s\n\n", info._note );
	::printf( "# vim: ft=conf\n" );
	return;
	M_EPILOG
}

void failure( int exitStatus_, char const* const format_, ... ) {
	M_PROLOG
	HString msg;
	va_list ap;
	va_start( ap, format_ );
	vfprintf( stderr, format_, ap );
	msg.vformat( format_, &ap );
	log << "failure: " << msg;
	va_end( ap );
	throw ( exitStatus_ );
	M_EPILOG
}

}

}

}

