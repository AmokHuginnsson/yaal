#include "config.hxx"
char const COPYRIGHT [ ] =
"/*\n"
"---           `yaal' "PACKAGE_VERSION" (c) 1978 by Marcin 'Amok' Konarski            ---\n"
"\n"
/* hcore.cxx - this file is integral part of `yaal' project.
 */
"	i.  You may not make any changes in Copyright information.\n"
"	ii. You must attach Copyright information to any part of every copy\n"
"	    of this software.\n"
"\n"
"Copyright:\n"
"\n"
" You are free to use this program as is, you can redistribute binary\n"
" package freely but:\n"
"  1. You cannot use any part of sources of this software.\n"
"  2. You cannot redistribute any part of sources of this software.\n"
"  3. No reverse engineering is allowed.\n"
"  4. If you want redistribute binary package you cannot demand any fees\n"
"     for this software.\n"
"     You cannot even demand cost of the carrier (CD for example).\n"
"  5. You cannot include it to any commercial enterprise (for example\n"
"     as a free add-on to payed software or payed newspaper).\n"
" This program is distributed in the hope that it will be useful, but WITHOUT\n"
" ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or\n"
" FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.\n"
"*/\n";

#include <cstdio>
#include <cstdlib>
#include <libintl.h>
#include <locale.h>

#include "base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hcore.hxx"
#include "hlog.hxx"
#include "hstring.hxx"
#include "htokenizer.hxx"
#include "hprogramoptionshandler.hxx"
#include "hsingleton.hxx"
#include "hopenssl.hxx"
#include "hsocket.hxx"
#include "pod.hxx"

namespace yaal
{

namespace hcore
{

int _debugLevel_ = 0;
HLog& log( HLog::get_instance( 1000 ) );

yaal::hcore::HString HOpenSSL::_sSLKey;
yaal::hcore::HString HOpenSSL::_sSLCert;

HProgramOptionsHandler& yaal_options( void )
	{
	M_PROLOG
	static HProgramOptionsHandler yaalOptions;
	return ( yaalOptions );
	M_EPILOG
	}

/* mathematical macros */
static double long const EPSILON = 0.000001;
bool eq( double long left_, double long right_ )
	{
	return ( ( ( ( ( left_ ) > ( right_ ) )
					? ( ( left_ ) - ( right_ ) )
					: ( ( right_ ) - ( left_ ) ) ) < EPSILON ) );
	}

bool set_hcore_variables( HString& option_, HString& value_ )
	{
	M_PROLOG
	if ( ! strcasecmp( option_, "set_env" ) )
		decode_set_env( value_ );
	else if ( ! strcasecmp( option_, "log_mask" ) )
		{
		HTokenizer t( value_, " \t" );
		for ( HTokenizer::HIterator it = t.begin(), end = t.end(); it != end; ++ it )
			{
			if ( ! strcasecmp( *it, "LOG_DEBUG" ) )
				HLog::_logMask |= LOG_TYPE::DEBUG;
			else if ( ! strcasecmp( *it, "LOG_INFO" ) )
				HLog::_logMask |= LOG_TYPE::INFO;
			else if ( ! strcasecmp( *it, "LOG_NOTICE" ) )
				HLog::_logMask |= LOG_TYPE::NOTICE;
			else if ( ! strcasecmp( *it, "LOG_WARNING" ) )
				HLog::_logMask |= LOG_TYPE::WARNING;
			else if ( ! strcasecmp( *it, "LOG_ERROR" ) )
				HLog::_logMask |= LOG_TYPE::ERROR;
			else if ( ! strcasecmp( *it, "LOG_VCSHEADER" ) )
				HLog::_logMask |= LOG_TYPE::VCSHEADER;
			else
				return ( true );
			}
		}
	else
		return ( true );
	return ( false );
	M_EPILOG
	}

void set_env( HString const& name_, HString const& value_, bool overwrite_ )
	{
	M_PROLOG
	int const TRUE = 1;
	int const FALSE = 0;
	M_ENSURE( ::setenv( name_.raw(), value_.raw(), overwrite_ ? TRUE : FALSE ) == 0 );
	return;
	M_EPILOG
	}

void decode_set_env( HString line )
	{
	M_PROLOG
	int long eon = 0;
	if ( ( line.length() < 3 )
			|| ( ( eon = line.find_one_of( " \t" ) ) == -1 ) )
		{
		log( LOG_TYPE::ERROR ) << "bad set_env argument: `";
		log << line << '\'' << endl;
		return;
		}
	int long valPos = line.find_other_than( " \t", eon );
	if ( valPos < 0 )
		{
		log( LOG_TYPE::ERROR ) << "no value for environment variable in set_env: `";
		log << line << '\'' << endl;
		return;
		}
	HString val = line.mid( valPos );
	line.set_at( eon, '\0' );
	set_env( line, val );
	return;
	M_EPILOG
	}

namespace
{

double long std_strtold( HString const& str_ )
	{
	return ( ::strtold( str_.raw(), NULL ) );
	}

}

/*! \brief Extend features of one library in another library.
 */
namespace extendable
{

typedef double long ( *yaal_strtold_t )( HString const& );
yaal_strtold_t acting_strtold = &std_strtold;

void set_strtold_impl( yaal_strtold_t newStrtold_ )
	{
	acting_strtold = newStrtold_;
	}

}

double long strtold( HString const& str_ )
	{
	return ( extendable::acting_strtold( str_ ) );
	}

class HCoreInitDeinit
	{
public:
	HCoreInitDeinit( void );
	} initDeinit;

void init_locale( char const* const package_ )
	{
	M_PROLOG
	char* gettextPath( ::getenv( "GETTEXT_PATH" ) );
	if ( gettextPath )
		{
		bindtextdomain( package_ ? package_ : PACKAGE_NAME, gettextPath );
		if ( package_ )
			{
			textdomain( package_ );
			setlocale( LC_ALL, "" );
			}
		}
	return;
	M_EPILOG
	}

HCoreInitDeinit::HCoreInitDeinit( void )
	{
	STATIC_ASSERT( sizeof( int ) >= 4 );
	STATIC_ASSERT( sizeof( u8_t ) == 1 );
	STATIC_ASSERT( sizeof( u16_t ) == 2 );
	STATIC_ASSERT( sizeof( u32_t ) == 4 );
#if 0
	STATIC_ASSERT( sizeof( u64_t ) == 8 );
#endif
	errno = 0;
	init_locale();
	char* env( ::getenv( "YAAL_DEBUG" ) );
	if ( env )
		_debugLevel_ = lexical_cast<int>( env );
	yaal_options()( "ssl_key", program_options_helper::option_value( HOpenSSL::_sSLKey ), HProgramOptionsHandler::OOption::TYPE::REQUIRED, "Path to the OpenSSL private key file.", "path" )
		( "ssl_cert", program_options_helper::option_value( HOpenSSL::_sSLCert ), HProgramOptionsHandler::OOption::TYPE::REQUIRED, "Path to the OpenSSL certificate file.", "path" )
		( "resolve_hostnames", program_options_helper::option_value( HSocket::_resolveHostnames ), HProgramOptionsHandler::OOption::TYPE::REQUIRED, "Resolve IP address into host names." )
		( "write_timeout", program_options_helper::option_value( _writeTimeout_ ), HProgramOptionsHandler::OOption::TYPE::REQUIRED, "Time-out for low level write operations." );
	yaal_options().process_rc_file( "yaal", "core", set_hcore_variables );
	if ( _writeTimeout_ < LOW_TIMEOUT_WARNING )
		log( LOG_TYPE::WARNING ) << "Low default timout for write operations!" << endl;
	return;
	}

void banner( void )
	{
	::printf(
		"%s\n"
		"Available libraries:\n"
		"\thcore\n", COPYRIGHT );
	return;
	}

}

}

extern "C"
int yaal_hcore_main( int, char** )
	{
	static char const dynamicLinkerPath[]
		__attribute__(( __section__(".interp") )) = __DYNAMIC_LINKER__;
	if ( dynamicLinkerPath[ 0 ] )
		{
		yaal::hcore::banner();
		::exit( 0 );
		}
	return ( 0 );
	}

