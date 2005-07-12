char const COPYRIGHT [ ] =
"/*\n"
"---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---\n"
"\n"
/* hcore.c - this file is integral part of `stdhapi' project.
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

#include <stdlib.h>
#include <string.h>

#include "hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "hcore.h"
#include "hlog.h"
#include "hstring.h"
#include "rc_file.h"

namespace stdhapi
{

namespace hcore
{

long int n_lLogMask = 0;
int n_iDebugLevel = 0;
HLog log;

OVariable n_psHCoreVariables [ ] =
	{
		{ D_NONE, NULL, NULL }
	};

/* mathematical macros */
#define D_EPSILON	0.000001
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
		while ( ! ( l_oStr = a_roValue.split ( " \t", l_iCtr ++ ) ).is_empty ( ) )
			{
			if ( ! strcasecmp ( l_oStr, "LOG_DEBUG" ) )
				n_lLogMask |= D_LOG_DEBUG;
			else if ( ! strcasecmp ( l_oStr, "LOG_INFO" ) )
				n_lLogMask |= D_LOG_INFO;
			else if ( ! strcasecmp ( l_oStr, "LOG_NOTICE" ) )
				n_lLogMask |= D_LOG_NOTICE;
			else if ( ! strcasecmp ( l_oStr, "LOG_WARNING" ) )
				n_lLogMask |= D_LOG_WARNING;
			else if ( ! strcasecmp ( l_oStr, "LOG_ERROR" ) )
				n_lLogMask |= D_LOG_ERROR;
			else if ( ! strcasecmp ( l_oStr, "LOG_CVSHEADER" ) )
				n_lLogMask |= D_LOG_CVSHEADER;
			else
				return ( true );
			}
		}
	else
		return ( true );
	return ( false );
	M_EPILOG
	}

void set_env ( char const * a_pcVarValue )
	{
	M_PROLOG
	char * l_pcPtr = NULL;
	if ( ( strlen ( a_pcVarValue ) < 3 )
			|| ( ( ! ( l_pcPtr = const_cast < char * > ( strpbrk ( a_pcVarValue, " \t" ) ) ) ) ) )
		{
		log ( D_LOG_ERROR ) << "bad set_env argument: `";
		log << a_pcVarValue << '\'' << endl;
		return;
		}
	* l_pcPtr ++ = 0;
	while ( ( * l_pcPtr == ' ' ) || ( * l_pcPtr == '\t' ) )l_pcPtr ++;
	if ( ! ( * l_pcPtr ) )
		{
		log ( D_LOG_ERROR ) << "no value for environment variable in set_env: `";
		log << a_pcVarValue << '\'' << endl;
		return;
		}
	M_ENSURE ( setenv ( a_pcVarValue, l_pcPtr, true ) == 0 );
	return;
	M_EPILOG
	}

extern "C"
void hcore_init ( void ) __attribute__ ( ( constructor ) );
void hcore_init ( void )
	{
	char * l_pcEnv = NULL;
	g_iErrNo = sizeof ( int );
	if ( g_iErrNo < 4 )
		{
		log << "Your CPU or compiler does not support required size of int.";
		log << endl;
		exit ( g_iErrNo );
		}
	g_iErrNo = 0;
	l_pcEnv = ::getenv ( "STDHAPI_DEBUG" );
	if ( l_pcEnv )
		n_iDebugLevel = strtol ( l_pcEnv, NULL, 10 );
	M_IRV ( rc_file::process_rc_file ( "stdhapi", "core",
				n_psHCoreVariables, set_hcore_variables ) );
	return;
	}

void stdhapi_hcore_banner ( void )
	{
	fprintf ( stderr,
			"%s\n"
			"Available libraries:\n"
			"\thcore\n", COPYRIGHT );
	return;
	}

static char const g_pcDynamicLinkerPath [ ]
	__attribute__(( __section__(".interp") )) = __DYNAMIC_LINKER__;

extern "C"
void stdhapi_hcore_main ( void ) __attribute__(( __noreturn__ ));
void stdhapi_hcore_main ( void )
	{
	if ( g_pcDynamicLinkerPath [ 0 ] )
		stdhapi_hcore_banner ( );
	exit ( 0 );
	}

}

}

