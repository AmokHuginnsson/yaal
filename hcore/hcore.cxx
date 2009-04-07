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

#include <cstdlib>
#include <cstring>

#include "config.hxx"
#if defined( HAVE_EXECINFO_H )
#include <execinfo.h>
#endif /* HAVE_EXECINFO_H */
#include <cxxabi.h>

#include "base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "hcore.hxx"
#include "hlog.hxx"
#include "hstring.hxx"
#include "hprogramoptionshandler.hxx"
#include "hsingleton.hxx"
#include "hopenssl.hxx"
#include "hsocket.hxx"

namespace yaal
{

namespace hcore
{

int n_iDebugLevel = 0;
typedef HSingleton<HLog> HLogService;
HLog& log = HLogService::get_instance( 1000 );

HProgramOptionsHandler yaalOptions;

/* mathematical macros */
static double long const D_EPSILON = 0.000001;
bool eq( double long const& a_dLeft, double long const& a_dRight )
	{
	return ( ( ( ( ( a_dLeft ) > ( a_dRight ) )
					? ( ( a_dLeft ) - ( a_dRight ) )
					: ( ( a_dRight ) - ( a_dLeft ) ) ) < D_EPSILON ) );
	}

bool set_hcore_variables( HString& a_roOption, HString& a_roValue )
	{
	M_PROLOG
	int l_iCtr = 0;
	HString l_oStr;
	if ( ! strcasecmp( a_roOption, "set_env" ) )
		set_env( a_roValue );
	else if ( ! strcasecmp( a_roOption, "log_mask" ) )
		{
		while ( ! ( l_oStr = a_roValue.split ( " \t", l_iCtr ++ ) ).is_empty() )
			{
			if ( ! strcasecmp( l_oStr, "LOG_DEBUG" ) )
				HLog::f_lLogMask |= LOG_TYPE::D_DEBUG;
			else if ( ! strcasecmp( l_oStr, "LOG_INFO" ) )
				HLog::f_lLogMask |= LOG_TYPE::D_INFO;
			else if ( ! strcasecmp( l_oStr, "LOG_NOTICE" ) )
				HLog::f_lLogMask |= LOG_TYPE::D_NOTICE;
			else if ( ! strcasecmp( l_oStr, "LOG_WARNING" ) )
				HLog::f_lLogMask |= LOG_TYPE::D_WARNING;
			else if ( ! strcasecmp( l_oStr, "LOG_ERROR" ) )
				HLog::f_lLogMask |= LOG_TYPE::D_ERROR;
			else if ( ! strcasecmp( l_oStr, "LOG_CVSHEADER" ) )
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

void set_env( HString line )
	{
	M_PROLOG
	int long eon = 0;
	if ( ( line.length() < 3 )
			|| ( ( eon = line.find_one_of( " \t" ) ) == -1 ) )
		{
		log ( LOG_TYPE::D_ERROR ) << "bad set_env argument: `";
		log << line << '\'' << endl;
		return;
		}
	int long valPos = line.find_other_than( " \t", eon );
	if ( valPos < 0 )
		{
		log ( LOG_TYPE::D_ERROR ) << "no value for environment variable in set_env: `";
		log << line << '\'' << endl;
		return;
		}
	int const D_TRUE = 1;
	HString val = line.mid( valPos );
	line.set_at( eon, '\0' );
	M_ENSURE( ::setenv( line.raw(), val.raw(), D_TRUE ) == 0 );
	return;
	M_EPILOG
	}

#ifdef _EXECINFO_H
execution_info::strings_ptr_t execution_info::get_call_stack( int const& a_iLevel )
#else /* _EXECINFO_H */
execution_info::strings_ptr_t execution_info::get_call_stack( int const& )
#endif /* not _EXECINFO_H */
	{
	strings_ptr_t frames( new strings_t );
#ifdef _EXECINFO_H
	int l_iCtr = 0, l_iSize = 0;
	char** l_ppcStrings = NULL;
	void** l_ppvPointer =	NULL;

	l_ppvPointer = xcalloc<void*>( a_iLevel + 1 );
	l_iSize = backtrace( l_ppvPointer, a_iLevel );
	l_ppcStrings = backtrace_symbols( l_ppvPointer, l_iSize );

	if ( a_iLevel < l_iSize )
		l_iSize = a_iLevel;
	char* ptr = NULL;
	char* end = NULL;
	int status = 0;
	for ( l_iCtr = 0; l_iCtr < l_iSize; l_iCtr ++ )
		{
		ptr = strchr( l_ppcStrings[ l_iCtr ], '(' );
		if ( ptr )
			{
			end = strchr( ptr, '+' );
			if ( end )
				(*end) = 0;
			ptr = abi::__cxa_demangle( ptr + 1, 0, 0, &status );
			if ( ptr )
				{
				frames->push_back( ptr );
				xfree( ptr );
				}
			}
		}

	xfree( l_ppcStrings );
	xfree( l_ppvPointer );
#endif /* _EXECINFO_H */
	return ( frames );
	}

void dump_call_stack( int const& no )
	{
	execution_info::strings_ptr_t frames = execution_info::get_call_stack( no );
	hcore::log << "Obtained " << frames->get_size() << " stack frames." << endl;
	yaal::copy( frames->begin(), frames->end(), stream_iterator( hcore::log, hcore::endl ) );
	}

class HCoreInitDeinit
	{
public:
	HCoreInitDeinit( void );
	} initDeinit;

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
	char* l_pcEnv = ::getenv( "YAAL_DEBUG" );
	if ( l_pcEnv )
		n_iDebugLevel = lexical_cast<int>( l_pcEnv );
	yaalOptions( "ssl_key", program_options_helper::option_value( HOpenSSL::f_oSSLKey ), 0, HProgramOptionsHandler::OOption::TYPE::D_REQUIRED, NULL, "Path to the OpenSSL private key file." )
		( "ssl_cert", program_options_helper::option_value( HOpenSSL::f_oSSLCert ), 0, HProgramOptionsHandler::OOption::TYPE::D_REQUIRED, NULL, "Path to the OpenSSL certificate file." )
		( "resolve_hostnames", program_options_helper::option_value( HSocket::f_bResolveHostnames ), 0, HProgramOptionsHandler::OOption::TYPE::D_REQUIRED, NULL, "Resolve IP address into host names." );
	yaalOptions.process_rc_file( "yaal", "core", set_hcore_variables );
	return;
	}

#if defined( __DYNAMIC_LINKER__ )
void yaal_hcore_banner( void )
	{
	fprintf( stdout,
			"%s\n"
			"Available libraries:\n"
			"\thcore\n", COPYRIGHT );
	return;
	}

static char const g_pcDynamicLinkerPath [ ]
	__attribute__(( __section__(".interp") )) = __DYNAMIC_LINKER__;

extern "C"
int yaal_hcore_main( int, char** )
	{
	if ( g_pcDynamicLinkerPath[ 0 ] )
		{
		yaal_hcore_banner();
		::exit( 0 );
		}
	return ( 0 );
	}
#endif /* __DYNAMIC_LINKER__ */

}

}

