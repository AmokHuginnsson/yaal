/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstdlib>  /* strtol */
#include <cstring>  /* strsignal */
#include <cstdio>	 /* perror function */
#include <libintl.h> /* gettext */

#include "hcore/base.hxx" /* M_PROLOG, M_EPILOG */
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools.hxx"
#include "hcore/hcore.hxx"
#include "hcore/unicode.hxx"
#include "hcore/hprogramoptionshandler.hxx"    /* read conf from rc */
#include "hcore/hlog.hxx"       /* log object */
#include "hcore/hstring.hxx"    /* HString class */
#include "hcore/htokenizer.hxx" /* HTokenizer class */
#include "util.hxx" /* atof_ex */
#include "signals.hxx" /* HSignalService::_killGracePeriod */
#include "hpipedchild.hxx" /* HPipedChild::_killGracePeriod */

using namespace yaal::hcore;

namespace yaal {

namespace {

double long yaal_strtold( HString const& str_, int* endIdx_ ) {
	M_PROLOG
	double long value( tools::util::atof_ex( str_, true ) );
	if ( endIdx_ ) {
		*endIdx_ = static_cast<int>( str_.get_length() );
	}
	return value;
	M_EPILOG
}

}

namespace tools {

HString _serialDevice_;
HString _defaultEncoding_ = "UTF-8";
HSerial::SPEED _baudRate_ = HSerial::SPEED::B_115200;
HSerial::flag_t _serialFlags_ = HSerial::flag_t( HSerial::FLAG::FLOW_CONTROL_HARDWARE ) | HSerial::FLAG::BITS_PER_BYTE_8;
bool _ignoreSignalSIGINT_ = false;
bool _ignoreSignalSIGTSTP_ = false;
bool _ignoreSignalSIGQUIT_ = false;
bool _disableXON_ = false;
bool _leaveCtrlC_ = false;
bool _leaveCtrlZ_ = false;
bool _leaveCtrlS_ = false;
bool _leaveCtrlQ_ = false;
bool _leaveCtrlBackSlash_ = false;

namespace util {
code_point_translation_t _transTableStripPL_;
}

namespace {

/* return true means error occurred, false - everything ok */
bool set_tools_variables( HString& option_, HString& value_ ) {
	M_PROLOG
	bool fail( false );
	if ( ! stricasecmp( option_, "set_env" ) ) {
		decode_set_env( value_ );
	} else if ( ! stricasecmp( option_, "serial_baudrate" ) ) {
		if ( ( value_.get_length() > 1 ) && ( value_[ 0 ] == 'B' ) ) {
			int baudRate( lexical_cast<int>( value_.substr( 1 ) ) );
			switch ( baudRate ) {
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
				break;
			}
		}
	} else if ( ! stricasecmp( option_, "serial_flags" ) ) {
		_serialFlags_ = HSerial::FLAG::DEFAULT ^ HSerial::FLAG::DEFAULT;
		HTokenizer t( value_, " \t", HTokenizer::DELIMITED_BY_ANY_OF );
		for ( HTokenizer::HIterator it = t.begin(), end = t.end(); it != end; ++ it ) {
			if ( ! stricasecmp( *it, "FLOW_CONTROL_HARDWARE" ) ) {
				_serialFlags_ |= HSerial::FLAG::FLOW_CONTROL_HARDWARE;
			} else if ( ! stricasecmp( *it, "FLOW_CONTROL_SOFTWARE" ) ) {
				_serialFlags_ |= HSerial::FLAG::FLOW_CONTROL_SOFTWARE;
			} else if ( ! stricasecmp( *it, "ECHO" ) ) {
				_serialFlags_ |= HSerial::FLAG::ECHO;
			} else if ( ! stricasecmp( *it, "BITS_PER_BYTE_8" ) ) {
				_serialFlags_ |= HSerial::FLAG::BITS_PER_BYTE_8;
			} else if ( ! stricasecmp( *it, "BITS_PER_BYTE_7" ) ) {
				_serialFlags_ |= HSerial::FLAG::BITS_PER_BYTE_7;
			} else if ( ! stricasecmp( *it, "BITS_PER_BYTE_6" ) ) {
				_serialFlags_ |= HSerial::FLAG::BITS_PER_BYTE_6;
			} else if ( ! stricasecmp( *it, "BITS_PER_BYTE_5" ) ) {
				_serialFlags_ |= HSerial::FLAG::BITS_PER_BYTE_5;
			} else if ( ! stricasecmp( *it, "CANONICAL" ) ) {
				_serialFlags_ |= HSerial::FLAG::CANONICAL;
			} else if ( ! stricasecmp( *it, "STOP_BITS_1" ) ) {
				_serialFlags_ |= HSerial::FLAG::STOP_BITS_1;
			} else if ( ! stricasecmp( *it, "STOP_BITS_2" ) ) {
				_serialFlags_ |= HSerial::FLAG::STOP_BITS_2;
			} else if ( ! stricasecmp( *it, "PARITY_CHECK" ) ) {
				_serialFlags_ |= HSerial::FLAG::PARITY_CHECK;
			} else if ( ! stricasecmp( *it, "PARITY_ODD" ) ) {
				_serialFlags_ |= HSerial::FLAG::PARITY_ODD;
			} else {
				fail = true;
			}
		}
	}
	return ( ! fail );
	M_EPILOG
}

}

class HToolsInitDeinit {
public:
	HToolsInitDeinit( void );
	~HToolsInitDeinit( void );
} initDeinit;

HToolsInitDeinit::HToolsInitDeinit( void ) {
	M_PROLOG
	try {
		yaal_options()(
			HProgramOptionsHandler::HOption()
			.long_form( "ignore_signal_SIGINT" )
			.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::OPTIONAL )
			.description( "ignore INT (interrupt) signal" )
			.recipient( _ignoreSignalSIGINT_ )
		)(
			HProgramOptionsHandler::HOption()
			.long_form( "ignore_signal_SIGTSTP" )
			.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::OPTIONAL )
			.description( "ignore TSTP (terminal stop, suspend) signal" )
			.recipient( _ignoreSignalSIGTSTP_ )
		)(
			HProgramOptionsHandler::HOption()
			.long_form( "ignore_signal_SIGQUIT" )
			.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::OPTIONAL )
			.description( "ignore QUIT, core dump signal" )
			.recipient( _ignoreSignalSIGQUIT_ )
		)(
			HProgramOptionsHandler::HOption()
			.long_form( "disable_XON" )
			.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::OPTIONAL )
			.description( "disable flow control events" )
			.recipient( _disableXON_ )
		)(
			HProgramOptionsHandler::HOption()
			.long_form( "leave_ctrl_c" )
			.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::OPTIONAL )
			.description( "disable special handling of CTRL+C sequence" )
			.recipient( _leaveCtrlC_ )
		)(
			HProgramOptionsHandler::HOption()
			.long_form( "leave_ctrl_z" )
			.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::OPTIONAL )
			.description( "disable special handling of CTRL+Z sequence" )
			.recipient( _leaveCtrlZ_ )
		)(
			HProgramOptionsHandler::HOption()
			.long_form( "leave_ctrl_s" )
			.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::OPTIONAL )
			.description( "disable special handling of CTRL+S sequence" )
			.recipient( _leaveCtrlS_ )
		)(
			HProgramOptionsHandler::HOption()
			.long_form( "leave_ctrl_q" )
			.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::OPTIONAL )
			.description( "disable special handling of CTRL+Q sequence" )
			.recipient( _leaveCtrlQ_ )
		)(
			HProgramOptionsHandler::HOption()
			.long_form( "leave_ctrl_\\" )
			.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::OPTIONAL )
			.description( "disable special handling of CTRL+\\ sequence" )
			.recipient( _leaveCtrlBackSlash_ )
		)(
			HProgramOptionsHandler::HOption()
			.long_form( "serial_device" )
			.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
			.description( "path to serial device" )
			.recipient( _serialDevice_ )
			.argument_name( "path" )
		)(
			HProgramOptionsHandler::HOption()
			.long_form( "default_encoding" )
			.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
			.description( "default character encoding used in text documents" )
			.recipient( _defaultEncoding_ )
			.argument_name( "encoding" )
		)(
			HProgramOptionsHandler::HOption()
			.long_form( "kill_grace_period" )
			.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
			.description( "wait for that many milliseconds before killing interior" )
			.recipient( HSignalService::_killGracePeriod )
			.argument_name( "milliseconds" )
		)(
			HProgramOptionsHandler::HOption()
			.long_form( "child_kill_grace_period" )
			.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
			.description( "wait for that many milliseconds before killing child process" )
			.recipient( HPipedChild::_killGracePeriod )
			.argument_name( "milliseconds" )
		)(
			HProgramOptionsHandler::HOption()
			.long_form( "huginn_max_call_stack_size" )
			.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
			.description( "default maximum call stack size for Huginn interpreter" )
			.recipient( _huginnMaxCallStack_ )
			.argument_name( "count" )
		)(
			HProgramOptionsHandler::HOption()
			.long_form( "compression_level" )
			.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
			.description( "default compression level for zlib library" )
			.recipient( _compressionLevel_ )
			.argument_name( "level" )
		)(
			HProgramOptionsHandler::HOption()
			.long_form( "compression_buffer_size" )
			.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
			.description( "size for compression buffer used in zlib library" )
			.recipient( _zBufferSize_ )
			.argument_name( "numBytes" )
		);
		errno = 0;
		extendable::set_strtold_impl( &yaal_strtold );
		yaal_options().process_rc_file( "tools", set_tools_variables );
		util::_transTableStripPL_.insert( make_pair( unicode::CODE_POINT::LATIN_SMALL_LETTER_A_WITH_OGONEK,      'a'_ycp ) );
		util::_transTableStripPL_.insert( make_pair( unicode::CODE_POINT::LATIN_CAPITAL_LETTER_A_WITH_OGONEK,    'A'_ycp ) );
		util::_transTableStripPL_.insert( make_pair( unicode::CODE_POINT::LATIN_SMALL_LETTER_C_WITH_ACUTE,       'c'_ycp ) );
		util::_transTableStripPL_.insert( make_pair( unicode::CODE_POINT::LATIN_CAPITAL_LETTER_C_WITH_ACUTE,     'C'_ycp ) );
		util::_transTableStripPL_.insert( make_pair( unicode::CODE_POINT::LATIN_SMALL_LETTER_E_WITH_OGONEK,      'e'_ycp ) );
		util::_transTableStripPL_.insert( make_pair( unicode::CODE_POINT::LATIN_CAPITAL_LETTER_E_WITH_OGONEK,    'E'_ycp ) );
		util::_transTableStripPL_.insert( make_pair( unicode::CODE_POINT::LATIN_SMALL_LETTER_L_WITH_STROKE,      'l'_ycp ) );
		util::_transTableStripPL_.insert( make_pair( unicode::CODE_POINT::LATIN_CAPITAL_LETTER_L_WITH_STROKE,    'L'_ycp ) );
		util::_transTableStripPL_.insert( make_pair( unicode::CODE_POINT::LATIN_SMALL_LETTER_N_WITH_ACUTE,       'n'_ycp ) );
		util::_transTableStripPL_.insert( make_pair( unicode::CODE_POINT::LATIN_CAPITAL_LETTER_N_WITH_ACUTE,     'N'_ycp ) );
		util::_transTableStripPL_.insert( make_pair( unicode::CODE_POINT::LATIN_SMALL_LETTER_O_WITH_ACUTE,       'o'_ycp ) );
		util::_transTableStripPL_.insert( make_pair( unicode::CODE_POINT::LATIN_CAPITAL_LETTER_O_WITH_ACUTE,     'O'_ycp ) );
		util::_transTableStripPL_.insert( make_pair( unicode::CODE_POINT::LATIN_SMALL_LETTER_S_WITH_ACUTE,       's'_ycp ) );
		util::_transTableStripPL_.insert( make_pair( unicode::CODE_POINT::LATIN_CAPITAL_LETTER_S_WITH_ACUTE,     'S'_ycp ) );
		util::_transTableStripPL_.insert( make_pair( unicode::CODE_POINT::LATIN_SMALL_LETTER_Z_WITH_ACUTE,       'z'_ycp ) );
		util::_transTableStripPL_.insert( make_pair( unicode::CODE_POINT::LATIN_CAPITAL_LETTER_Z_WITH_ACUTE,     'Z'_ycp ) );
		util::_transTableStripPL_.insert( make_pair( unicode::CODE_POINT::LATIN_SMALL_LETTER_Z_WITH_DOT_ABOVE,   'z'_ycp ) );
		util::_transTableStripPL_.insert( make_pair( unicode::CODE_POINT::LATIN_CAPITAL_LETTER_Z_WITH_DOT_ABOVE, 'Z'_ycp ) );
	} catch ( HException const& e ) {
		fprintf( stderr, "Failed to initialize yaal-tools: %s\n", e.what() );
		exit( 1 );
	}
	return;
	M_EPILOG
}

HToolsInitDeinit::~HToolsInitDeinit( void ) {
	return;
}

void banner( void ) {
	::printf( "\ttools\n" );
	return;
}

}

}

#ifdef __DYNAMIC_LINKER__
extern "C"
int yaal_tools_main( int, char** ) __attribute__((noreturn));
extern "C"
int yaal_tools_main( int, char** ) {
	static char const dynamicLinkerPath[]
#ifdef HAVE_INTERP_ALLOWED
		__attribute__(( __section__(".interp") ))
#endif /* #ifdef HAVE_INTERP_ALLOWED */
		= __DYNAMIC_LINKER__;
	if ( dynamicLinkerPath[ 0 ] ) {
		yaal::hcore::banner();
		yaal::tools::banner();
		::printf( "\n" );
	}
	::exit( 0 );
}
#else /* #ifdef __DYNAMIC_LINKER__ */
extern "C"
int yaal_tools_main( int, char** );
extern "C"
int yaal_tools_main( int, char** ) {
	return ( 0 );
}
#endif /* #else #ifdef __DYNAMIC_LINKER__ */

