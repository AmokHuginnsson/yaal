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
M_VCSID( "$Id: "__TID__" $" )
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

HString _serialDevice_;
HString _defaultEncoding_ = "ISO-8859-2";
HSerial::speed_t _baudRate_ = HSerial::SPEED::B_115200;
HSerial::flag_t _serialFlags_ = HSerial::flag_t( HSerial::FLAG::FLOW_CONTROL_HARDWARE ) | HSerial::FLAG::BITS_PER_BYTE_8;
int _collectorConnectionTimeOut_ = 9999;
bool _ignoreSignalSIGINT_ = false;
bool _ignoreSignalSIGTSTP_ = false;
bool _ignoreSignalSIGQUIT_ = false;

namespace util
	{
extern char _transTableStripPL_ [ 256 ];
	}

/* return true means error occured, false - every thing ok */
bool set_tools_variables( HString& option_, HString& value_ )
	{
	M_PROLOG
	int baudRate = 0;
	if ( ! strcasecmp( option_, "set_env" ) )
		decode_set_env( value_ );
	else if ( ! strcasecmp( option_, "serial_baudrate" ) )
		{
		if ( ( value_.get_length() > 1 ) && ( value_ [ 0 ] == 'B' ) )
			{
			baudRate = lexical_cast<int>( value_.raw() + 1 );
			switch ( baudRate )
				{
				case ( 115200 ): _baudRate_ = HSerial::SPEED::B_115200; break;
#if ( HAVE_DECL_B76800 )
				case (  76800 ): _baudRate_ = HSerial::SPEED::B_76800;  break;
#endif /* HAVE_DECL_B76800 */
				case (  57600 ): _baudRate_ = HSerial::SPEED::B_57600;  break;
				case (  38400 ): _baudRate_ = HSerial::SPEED::B_38400;  break;
#if ( HAVE_DECL_B28800 )
				case (  28800 ): _baudRate_ = HSerial::SPEED::B_28800;  break;
#endif /* HAVE_DECL_B28800 */
				case (  19200 ): _baudRate_ = HSerial::SPEED::B_19200;  break;
#if ( HAVE_DECL_B14400 )
				case (  14400 ): _baudRate_ = HSerial::SPEED::B_14400;  break;
#endif /* HAVE_DECL_B14400 */
				case (   9600 ): _baudRate_ = HSerial::SPEED::B_9600;   break;
#if ( HAVE_DECL_B7200 )
				case (   7200 ): _baudRate_ = HSerial::SPEED::B_7200;   break;
#endif /* HAVE_DECL_B7200 */
				case (   4800 ): _baudRate_ = HSerial::SPEED::B_4800;   break;
				case (   2400 ): _baudRate_ = HSerial::SPEED::B_2400;   break;
				default:
					M_THROW( _( "unknown baud rate" ), baudRate );
				}
			}
		}
	else if ( ! strcasecmp( option_, "serial_flags" ) )
		{
		HTokenizer t( value_, " \t" );
		for ( HTokenizer::HIterator it = t.begin(), end = t.end(); it != end; ++ it )
			{
			if ( ! strcasecmp( *it, "FLOW_CONTROL_HARDWARE" ) )
				_serialFlags_ = HSerial::FLAG::FLOW_CONTROL_HARDWARE;
			else if ( ! strcasecmp( *it, "SOFTWARE_CONTROL_SOFTWARE" ) )
				_serialFlags_ = HSerial::FLAG::FLOW_CONTROL_SOFTWARE;
			else if ( ! strcasecmp( *it, "ECHO" ) )
				_serialFlags_ = HSerial::FLAG::ECHO;
			else if ( ! strcasecmp( *it, "BITS_PER_BYTE_8" ) )
				_serialFlags_ = HSerial::FLAG::BITS_PER_BYTE_8;
			else if ( ! strcasecmp( *it, "BITS_PER_BYTE_7" ) )
				_serialFlags_ = HSerial::FLAG::BITS_PER_BYTE_7;
			else if ( ! strcasecmp( *it, "BITS_PER_BYTE_6" ) )
				_serialFlags_ = HSerial::FLAG::BITS_PER_BYTE_6;
			else if ( ! strcasecmp( *it, "BITS_PER_BYTE_5" ) )
				_serialFlags_ = HSerial::FLAG::BITS_PER_BYTE_5;
			else if ( ! strcasecmp( *it, "CANONICAL" ) )
				_serialFlags_ = HSerial::FLAG::CANONICAL;
			else if ( ! strcasecmp( *it, "STOP_BITS_1" ) )
				_serialFlags_ = HSerial::FLAG::STOP_BITS_1;
			else if ( ! strcasecmp( *it, "STOP_BITS_2" ) )
				_serialFlags_ = HSerial::FLAG::STOP_BITS_2;
			else if ( ! strcasecmp( *it, "PARITY_CHECK" ) )
				_serialFlags_ = HSerial::FLAG::PARITY_CHECK;
			else if ( ! strcasecmp( *it, "PARITY_ODD" ) )
				_serialFlags_ = HSerial::FLAG::PARITY_ODD;
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
	yaalOptions( "ignore_signal_SIGINT", program_options_helper::option_value( _ignoreSignalSIGINT_ ), HProgramOptionsHandler::OOption::TYPE::OPTIONAL, "ignore INT (interrupt) signal" )
			( "ignore_signal_SIGTSTP", program_options_helper::option_value( _ignoreSignalSIGTSTP_ ), HProgramOptionsHandler::OOption::TYPE::OPTIONAL, "ignore TSTP (terminal stop, suspend) signal" )
			( "ignore_signal_SIGQUIT", program_options_helper::option_value( _ignoreSignalSIGQUIT_ ), HProgramOptionsHandler::OOption::TYPE::OPTIONAL, "ignore QUIT, core dump signal" )
			( "serial_device", program_options_helper::option_value( _serialDevice_ ), HProgramOptionsHandler::OOption::TYPE::REQUIRED, "path to serial device", "path" )
			( "default_encoding", program_options_helper::option_value( _defaultEncoding_ ), HProgramOptionsHandler::OOption::TYPE::REQUIRED, "dafault character encoding used in text documents", "encoding" )
			( "compression_level", program_options_helper::option_value( _compressionLevel_ ), HProgramOptionsHandler::OOption::TYPE::REQUIRED, "default compression level for zlib library", "level" )
			( "compression_buffer_size", program_options_helper::option_value( _zBufferSize_ ), HProgramOptionsHandler::OOption::TYPE::REQUIRED, "size for compression buffer used in zlib library", "numBytes" )
			( "collector_connection_timeout", program_options_helper::option_value( _collectorConnectionTimeOut_ ), HProgramOptionsHandler::OOption::TYPE::REQUIRED, "timeout on collector device read", "seconds" );
	int ctr = 0;
	errno = 0;
	extendable::my_strtold = smart_strtold;
	yaalOptions.process_rc_file ( "yaal", "tools", set_tools_variables );
	for ( ctr = 0; ctr < 256; ctr ++ )
		util::_transTableStripPL_[ ctr ] = static_cast<char>( ctr );
	util::_transTableStripPL_[ static_cast<char unsigned>( '±' ) ] = 'a';
	util::_transTableStripPL_[ static_cast<char unsigned>( '¡' ) ] = 'A';
	util::_transTableStripPL_[ static_cast<char unsigned>( 'æ' ) ] = 'c';
	util::_transTableStripPL_[ static_cast<char unsigned>( 'Æ' ) ] = 'C';
	util::_transTableStripPL_[ static_cast<char unsigned>( 'ê' ) ] = 'e';
	util::_transTableStripPL_[ static_cast<char unsigned>( 'Ê' ) ] = 'E';
	util::_transTableStripPL_[ static_cast<char unsigned>( '³' ) ] = 'l';
	util::_transTableStripPL_[ static_cast<char unsigned>( '£' ) ] = 'L';
	util::_transTableStripPL_[ static_cast<char unsigned>( 'ñ' ) ] = 'n';
	util::_transTableStripPL_[ static_cast<char unsigned>( 'Ñ' ) ] = 'N';
	util::_transTableStripPL_[ static_cast<char unsigned>( 'ó' ) ] = 'o';
	util::_transTableStripPL_[ static_cast<char unsigned>( 'Ó' ) ] = 'O';
	util::_transTableStripPL_[ static_cast<char unsigned>( '¶' ) ] = 's';
	util::_transTableStripPL_[ static_cast<char unsigned>( '¦' ) ] = 'S';
	util::_transTableStripPL_[ static_cast<char unsigned>( '¼' ) ] = 'z';
	util::_transTableStripPL_[ static_cast<char unsigned>( '¬' ) ] = 'Z';
	util::_transTableStripPL_[ static_cast<char unsigned>( '¿' ) ] = 'z';
	util::_transTableStripPL_[ static_cast<char unsigned>( '¯' ) ] = 'Z';
	return;
	M_EPILOG
	}

HToolsInitDeinit::~HToolsInitDeinit( void )
	{
	return;
	}

#if defined( __DYNAMIC_LINKER__ )
static char const _dynamicLinkerPath_ [ ]
	__attribute__(( __section__(".interp") )) = __DYNAMIC_LINKER__;

void yaal_tools_banner ( void )
	{
	fprintf ( stdout, "\ttools\n" );
	return;
	}

extern "C"
int yaal_tools_main( int, char** )
	{
	if ( _dynamicLinkerPath_[ 0 ] )
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

