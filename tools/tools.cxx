/*
---          `yaal' (c) 1978 by Marcin 'Amok' Konarski           ---

	tools.cxx - this file is integral part of `yaal' project.

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

#include <cstdlib>  /* strtol */
#include <cstring>  /* strsignal */
#include <cstdio>	 /* perror function */
#include <libintl.h> /* gettext */

#include "hcore/base.hxx" /* M_PROLOG, M_EPILOG */
M_VCSID( "$Id: "__ID__" $" )
#include "tools.hxx"
#include "hcore/hcore.hxx"
#include "hcore/xalloc.hxx"
#include "hcore/hprogramoptionshandler.hxx"    /* read conf from rc */
#include "hcore/hlog.hxx"       /* log object */
#include "hcore/hstring.hxx"    /* HString class */
#include "hcore/htokenizer.hxx" /* HTokenizer class */
#include "util.hxx" /* atof_ex */

using namespace yaal::hcore;

namespace yaal
{

namespace
{

double long smart_strtold( HString const& str )
	{
	M_PROLOG
	return ( tools::util::atof_ex( str, true ) );
	M_EPILOG
	}

}

namespace extendable
{

typedef double long ( *my_strtold_t )( HString const& );
extern my_strtold_t my_strtold;

}

namespace tools
{

HString n_oSerialDevice;
HSerial::speed_t n_eBaudRate = HSerial::SPEED::B_115200;
HSerial::flag_t n_eSerialFlags = HSerial::flag_t( HSerial::FLAG::FLOW_CONTROL_HARDWARE ) | HSerial::FLAG::BITS_PER_BYTE_8;
int n_iCollectorConnectionTimeOut = 9999;
bool n_bIgnoreSignalSIGINT = false;
bool n_bIgnoreSignalSIGTSTP = false;
bool n_bIgnoreSignalSIGQUIT = false;
	
namespace util
	{
extern char n_pcTransTableStripPL [ 256 ];
	}

/* return true means error occured, false - every thing ok */
bool set_tools_variables ( HString & a_roOption, HString & a_roValue )
	{
	M_PROLOG
	int l_iBaudRate = 0;
	if ( ! strcasecmp ( a_roOption, "set_env" ) )
		set_env ( a_roValue );
	else if ( ! strcasecmp ( a_roOption, "serial_baudrate" ) )
		{
		if ( ( a_roValue.get_length() > 1 ) && ( a_roValue [ 0 ] == 'B' ) )
			{
			l_iBaudRate = lexical_cast<int>( a_roValue.raw() + 1 );
			switch ( l_iBaudRate )
				{
				case ( 115200 ): n_eBaudRate = HSerial::SPEED::B_115200; break;
#if ( HAVE_DECL_B76800 )
				case (  76800 ): n_eBaudRate = HSerial::SPEED::B_76800;  break;
#endif /* HAVE_DECL_B76800 */
				case (  57600 ): n_eBaudRate = HSerial::SPEED::B_57600;  break;
				case (  38400 ): n_eBaudRate = HSerial::SPEED::B_38400;  break;
#if ( HAVE_DECL_B28800 )
				case (  28800 ): n_eBaudRate = HSerial::SPEED::B_28800;  break;
#endif /* HAVE_DECL_B28800 */
				case (  19200 ): n_eBaudRate = HSerial::SPEED::B_19200;  break;
#if ( HAVE_DECL_B14400 )
				case (  14400 ): n_eBaudRate = HSerial::SPEED::B_14400;  break;
#endif /* HAVE_DECL_B14400 */
				case (   9600 ): n_eBaudRate = HSerial::SPEED::B_9600;   break;
#if ( HAVE_DECL_B7200 )
				case (   7200 ): n_eBaudRate = HSerial::SPEED::B_7200;   break;
#endif /* HAVE_DECL_B7200 */
				case (   4800 ): n_eBaudRate = HSerial::SPEED::B_4800;   break;
				case (   2400 ): n_eBaudRate = HSerial::SPEED::B_2400;   break;
				default:
					M_THROW ( _ ( "unknown baud rate" ), l_iBaudRate );
				}
			}
		}
	else if ( ! strcasecmp ( a_roOption, "serial_flags" ) )
		{
		HTokenizer t( a_roValue, " \t" );
		for ( HTokenizer::HIterator it = t.begin(), end = t.end(); it != end; ++ it )
			{
			if ( ! strcasecmp ( *it, "FLOW_CONTROL_HARDWARE" ) )
				n_eSerialFlags = HSerial::FLAG::FLOW_CONTROL_HARDWARE;
			else if ( ! strcasecmp ( *it, "SOFTWARE_CONTROL_SOFTWARE" ) )
				n_eSerialFlags = HSerial::FLAG::FLOW_CONTROL_SOFTWARE;
			else if ( ! strcasecmp ( *it, "ECHO" ) )
				n_eSerialFlags = HSerial::FLAG::ECHO;
			else if ( ! strcasecmp ( *it, "BITS_PER_BYTE_8" ) )
				n_eSerialFlags = HSerial::FLAG::BITS_PER_BYTE_8;
			else if ( ! strcasecmp ( *it, "BITS_PER_BYTE_7" ) )
				n_eSerialFlags = HSerial::FLAG::BITS_PER_BYTE_7;
			else if ( ! strcasecmp ( *it, "BITS_PER_BYTE_6" ) )
				n_eSerialFlags = HSerial::FLAG::BITS_PER_BYTE_6;
			else if ( ! strcasecmp ( *it, "BITS_PER_BYTE_5" ) )
				n_eSerialFlags = HSerial::FLAG::BITS_PER_BYTE_5;
			else if ( ! strcasecmp ( *it, "CANONICAL" ) )
				n_eSerialFlags = HSerial::FLAG::CANONICAL;
			else if ( ! strcasecmp ( *it, "STOP_BITS_1" ) )
				n_eSerialFlags = HSerial::FLAG::STOP_BITS_1;
			else if ( ! strcasecmp ( *it, "STOP_BITS_2" ) )
				n_eSerialFlags = HSerial::FLAG::STOP_BITS_2;
			else if ( ! strcasecmp ( *it, "PARITY_CHECK" ) )
				n_eSerialFlags = HSerial::FLAG::PARITY_CHECK;
			else if ( ! strcasecmp ( *it, "PARITY_ODD" ) )
				n_eSerialFlags = HSerial::FLAG::PARITY_ODD;
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
	yaalOptions( "ignore_signal_SIGINT", program_options_helper::option_value( n_bIgnoreSignalSIGINT ), HProgramOptionsHandler::OOption::TYPE::OPTIONAL, "ignore INT (interrupt) signal" )
			( "ignore_signal_SIGTSTP", program_options_helper::option_value( n_bIgnoreSignalSIGTSTP ), HProgramOptionsHandler::OOption::TYPE::OPTIONAL, "ignore TSTP (terminal stop, suspend) signal" )
			( "ignore_signal_SIGQUIT", program_options_helper::option_value( n_bIgnoreSignalSIGQUIT ), HProgramOptionsHandler::OOption::TYPE::OPTIONAL, "ignore QUIT, core dump signal" )
			( "serial_device", program_options_helper::option_value( n_oSerialDevice ), HProgramOptionsHandler::OOption::TYPE::REQUIRED, "path to serial device", "path" )
			( "collector_connection_timeout", program_options_helper::option_value( n_iCollectorConnectionTimeOut ), HProgramOptionsHandler::OOption::TYPE::REQUIRED, "timeout on collector device read", "seconds" );
	int l_iCtr = 0;
	errno = 0;
	extendable::my_strtold = smart_strtold;
	yaalOptions.process_rc_file ( "yaal", "tools", set_tools_variables );
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
	return;
	}

#if defined( __DYNAMIC_LINKER__ )
static char const g_pcDynamicLinkerPath [ ]
	__attribute__(( __section__(".interp") )) = __DYNAMIC_LINKER__;

void yaal_tools_banner ( void )
	{
	fprintf ( stdout, "\ttools\n" );
	return;
	}

extern "C"
int yaal_tools_main( int, char** )
	{
	if ( g_pcDynamicLinkerPath[ 0 ] )
		{
		yaal_hcore_banner();
		yaal_tools_banner();
		::exit( 0 );
		}
	return ( 0 );
	}
#endif /* __DYNAMIC_LINKER__ */

}

}

