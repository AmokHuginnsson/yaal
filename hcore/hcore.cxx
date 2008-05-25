#include "config.h"
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
"  1. You can not use any part of sources of this software.\n"
"  2. You can not redistribute any part of sources of this software.\n"
"  3. No reverse engineering is allowed.\n"
"  4. If you want redistribute binary package you can not demand any fees\n"
"     for this software.\n"
"     You can not even demand cost of the carrier (CD for example).\n"
"  5. You can not include it to any commercial enterprise (for example\n"
"     as a free add-on to payed software or payed newspaper).\n"
" This program is distributed in the hope that it will be useful, but WITHOUT\n"
" ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or\n"
" FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.\n"
"*/\n";

#include <cstdlib>
#include <cstring>

#include "hexception.h"
M_VCSID ( "$Id$" )
#include "hcore.h"
#include "hlog.h"
#include "hstring.h"
#include "rc_file.h"
#include "hsingleton.h"
#include "hopenssl.h"
#include "hsocket.h"

namespace yaal
{

namespace hcore
{

int n_iDebugLevel = 0;
typedef HSingleton<HLog> HLogService;
HLog& log = HLogService::get_instance( 1000 );

OOption n_psHCoreVariables[] =
	{
		{ "ssl_key", D_HSTRING, &HOpenSSL::f_oSSLKey, 0, OOption::D_REQUIRED, NULL, "Path to the OpenSSL private key file.", NULL },
		{ "ssl_cert", D_HSTRING, &HOpenSSL::f_oSSLCert, 0, OOption::D_REQUIRED, NULL, "Path to the OpenSSL certificate file.", NULL },
		{ "resolve_hostnames", D_BOOL, &HSocket::f_bResolveHostnames, 0, OOption::D_REQUIRED, NULL, "Resolve IP address into host names.", NULL },
		{ NULL, D_VOID, NULL, 0, OOption::D_NONE, NULL, NULL, NULL }
	};

/* mathematical macros */
static double const D_EPSILON = 0.000001;
bool eq ( double const & a_dLeft, double const & a_dRight )
	{
	return ( ( ( ( ( a_dLeft ) > ( a_dRight ) )
					? ( ( a_dLeft ) - ( a_dRight ) )
					: ( ( a_dRight ) - ( a_dLeft ) ) ) < D_EPSILON ) );
	}

bool set_hcore_variables ( HString & a_roOption, HString & a_roValue )
	{
	M_PROLOG
	int l_iCtr = 0;
	HString l_oStr;
	if ( ! strcasecmp ( a_roOption, "set_env" ) )
		set_env ( a_roValue );
	else if ( ! strcasecmp ( a_roOption, "log_mask" ) )
		{
		while ( ! ( l_oStr = a_roValue.split ( " \t", l_iCtr ++ ) ).is_empty() )
			{
			if ( ! strcasecmp ( l_oStr, "LOG_DEBUG" ) )
				HLog::f_lLogMask |= LOG_TYPE::D_DEBUG;
			else if ( ! strcasecmp ( l_oStr, "LOG_INFO" ) )
				HLog::f_lLogMask |= LOG_TYPE::D_INFO;
			else if ( ! strcasecmp ( l_oStr, "LOG_NOTICE" ) )
				HLog::f_lLogMask |= LOG_TYPE::D_NOTICE;
			else if ( ! strcasecmp ( l_oStr, "LOG_WARNING" ) )
				HLog::f_lLogMask |= LOG_TYPE::D_WARNING;
			else if ( ! strcasecmp ( l_oStr, "LOG_ERROR" ) )
				HLog::f_lLogMask |= LOG_TYPE::D_ERROR;
			else if ( ! strcasecmp ( l_oStr, "LOG_CVSHEADER" ) )
				HLog::f_lLogMask |= LOG_TYPE::D_CVSHEADER;
			else
				return ( true );
			}
		}
	else
		return ( true );
	return ( false );
	M_EPILOG
	}

void set_env ( char const * const a_pcVarValue )
	{
	M_PROLOG
	char * l_pcPtr = NULL;
	if ( ( strlen ( a_pcVarValue ) < 3 )
			|| ( ( ! ( l_pcPtr = const_cast < char * > ( strpbrk ( a_pcVarValue,
								" \t" ) ) ) ) ) )
		{
		log ( LOG_TYPE::D_ERROR ) << "bad set_env argument: `";
		log << a_pcVarValue << '\'' << endl;
		return;
		}
	* l_pcPtr ++ = 0;
	while ( ( * l_pcPtr == ' ' ) || ( * l_pcPtr == '\t' ) )
		l_pcPtr ++;
	if ( ! ( * l_pcPtr ) )
		{
		log ( LOG_TYPE::D_ERROR ) << "no value for environment variable in set_env: `";
		log << a_pcVarValue << '\'' << endl;
		return;
		}
	int const D_TRUE = 1;
	M_ENSURE ( setenv ( a_pcVarValue, l_pcPtr, D_TRUE ) == 0 );
	return;
	M_EPILOG
	}

bool to_bool( char const* const a_pcValue )
	{
	static HString l_oMessage;
	bool bVal = false;
	if ( ! strcasecmp ( a_pcValue, "yes" ) )
		bVal = true;
	else if ( ! strcasecmp ( a_pcValue, "no" ) )
		bVal = false;
	else if ( ! strcasecmp ( a_pcValue, "true" ) )
		bVal = true;
	else if ( ! strcasecmp ( a_pcValue, "false" ) )
		bVal = false;
	else if ( ! strcasecmp ( a_pcValue, "on" ) )
		bVal = true;
	else if ( ! strcasecmp ( a_pcValue, "off" ) )
		bVal = false;
	else
		{
		l_oMessage = "bad value: ";
		l_oMessage += a_pcValue;
		M_THROW ( l_oMessage, bVal );
		}
	return ( bVal );
	}

int long to_int( char const* const a_pcValue )
	{
	int l_iBase = 10;
	if ( ( ::strlen( a_pcValue ) > 2 ) && ( a_pcValue[ 1 ] == 'x' ) )
		l_iBase = 16;
	return ( ::strtol( a_pcValue, NULL, l_iBase ) );
	}

double long to_double( char const* const a_pcValue )
	{
	return ( ::strtold( a_pcValue, NULL ) );
	}

extern "C"
void hcore_init( void ) __attribute__(( constructor ));
void hcore_init( void )
	{
	char * l_pcEnv = NULL;
	STATIC_ASSERT( sizeof ( int ) >= 4 );
	errno = 0;
	l_pcEnv = ::getenv( "YAAL_DEBUG" );
	if ( l_pcEnv )
		n_iDebugLevel = to_int( l_pcEnv );
	rc_file::process_rc_file( "yaal", "core",
				n_psHCoreVariables, set_hcore_variables );
	return;
	}

void yaal_hcore_banner ( void )
	{
	fprintf ( stdout,
			"%s\n"
			"Available libraries:\n"
			"\thcore\n", COPYRIGHT );
	return;
	}

static char const g_pcDynamicLinkerPath [ ]
	__attribute__(( __section__(".interp") )) = __DYNAMIC_LINKER__;

extern "C"
void yaal_hcore_main ( void ) __attribute__(( __noreturn__ ));
void yaal_hcore_main ( void )
	{
	if ( g_pcDynamicLinkerPath [ 0 ] )
		yaal_hcore_banner();
	exit ( 0 );
	}

}

}

