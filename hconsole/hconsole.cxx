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

#include "config.hxx"

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "hcore/hcore.hxx"
#include "hcore/hlog.hxx"
#include "hcore/hprogramoptionshandler.hxx"
#include "hcore/htokenizer.hxx"
#include "tools/tools.hxx"
#include "console.hxx"

#ifdef HAVE_CURSES_H
#	include <curses.h>
#elif defined ( HAVE_NCURSES_CURSES_H )
#	include <ncurses/curses.h>
#else /* HAVE_NCURSES_CURSES_H */
#	error "No ncurses header available."
#endif /* not HAVE_NCURSES_CURSES_H */

using namespace yaal::hcore;
using namespace yaal::tools;

namespace yaal
{

namespace hconsole
{

int n_iLatency = 1;
int n_iAttributeDisabled = ( COLORS::D_FG_GREEN | COLORS::D_BG_BLACK | COLORS::D_BG_BLINK ) << 8
															| ( COLORS::D_FG_LIGHTGRAY | COLORS::D_BG_BLACK | COLORS::D_BG_BLINK );
int n_iAttributeEnabled = ( COLORS::D_FG_BRIGHTGREEN | COLORS::D_BG_BLACK | COLORS::D_BG_BLINK ) << 8
															| ( COLORS::D_FG_WHITE | COLORS::D_BG_BLACK | COLORS::D_BG_BLINK );
int n_iAttributeFocused = ( COLORS::D_FG_BRIGHTGREEN | COLORS::D_BG_BLACK | COLORS::D_BG_BLINK ) << 8
															| ( COLORS::D_FG_BLACK | COLORS::D_BG_LIGHTGRAY ); 
int n_iAttributeStatusBar = ( COLORS::D_FG_WHITE | COLORS::D_BG_BLACK ) << 8
															| ( COLORS::D_FG_LIGHTGRAY | COLORS::D_BG_BLACK );
int n_iAttributeSearchHighlight = ( COLORS::D_FG_BLACK | COLORS::D_BG_BROWN | COLORS::D_BG_BLINK ) << 8
																		| ( COLORS::D_FG_BLACK | COLORS::D_BG_BROWN );
bool	n_bUseMouse = false;
bool	n_bDisableXON = false;
bool	n_bLeaveCtrlC = false;
bool	n_bLeaveCtrlZ = false;
bool	n_bLeaveCtrlS = false;
bool	n_bLeaveCtrlQ = false;
bool	n_bLeaveCtrlBackSlash = false;
char	n_cCommandComposeCharacter = 'x';
int		n_iCommandComposeDelay = 16;

void set_color_bits( int & a_riWord, int a_iBits, int a_iWhat )
	{
	int unsigned l_uiMask = 0x000f;
	if ( a_iWhat )
		{
		a_iWhat <<= 2;
		a_iBits <<= a_iWhat;
		l_uiMask <<= a_iWhat;
		}
	a_riWord &= ~ l_uiMask;
	a_riWord |= a_iBits;
	return;
	}

int get_color_bits( HString& a_roValue, int a_iWhat )
	{
	M_PROLOG
	HTokenizer t( a_roValue, " \t" );
	HString l_oStr = t[ a_iWhat ];
	if ( l_oStr.is_empty() )
		return ( 0 );
	if ( ! strcasecmp( l_oStr, "BLACK" ) )
		return ( COLORS::D_FG_BLACK );
	else if ( ! strcasecmp( l_oStr, "RED" ) )
		return ( COLORS::D_FG_RED );
	else if ( ! strcasecmp( l_oStr, "GREEN" ) )
		return ( COLORS::D_FG_GREEN );
	else if ( ! strcasecmp( l_oStr, "BROWN" ) )
		return ( COLORS::D_FG_BROWN );
	else if ( ! strcasecmp( l_oStr, "BLUE" ) )
		return ( COLORS::D_FG_BLUE );
	else if ( ! strcasecmp( l_oStr, "MAGENTA" ) )
		return ( COLORS::D_FG_MAGENTA );
	else if ( ! strcasecmp( l_oStr, "CYAN" ) )
		return ( COLORS::D_FG_CYAN );
	else if ( ! strcasecmp( l_oStr, "LIGHTGRAY" ) )
		return ( COLORS::D_FG_LIGHTGRAY );
	else if ( ! strcasecmp( l_oStr, "GRAY" ) )
		return ( COLORS::D_FG_GRAY );
	else if ( ! strcasecmp( l_oStr, "BRGIHTRED" ) )
		return ( COLORS::D_FG_BRIGHTRED );
	else if ( ! strcasecmp( l_oStr, "BRIGHTGREEN" ) )
		return ( COLORS::D_FG_BRIGHTGREEN );
	else if ( ! strcasecmp( l_oStr, "YELLOW" ) )
		return ( COLORS::D_FG_YELLOW );
	else if ( ! strcasecmp( l_oStr, "BRIGHTBLUE" ) )
		return ( COLORS::D_FG_BRIGHTBLUE );
	else if ( ! strcasecmp( l_oStr, "BRIGHTMAGENTA" ) )
		return ( COLORS::D_FG_BRIGHTMAGENTA );
	else if ( ! strcasecmp( l_oStr, "BRIGHTCYAN" ) )
		return ( COLORS::D_FG_BRIGHTCYAN );
	else if ( ! strcasecmp( l_oStr, "WHITE" ) )
		return ( COLORS::D_FG_WHITE );
	return ( 0 );
	M_EPILOG
	}

void set_color( HString& a_roValue, int& a_riAttribute )
	{
	M_PROLOG
	int l_iCtr = 0;
	int l_piTab[ 4 ] = { 2, 3, 0, 1 };
	for ( l_iCtr = 0; l_iCtr < 4; l_iCtr ++ )
		set_color_bits( a_riAttribute,
				get_color_bits( a_roValue, l_iCtr ), l_piTab [ l_iCtr ] );
	return;
	M_EPILOG
	}

bool set_hconsole_variables( HString& a_roOption, HString& a_roValue )
	{
	M_PROLOG
	if ( ! strcasecmp ( a_roOption, "set_env" ) )
		set_env ( a_roValue );
	else if ( ! strcasecmp ( a_roOption, "attribute_disabled" ) )
		set_color ( a_roValue, n_iAttributeDisabled );
	else if ( ! strcasecmp ( a_roOption, "attribute_enabled" ) )
		set_color ( a_roValue, n_iAttributeEnabled );
	else if ( ! strcasecmp ( a_roOption, "attribute_focused" ) )
		set_color ( a_roValue, n_iAttributeFocused );
	else if ( ! strcasecmp ( a_roOption, "attribute_statusbar" ) )
		set_color ( a_roValue, n_iAttributeStatusBar );
	else if ( ! strcasecmp ( a_roOption, "attribute_search_highlight" ) )
		set_color ( a_roValue, n_iAttributeSearchHighlight );
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
	yaalOptions( "use_mouse", program_options_helper::option_value( n_bUseMouse ), 0, HProgramOptionsHandler::OOption::TYPE::D_OPTIONAL, NULL, "enable mouse support" )
		( "disable_XON", program_options_helper::option_value( n_bDisableXON ), 0, HProgramOptionsHandler::OOption::TYPE::D_OPTIONAL, NULL, "disable flow control events" )
		( "leave_ctrl_c", program_options_helper::option_value( n_bLeaveCtrlC ), 0, HProgramOptionsHandler::OOption::TYPE::D_OPTIONAL, NULL, "disable special handling of CTRL+C sequence" )
		( "leave_ctrl_z", program_options_helper::option_value( n_bLeaveCtrlZ ), 0, HProgramOptionsHandler::OOption::TYPE::D_OPTIONAL, NULL, "disable special handling of CTRL+Z sequence" )
		( "leave_ctrl_s", program_options_helper::option_value( n_bLeaveCtrlS ), 0, HProgramOptionsHandler::OOption::TYPE::D_OPTIONAL, NULL, "disable special handling of CTRL+S sequence" )
		( "leave_ctrl_q", program_options_helper::option_value( n_bLeaveCtrlQ ), 0, HProgramOptionsHandler::OOption::TYPE::D_OPTIONAL, NULL, "disable special handling of CTRL+Q sequence" )
		( "leave_ctrl_\\", program_options_helper::option_value( n_bLeaveCtrlBackSlash ), 0, HProgramOptionsHandler::OOption::TYPE::D_OPTIONAL, NULL, "disable special handling of CTRL+\\ sequence" )
		( "esc_delay", program_options_helper::option_value( ESCDELAY ), 0, HProgramOptionsHandler::OOption::TYPE::D_REQUIRED, NULL, "ncurses escape sequence time span" ) /* defined inside ncurses lib */
		( "latency", program_options_helper::option_value( n_iLatency ), 0, HProgramOptionsHandler::OOption::TYPE::D_REQUIRED, NULL, "how often invoke idle event" )
		( "command_compose_character", program_options_helper::option_value( n_cCommandComposeCharacter ), 0, HProgramOptionsHandler::OOption::TYPE::D_REQUIRED, NULL,
			"character that shall be uses as command composition base" )
		( "command_compose_delay", program_options_helper::option_value( n_iCommandComposeDelay ), 0, HProgramOptionsHandler::OOption::TYPE::D_REQUIRED, NULL, "command composition time span" );
	yaalOptions.process_rc_file( "yaal", "console", set_hconsole_variables );
	return;
	M_EPILOG
	}

#if defined( __DYNAMIC_LINKER__ )
static char const g_pcDynamicLinkerPath [ ]
	__attribute__(( __section__(".interp") )) = __DYNAMIC_LINKER__;

void yaal_hconsole_banner ( void )
	{
	fprintf ( stdout, "\thconsole\n" );
	return;
	}

extern "C"
int yaal_hconsole_main( int, char** )
	{
	if ( g_pcDynamicLinkerPath[ 0 ] )
		{
		yaal_hcore_banner();
		yaal_tools_banner();
		yaal_hconsole_banner();
		::exit( 0 );
		}
	return ( 0 );
	}
#endif /* __DYNAMIC_LINKER__ */

}

}

