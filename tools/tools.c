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
#include <signal.h>	 /* signal handling */
#include <termios.h> /* B115200 */

#include "../hcore/hexception.h" /* M_PROLOG, M_EPILOG */
M_CVSID ( "$CVSHeader$" );
#include "../hcore/xalloc.h"
#include "../hcore/rc_file.h"    /* read conf from rc */
#include "../hcore/hlog.h"       /* log object */
#include "../hcore/hstring.h"    /* HString class */

namespace tools
{

char * n_pcSerialDevice = NULL;
int n_iBaudRate = B115200;
int n_iCollectorConnectionTimeOut = 9999;
bool n_bIgnoreSignalSIGINT = false;
bool n_bIgnoreSignalSIGTSTP = false;
bool n_bIgnoreSignalSIGQUIT = false;

OVariable n_psVariables [ ] =
	{
		{ D_TYPE_BOOL, "ignore_signal_SIGINT", & n_bIgnoreSignalSIGINT },
		{ D_TYPE_BOOL, "ignore_signal_SIGTSTP", & n_bIgnoreSignalSIGTSTP },
		{ D_TYPE_BOOL, "ignore_signal_SIGQUIT", & n_bIgnoreSignalSIGQUIT },
		{ D_TYPE_INT, "collector_connection_timeout",
		& n_iCollectorConnectionTimeOut },
		{ D_TYPE_CHAR_POINTER, "serial_device", & n_pcSerialDevice },
		{ 0, NULL, NULL }
	};
	
}

namespace util
	{
extern char n_pcTransTableStripPL [ 256 ];
	}

/* return true means error occured, false - every thing ok */
bool set_tools_variables ( HString & a_roOption, HString & a_roValue )
	{
	int l_iBaudRate = 0;
	if ( ! strcasecmp ( a_roOption, "serial_baudrate" ) )
		{
		if ( ( a_roValue.get_length ( ) > 1 ) && ( a_roValue [ 0 ] == 'B' ) )
			{
			switch ( strtol ( ( ( char * ) a_roValue ) + 1, NULL, 10 ) )
				{
#ifdef __HOST_OS_TYPE_FREEBSD__
				case (  76800 ): l_iBaudRate = B76800;  break;
				case (  28800 ): l_iBaudRate = B28800;  break;
				case (  14400 ): l_iBaudRate = B14400;  break;
				case (   7200 ): l_iBaudRate = B7200;   break;
#endif /* __HOST_OS_TYPE_FREEBSD__ */
				case ( 115200 ): l_iBaudRate = B115200; break;
				case (  57600 ): l_iBaudRate = B57600;  break;
				case (  38400 ): l_iBaudRate = B38400;  break;
				case (  19200 ): l_iBaudRate = B19200;  break;
				case (   9600 ): l_iBaudRate = B9600;   break;
				case (   4800 ): l_iBaudRate = B4800;   break;
				case (   2400 ): l_iBaudRate = B2400;   break;
				}
			}
		}
	if ( l_iBaudRate )tools::n_iBaudRate = l_iBaudRate;
	return ( ! l_iBaudRate );
	}

void tools_init ( void ); __attribute__ ( ( constructor ) )
void tools_init ( void )
	{
	int l_iCtr = 0;
	g_iErrNo = 0;
	rc_file::process_rc_file ( "stdhapi", "tools", tools::n_psVariables,
			set_tools_variables );
	for ( l_iCtr = 0; l_iCtr < 256; l_iCtr ++ )
		util::n_pcTransTableStripPL [ l_iCtr ] = l_iCtr;
	util::n_pcTransTableStripPL [ ( unsigned char ) '±' ] = 'a';
	util::n_pcTransTableStripPL [ ( unsigned char ) '¡' ] = 'A';
	util::n_pcTransTableStripPL [ ( unsigned char ) 'æ' ] = 'c';
	util::n_pcTransTableStripPL [ ( unsigned char ) 'Æ' ] = 'C';
	util::n_pcTransTableStripPL [ ( unsigned char ) 'ê' ] = 'e';
	util::n_pcTransTableStripPL [ ( unsigned char ) 'Ê' ] = 'E';
	util::n_pcTransTableStripPL [ ( unsigned char ) '³' ] = 'l';
	util::n_pcTransTableStripPL [ ( unsigned char ) '£' ] = 'L';
	util::n_pcTransTableStripPL [ ( unsigned char ) 'ñ' ] = 'n';
	util::n_pcTransTableStripPL [ ( unsigned char ) 'Ñ' ] = 'N';
	util::n_pcTransTableStripPL [ ( unsigned char ) 'ó' ] = 'o';
	util::n_pcTransTableStripPL [ ( unsigned char ) 'Ó' ] = 'O';
	util::n_pcTransTableStripPL [ ( unsigned char ) '¶' ] = 's';
	util::n_pcTransTableStripPL [ ( unsigned char ) '¦' ] = 'S';
	util::n_pcTransTableStripPL [ ( unsigned char ) '¼' ] = 'z';
	util::n_pcTransTableStripPL [ ( unsigned char ) '¬' ] = 'Z';
	util::n_pcTransTableStripPL [ ( unsigned char ) '¿' ] = 'z';
	util::n_pcTransTableStripPL [ ( unsigned char ) '¯' ] = 'Z';
	return;
	}

void tools_fini ( void ); __attribute__ ( ( destructor ) )
void tools_fini ( void )
	{
	if ( tools::n_pcSerialDevice )xfree ( ( void * ) tools::n_pcSerialDevice );
	tools::n_pcSerialDevice = NULL;
	return;
	}

/* older versions of g++ fail to handle __attribute__((constructor))
   if no static object exists */

#if __GNUC__ < 3 || \
	 ( __GNUC__ == 3 && __GNUC_MINOR__ < 3 )

HString g_oDummyTOOLS;

#endif

