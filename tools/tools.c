/*
---          `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski           ---

	tools.c - this file is integral part of `stdhapi' project.

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

#include <stdlib.h>  /* strtol */
#include <string.h>  /* strsignal */
#include <stdio.h>	 /* perror function */
#include <termios.h> /* B115200 */
#include <libintl.h> /* gettext */

#include "hcore/hexception.h" /* M_PROLOG, M_EPILOG */
M_CVSID ( "$CVSHeader$" );
#include "tools.h"
#include "hcore/hcore.h"
#include "hcore/xalloc.h"
#include "hcore/rc_file.h"    /* read conf from rc */
#include "hcore/hlog.h"       /* log object */
#include "hcore/hstring.h"    /* HString class */
#include "hconsole/hconsole.h"

using namespace stdhapi::hcore;
using namespace stdhapi::hconsole;

namespace stdhapi
{

namespace tools
{

char * n_pcSerialDevice = NULL;
HSerial::speed_t n_eBaudRate = HSerial::D_SPEED_B115200;
HSerial::flags_t n_eSerialFlags = HSerial::D_FLAGS_HARDWARE_FLOW_CONTROL | HSerial::D_FLAGS_BITS_PER_BYTE_8;
int n_iCollectorConnectionTimeOut = 9999;
bool n_bIgnoreSignalSIGINT = false;
bool n_bIgnoreSignalSIGTSTP = false;
bool n_bIgnoreSignalSIGQUIT = false;

OVariable n_psVariables [ ] =
	{
		{ D_BOOL, "ignore_signal_SIGINT", & n_bIgnoreSignalSIGINT },
		{ D_BOOL, "ignore_signal_SIGTSTP", & n_bIgnoreSignalSIGTSTP },
		{ D_BOOL, "ignore_signal_SIGQUIT", & n_bIgnoreSignalSIGQUIT },
		{ D_INT, "collector_connection_timeout",
		& n_iCollectorConnectionTimeOut },
		{ D_CHAR_POINTER, "serial_device", & n_pcSerialDevice },
		{ D_VOID, NULL, NULL }
	};
	
namespace util
	{
extern char n_pcTransTableStripPL [ 256 ];
	}

/* return true means error occured, false - every thing ok */
bool const set_tools_variables ( HString & a_roOption, HString & a_roValue )
	{
	int l_iBaudRate = 0, l_iCtr = 0;
	HString l_oStr;
	if ( ! strcasecmp ( a_roOption, "set_env" ) )
		set_env ( a_roValue );
	else if ( ! strcasecmp ( a_roOption, "serial_baudrate" ) )
		{
		if ( ( a_roValue.get_length ( ) > 1 ) && ( a_roValue [ 0 ] == 'B' ) )
			{
			l_iBaudRate = strtol ( static_cast < char const * const > ( a_roValue ) + 1, NULL, 10 );
			switch ( l_iBaudRate )
				{
				case ( 115200 ): n_eBaudRate = HSerial::D_SPEED_B115200; break;
#if ( HAVE_DECL_B76800 )
				case (  76800 ): n_eBaudRate = HSerial::D_SPEED_B76800;  break;
#endif /* HAVE_DECL_B76800 */
				case (  57600 ): n_eBaudRate = HSerial::D_SPEED_B57600;  break;
				case (  38400 ): n_eBaudRate = HSerial::D_SPEED_B38400;  break;
#if ( HAVE_DECL_B28800 )
				case (  28800 ): n_eBaudRate = HSerial::D_SPEED_B28800;  break;
#endif /* HAVE_DECL_B28800 */
				case (  19200 ): n_eBaudRate = HSerial::D_SPEED_B19200;  break;
#if ( HAVE_DECL_B14400 )
				case (  14400 ): n_eBaudRate = HSerial::D_SPEED_B14400;  break;
#endif /* HAVE_DECL_B14400 */
				case (   9600 ): n_eBaudRate = HSerial::D_SPEED_B9600;   break;
#if ( HAVE_DECL_B7200 )
				case (   7200 ): n_eBaudRate = HSerial::D_SPEED_B7200;   break;
#endif /* HAVE_DECL_B7200 */
				case (   4800 ): n_eBaudRate = HSerial::D_SPEED_B4800;   break;
				case (   2400 ): n_eBaudRate = HSerial::D_SPEED_B2400;   break;
				default:
					M_THROW ( _ ( "unknown baud rate" ), l_iBaudRate );
				}
			}
		}
	else if ( ! strcasecmp ( a_roOption, "serial_flags" ) )
		{
		while ( ! ( l_oStr = a_roValue.split ( " \t", l_iCtr ++ ) ).is_empty ( ) )
			{
			if ( ! strcasecmp ( l_oStr, "HARDWARE_FLOW_CONTROL" ) )
				n_eSerialFlags = HSerial::D_FLAGS_HARDWARE_FLOW_CONTROL;
			else if ( ! strcasecmp ( l_oStr, "SOFTWARE_FLOW_CONTROL" ) )
				n_eSerialFlags = HSerial::D_FLAGS_SOFTWARE_FLOW_CONTROL;
			else if ( ! strcasecmp ( l_oStr, "ECHO" ) )
				n_eSerialFlags = HSerial::D_FLAGS_ECHO;
			else if ( ! strcasecmp ( l_oStr, "BITS_PER_BYTE_8" ) )
				n_eSerialFlags = HSerial::D_FLAGS_BITS_PER_BYTE_8;
			else if ( ! strcasecmp ( l_oStr, "BITS_PER_BYTE_7" ) )
				n_eSerialFlags = HSerial::D_FLAGS_BITS_PER_BYTE_7;
			else if ( ! strcasecmp ( l_oStr, "BITS_PER_BYTE_6" ) )
				n_eSerialFlags = HSerial::D_FLAGS_BITS_PER_BYTE_6;
			else if ( ! strcasecmp ( l_oStr, "BITS_PER_BYTE_5" ) )
				n_eSerialFlags = HSerial::D_FLAGS_BITS_PER_BYTE_5;
			else if ( ! strcasecmp ( l_oStr, "CANONICAL" ) )
				n_eSerialFlags = HSerial::D_FLAGS_CANONICAL;
			else if ( ! strcasecmp ( l_oStr, "STOP_BITS_1" ) )
				n_eSerialFlags = HSerial::D_FLAGS_STOP_BITS_1;
			else if ( ! strcasecmp ( l_oStr, "STOP_BITS_2" ) )
				n_eSerialFlags = HSerial::D_FLAGS_STOP_BITS_2;
			else if ( ! strcasecmp ( l_oStr, "PARITY_CHECK" ) )
				n_eSerialFlags = HSerial::D_FLAGS_PARITY_CHECK;
			else if ( ! strcasecmp ( l_oStr, "PARITY_ODD" ) )
				n_eSerialFlags = HSerial::D_FLAGS_PARITY_ODD;
			else
				return ( true );
			}
		}
	return ( false );
	}

extern "C"
void tools_init ( void ); __attribute__ ( ( constructor ) )
void tools_init ( void )
	{
	int l_iCtr = 0;
	g_iErrNo = 0;
	rc_file::process_rc_file ( "stdhapi", "tools", tools::n_psVariables,
			set_tools_variables );
	for ( l_iCtr = 0; l_iCtr < 256; l_iCtr ++ )
		util::n_pcTransTableStripPL [ l_iCtr ] = static_cast < char > ( l_iCtr );
	util::n_pcTransTableStripPL [ static_cast < unsigned char > ( '±' ) ] = 'a';
	util::n_pcTransTableStripPL [ static_cast < unsigned char > ( '¡' ) ] = 'A';
	util::n_pcTransTableStripPL [ static_cast < unsigned char > ( 'æ' ) ] = 'c';
	util::n_pcTransTableStripPL [ static_cast < unsigned char > ( 'Æ' ) ] = 'C';
	util::n_pcTransTableStripPL [ static_cast < unsigned char > ( 'ê' ) ] = 'e';
	util::n_pcTransTableStripPL [ static_cast < unsigned char > ( 'Ê' ) ] = 'E';
	util::n_pcTransTableStripPL [ static_cast < unsigned char > ( '³' ) ] = 'l';
	util::n_pcTransTableStripPL [ static_cast < unsigned char > ( '£' ) ] = 'L';
	util::n_pcTransTableStripPL [ static_cast < unsigned char > ( 'ñ' ) ] = 'n';
	util::n_pcTransTableStripPL [ static_cast < unsigned char > ( 'Ñ' ) ] = 'N';
	util::n_pcTransTableStripPL [ static_cast < unsigned char > ( 'ó' ) ] = 'o';
	util::n_pcTransTableStripPL [ static_cast < unsigned char > ( 'Ó' ) ] = 'O';
	util::n_pcTransTableStripPL [ static_cast < unsigned char > ( '¶' ) ] = 's';
	util::n_pcTransTableStripPL [ static_cast < unsigned char > ( '¦' ) ] = 'S';
	util::n_pcTransTableStripPL [ static_cast < unsigned char > ( '¼' ) ] = 'z';
	util::n_pcTransTableStripPL [ static_cast < unsigned char > ( '¬' ) ] = 'Z';
	util::n_pcTransTableStripPL [ static_cast < unsigned char > ( '¿' ) ] = 'z';
	util::n_pcTransTableStripPL [ static_cast < unsigned char > ( '¯' ) ] = 'Z';
	return;
	}

void tools_fini ( void ); __attribute__ ( ( destructor ) )
void tools_fini ( void )
	{
	if ( tools::n_pcSerialDevice )
		xfree ( tools::n_pcSerialDevice );
	tools::n_pcSerialDevice = NULL;
	return;
	}

static char const g_pcDynamicLinkerPath [ ]
	__attribute__(( __section__(".interp") )) = __DYNAMIC_LINKER__;

void stdhapi_tools_banner ( void )
	{
	fprintf ( stdout, "\ttools\n" );
	return;
	}

extern "C"
void stdhapi_tools_main ( void ) __attribute__(( __noreturn__ ));
void stdhapi_tools_main ( void )
	{
	if ( g_pcDynamicLinkerPath [ 0 ] )
		{
		stdhapi_hcore_banner ( );
		stdhapi_hconsole_banner ( );
		stdhapi_tools_banner ( );
		}
	exit ( 0 );
	}

}

}

/* older versions of g++ fail to handle __attribute__((constructor))
   if no static object exists */

