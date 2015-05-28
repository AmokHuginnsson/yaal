/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hconsole.cxx - this file is integral part of `yaal' project.

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

#include <cstdlib>
#include <cstring>
#include <cstdio>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hconsole.hxx"
#include "hcore/hcore.hxx"
#include "hcore/hlog.hxx"
#include "hcore/hprogramoptionshandler.hxx"
#include "hcore/htokenizer.hxx"
#include "tools/tools.hxx"
#include "console.hxx"

using namespace yaal::hcore;
using namespace yaal::tools;

namespace yaal {

namespace hconsole {

int _latency_ = 1;
int _screenBackground_ = COLORS::BG_BLACK;
HWidget::OAttribute _attributeDisabled_ = {
	COLORS::FG_GREEN | COLORS::BG_BLACK | COLORS::BG_BLINK,
	COLORS::FG_LIGHTGRAY | COLORS::BG_BLACK | COLORS::BG_BLINK
};
HWidget::OAttribute _attributeEnabled_ = {
	COLORS::FG_BRIGHTGREEN | COLORS::BG_BLACK | COLORS::BG_BLINK,
	COLORS::FG_WHITE | COLORS::BG_BLACK | COLORS::BG_BLINK
};
HWidget::OAttribute _attributeFocused_ = {
	COLORS::FG_BRIGHTGREEN | COLORS::BG_BLACK | COLORS::BG_BLINK,
	COLORS::FG_BLACK | COLORS::BG_LIGHTGRAY
};
int _attributeStatusBar_ = ( COLORS::FG_WHITE | COLORS::BG_BLACK ) << 8
															| ( COLORS::FG_LIGHTGRAY | COLORS::BG_BLACK );
int _attributeSearchHighlight_ = ( COLORS::FG_BLACK | COLORS::BG_BROWN | COLORS::BG_BLINK ) << 8
																		| ( COLORS::FG_BLACK | COLORS::BG_BROWN );
USE_MOUSE::use_mouse_t _useMouse_ = USE_MOUSE::AUTO;
char _commandComposeCharacter_ = 'x';
int  _commandComposeDelay_ = 16;

inline void set_color_bits( int& word_, int bits_, int what_ ) {
	int unsigned mask = 0x000f;
	if ( what_ ) {
		what_ <<= 2;
		bits_ <<= what_;
		mask <<= what_;
	}
	word_ &= ~ mask;
	word_ |= bits_;
	return;
}

namespace {

int get_color_bits( HString& value_, int what_ ) {
	M_PROLOG
	HTokenizer t( value_, " \t", HTokenizer::DELIMITED_BY_ANY_OF );
	HString str = t[ what_ ];
	if ( str.is_empty() )
		return ( 0 );
	if ( ! strcasecmp( str, "BLACK" ) )
		return ( COLORS::FG_BLACK );
	else if ( ! strcasecmp( str, "RED" ) )
		return ( COLORS::FG_RED );
	else if ( ! strcasecmp( str, "GREEN" ) )
		return ( COLORS::FG_GREEN );
	else if ( ! strcasecmp( str, "BROWN" ) )
		return ( COLORS::FG_BROWN );
	else if ( ! strcasecmp( str, "BLUE" ) )
		return ( COLORS::FG_BLUE );
	else if ( ! strcasecmp( str, "MAGENTA" ) )
		return ( COLORS::FG_MAGENTA );
	else if ( ! strcasecmp( str, "CYAN" ) )
		return ( COLORS::FG_CYAN );
	else if ( ! strcasecmp( str, "LIGHTGRAY" ) )
		return ( COLORS::FG_LIGHTGRAY );
	else if ( ! strcasecmp( str, "GRAY" ) )
		return ( COLORS::FG_GRAY );
	else if ( ! strcasecmp( str, "BRIGHTRED" ) )
		return ( COLORS::FG_BRIGHTRED );
	else if ( ! strcasecmp( str, "BRIGHTGREEN" ) )
		return ( COLORS::FG_BRIGHTGREEN );
	else if ( ! strcasecmp( str, "YELLOW" ) )
		return ( COLORS::FG_YELLOW );
	else if ( ! strcasecmp( str, "BRIGHTBLUE" ) )
		return ( COLORS::FG_BRIGHTBLUE );
	else if ( ! strcasecmp( str, "BRIGHTMAGENTA" ) )
		return ( COLORS::FG_BRIGHTMAGENTA );
	else if ( ! strcasecmp( str, "BRIGHTCYAN" ) )
		return ( COLORS::FG_BRIGHTCYAN );
	else if ( ! strcasecmp( str, "WHITE" ) )
		return ( COLORS::FG_WHITE );
	return ( 0 );
	M_EPILOG
}

void set_color( HString& value_, HWidget::OAttribute& attribute_ ) {
	M_PROLOG
	int labelBg( get_color_bits( value_, 1 ) );
	int labelFg( get_color_bits( value_, 0 ) );
	int dataBg( get_color_bits( value_, 3 ) );
	int dataFg( get_color_bits( value_, 2 ) );
	attribute_._label = static_cast<u8_t>( COLORS::fg_to_bg( labelBg ) | labelFg );
	attribute_._data = static_cast<u8_t>( COLORS::fg_to_bg( dataBg ) | dataFg );
	return;
	M_EPILOG
}

void set_color( HString& value_, int& attribute_ ) {
	M_PROLOG
	int ctr = 0;
	int tab[ 4 ] = { 2, 3, 0, 1 };
	for ( ctr = 0; ctr < 4; ctr ++ )
		set_color_bits( attribute_,
				get_color_bits( value_, ctr ), tab [ ctr ] );
	return;
	M_EPILOG
}

bool set_hconsole_variables( HString& option_, HString& value_ ) {
	M_PROLOG
	bool fail( false );
	if ( ! strcasecmp( option_, "set_env" ) ) {
		decode_set_env( value_ );
	} else if ( ! strcasecmp( option_, "use_mouse" ) ) {
		/* enable mouse support */
		if ( ! strcasecmp( value_, "auto" ) ) {
			_useMouse_ = USE_MOUSE::AUTO;
		} else {
			bool useMouse( lexical_cast<bool>( value_ ) );
			if ( useMouse ) {
				_useMouse_ = USE_MOUSE::YES;
			} else {
				_useMouse_ = USE_MOUSE::NO;
			}
		}
	} else if ( ! strcasecmp( option_, "screen_background" ) ) {
		_screenBackground_ = COLORS::fg_to_bg( get_color_bits( value_, 0 ) );
	} else if ( ! strcasecmp( option_, "attribute_disabled" ) ) {
		set_color( value_, _attributeDisabled_ );
	} else if ( ! strcasecmp( option_, "attribute_enabled" ) ) {
		set_color( value_, _attributeEnabled_ );
	} else if ( ! strcasecmp( option_, "attribute_focused" ) ) {
		set_color( value_, _attributeFocused_ );
	} else if ( ! strcasecmp( option_, "attribute_statusbar" ) ) {
		set_color( value_, _attributeStatusBar_ );
	} else if ( ! strcasecmp( option_, "attribute_search_highlight" ) ) {
		set_color( value_, _attributeSearchHighlight_ );
	} else {
		fail = true;
	}
	return ( ! fail );
	M_EPILOG
}

}

class HConsoleInitDeinit {
public:
	HConsoleInitDeinit( void );
} initDeinit;


HConsoleInitDeinit::HConsoleInitDeinit( void ) {
	M_PROLOG
	errno = 0;
	int escdelay( 0 );
	yaal_options()(
		"esc_delay",
		HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED,
		"ncurses escape sequence time span",
		program_options_helper::option_value( escdelay ), /* defined inside ncurses lib */
		"seconds"
	)(
		"latency",
		HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED,
		"how often invoke idle event",
		program_options_helper::option_value( _latency_ ),
		"seconds"
	)(
		"command_compose_character",
		HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED,
		"character that shall be uses as command composition base",
		program_options_helper::option_value( _commandComposeCharacter_ ),
		"character"
	)(
		"command_compose_delay",
		HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED,
		"command composition time span",
		program_options_helper::option_value( _commandComposeDelay_ ),
		"seconds"
	);
	yaal_options().process_rc_file( "yaal", "console", set_hconsole_variables );
	HConsole::set_escdelay( escdelay );
	return;
	M_EPILOG
}

void banner( void ) {
	::printf( "\thconsole\n" );
	return;
}

}

}

#ifdef __DYNAMIC_LINKER__
extern "C"
int yaal_hconsole_main( int, char** ) __attribute__((noreturn));
extern "C"
int yaal_hconsole_main( int, char** ) {
	static char const dynamicLinkerPath[]
#ifdef HAVE_INTERP_ALLOWED
		__attribute__(( __section__(".interp") ))
#endif /* #ifdef HAVE_INTERP_ALLOWED */
		= __DYNAMIC_LINKER__;
	if ( dynamicLinkerPath[ 0 ] ) {
		yaal::hcore::banner();
		yaal::tools::banner();
		yaal::hconsole::banner();
		::printf( "\n" );
	}
	::exit( 0 );
}
#else /* #ifdef __DYNAMIC_LINKER__ */
extern "C"
int yaal_hconsole_main( int, char** );
extern "C"
int yaal_hconsole_main( int, char** ) {
	return ( 0 );
}
#endif /* #else #ifdef __DYNAMIC_LINKER__ */

