/* Read yaal/LICENSE.md file for copyright and licensing information. */

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
#include "hcore/hstack.hxx"
#include "hcore/hregex.hxx"
#include "hterminal.hxx"
#include "ansi.hxx"

using namespace yaal::hcore;
using namespace yaal::ansi;

namespace yaal {

namespace hcore {

namespace hidden {

double long std_strtold( HString const&, int* );

}

}

namespace tools {

namespace util {

extern code_point_translation_t _transTableStripPL_;

HString& usun_ogonki( HString& string_ ) {
	M_PROLOG
	for ( HString::HIterator it( string_.begin() ), end( string_.end() ); it != end; ++ it ) {
		code_point_t cp( *it );
		code_point_translation_t::const_iterator t( _transTableStripPL_.find( cp ) );
		if ( t != _transTableStripPL_.end() ) {
			*it = t->second;
		}
	}
	return ( string_ );
	M_EPILOG
}

yaal::hcore::HString article( yaal::hcore::HString const& word_ ) {
	int long pos( word_.find_one_of( character_class<CHARACTER_CLASS::LETTER>().data() ) );
	return ( pos != HString::npos ? ( character_class<CHARACTER_CLASS::VOWEL>().has( word_[pos] ) ? "an" : "a" ) : "" );
}

double long atof_ex( HString const& string_, bool parse_ ) {
	M_PROLOG
	double long value( 0 );
	HString str = string_;
	str.replace( ",", "." ).replace( " ", "" ).replace( "\t", "" );
	if ( parse_ ) {
		HExpression analyzer;
		try {
			if ( analyzer.compile( str ) ) {
				value = analyzer.evaluate();
			} else {
				throw HExpressionException( HString( analyzer.get_error() ) + " for: " + string_ + ", at: " + analyzer.get_error_token() );
			}
		} catch ( HExpressionException const& e ) {
			throw HExpressionException( HString( e.what() ) + " - " + analyzer.get_error() + " for: " + string_ + ", at: " + analyzer.get_error_token() );
		}
	} else {
		value = hidden::std_strtold( str, nullptr );
	}
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

HOptionInfo& HOptionInfo::name( char const* name_ ) {
	_name = name_;
	return ( *this );
}

char const* HOptionInfo::name( void ) const {
	return ( _name );
}

HOptionInfo& HOptionInfo::intro( char const* intro_ ) {
	_intro = intro_;
	return ( *this );
}

char const* HOptionInfo::intro( void ) const {
	return ( _intro );
}

HOptionInfo& HOptionInfo::syntax( char const* syntax_ ) {
	_syntax = syntax_;
	return ( *this );
}

char const* HOptionInfo::syntax( void ) const {
	return ( _syntax );
}

HOptionInfo& HOptionInfo::description( char const* description_ ) {
	_description = description_;
	return ( *this );
}

char const* HOptionInfo::description( void ) const {
	return ( _description );
}

HOptionInfo& HOptionInfo::note( char const* note_ ) {
	_note = note_;
	return ( *this );
}

char const* HOptionInfo::note( void ) const {
	return ( _note );
}

HOptionInfo& HOptionInfo::theme( HTheme const& theme_ ) {
	_theme = theme_;
	return ( *this );
}

HTheme const& HOptionInfo::theme( void ) const {
	return ( _theme );
}

HOptionInfo& HOptionInfo::color( bool color_ ) {
	_color = color_;
	return ( *this );
}

bool HOptionInfo::color( void ) const {
	return ( _color );
}

HOptionInfo& HOptionInfo::markdown( bool markdown_ ) {
	_markdown = markdown_;
	return ( *this );
}

bool HOptionInfo::markdown( void ) const {
	return ( _markdown );
}

yaal::hcore::HProgramOptionsHandler const& HOptionInfo::opt( void ) const {
	return ( _opt );
}

class HHighlighter {
public:
	typedef HStack<HString> colors_t;
private:
	HTheme _theme;
	colors_t _colors;
	bool _strong;
	bool _emphasis;
	bool _code;
	bool _special;
public:
	HHighlighter( HTheme const& theme_ = HTheme() )
		: _theme( theme_ )
		, _colors()
		, _strong( false )
		, _emphasis( false )
		, _code( false )
		, _special( false ) {
		M_PROLOG
		_colors.push( *reset );
		return;
		M_EPILOG
	}
	yaal::hcore::HString highlight( yaal::hcore::HString const& str_ ) {
		M_PROLOG
		HString s;
		HString c;
		auto apply = [this, &c]( bool& state_, COLOR::color_t color_ ) {
			if ( state_ ) {
				c.assign( *COLOR::to_ansi( COLOR::ATTR_RESET ) );
				_colors.pop();
			} else {
				c.clear();
				_colors.push( *COLOR::to_ansi( color_ ) );
			}
			c.append( _colors.top() );
			state_ = !state_;
		};
		for ( HString::const_iterator it( str_.begin() ), end( str_.end() ); it != end; ++ it ) {
			if ( *it == '\\' ) {
				++ it;
				if ( ! ( it != end ) ) {
					break;
				}
				s.append( *it );
				continue;
			}
			c = *it;
			if ( *it == '`' ) {
				apply( _code, _theme.code() );
			} else if ( *it == '$' ) {
				apply( _special, _theme.special() );
			} else if ( ( *it == '*' ) || ( *it == '_' ) ) {
				code_point_t prefix( *it );
				++ it;
				if ( ( it != end ) && ( *it == prefix ) ) {
					apply( _strong, _theme.strong() );
				} else {
					-- it;
					apply( _emphasis, _theme.emphasis() );
				}
			}
			s.append( c );
		}
		return ( s );
		M_EPILOG
	}
};

namespace {
yaal::hcore::HString hl( HHighlighter& highlighter_, yaal::hcore::HString const& str_, bool color_, bool markdown_ ) {
	M_PROLOG
	HString s;
	if ( color_ ) {
		s = highlighter_.highlight( str_ );
	} else if ( markdown_ ) {
		s = str_;
	} else {
		HRegex re1( "(?<!\\\\)[*_$]" );
		HRegex re2( "\\\\([`*{_$])" );
		s = re1.replace( str_, "" );
		s = re2.replace( s, "$1" );
	}
	return ( s );
	M_EPILOG
}
yaal::hcore::HString hl( yaal::hcore::HString const& str_, HTheme const& theme_, bool color_, bool markdown_ ) {
	M_PROLOG
	HHighlighter highlighter( theme_ );
	return ( hl( highlighter, str_, color_, markdown_ ) );
	M_EPILOG
}
yaal::hcore::HString plain( yaal::hcore::HString const& str_ ) {
	M_PROLOG
	HHighlighter highlighter;
	return ( hl( highlighter, str_, false, false ) );
	M_EPILOG
}
yaal::hcore::HString escape_markdown( yaal::hcore::HString const& string_ ) {
	M_PROLOG
	HString s( string_ );
	s.replace( "$", "\\$" );
	s.replace( "*", "\\*" );
	s.replace( "_", "\\_" );
	s.replace( "`", "\\`" );
	s.replace( "{", "\\{" );
	return ( s );
	M_EPILOG
}
char const _nonBreakingSpace_[] = "\xc2\xa0";
void insert_non_breaking_spaces( yaal::hcore::HString& str_ ) {
	M_PROLOG
	static char const articles[][5] = { "A", " a", "An", " an", "The", " the", "To", " to" };
	for ( char const* a : articles ) {
		str_.replace( to_string( a ).append( ' ' ), to_string( a ).append( _nonBreakingSpace_ ) );
	}
	return;
	M_EPILOG
}
void insert_line_breaks( yaal::hcore::HString& str_, int atColumn_, char const* prefix_ = nullptr ) {
	M_PROLOG
	int long pos( 0 );
	insert_non_breaking_spaces( str_ );
	while ( pos < ( str_.get_length() - atColumn_ ) ) {
		int long lineBreakPos( str_.find( '\n'_ycp, pos ) );
		if ( ( lineBreakPos != HString::npos ) && ( lineBreakPos - pos < atColumn_ ) ) {
			pos = lineBreakPos + 1;
			continue;
		}
		int long spacePos( str_.find_last_one_of( character_class<CHARACTER_CLASS::WHITESPACE>().data(), pos + atColumn_ ) );
		if ( spacePos != HString::npos ) {
			str_.set_at( spacePos, '\n'_ycp );
			pos = spacePos + 1;
		} else {
			spacePos = str_.find_one_of( character_class<CHARACTER_CLASS::WHITESPACE>().data(), pos + atColumn_ );
			if ( spacePos != HString::npos ) {
				str_.set_at( spacePos, '\n'_ycp );
				pos = spacePos + 1;
			} else {
				break;
			}
		}
	}
	str_.replace( _nonBreakingSpace_, " " );
	if ( prefix_ ) {
		str_.replace( "\n", to_string( "\n" ).append( prefix_ ) );
	}
	return;
	M_EPILOG
}
yaal::hcore::HString alias_desc( HProgramOptionsHandler::HOption const& opt_ ) {
	M_PROLOG
	HString aliasDesc( "an alias for " );
	bool hasShortFrom( is_byte( opt_.short_form() ) && ( opt_.short_form() > 0 ) );
	if ( hasShortFrom ) {
		aliasDesc.append( "**-" ).append( static_cast<char>( opt_.short_form() ) ).append( "**" );
	}
	if ( ! opt_.long_form().is_empty() ) {
		if ( hasShortFrom ) {
			aliasDesc.append( ", " );
		}
		aliasDesc.append( "**--" ).append( opt_.long_form() ).append( "**" );
	}
	return ( aliasDesc );
	M_EPILOG
}
}

void show_help( HOptionInfo const& info, HStreamInterface& out_ ) {
	M_PROLOG
	errno = 0;
	HString name;
	bool color( is_a_tty( out_ ) && info.color() );
	int columns( 0 );
	if ( color && HTerminal::get_instance().exists() ) {
		HTerminal::HSize s( HTerminal::get_instance().size() );
		columns = s.columns();
	}
	columns = xmath::clip( 80, columns, 128 );
	if ( color ) {
		name.append( *COLOR::to_ansi( info.theme().strong() ) );
	}
	name.append( info.name() );
	if ( color ) {
		name.append( *ansi::reset );
	}
	HString syntax( info.syntax() ? info.syntax() : "" );
	if ( syntax.is_empty() ) {
		syntax.assign( "[*OPTION*]... [*FILE*]..." );
	}
	HString desc;
	if ( info.description() ) {
		desc.assign( info.description() );
		insert_line_breaks( desc, columns );
	}
	out_ << "Usage: " << name << " " << hl( syntax, info.theme(), color, info.markdown() ) << "\n"
		<< ( info.markdown() ? "\n" : "" )
		<< name << " - " << hl( info.intro(), info.theme(), color, info.markdown() ) << "\n\n"
		<< ( info.description() ? hl( desc, info.theme(), color, info.markdown() ) : "" ) << ( info.description() ? "\n\n" : "" )
		<< "Mandatory arguments to long options are mandatory for short options too." << ( info.markdown() ? "  \n" : "\n" )
		<< "Options:\n" << ( info.markdown() ? "\n" : "" );
	int longestLongLength( 0 );
	int longestShortLength( 0 );
	HProgramOptionsHandler::options_t const& opts( info.opt().get_options() );
	for ( HProgramOptionsHandler::options_t::const_iterator it( opts.begin() ), end( opts.end() );
			it != end; ++ it ) {
		HProgramOptionsHandler::HOption const& o = *it;
		/* + 2 for --, + 1 for =, 2 for [] */
		int tmp(
			static_cast<int>( ( ! o.long_form().is_empty() ? o.long_form().get_length() + 2 : 0 )
				+ ( ! o.argument_name().is_empty() ? o.argument_name().get_length() + 1 : 0 )
				+ ( o.switch_type() == HProgramOptionsHandler::HOption::ARGUMENT::OPTIONAL ? 2 : 0 ) )
		);
		if ( tmp > longestLongLength ) {
			longestLongLength = tmp;
		}
		if ( is_byte( it->short_form() ) ) {
			longestShortLength = 2;
		}
	}
	static int const INDENT( info.markdown() ? 4 : 2 );
	/* + INDENT for two prefixing spaces, + 2 for 2 spaces separating options from descriptions, + 2 for comma and space */
	int cols( columns - ( longestLongLength + longestShortLength + INDENT + 2 + 2 ) );
	/* display each option description */
	int const COUNT( static_cast<int>( opts.size() ) );
	HString description;
	for ( int i( 0 ); i < COUNT; ++ i ) {
		HProgramOptionsHandler::HOption const& o = opts[ i ];
		if ( ! ( is_byte( o.short_form() ) || ! o.long_form().is_empty() ) ) {
			continue;
		}
		HString sf;
		int extraSFL( 0 );
		/* if short form char exist, build full form of short form */
		if ( is_byte( o.short_form() ) ) {
			if ( color ) {
				sf.append( *COLOR::to_ansi( info.theme().strong() ) );
			}
			sf.append( "-" );
			sf.append( static_cast<char>( o.short_form() ) );
			if ( color ) {
				sf.append( *ansi::reset );
				extraSFL = static_cast<int>( strlen( *COLOR::to_ansi( info.theme().strong() ) ) + strlen( *ansi::reset ) );
			}
		}
		char const* comma( is_byte( o.short_form() ) && ! o.long_form().is_empty() ? "," : " " );
		if ( ! description ) {
			description = o.description();
			insert_non_breaking_spaces( description );
		}
		/* if long form word exist, build full form of long form */
		HString lf;
		int extraLFL( 0 );
		if ( ! o.long_form().is_empty() ) {
			if ( color ) {
				lf.append( *COLOR::to_ansi( info.theme().strong() ) );
			}
			lf.append( "--" );
			lf.append( o.long_form() );
			if ( color ) {
				lf.append( *ansi::reset );
				extraLFL = static_cast<int>( strlen( *COLOR::to_ansi( info.theme().strong() ) ) + strlen( *ansi::reset ) );
			}
		}
		if ( !o.argument_name().is_empty() ) {
			if ( o.switch_type() == HProgramOptionsHandler::HOption::ARGUMENT::OPTIONAL ) {
				lf.append( "[" );
			}
			if ( ! o.long_form().is_empty() ) {
				lf.append( "=" );
			}
			if ( color ) {
				lf.append( *COLOR::to_ansi( info.theme().emphasis() ) );
			}
			lf.append( o.argument_name() );
			if ( color ) {
				lf.append( *ansi::reset );
				extraLFL += static_cast<int>( strlen( *COLOR::to_ansi( info.theme().emphasis() ) ) + strlen( *ansi::reset ) );
			}
			if ( o.switch_type() == HProgramOptionsHandler::HOption::ARGUMENT::OPTIONAL ) {
				lf.append( "]" );
			}
		}
		if ( i > 0 ) { /* subsequent options */
			HProgramOptionsHandler::HOption const& p = opts[ i - 1 ];
			if ( ! o.long_form().is_empty() && ! p.long_form().is_empty() && ( o.long_form() == p.long_form() ) ) {
				lf.clear();
				extraLFL = 0;
				comma = " ";
				if ( description == o.description() ) {
					description = alias_desc( p );
				}
			}
			if ( is_byte( o.short_form() ) && is_byte( p.short_form() ) && (  o.short_form() == p.short_form() ) ) {
				sf.clear();
				extraSFL = 0;
				comma = " ";
				if ( description == o.description() ) {
					description = alias_desc( p );
				}
			}
		}
		bool specialSpace( ! ( o.long_form().is_empty() && ! o.argument_name().is_empty() ) );
		out_ << ( info.markdown() ? "    " : "  " )
			<< setw( static_cast<int>( longestShortLength + extraSFL ) ) << sf << comma << ( specialSpace ? " " : "" )
			<< setw( static_cast<int>( longestLongLength + extraLFL ) ) << hcore::left << lf << hcore::right << "  "
			<< ( specialSpace ? "" : " " );
		desc = description;
		if ( ! o.default_value().is_empty() ) {
			desc.append( " (default: *" ).append( escape_markdown( o.default_value() ) ).append( "*)" );
		}
		HHighlighter highlighter( info.theme() );
		bool loop( true );
		do {
			int eol( 0 );
			int ws( 0 );
			int words( 0 );
			while ( ( ws < cols ) && ( ws >= 0 ) ) {
				eol = ws;
				ws = static_cast<int>( desc.find_one_of( character_class<CHARACTER_CLASS::WHITESPACE>().data(), ws ) );
				if ( ( ws < 0 ) && ( words < 2 ) ) {
					eol = static_cast<int>( desc.get_length() );
				}
				if ( ( ws < 0 ) || ( ws > cols ) ) {
					if ( words < 2 ) {
						eol = ws;
					}
					break;
				}
				eol = ws;
				ws = static_cast<int>( desc.find_other_than( character_class<CHARACTER_CLASS::WHITESPACE>().data(), ws ) );
				if ( ws > 0 ) {
					++ words;
				}
			}
			if ( ( ws >= cols ) || ( desc.get_length() > cols ) ) {
				out_ << hl( highlighter, desc.left( eol ).trim_right().replace( _nonBreakingSpace_, " " ), color, info.markdown() ) << "\n";
				desc.shift_left( eol );
				desc.trim_left();
				desc.insert( 0, "    ", info.markdown() ? 4 : 2 );
				if ( i < ( COUNT - 1 ) ) {
					HProgramOptionsHandler::HOption const& n = opts[ i + 1 ];
					if ( ( ! o.long_form().is_empty() && ! n.long_form().is_empty() && ( o.long_form() == n.long_form() ) )
							|| ( is_byte( o.short_form() ) && is_byte( n.short_form() ) && ( o.short_form() == n.short_form() ) ) ) {
						description = desc;
						break;
					}
				}
				out_ << setw( static_cast<int>( longestLongLength + longestShortLength + 2 + 2 + 2 ) ) << "";
			} else {
				out_ << hl( highlighter, desc.replace( _nonBreakingSpace_, " " ), color, info.markdown() ) << "\n";
				description.clear();
				loop = false;
			}
		} while ( loop );
	}
	out_ << "\n"
		<< hl(
			"All long form options can be used in program configuration file: *"_ys
				.append( info.name() ).append( "rc*." ),
			info.theme(), color, info.markdown()
		) << endl;
	if ( info.note() ) {
		out_ << "\n" << hl( info.note(), info.theme(), color, info.markdown() ) << endl;
	}
	return;
	M_EPILOG
}

void dump_configuration( HOptionInfo const& info, HStreamInterface& out_ ) {
	M_PROLOG
	static int const MAXIMUM_LINE_LENGTH = 72;
	if ( info.name() ) {
		out_ << "# this is configuration file for: `" << info.name() << "' program\n";
	}
	if ( info.intro() ) {
		out_ << "# " << plain( info.intro() ) << "\n";
	}
	if ( info.name() || info.intro() ) {
		out_ << "\n";
	}
	HString desc;
	if ( info.description() ) {
		desc.assign( plain( info.description() ) );
		insert_line_breaks( desc, MAXIMUM_LINE_LENGTH, "# " );
		out_ << "# " << desc << "\n\n";
	}
	out_ <<
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
	HString description;
	HProgramOptionsHandler::options_t const& opts = info.opt().get_options();
	int const COUNT = static_cast<int>( opts.size() );
	for ( int i = 0; i < COUNT; ++ i ) {
		HProgramOptionsHandler::HOption const& o = opts[ i ];
		if ( o.long_form().is_empty() ) {
			continue;
		}
		if ( i > 0 ) { /* subsequent options */
			HProgramOptionsHandler::HOption const& p = opts[ i - 1 ];
			if ( !o.long_form().is_empty() && !p.long_form().is_empty()
					&& ( o.long_form() == p.long_form() )
					&& ( o.description() == description ) ) {
				description = "";
			}
			if ( is_byte( o.short_form() ) && is_byte( p.short_form() )
					&& ( o.short_form() == p.short_form() )
					&& ( o.description() == description ) ) {
				description = "";
			}
		}
		out_ << "### " << o.long_form() << " ###\n# type: ";
		switch ( o.recipient_type() ) {
			case ( TYPE::BOOL ): {
				out_ << "boolean";
			} break;
			case ( TYPE::INT ):
			case ( TYPE::INT_SHORT ):
			case ( TYPE::INT_LONG ): {
				out_ << "integer";
			} break;
			case ( TYPE::FLOAT ):
			case ( TYPE::DOUBLE ):
			case ( TYPE::DOUBLE_LONG ): {
				out_ << "floating point";
			} break;
			case ( TYPE::HSTRING ): {
				out_ << "character string";
			} break;
			default: {
				out_ << "special";
			} break;
		}
		if ( ! o.default_value().is_empty() ) {
			out_ << ", default: " << o.default_value();
		}
		out_ << "\n";
		if ( description.is_empty() ) {
			description = o.description();
		}
		desc = description;
		bool loop = true;
		do {
			int eol = 0;
			while ( ( eol < MAXIMUM_LINE_LENGTH ) && ( eol >= 0 ) ) {
				eol = static_cast<int>( desc.find_one_of( character_class<CHARACTER_CLASS::WHITESPACE>().data(), eol ) );
				if ( ( eol < 0 ) || ( eol > MAXIMUM_LINE_LENGTH ) ) {
					break;
				}
				eol = static_cast<int>( desc.find_other_than( character_class<CHARACTER_CLASS::WHITESPACE>().data(), eol ) );
			}
			if ( eol >= MAXIMUM_LINE_LENGTH ) {
				out_ << "# " << plain( desc.left( eol ) ) << "\n";
				desc.shift_left( eol );
				desc.trim_left();
				description = desc;
			} else {
				out_ << "# " << plain( desc ) << "\n";
				description.clear();
				loop = false;
			}
		} while ( loop );
		switch ( o.recipient_type() ) {
			case ( TYPE::BOOL ):
			case ( TYPE::INT ):
			case ( TYPE::INT_LONG ):
			case ( TYPE::DOUBLE_LONG ):
			case ( TYPE::DOUBLE ):
				out_ << o.long_form() << " " << o.get() << "\n";
			break;
			case ( TYPE::HSTRING ): {
				HString s = o.get();
				if ( ! s.is_empty() ) {
					out_ << o.long_form() << " \"" << s << "\"\n";
				} else {
					out_ << "# " << o.long_form() << "\n";
				}
			}
			break;
			default:
				out_ << "# " << o.long_form() << " " << o.get() << "\n";
			break;
		}
		out_ << "\n";
	}
	if ( info.note() ) {
		out_ << "# " << plain( info.note() ) << "\n\n";
	}
	out_ << "# vim: set ft=conf:" << endl;
	return;
	M_EPILOG
}

void failure( int exitStatus_, char const* format_, ... ) {
	M_PROLOG
	static int const MAX_FAILURE_MESSAGE_LENGTH( 4096 );
	bool color( is_a_tty( stderr ) );
	char msg[MAX_FAILURE_MESSAGE_LENGTH];
	va_list ap;
	va_start( ap, format_ );
	vsnprintf( msg, MAX_FAILURE_MESSAGE_LENGTH, format_, ap );
	va_end( ap );
	HHighlighter highlighter;
	HString formated( color ? highlighter.highlight( msg ) : msg );
	HUTF8String utf8( formated );
	fwrite( utf8.c_str(), 1, static_cast<size_t>( utf8.byte_count() ), stderr );
	log << "failure: " << plain( msg );
	throw ( exitStatus_ );
	M_EPILOG
}

yaal::hcore::HString highlight( yaal::hcore::HString const& str_, HTheme const& theme_ ) {
	M_PROLOG
	HHighlighter highlighter( theme_ );
	return ( highlighter.highlight( str_ ) );
	M_EPILOG
}

char const* near_keys( char key_ ) {
	char const* near( "" );
	switch ( key_ ) {
		case ( '`' ):  near = "1\t";     break;
		case ( '1' ):  near = "`\tq2";   break;
		case ( '2' ):  near = "1qw3";    break;
		case ( '3' ):  near = "2we4";    break;
		case ( '4' ):  near = "3er5";    break;
		case ( '5' ):  near = "4rt6";    break;
		case ( '6' ):  near = "5ty7";    break;
		case ( '7' ):  near = "6yu8";    break;
		case ( '8' ):  near = "7ui9";    break;
		case ( '9' ):  near = "8io0";    break;
		case ( '0' ):  near = "9op-";    break;
		case ( '-' ):  near = "0p[=";    break;
		case ( '=' ):  near = "-[]";     break;
		case ( '\t' ): near = "`1q";     break;
		case ( 'q' ):  near = "\t12wa";  break;
		case ( 'w' ):  near = "q23esa";  break;
		case ( 'e' ):  near = "w34rds";  break;
		case ( 'r' ):  near = "e45tfd";  break;
		case ( 't' ):  near = "r56ygf";  break;
		case ( 'y' ):  near = "t67uhg";  break;
		case ( 'u' ):  near = "y78ijh";  break;
		case ( 'i' ):  near = "u89okj";  break;
		case ( 'o' ):  near = "i90plk";  break;
		case ( 'p' ):  near = "o0-[;l";  break;
		case ( '[' ):  near = "p-=]';";  break;
		case ( ']' ):  near = "[=\\'";   break;
		case ( '\\' ): near = "]";       break;
		case ( 'a' ):  near = "qwsz";    break;
		case ( 's' ):  near = "awedxz";  break;
		case ( 'd' ):  near = "serfcx";  break;
		case ( 'f' ):  near = "drtgvc";  break;
		case ( 'g' ):  near = "ftyhbv";  break;
		case ( 'h' ):  near = "gyujnb";  break;
		case ( 'j' ):  near = "huikmn";  break;
		case ( 'k' ):  near = "jiol,m";  break;
		case ( 'l' ):  near = "kop;.,";  break;
		case ( ';' ):  near = "lp['/.";  break;
		case ( '\'' ): near = ";[]/";    break;
		case ( 'z' ):  near = "asx";     break;
		case ( 'x' ):  near = "zsdc";    break;
		case ( 'c' ):  near = "xdfv";    break;
		case ( 'v' ):  near = "cfgb";    break;
		case ( 'b' ):  near = "vghn";    break;
		case ( 'n' ):  near = "bhjm";    break;
		case ( 'm' ):  near = "njk,";    break;
		case ( ',' ):  near = "mkl.";    break;
		case ( '.' ):  near = ",l;/";    break;
		case ( '/' ):  near = ".;'";     break;
		/* shift */
		case ( '~' ):  near = "!";       break;
		case ( '!' ):  near = "~Q@";     break;
		case ( '@' ):  near = "!QW#";    break;
		case ( '#' ):  near = "@WE$";    break;
		case ( '$' ):  near = "#ER%";    break;
		case ( '%' ):  near = "$RT^";    break;
		case ( '^' ):  near = "%TY&";    break;
		case ( '&' ):  near = "^YU*";    break;
		case ( '*' ):  near = "&UI(";    break;
		case ( '(' ):  near = "*IO)";    break;
		case ( ')' ):  near = "(OP_";    break;
		case ( '_' ):  near = ")P{+";    break;
		case ( '+' ):  near = "_{}";     break;
		case ( 'Q' ):  near = "!@WA";    break;
		case ( 'W' ):  near = "Q@#ESA";  break;
		case ( 'E' ):  near = "W#$RDS";  break;
		case ( 'R' ):  near = "E$%TFD";  break;
		case ( 'T' ):  near = "R%^YGF";  break;
		case ( 'Y' ):  near = "T^&UHG";  break;
		case ( 'U' ):  near = "Y&*IJH";  break;
		case ( 'I' ):  near = "U*(OKJ";  break;
		case ( 'O' ):  near = "I()PLK";  break;
		case ( 'P' ):  near = "O)_{:L";  break;
		case ( '{' ):  near = "P_+}\":"; break;
		case ( '}' ):  near = "{+|\"";   break;
		case ( '|' ):  near = "}";       break;
		case ( 'A' ):  near = "QWSZ";    break;
		case ( 'S' ):  near = "AWEDXZ";  break;
		case ( 'D' ):  near = "SERFCX";  break;
		case ( 'F' ):  near = "DRTGVC";  break;
		case ( 'G' ):  near = "FTYHBV";  break;
		case ( 'H' ):  near = "GYUJNB";  break;
		case ( 'J' ):  near = "HUIKMN";  break;
		case ( 'K' ):  near = "JIOL<M";  break;
		case ( 'L' ):  near = "KOP:><";  break;
		case ( ':' ):  near = "LP{\"?>"; break;
		case ( '"' ):  near = ":{}?";    break;
		case ( 'Z' ):  near = "ASX";     break;
		case ( 'X' ):  near = "ZSDC";    break;
		case ( 'C' ):  near = "XDFV";    break;
		case ( 'V' ):  near = "CFGB";    break;
		case ( 'B' ):  near = "VGHN";    break;
		case ( 'N' ):  near = "BHJM";    break;
		case ( 'M' ):  near = "NJK<";    break;
		case ( '<' ):  near = "MKL>";    break;
		case ( '>' ):  near = "<L:?";    break;
		case ( '?' ):  near = ">:\"";    break;
	}
	return ( near );
}

}

}

}

