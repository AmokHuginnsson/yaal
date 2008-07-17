/*
---          `yaal' (c) 1978 by Marcin 'Amok' Konarski           ---

	tools.cxx - this file is integral part of `yaal' project.

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

#include <cstdlib>  /* strtol */
#include <cstring>  /* strsignal */
#include <cstdio>	 /* perror function */
#include <libintl.h> /* gettext */

#include "hcore/hexception.h" /* M_PROLOG, M_EPILOG */
M_VCSID ( "$Id$" )
#include "tools.h"
#include "hcore/hcore.h"
#include "hcore/xalloc.h"
#include "hcore/rc_file.h"    /* read conf from rc */
#include "hcore/hlog.h"       /* log object */
#include "hcore/hstring.h"    /* HString class */

using namespace yaal::hcore;

namespace yaal
{

namespace tools
{

char * n_pcSerialDevice = NULL;
HSerial::speed_t n_eBaudRate = HSerial::D_SPEED_B115200;
HSerial::flags_t n_eSerialFlags = HSerial::D_FLAGS_FLOW_CONTROL_HARDWARE | HSerial::D_FLAGS_BITS_PER_BYTE_8;
int n_iCollectorConnectionTimeOut = 9999;
bool n_bIgnoreSignalSIGINT = false;
bool n_bIgnoreSignalSIGTSTP = false;
bool n_bIgnoreSignalSIGQUIT = false;

OOption n_psVariables [ ] =
	{
		{ "ignore_signal_SIGINT", D_BOOL, &n_bIgnoreSignalSIGINT, 0, OOption::D_OPTIONAL, NULL, "ignore INT (interrupt) signal", NULL },
		{ "ignore_signal_SIGTSTP", D_BOOL, &n_bIgnoreSignalSIGTSTP, 0, OOption::D_OPTIONAL, NULL, "ignore TSTP (terminal stop, suspend) signal", NULL },
		{ "ignore_signal_SIGQUIT", D_BOOL, &n_bIgnoreSignalSIGQUIT, 0, OOption::D_OPTIONAL, NULL, "ignore QUIT, core dump signal", NULL },
		{ "serial_device", D_CHAR_PTR, &n_pcSerialDevice, 0, OOption::D_REQUIRED, NULL, "path to serial device", NULL },
		{ "collector_connection_timeout", D_INT, &n_iCollectorConnectionTimeOut, 0, OOption::D_REQUIRED, NULL, "timeout on collector device read", NULL },
		{ NULL, D_VOID, NULL, 0, OOption::D_NONE, NULL, NULL, NULL }
	};
	
namespace util
	{
extern char n_pcTransTableStripPL [ 256 ];
	}

/* return true means error occured, false - every thing ok */
bool set_tools_variables ( HString & a_roOption, HString & a_roValue )
	{
	M_PROLOG
	int l_iBaudRate = 0, l_iCtr = 0;
	HString l_oStr;
	if ( ! strcasecmp ( a_roOption, "set_env" ) )
		set_env ( a_roValue );
	else if ( ! strcasecmp ( a_roOption, "serial_baudrate" ) )
		{
		if ( ( a_roValue.get_length() > 1 ) && ( a_roValue [ 0 ] == 'B' ) )
			{
			l_iBaudRate = lexical_cast<int>( a_roValue.raw() + 1 );
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
		while ( ! ( l_oStr = a_roValue.split ( " \t", l_iCtr ++ ) ).is_empty() )
			{
			if ( ! strcasecmp ( l_oStr, "FLOW_CONTROL_HARDWARE" ) )
				n_eSerialFlags = HSerial::D_FLAGS_FLOW_CONTROL_HARDWARE;
			else if ( ! strcasecmp ( l_oStr, "SOFTWARE_CONTROL_SOFTWARE" ) )
				n_eSerialFlags = HSerial::D_FLAGS_FLOW_CONTROL_SOFTWARE;
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
	M_EPILOG
	}

class HToolsInitDeinit
	{
public:
	HToolsInitDeinit( void );
	~HToolsInitDeinit( void );
	} initDeinit;

HToolsInitDeinit::HToolsInitDeinit( void )
	{
	M_PROLOG
	int l_iCtr = 0;
	errno = 0;
	rc_file::process_rc_file ( "yaal", "tools", tools::n_psVariables,
			set_tools_variables );
	for ( l_iCtr = 0; l_iCtr < 256; l_iCtr ++ )
		util::n_pcTransTableStripPL[ l_iCtr ] = static_cast<char>( l_iCtr );
	util::n_pcTransTableStripPL[ static_cast<char unsigned>( '±' ) ] = 'a';
	util::n_pcTransTableStripPL[ static_cast<char unsigned>( '¡' ) ] = 'A';
	util::n_pcTransTableStripPL[ static_cast<char unsigned>( 'æ' ) ] = 'c';
	util::n_pcTransTableStripPL[ static_cast<char unsigned>( 'Æ' ) ] = 'C';
	util::n_pcTransTableStripPL[ static_cast<char unsigned>( 'ê' ) ] = 'e';
	util::n_pcTransTableStripPL[ static_cast<char unsigned>( 'Ê' ) ] = 'E';
	util::n_pcTransTableStripPL[ static_cast<char unsigned>( '³' ) ] = 'l';
	util::n_pcTransTableStripPL[ static_cast<char unsigned>( '£' ) ] = 'L';
	util::n_pcTransTableStripPL[ static_cast<char unsigned>( 'ñ' ) ] = 'n';
	util::n_pcTransTableStripPL[ static_cast<char unsigned>( 'Ñ' ) ] = 'N';
	util::n_pcTransTableStripPL[ static_cast<char unsigned>( 'ó' ) ] = 'o';
	util::n_pcTransTableStripPL[ static_cast<char unsigned>( 'Ó' ) ] = 'O';
	util::n_pcTransTableStripPL[ static_cast<char unsigned>( '¶' ) ] = 's';
	util::n_pcTransTableStripPL[ static_cast<char unsigned>( '¦' ) ] = 'S';
	util::n_pcTransTableStripPL[ static_cast<char unsigned>( '¼' ) ] = 'z';
	util::n_pcTransTableStripPL[ static_cast<char unsigned>( '¬' ) ] = 'Z';
	util::n_pcTransTableStripPL[ static_cast<char unsigned>( '¿' ) ] = 'z';
	util::n_pcTransTableStripPL[ static_cast<char unsigned>( '¯' ) ] = 'Z';
	return;
	M_EPILOG
	}

HToolsInitDeinit::~HToolsInitDeinit( void )
	{
	if ( tools::n_pcSerialDevice )
		xfree ( tools::n_pcSerialDevice );
	tools::n_pcSerialDevice = NULL;
	return;
	}

static char const g_pcDynamicLinkerPath [ ]
	__attribute__(( __section__(".interp") )) = __DYNAMIC_LINKER__;

void yaal_tools_banner ( void )
	{
	fprintf ( stdout, "\ttools\n" );
	return;
	}

extern "C"
int yaal_tools_main( int, char** ) __attribute__(( __noreturn__ ));
int yaal_tools_main( int, char** )
	{
	if ( g_pcDynamicLinkerPath[ 0 ] )
		{
		yaal_hcore_banner();
		yaal_tools_banner();
		}
	::exit( 0 );
	}

}

}

/* older versions of g++ fail to handle __attribute__((constructor))
   if no static object exists */

