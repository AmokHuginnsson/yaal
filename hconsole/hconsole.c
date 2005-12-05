/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hconsole.c - this file is integral part of `stdhapi' project.

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

#include <stdlib.h>
#include <string.h>

#include "config.h"

#ifdef HAVE_NCURSES_H
#	include <ncurses.h>
#elif defined ( HAVE_NCURSES_NCURSES_H )
#	include <ncurses/ncurses.h>
#else /* HAVE_NCURSES_NCURSES_H */
#	error "No ncurses header available."
#endif /* not HAVE_NCURSES_NCURSES_H */

#include "hcore/hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "hcore/hcore.h"
#include "hcore/hlog.h"
#include "hcore/rc_file.h"
#include "console.h"

using namespace stdhapi::hcore;

namespace stdhapi
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

OVariable n_psVariables [ ] =
	{
		{ D_BOOL, "use_mouse", & n_bUseMouse },
		{ D_BOOL, "disable_XON", & n_bDisableXON },
		{ D_BOOL, "leave_ctrl_c", & n_bLeaveCtrlC },
		{ D_BOOL, "leave_ctrl_z", & n_bLeaveCtrlZ },
		{ D_BOOL, "leave_ctrl_s", & n_bLeaveCtrlS },
		{ D_BOOL, "leave_ctrl_q", & n_bLeaveCtrlQ },
		{ D_BOOL, "leave_ctrl_\\", & n_bLeaveCtrlBackSlash },
		{ D_INT, "esc_delay", & ESCDELAY }, /* defined inside ncurses lib */
		{ D_INT, "latency", & n_iLatency },
		{ D_CHAR, "command_compose_character", & n_cCommandComposeCharacter },
		{ D_INT, "command_compose_delay", & n_iCommandComposeDelay },
		{ D_VOID, NULL, NULL }
	};

void set_color_bits ( int & a_riWord, int a_iBits, int a_iWhat )
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

int get_color_bits ( HString & a_roValue, int a_iWhat )
	{
	HString l_oStr = a_roValue.split ( " \t", a_iWhat );
	if ( l_oStr.is_empty ( ) )
		return ( 0 );
	if ( ! strcasecmp ( l_oStr, "BLACK" ) )
		return ( COLORS::D_FG_BLACK );
	else if ( ! strcasecmp ( l_oStr, "RED" ) )
		return ( COLORS::D_FG_RED );
	else if ( ! strcasecmp ( l_oStr, "GREEN" ) )
		return ( COLORS::D_FG_GREEN );
	else if ( ! strcasecmp ( l_oStr, "BROWN" ) )
		return ( COLORS::D_FG_BROWN );
	else if ( ! strcasecmp ( l_oStr, "BLUE" ) )
		return ( COLORS::D_FG_BLUE );
	else if ( ! strcasecmp ( l_oStr, "MAGENTA" ) )
		return ( COLORS::D_FG_MAGENTA );
	else if ( ! strcasecmp ( l_oStr, "CYAN" ) )
		return ( COLORS::D_FG_CYAN );
	else if ( ! strcasecmp ( l_oStr, "LIGHTGRAY" ) )
		return ( COLORS::D_FG_LIGHTGRAY );
	else if ( ! strcasecmp ( l_oStr, "GRAY" ) )
		return ( COLORS::D_FG_GRAY );
	else if ( ! strcasecmp ( l_oStr, "BRGIHTRED" ) )
		return ( COLORS::D_FG_BRIGHTRED );
	else if ( ! strcasecmp ( l_oStr, "BRIGHTGREEN" ) )
		return ( COLORS::D_FG_BRIGHTGREEN );
	else if ( ! strcasecmp ( l_oStr, "YELLOW" ) )
		return ( COLORS::D_FG_YELLOW );
	else if ( ! strcasecmp ( l_oStr, "BRIGHTBLUE" ) )
		return ( COLORS::D_FG_BRIGHTBLUE );
	else if ( ! strcasecmp ( l_oStr, "BRIGHTMAGENTA" ) )
		return ( COLORS::D_FG_BRIGHTMAGENTA );
	else if ( ! strcasecmp ( l_oStr, "BRIGHTCYAN" ) )
		return ( COLORS::D_FG_BRIGHTCYAN );
	else if ( ! strcasecmp ( l_oStr, "WHITE" ) )
		return ( COLORS::D_FG_WHITE );
	return ( 0 );
	}

void set_color ( HString & a_roValue, int & a_riAttribute )
	{
	int l_iCtr = 0;
	int l_piTab [ 4 ] = { 2, 3, 0, 1 };
	for ( l_iCtr = 0; l_iCtr < 4; l_iCtr ++ )
		set_color_bits ( a_riAttribute,
				get_color_bits ( a_roValue, l_iCtr ), l_piTab [ l_iCtr ] );
	return;
	}

bool const set_hconsole_variables ( HString & a_roOption, HString & a_roValue )
	{
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
	}

extern "C"
void hconsole_init ( void ); __attribute__ ( ( constructor ) )
void hconsole_init ( void )
	{
	g_iErrNo = 0;
	M_IRV ( rc_file::process_rc_file ( "stdhapi", "console",
				n_psVariables, set_hconsole_variables ) );
	return;
	}

static char const g_pcDynamicLinkerPath [ ]
	__attribute__(( __section__(".interp") )) = __DYNAMIC_LINKER__;

void stdhapi_hconsole_banner ( void )
	{
	fprintf ( stdout, "\thconsole\n" );
	return;
	}

extern "C"
void stdhapi_hconsole_main ( void ) __attribute__(( __noreturn__ ));
void stdhapi_hconsole_main ( void )
	{
	if ( g_pcDynamicLinkerPath [ 0 ] )
		{
		stdhapi_hcore_banner ( );
		stdhapi_hconsole_banner ( );
		}
	exit ( 0 );
	}

}

}

