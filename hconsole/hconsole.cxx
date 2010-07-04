/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hconsole.cxx - this file is integral part of `yaal' project.

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

#include <cstdlib>
#include <cstring>
#include <cstdio>

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hcore/hcore.hxx"
#include "hcore/hlog.hxx"
#include "hcore/hprogramoptionshandler.hxx"
#include "hcore/htokenizer.hxx"
#include "tools/tools.hxx"
#include "console.hxx"

using namespace yaal::hcore;
using namespace yaal::tools;

namespace yaal
{

namespace hconsole
{

int _latency_ = 1;
int _attributeDisabled_ = ( COLORS::FG_GREEN | COLORS::BG_BLACK | COLORS::BG_BLINK ) << 8
															| ( COLORS::FG_LIGHTGRAY | COLORS::BG_BLACK | COLORS::BG_BLINK );
int _attributeEnabled_ = ( COLORS::FG_BRIGHTGREEN | COLORS::BG_BLACK | COLORS::BG_BLINK ) << 8
															| ( COLORS::FG_WHITE | COLORS::BG_BLACK | COLORS::BG_BLINK );
int _attributeFocused_ = ( COLORS::FG_BRIGHTGREEN | COLORS::BG_BLACK | COLORS::BG_BLINK ) << 8
															| ( COLORS::FG_BLACK | COLORS::BG_LIGHTGRAY ); 
int _attributeStatusBar_ = ( COLORS::FG_WHITE | COLORS::BG_BLACK ) << 8
															| ( COLORS::FG_LIGHTGRAY | COLORS::BG_BLACK );
int _attributeSearchHighlight_ = ( COLORS::FG_BLACK | COLORS::BG_BROWN | COLORS::BG_BLINK ) << 8
																		| ( COLORS::FG_BLACK | COLORS::BG_BROWN );
bool	_useMouse_ = false;
bool	_disableXON_ = false;
bool	_leaveCtrlC_ = false;
bool	_leaveCtrlZ_ = false;
bool	_leaveCtrlS_ = false;
bool	_leaveCtrlQ_ = false;
bool	_leaveCtrlBackSlash_ = false;
char	_commandComposeCharacter_ = 'x';
int		_commandComposeDelay_ = 16;

void set_color_bits( int & word_, int bits_, int what_ )
	{
	int unsigned mask = 0x000f;
	if ( what_ )
		{
		what_ <<= 2;
		bits_ <<= what_;
		mask <<= what_;
		}
	word_ &= ~ mask;
	word_ |= bits_;
	return;
	}

int get_color_bits( HString& value_, int what_ )
	{
	M_PROLOG
	HTokenizer t( value_, " \t" );
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
	else if ( ! strcasecmp( str, "BRGIHTRED" ) )
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

void set_color( HString& value_, int& attribute_ )
	{
	M_PROLOG
	int ctr = 0;
	int tab[ 4 ] = { 2, 3, 0, 1 };
	for ( ctr = 0; ctr < 4; ctr ++ )
		set_color_bits( attribute_,
				get_color_bits( value_, ctr ), tab [ ctr ] );
	return;
	M_EPILOG
	}

bool set_hconsole_variables( HString& option_, HString& value_ )
	{
	M_PROLOG
	if ( ! strcasecmp( option_, "set_env" ) )
		decode_set_env( value_ );
	else if ( ! strcasecmp( option_, "attribute_disabled" ) )
		set_color( value_, _attributeDisabled_ );
	else if ( ! strcasecmp( option_, "attribute_enabled" ) )
		set_color( value_, _attributeEnabled_ );
	else if ( ! strcasecmp( option_, "attribute_focused" ) )
		set_color( value_, _attributeFocused_ );
	else if ( ! strcasecmp( option_, "attribute_statusbar" ) )
		set_color( value_, _attributeStatusBar_ );
	else if ( ! strcasecmp( option_, "attribute_search_highlight" ) )
		set_color( value_, _attributeSearchHighlight_ );
	else
		return ( true );
	return ( false );
	M_EPILOG
	}

class HConsoleInitDeinit
	{
public:
	HConsoleInitDeinit( void );
	} initDeinit;


HConsoleInitDeinit::HConsoleInitDeinit( void )
	{
	M_PROLOG
	errno = 0;
	int escdelay( 0 );
	yaal_options()( "use_mouse", program_options_helper::option_value( _useMouse_ ), HProgramOptionsHandler::OOption::TYPE::OPTIONAL, "enable mouse support" )
		( "disable_XON", program_options_helper::option_value( _disableXON_ ), HProgramOptionsHandler::OOption::TYPE::OPTIONAL, "disable flow control events" )
		( "leave_ctrl_c", program_options_helper::option_value( _leaveCtrlC_ ), HProgramOptionsHandler::OOption::TYPE::OPTIONAL, "disable special handling of CTRL+C sequence" )
		( "leave_ctrl_z", program_options_helper::option_value( _leaveCtrlZ_ ), HProgramOptionsHandler::OOption::TYPE::OPTIONAL, "disable special handling of CTRL+Z sequence" )
		( "leave_ctrl_s", program_options_helper::option_value( _leaveCtrlS_ ), HProgramOptionsHandler::OOption::TYPE::OPTIONAL, "disable special handling of CTRL+S sequence" )
		( "leave_ctrl_q", program_options_helper::option_value( _leaveCtrlQ_ ), HProgramOptionsHandler::OOption::TYPE::OPTIONAL, "disable special handling of CTRL+Q sequence" )
		( "leave_ctrl_\\", program_options_helper::option_value( _leaveCtrlBackSlash_ ), HProgramOptionsHandler::OOption::TYPE::OPTIONAL, "disable special handling of CTRL+\\ sequence" )
		( "esc_delay", program_options_helper::option_value( escdelay ), HProgramOptionsHandler::OOption::TYPE::REQUIRED, "ncurses escape sequence time span", "seconds" ) /* defined inside ncurses lib */
		( "latency", program_options_helper::option_value( _latency_ ), HProgramOptionsHandler::OOption::TYPE::REQUIRED, "how often invoke idle event", "seconds" )
		( "command_compose_character", program_options_helper::option_value( _commandComposeCharacter_ ), HProgramOptionsHandler::OOption::TYPE::REQUIRED,
			"character that shall be uses as command composition base", "character" )
		( "command_compose_delay", program_options_helper::option_value( _commandComposeDelay_ ), HProgramOptionsHandler::OOption::TYPE::REQUIRED, "command composition time span", "seconds" );
	yaal_options().process_rc_file( "yaal", "console", set_hconsole_variables );
	HConsole::set_escdelay( escdelay );
	return;
	M_EPILOG
	}

void banner( void )
	{
	::printf( "\thconsole\n" );
	return;
	}

}

}

extern "C"
int yaal_hconsole_main( int, char** )
	{
	static char const dynamicLinkerPath[]
		__attribute__(( __section__(".interp") )) = __DYNAMIC_LINKER__;
	if ( dynamicLinkerPath[ 0 ] )
		{
		yaal::hcore::banner();
		yaal::tools::banner();
		yaal::hconsole::banner();
		::exit( 0 );
		}
	return ( 0 );
	}

