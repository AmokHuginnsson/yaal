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
#include "hterminal.hxx"

using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace util {

char _transTableStripPL_[ 256 ];

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
	int columns( 0 );
	if ( _terminal_.exists() ) {
		HTerminal::coord_t c( _terminal_.size() );
		columns = c.second;
	}
	if ( columns <= 0 ) {
		char const* envColumns( ::getenv( "COLUMNS" ) );
		if ( envColumns ) {
			columns = lexical_cast<int>( envColumns );
		}
	}
	columns = xmath::clip( 80, columns, 128 );
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
		if ( ! description ) {
			description = o._description;
		}
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
			switch ( o._value->get_type() ) {
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
			switch ( o._value->get_type() ) {
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

