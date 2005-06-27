/*
---         `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski           ---

	rc_file.c - this file is integral part of `stdhapi' project.

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

#include <stdlib.h>  /* getenv ( ) */
#include <string.h>  /* strcpy ( ), strcat ( ) */
#include <stdio.h>   /* fopen ( ) */
#include <libintl.h> /* gettext ( ) */

#include "hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "rc_file.h"
#include "xalloc.h"
#include "hlog.h"
#include "hpattern.h"

namespace stdhapi
{

namespace hcore
{

namespace rc_file
{
	
int rc_open ( char const * a_pcRcName, bool a_bLocal, HFile & a_roFile )
	{
	M_PROLOG
	int l_iError = 0;
	char * l_pcHomePath = 0;
	HString l_oRcPath;
	if ( a_roFile )
		a_roFile.close ( );
	if ( a_bLocal )
		{
		l_pcHomePath = getenv( "HOME" );
		if ( ! l_pcHomePath )
			{
			perror ( "rc_open: getenv ( )" );
			abort ( );
			}
		l_oRcPath = l_pcHomePath;
		l_oRcPath += "/.";
		l_oRcPath += a_pcRcName;
		l_oRcPath += "rc";
		}
	else
		{
		l_oRcPath = "/etc/";
		l_oRcPath += a_pcRcName;
		l_oRcPath += "rc";
		}
	l_iError = a_roFile.open ( l_oRcPath );
	if ( l_iError )
		l_oRcPath +=	" not found, ";
	else
		{
		l_oRcPath = "config read from: " + l_oRcPath;
		l_oRcPath += ", ";
		}
	log << l_oRcPath;
	return ( l_iError );
	M_EPILOG
	}

bool substitute_environment ( HString & a_roString )
	{
	int l_iLength = 0;
	char const * l_pcStart = NULL;
	HString l_oName;
	HPattern l_oPattern;
	if ( a_roString )
		{
		M_ENSURE ( l_oPattern.parse_re ( "${[^{}]\\+}" ) == 0 );
		if ( ( l_pcStart = l_oPattern.matches ( a_roString, & l_iLength ) ) )
			{
			l_oName = a_roString.mid ( l_pcStart - static_cast < char * > ( a_roString ), l_iLength );
			l_pcStart = ::getenv ( l_oName.mid ( 2, l_iLength - 3 ) );
			if ( l_pcStart )
				{
				a_roString.replace ( l_oName, l_pcStart );
				return ( true );
				}
			}
		}
	return ( false );
	}

int process_rc_file_internal ( char const * a_pcRcName, char const * a_pcSection,
		OVariable const * a_psVaraibles, int a_iCount,
		bool ( * set_variables ) ( HString &, HString & ) )
	{
	M_PROLOG
	bool l_pbTFTab [ ] = { false, true }, l_bSection = false, l_bOptionOK;
	int l_iCtr = 0, l_iCtrOut = 0, l_iLine = 0;
	HFile l_oRc;
	HString l_oOption, l_oValue, l_oMessage;
	log ( D_LOG_INFO ) << "process_rc_file ( ): ";
	if ( a_iCount < 0 )
		M_THROW ( _ ( "bad variable count" ), a_iCount );
	for ( l_iCtrOut = 0; l_iCtrOut < 2; l_iCtrOut ++ )
		{
		if ( ! rc_open ( a_pcRcName, l_pbTFTab [ l_iCtrOut ], l_oRc ) )
			{
			while ( read_rc_line ( l_oOption, l_oValue, l_oRc, l_iLine ) )
				{
				if ( a_pcSection )
					{
					if ( l_oValue.is_empty ( ) )
						{
						M_IRV ( l_oValue.format ( "[%s]", a_pcSection ) );
						if ( l_oOption == l_oValue )
								{
								if ( n_iDebugLevel )
									fprintf ( stderr, "section: [%s]\n", static_cast < char * > ( l_oOption ) );
								log << "section: " << a_pcSection << ", ";
								l_bSection = true;
								continue;
								}
						else
							l_bSection = false;
						}
					if ( ! l_bSection )
						continue;
					}
				while ( substitute_environment ( l_oValue ) )
					;
				if ( n_iDebugLevel )
					fprintf ( stderr, "option: [%s], value [%s]\n", static_cast < char * > ( l_oOption ),
							static_cast < char * > ( l_oValue ) );
				l_iCtr = 0;
				l_bOptionOK = false;
				while ( ( l_iCtr < a_iCount ) && a_psVaraibles [ l_iCtr ].f_pcKey )
					{
					if ( ! strcasecmp ( l_oOption, a_psVaraibles [ l_iCtr ].f_pcKey ) )
						{
						switch ( a_psVaraibles [ l_iCtr ].f_iType )
							{
							case ( D_BOOL ):
								{
								rc_set_variable ( l_oValue,
										* static_cast < bool * > ( a_psVaraibles [ l_iCtr ].f_pvValue ) );
								break;
								}
							case ( D_CHAR ):
								{
								rc_set_variable ( l_oValue,
										* static_cast < char * > ( a_psVaraibles [ l_iCtr ].f_pvValue ) );
								break;
								}
							case ( D_INT ):
								{
								rc_set_variable ( l_oValue,
										* static_cast < int * > ( a_psVaraibles [ l_iCtr ].f_pvValue ) );
								break;
								}
							case ( D_CHAR_POINTER ):
								{
								rc_set_variable ( l_oValue,
										static_cast < char ** > ( a_psVaraibles [ l_iCtr ].f_pvValue ) );
								break;
								}
							case ( D_HSTRING ):
								{
								( * static_cast < HString * > ( a_psVaraibles [ l_iCtr ].f_pvValue ) ) = l_oValue;
								break;
								}
							default :
								{
								M_THROW ( "unknown type",
										a_psVaraibles [ l_iCtr ].f_iType );
								break;
								}
							}
						l_bOptionOK = true;
						}
					l_iCtr ++;
					}
				if ( set_variables && set_variables ( l_oOption, l_oValue )
						&& ! l_bOptionOK )
					{
					log << "failed." << endl;
					M_IRV ( l_oMessage.format ( "Error: unknown option found: `%s', "
								"with value: `%s', on line %d.\n",
								static_cast < char const * > ( l_oOption ),
								static_cast < char const * > ( l_oValue ), l_iLine ) );
					log ( D_LOG_ERROR ) << l_oMessage;
					fprintf ( stderr, l_oMessage );
					}
				}
			}
		}
	if ( l_oRc )
		l_oRc.close ( );
	log << "done." << endl;
	return ( 0 );
	M_EPILOG
	}

/* Reads one line from a_psFile, stores beginning of line in a_roOption, 
 * stores rest of line in a_pcValue, returns 1 if there are more lines
 * to read and 0 in other case. */

char const n_pcWhiteSpace [ ] = " \t\n\v\f\r";

void strip_comment ( char * a_pcBuffer )
	{
	M_PROLOG
	bool l_bApostrophe = false, l_bQuotation = false;
	int l_iCtr = 0, l_iLenght = strlen ( a_pcBuffer );
	for ( l_iCtr = 0; l_iCtr < l_iLenght; l_iCtr ++ )
		{
		switch ( a_pcBuffer [ l_iCtr ] )
			{
			case ( '\'' ):
				{
				l_bApostrophe = ! l_bApostrophe;
				break;
				}
			case ( '"' ):
				{
				l_bQuotation = ! l_bQuotation;
				break;
				}
			case ( '#' ):
				{
				if ( ! ( l_bQuotation || l_bApostrophe ) )
					{
					a_pcBuffer [ l_iCtr ] = 0;
					return;
					}
				break;
				}
			default:
				{
				break;
				}
			}
		}
	return;
	M_EPILOG
	}

int read_rc_line ( HString & a_roOption, HString & a_roValue, HFile & a_roFile,
		int & a_riLine )
	{
	M_PROLOG
	int l_iIndex = 0, l_iLenght = 0, l_iEnd = 0;
	char * l_pcBuffer = NULL, * l_pcPtr = NULL;
	a_roOption = a_roValue = "";
	while ( a_roFile.read_line ( a_roOption, true ) >= 0 )
		{
		a_riLine ++;
		l_pcBuffer = a_roOption;
		l_iIndex = 0;
		if ( ! l_pcBuffer [ l_iIndex ] )
			continue; /* empty line */
		/* we are looking for first non-whitespace on the line */
		l_iIndex = strspn ( l_pcBuffer, n_pcWhiteSpace );
		if ( ! l_pcBuffer [ l_iIndex ] || ( l_pcBuffer [ l_iIndex ] == '#' ) )
			continue; /* there is only white spaces or comments on that line */
		/* at this point we know we have _some_ option */
		strip_comment ( l_pcBuffer );
		/* strip comment from end of line */
		l_iLenght = strlen ( l_pcBuffer );
		if ( l_iIndex )
			{
			memmove ( l_pcBuffer, l_pcBuffer + l_iIndex, l_iLenght - l_iIndex );
			l_iLenght -= l_iIndex;
			l_pcBuffer [ l_iLenght ] = 0;
			}
		/* now we look for first whitespace after option */
		if ( ( l_pcPtr = strpbrk ( l_pcBuffer, n_pcWhiteSpace ) ) )
			{
			/* we have found a whitespace, so there is probability that */
			/* have a value :-o */
			l_iIndex = l_pcPtr - l_pcBuffer;
			l_pcBuffer [ l_iIndex ++ ] = 0;
			l_iIndex += strspn ( l_pcBuffer + l_iIndex, n_pcWhiteSpace );
			if ( l_pcBuffer [ l_iIndex ] )
				{
				/* we have found a non-whitespace, so there certainly is a value */
				l_iEnd = l_iLenght - strrnspn ( l_pcBuffer, n_pcWhiteSpace, l_iLenght );
				/* now we strip apostrophe or quotation marks */
				if ( ( ( l_pcBuffer [ l_iEnd ] == '\'' )
							|| ( l_pcBuffer [ l_iEnd ] == '"' ) )
						&& ( l_pcBuffer [ l_iEnd ] == l_pcBuffer [ l_iIndex ] ) )
					l_iIndex ++, l_iEnd --;
				l_pcBuffer [ l_iEnd + 1 ] = 0;
				a_roValue = l_pcBuffer + l_iIndex;
				}
			}
		return ( 1 );
		}
	return ( 0 );
	M_EPILOG
	}
	
void rc_set_variable ( char const * a_pcValue, bool & a_rbVariable )
	{
	M_PROLOG
	static HString l_oMessage;
	if ( ! strcasecmp ( a_pcValue, "yes" ) )
		a_rbVariable = true;
	else if ( ! strcasecmp ( a_pcValue, "no" ) )
		a_rbVariable = false;
	else if ( ! strcasecmp ( a_pcValue, "true" ) )
		a_rbVariable = true;
	else if ( ! strcasecmp ( a_pcValue, "false" ) )
		a_rbVariable = false;
	else if ( ! strcasecmp ( a_pcValue, "on" ) )
		a_rbVariable = true;
	else if ( ! strcasecmp ( a_pcValue, "off" ) )
		a_rbVariable = false;
	else
		{
		l_oMessage = "bad value: ";
		l_oMessage += a_pcValue;
		M_THROW ( l_oMessage, a_rbVariable );
		}
	M_EPILOG
	return;
	}

void rc_set_variable ( char const * a_pcValue, char ** a_ppcVariable )
	{
	if ( * a_ppcVariable )
		xfree ( * a_ppcVariable );
	* a_ppcVariable = NULL;
	* a_ppcVariable = xstrdup ( a_pcValue );
	return;
	}

void rc_set_variable ( char const * a_pcValue, int & a_riVariable )
	{
	int l_iBase = 10;
	if ( ( strlen ( a_pcValue ) > 2 ) && ( a_pcValue [ 1 ] == 'x' ) )
		l_iBase = 16;
	a_riVariable = strtol ( a_pcValue, NULL, l_iBase );
	}

void rc_set_variable ( char const * a_pcValue, char & a_rcVariable )
	{
	a_rcVariable = a_pcValue [ 0 ];
	}

}

}

}

