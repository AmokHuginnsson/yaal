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
#include "ansi.hxx"
#include "hcore/hprogramoptionshandler.hxx"
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

yaal::hcore::HString cardinal( int no_ ) {
	M_PROLOG
	static char const CARDINALS[][10] = {
		"zero"
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
		"fifteen",
		"sixteen",
		"seventeen",
		"eighteen",
		"nineteen"
	};
	HString c;
	if ( no_ < 0 ) {
		c.append( "minus " );
		no_ = -no_;
	}
	if ( no_ < 20 ) {
		c.append( CARDINALS[no_] );
	} else {
		c.append( no_ );
	}
	return ( c );
	M_EPILOG
}

yaal::hcore::HString ordinal( int no_ ) {
	M_PROLOG
	static char const ORDINALS[][10] = {
		"first",
		"second",
		"third",
		"fourth",
		"fifth",
		"sixth",
		"seventh",
		"eighth",
		"ninth",
		"tenth",
		"eleventh",
		"twelfth"
	};
	HString o;
	if ( no_ < 1 ) {
		throw HRuntimeException( "No ordinal for non-positive number: "_ys.append( no_ ).append( "." ) );
	} else if ( no_ < 13 ) {
		o = ORDINALS[no_ - 1];
	} else {
		static char const SUFFIX[][4] = {
			"th" /* 0 */,
			"st" /* 1 */,
			"nd" /* 2 */,
			"rd" /* 3 */,
			"th" /* 4 */,
			"th" /* 5 */,
			"th" /* 6 */,
			"th" /* 7 */,
			"th" /* 8 */,
			"th" /* 9 */
		};
		int digit( no_ % 10 );
		o = cardinal( no_ );
		o.append( SUFFIX[digit] );
	}
	return ( o );
	M_EPILOG
}

inline bool is_byte( int value ) {
	return ( value <= static_cast<int>( meta::max_unsigned<char unsigned>::value ) );
}

void show_help( OOptionInfo const& info ) {
	M_PROLOG
	errno = 0;
	HString name;
	if ( is_a_tty( stdout ) ) {
		name.append( *ansi::bold );
	}
	name.append( info._name );
	if ( is_a_tty( stdout ) ) {
		name.append( *ansi::reset );
	}
	HString syntax( info._syntax ? info._syntax : "" );
	if ( syntax.is_empty() ) {
		syntax.append( "[" );
		if ( is_a_tty( stdout ) ) {
			syntax.append( *ansi::underline );
		}
		syntax.append( "OPTION" );
		if ( is_a_tty( stdout ) ) {
			syntax.append( *ansi::reset );
		}
		syntax.append( "]... [" );
		if ( is_a_tty( stdout ) ) {
			syntax.append( *ansi::underline );
		}
		syntax.append( "FILE" );
		if ( is_a_tty( stdout ) ) {
			syntax.append( *ansi::reset );
		}
		syntax.append( "]..." );
	}
	cout << "Usage: " << name << " " << syntax << "\n"
		<< name << " - " << info._intro << "\n\n"
		"Mandatory arguments to long options are mandatory for short options too.\n"
		"Options:\n";
	int longestLongLength( 0 );
	int longestShortLength( 0 );
	HProgramOptionsHandler::options_t const& opts( info._opt.get_options() );
	for ( HProgramOptionsHandler::options_t::const_iterator it( opts.begin() ), end( opts.end() );
			it != end; ++ it ) {
		HProgramOptionsHandler::HOption const& o = *it;
		/* + 2 for --, + 1 for =, 2 for [] */
		int tmp( static_cast<int>( ( ! o.long_form().is_empty() ? o.long_form().get_length() + 2 : 0 )
					+ ( ! o.argument_name().is_empty() ? o.argument_name().get_length() + 1 : 0 )
					+ ( o.switch_type() == HProgramOptionsHandler::HOption::ARGUMENT::OPTIONAL ? 2 : 0 ) ) );
		if ( tmp > longestLongLength )
			longestLongLength = tmp;
		if ( is_byte( it->short_form() ) )
			longestShortLength = 2;
	}
	HString desc;
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
	int const COUNT( static_cast<int>( opts.size() ) );
	char const* description( nullptr );
	for ( int i( 0 ); i < COUNT; ++ i ) {
		HProgramOptionsHandler::HOption const& o = opts[ i ];
		if ( ! ( is_byte( o.short_form() ) || ! o.long_form().is_empty() ) ) {
			continue;
		}
		HString sf;
		int extraSFL( 0 );
		/* if short form char exist, build full form of short form */
		if ( is_byte( o.short_form() ) ) {
			if ( is_a_tty( stdout ) ) {
				sf.append( *ansi::bold );
			}
			sf.append( "-" );
			sf.append( static_cast<char>( o.short_form() ) );
			if ( is_a_tty( stdout ) ) {
				sf.append( *ansi::reset );
				extraSFL = static_cast<int>( strlen( *ansi::bold ) + strlen( *ansi::reset ) );
			}
		}
		char const* comma( is_byte( o.short_form() ) && ! o.long_form().is_empty() ? "," : " " );
		if ( ! description ) {
			description = o.description().raw();
		}
		/* if long form word exist, build full form of long form */
		HString lf;
		int extraLFL( 0 );
		if ( ! o.long_form().is_empty() ) {
			if ( is_a_tty( stdout ) ) {
				lf.append( *ansi::bold );
			}
			lf.append( "--" );
			lf.append( o.long_form() );
			if ( is_a_tty( stdout ) ) {
				lf.append( *ansi::reset );
				extraLFL = static_cast<int>( strlen( *ansi::bold ) + strlen( *ansi::reset ) );
			}
		}
		if ( !o.argument_name().is_empty() ) {
			if ( o.switch_type() == HProgramOptionsHandler::HOption::ARGUMENT::OPTIONAL ) {
				lf.append( "[" );
			}
			lf.append( "=" );
			if ( is_a_tty( stdout ) ) {
				lf.append( *ansi::underline );
			}
			lf.append( o.argument_name() );
			if ( is_a_tty( stdout ) ) {
				lf.append( *ansi::reset );
				extraLFL += static_cast<int>( strlen( *ansi::underline ) + strlen( *ansi::reset ) );
			}
			if ( o.switch_type() == HProgramOptionsHandler::HOption::ARGUMENT::OPTIONAL ) {
				lf.append( "]" );
			}
		}
		if ( i > 0 ) /* subsequent options */ {
			HProgramOptionsHandler::HOption const& p = opts[ i - 1 ];
			if ( ! o.long_form().is_empty() && ! p.long_form().is_empty() && ( o.long_form() == p.long_form() ) ) {
				lf.clear();
				extraLFL = 0;
				comma = " ";
				if ( description == o.description().raw() ) {
					description = "";
				}
			}
			if ( is_byte( o.short_form() ) && is_byte( p.short_form() ) && (  o.short_form() == p.short_form() ) ) {
				sf.clear();
				extraSFL = 0;
				comma = " ";
				if ( description == o.description().raw() ) {
					description = "";
				}
			}
		}
		cout << "  " << setw( static_cast<int>( longestShortLength + extraSFL ) ) << sf << comma << " "
			<< setw( static_cast<int>( longestLongLength + extraLFL ) ) << left << lf << right << "  ";
		/* + 2 for two prefixing spaces, + 2 for 2 spaces separating options from descriptions, + 2 for comma and space */
		desc = description;
		if ( ! o.default_value().is_empty() ) {
			desc.append( " (default: " ).append( o.default_value() ).append( ")" );
		}
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
				cout << desc.left( eol ) << "\n";
				desc.shift_left( eol );
				desc.trim_left();
				desc.insert( 0, 2, "  " );
				if ( i < ( COUNT - 1 ) ) {
					HProgramOptionsHandler::HOption const& n = opts[ i + 1 ];
					if ( ( ! o.long_form().is_empty() && ! n.long_form().is_empty() && ( o.long_form() == n.long_form() ) )
							|| ( is_byte( o.short_form() ) && is_byte( n.short_form() ) && ( o.short_form() == n.short_form() ) ) ) {
						description = desc.raw();
						break;
					}
				}
				cout << setw( static_cast<int>( longestLongLength + longestShortLength + 2 + 2 + 2 ) ) << "";
			} else {
				cout << desc << "\n";
				description = nullptr;
				loop = false;
			}
		} while ( loop );
	}
	if ( info._note ) {
		cout << "\n" << info._note << endl;
	}
	return;
	M_EPILOG
}

void dump_configuration( OOptionInfo const& info ) {
	M_PROLOG
	if ( info._name ) {
		cout << "# this is configuration file for: `" << info._name << "' program\n";
	}
	if ( info._intro ) {
		cout << "# " << info._intro << "\n";
	}
	if ( info._name || info._intro ) {
		cout << "\n";
	}
	cout <<
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
"# log_path ${HOME}/var/log/program.log\n\n";
	HString desc;
	char const* description = nullptr;
	HProgramOptionsHandler::options_t const& opts = info._opt.get_options();
	int const COUNT = static_cast<int>( opts.size() );
	for ( int i = 0; i < COUNT; ++ i ) {
		HProgramOptionsHandler::HOption const& o = opts[ i ];
		if ( o.long_form().is_empty() ) {
			continue;
		}
		if ( i > 0 ) /* subsequent options */ {
			HProgramOptionsHandler::HOption const& p = opts[ i - 1 ];
			if ( !o.long_form().is_empty() && !p.long_form().is_empty()
					&& ( o.long_form() == p.long_form() )
					&& ( o.description().raw() == description ) )
				description = "";
			if ( is_byte( o.short_form() ) && is_byte( p.short_form() )
					&& ( o.short_form() == p.short_form() )
					&& ( o.description().raw() == description ) )
				description = "";
		}
		static int const MAXIMUM_LINE_LENGTH = 72;
		cout << "### " << o.long_form() << " ###\n# type: ";
		switch ( o.recipient_type() ) {
			case ( TYPE::BOOL ): {
				cout << "boolean";
			} break;
			case ( TYPE::INT ):
			case ( TYPE::INT_SHORT ):
			case ( TYPE::INT_LONG ): {
				cout << "integer";
			} break;
			case ( TYPE::FLOAT ):
			case ( TYPE::DOUBLE ):
			case ( TYPE::DOUBLE_LONG ): {
				cout << "floating point";
			} break;
			case ( TYPE::HSTRING ): {
				cout << "character string";
			} break;
			default: {
				cout << "special";
			} break;
		}
		if ( ! o.default_value().is_empty() ) {
			cout << ", default: " << o.default_value();
		}
		cout << "\n";
		if ( ! description ) {
			description = o.description().raw();
		}
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
				cout << "# " << desc.left( eol ) << "\n";
				desc.shift_left( eol );
				desc.trim_left();
				description = desc.raw();
			} else {
				cout << "# " << desc << "\n";
				description = nullptr;
				loop = false;
			}
		} while ( loop );
		switch ( o.recipient_type() ) {
			case ( TYPE::BOOL ):
			case ( TYPE::INT ):
			case ( TYPE::INT_LONG ):
			case ( TYPE::DOUBLE_LONG ):
			case ( TYPE::DOUBLE ):
				cout << o.long_form() << " " << o.get() << "\n";
			break;
			case ( TYPE::HSTRING ): {
				HString s = o.get();
				if ( ! s.is_empty() ) {
					cout << o.long_form() << " \"" << s << "\"\n";
				} else {
					cout << "# " << o.long_form() << "\n";
				}
			}
			break;
			default:
				cout << "# " << o.long_form() << " " << o.get() << "\n";
			break;
		}
		cout << "\n";
	}
	if ( info._note ) {
		cout << "# " << info._note << "\n\n";
	}
	cout << "# vim: ft=conf" << endl;
	return;
	M_EPILOG
}

void failure( int exitStatus_, char const* format_, ... ) {
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

