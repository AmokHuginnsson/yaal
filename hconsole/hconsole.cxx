/* Read yaal/LICENSE.md file for copyright and licensing information. */

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
COLOR::color_t _screenBackground_ = COLOR::ATTR_DEFAULT;
HWidget::OAttribute _attributeDisabled_ = {
	static_cast<COLOR::color_t>( COLOR::FG_GREEN | COLOR::BG_BLACK | COLOR::BG_BLINK ),
	static_cast<COLOR::color_t>( COLOR::FG_LIGHTGRAY | COLOR::BG_BLACK | COLOR::BG_BLINK )
};
HWidget::OAttribute _attributeEnabled_ = {
	static_cast<COLOR::color_t>( COLOR::FG_BRIGHTGREEN | COLOR::BG_BLACK | COLOR::BG_BLINK ),
	static_cast<COLOR::color_t>( COLOR::FG_WHITE | COLOR::BG_BLACK | COLOR::BG_BLINK )
};
HWidget::OAttribute _attributeFocused_ = {
	static_cast<COLOR::color_t>( COLOR::FG_BRIGHTGREEN | COLOR::BG_BLACK | COLOR::BG_BLINK ),
	static_cast<COLOR::color_t>( COLOR::FG_BLACK | COLOR::BG_LIGHTGRAY )
};
HWidget::OAttribute _attributeStatusBar_ = {
	static_cast<COLOR::color_t>( COLOR::FG_WHITE | COLOR::BG_BLACK ),
	static_cast<COLOR::color_t>( COLOR::FG_LIGHTGRAY | COLOR::BG_BLACK )
};
HWidget::OAttribute _attributeSearchHighlight_ = {
	static_cast<COLOR::color_t>( COLOR::FG_BLACK | COLOR::BG_BROWN | COLOR::BG_BLINK ),
	static_cast<COLOR::color_t>( COLOR::FG_BLACK | COLOR::BG_BROWN )
};
USE_MOUSE _useMouse_ = USE_MOUSE::AUTO;
char _commandComposeCharacter_ = 'x';
int  _commandComposeDelay_ = 16;

inline void set_color_bits( int& word_, int bits_, int what_ ) {
	int unsigned mask = 0x000f;
	if ( what_ ) {
		what_ <<= 2;
		bits_ <<= what_;
		mask <<= what_;
	}
	word_ = static_cast<int>( static_cast<int unsigned>( word_ ) & ~ mask );
	word_ |= bits_;
	return;
}

namespace {

COLOR::color_t get_color_bits( HString& value_, int what_ ) {
	M_PROLOG
	HTokenizer t( value_, " \t", HTokenizer::DELIMITED_BY_ANY_OF );
	HString str = t[ what_ ];
	return ( ! str.is_empty() ? COLOR::from_string( str ) : COLOR::ATTR_DEFAULT );
	M_EPILOG
}

void set_color( HString& value_, HWidget::OAttribute& attribute_ ) {
	M_PROLOG
	COLOR::color_t labelBg( get_color_bits( value_, 1 ) );
	COLOR::color_t labelFg( get_color_bits( value_, 0 ) );
	COLOR::color_t dataBg( get_color_bits( value_, 3 ) );
	COLOR::color_t dataFg( get_color_bits( value_, 2 ) );
	attribute_._label = static_cast<COLOR::color_t>( COLOR::fg_to_bg( labelBg ) | labelFg );
	attribute_._data = static_cast<COLOR::color_t>( COLOR::fg_to_bg( dataBg ) | dataFg );
	return;
	M_EPILOG
}

bool set_hconsole_variables( HString& option_, HString& value_ ) {
	M_PROLOG
	bool fail( false );
	if ( ! stricasecmp( option_, "set_env" ) ) {
		decode_set_env( value_ );
	} else if ( ! stricasecmp( option_, "use_mouse" ) ) {
		/* enable mouse support */
		if ( ! stricasecmp( value_, "auto" ) ) {
			_useMouse_ = USE_MOUSE::AUTO;
		} else {
			bool useMouse( lexical_cast<bool>( value_ ) );
			if ( useMouse ) {
				_useMouse_ = USE_MOUSE::YES;
			} else {
				_useMouse_ = USE_MOUSE::NO;
			}
		}
	} else if ( ! stricasecmp( option_, "screen_background" ) ) {
		_screenBackground_ = COLOR::fg_to_bg( get_color_bits( value_, 0 ) );
	} else if ( ! stricasecmp( option_, "attribute_disabled" ) ) {
		set_color( value_, _attributeDisabled_ );
	} else if ( ! stricasecmp( option_, "attribute_enabled" ) ) {
		set_color( value_, _attributeEnabled_ );
	} else if ( ! stricasecmp( option_, "attribute_focused" ) ) {
		set_color( value_, _attributeFocused_ );
	} else if ( ! stricasecmp( option_, "attribute_statusbar" ) ) {
		set_color( value_, _attributeStatusBar_ );
	} else if ( ! stricasecmp( option_, "attribute_search_highlight" ) ) {
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
	try {
		errno = 0;
		int escdelay( 0 );
		yaal_options()(
			HProgramOptionsHandler::HOption()
			.long_form( "esc_delay" )
			.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
			.description( "ncurses escape sequence time span" )
			.recipient( escdelay ) /* defined inside ncurses lib */
			.argument_name( "seconds" )
		)(
			HProgramOptionsHandler::HOption()
			.long_form( "latency" )
			.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
			.description( "how often invoke idle event" )
			.recipient( _latency_ )
			.argument_name( "seconds" )
		)(
			HProgramOptionsHandler::HOption()
			.long_form( "command_compose_character" )
			.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
			.description( "character that shall be uses as command composition base" )
			.recipient( _commandComposeCharacter_ )
			.argument_name( "character" )
		)(
			HProgramOptionsHandler::HOption()
			.long_form( "command_compose_delay" )
			.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
			.description( "command composition time span" )
			.recipient( _commandComposeDelay_ )
			.argument_name( "seconds" )
		);
		yaal_options().process_rc_file( "console", set_hconsole_variables );
		HConsole::set_escdelay( escdelay );
	} catch ( HException const& e ) {
		fprintf( stderr, "Failed to initialize yaal-hconsole: %s\n", e.what() );
		exit( 1 );
	}
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

