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

#include <stdlib.h> /* getenv ( ) */
#include <string.h> /* strcpy ( ), strcat ( ) */
#include <stdio.h>  /* fopen ( ) */

#include "../config.h"

#include "hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "rc_file.h"
#include "xalloc.h"
#include "hlog.h"

namespace rc_file
{

int process_rc_file ( char const * a_pcRcName, char const * a_pcSection,
		OVariable * a_psVaraibles,
		bool ( * set_variables ) ( HString &, HString & ) )
	{
	M_PROLOG
	bool l_pbTFTab [ ] = { false, true }, l_bSection = false, l_bOptionOK;
	int l_iCtr = 0, l_iCtrOut = 0, l_iLine = 0;
	FILE * l_psRc = 0;
	HString l_oOption, l_oValue, l_oMessage;
	log ( D_LOG_INFO ) << "process_rc_file ( ): ";
	for ( l_iCtrOut = 0; l_iCtrOut < 2; l_iCtrOut ++ )
		{
		l_psRc = rc_open ( a_pcRcName, l_pbTFTab [ l_iCtrOut ], l_psRc );
		if ( l_psRc )while ( read_rc_line ( l_oOption, l_oValue, l_psRc, l_iLine ) )
			{
			if ( g_iDebugLevel )
				fprintf ( stderr, "option: [%s], value [%s]\n", ( char * ) l_oOption,
						( char * ) l_oValue );
			if ( a_pcSection )
				{
				if ( l_oValue.is_empty ( ) )
					{
					l_oValue.format ( "[%s]", a_pcSection );
					if ( l_oOption == l_oValue )
							{
							log << "section: " << a_pcSection << ", ";
							l_bSection = true;
							continue;
							}
					else l_bSection = false;
					}
				if ( ! l_bSection )continue;
				}
			l_iCtr = 0;
			l_bOptionOK = false;
			while ( a_psVaraibles [ l_iCtr ].f_pcKey )
				{
				if ( ! strcasecmp ( l_oOption, a_psVaraibles [ l_iCtr ].f_pcKey ) )
					{
					switch ( a_psVaraibles [ l_iCtr ].f_iType )
						{
						case ( D_TYPE_BOOL ):
							{
							rc_set_variable ( l_oValue,
									* ( bool * ) a_psVaraibles [ l_iCtr ].f_pvValue );
							break;
							}
						case ( D_TYPE_CHAR ):
							{
							rc_set_variable ( l_oValue,
									* ( char * ) a_psVaraibles [ l_iCtr ].f_pvValue );
							break;
							}
						case ( D_TYPE_INT ):
							{
							rc_set_variable ( l_oValue,
									* ( int * ) a_psVaraibles [ l_iCtr ].f_pvValue );
							break;
							}
						case ( D_TYPE_CHAR_POINTER ):
							{
							rc_set_variable ( l_oValue,
									( char ** ) a_psVaraibles [ l_iCtr ].f_pvValue );
							break;
							}
						case ( D_TYPE_HSTRING ):
							{
							( * ( HString * ) a_psVaraibles [ l_iCtr ].f_pvValue ) = l_oValue;
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
				l_oMessage.format ( "Error: unknown option found: `%s', "
						"with value: `%s', on line %d.\n",
						( char const * ) l_oOption, ( char const * ) l_oValue, l_iLine );
				log ( D_LOG_ERROR ) << l_oMessage;
				fprintf ( stderr, l_oMessage );
				}
			}
		}
	if ( l_psRc )rc_close ( l_psRc );
	l_psRc = NULL;
	log << "done." << endl;
	return ( 0 );
	M_EPILOG
	}

/* Reads one line from a_psFile, stores beginning of line in a_roOption, 
 * stores rest of line in a_pcValue, returns 1 if there are more lines
 * to read and 0 in other case. */

int read_rc_line ( HString & a_roOption, HString & a_roValue, FILE * a_psFile,
		int & a_riLine )
	{
	M_PROLOG
	static size_t	l_iBlockSize = 256; /* size of buffer allocated to read line */
	static char * l_pcBuffer = 0; /* buffer for read lines */
	int l_iIndex = 0, l_iLenght = 0, l_iSub = 0;
	char * l_pcPtr = NULL;
#ifndef HAVE_GETLINE /* if we do not have getline we need to simulate its beh */
	int l_iReadLen = 0;
	if ( ! l_pcBuffer )l_iBlockSize = 256;
#endif /* not HAVE_GETLINE */
	if ( ! a_psFile ) /* the file is closed, we can deallocate allocated memory */
		{
		if ( l_pcBuffer )
			{
			xfree ( l_pcBuffer );
			l_pcBuffer = 0;
			l_iBlockSize = 0;
			}
		return ( 0 );
		}
	if ( ! l_pcBuffer )l_pcBuffer = ( char * ) xcalloc ( l_iBlockSize );
	a_roOption = a_roValue = "";
#ifdef HAVE_GETLINE
	while ( getline ( &l_pcBuffer, &l_iBlockSize, a_psFile ) > 0 )
#else /* HAVE_GETLINE */
	while ( ( l_iReadLen = fread ( l_pcBuffer, sizeof ( char ), l_iBlockSize, a_psFile ) ) )
#endif /* not HAVE_GETLINE */
		{
		a_riLine ++;
#ifndef HAVE_GETLINE
		l_pcPtr = ( char * ) memchr ( l_pcBuffer, '\n', l_iReadLen );
		if ( ! l_pcPtr )continue;
		* ++ l_pcPtr = 0;
		fseek ( a_psFile, l_pcPtr - l_pcBuffer - l_iReadLen, SEEK_CUR );
#endif /* not HAVE_GETLINE */
		/* we are looking for first non-whitespace on the line */
		for ( l_iIndex = 0; l_iIndex < ( int ) ( l_iBlockSize - 1 ); l_iIndex++ )
			{
			if ( ( l_pcBuffer [ l_iIndex ] == ' ')
					|| ( l_pcBuffer [ l_iIndex ] == '\t' ) )continue;
			else 
				{
				if ( ( l_pcBuffer [ l_iIndex ] == '#' )
						|| ( l_pcBuffer [ l_iIndex ] == '\r' )
						|| ( l_pcBuffer [ l_iIndex ] == '\n' ) )l_iIndex = -99;
				/* this line is empty or has only a comment */
				break;
				}
			}
		if ( ( l_iIndex > -1 ) && ( l_iIndex < ( int ) ( l_iBlockSize - 1 ) ) )
			{
			/* at this point we know we have _some_ option */
			/* now we look for first whitespace after option */
			l_iLenght = strchr ( l_pcBuffer + l_iIndex, ' ' ) - l_pcBuffer;
			l_iSub = strchr ( l_pcBuffer + l_iIndex, '\t' ) - l_pcBuffer;
			if ( ( l_iLenght < 0 ) && ( l_iSub < 0 ) )
				{
				/* we did not found any whitespace, so we have no value at this line */
				l_iSub = l_iIndex;
				l_iLenght = strlen ( l_pcBuffer ) - 1;
				/* strip comment from end of line */
				if ( ( l_pcPtr = strchr ( l_pcBuffer + l_iSub, '#' ) ) )
					l_iLenght = l_pcPtr - l_pcBuffer - 1;
				for ( l_iIndex = l_iLenght; l_iIndex > l_iSub ; l_iIndex-- )
					if ( ! ( ( l_pcBuffer [ l_iIndex ] == ' ')
							|| ( l_pcBuffer [ l_iIndex ] == '\t' ) 
							|| ( l_pcBuffer [ l_iIndex ] == '\r' )
							|| ( l_pcBuffer [ l_iIndex ] == '\n' ) ) ) break;
				l_pcBuffer [ l_iIndex + 1 ] = 0;
				a_roOption = l_pcBuffer + l_iSub;
				l_pcBuffer [ l_iIndex + 1 ] = ' ';
				}
			else
				{
				/* we have found a whitespace, so there is probability that */
				/* have a value :-o */
				l_iSub = l_iSub > 0 ? l_iSub : ( l_iBlockSize - 1 );
				l_iLenght = l_iLenght > 0 ? l_iLenght : ( l_iBlockSize - 1 );
				l_iSub = l_iSub < l_iLenght ? l_iSub : l_iLenght;
				l_pcBuffer [ l_iSub ] = 0;
				a_roOption = l_pcBuffer + l_iIndex;
				l_pcBuffer [ l_iSub ] = ' ';
				for ( l_iIndex = l_iSub; l_iIndex < ( int ) ( l_iBlockSize - 1 );
						l_iIndex++ )
					{
					if ( ( l_pcBuffer [ l_iIndex ] == ' ')
							|| ( l_pcBuffer [ l_iIndex ] == '\t' ) )continue;
					else 
						{
						if ( ( l_pcBuffer [ l_iIndex ] == '#' )
								|| ( l_pcBuffer [ l_iIndex ] == '\r' )
								|| ( l_pcBuffer [ l_iIndex ] == '\n' ) )l_iIndex = -99;
						break;
						}
					}
				if ( ( l_iIndex > -1 ) && ( l_iIndex < ( int ) ( l_iBlockSize - 1 ) ) )
					{
					/* we have found a non-whitespace, so there certainly is a value */
					l_iSub = l_iIndex;
					l_iLenght = strlen ( l_pcBuffer ) - 1;
					/* strip comment from end of line */
					if ( ( l_pcPtr = strchr ( l_pcBuffer + l_iSub, '#' ) ) )
						l_iLenght = l_pcPtr - l_pcBuffer - 1;
					for ( l_iIndex = l_iLenght; l_iIndex > l_iSub ; l_iIndex-- )
						if ( ! ( ( l_pcBuffer [ l_iIndex ] == ' ')
								|| ( l_pcBuffer [ l_iIndex ] == '\t' ) 
								|| ( l_pcBuffer [ l_iIndex ] == '\r' )
								|| ( l_pcBuffer [ l_iIndex ] == '\n' ) ) ) break;
					/* now we strip apostrophe or quotation marks */
					if ( ( ( l_pcBuffer [ l_iSub ] == '\'' )
								|| ( l_pcBuffer [ l_iSub ] == '"' ) )
							&& ( l_pcBuffer [ l_iSub ] == l_pcBuffer [ l_iIndex ] ) )
						l_iIndex --, l_iSub ++;
					l_pcBuffer [ l_iIndex + 1 ] = 0;
					a_roValue = l_pcBuffer + l_iSub;
					}
				}
			return ( 1 );
			}
		}
	return ( 0 );
	M_EPILOG
	}
	
FILE * rc_open ( char const * a_pcRcName, bool a_bLocal, FILE * a_psFile )
	{
	M_PROLOG
	char * l_pcHomePath = 0;
	FILE * l_psRc = 0;
	HString l_oRcPath;
	if ( a_psFile )fclose ( a_psFile );
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
	l_psRc = fopen ( l_oRcPath, "r" );
	if ( ! l_psRc )l_oRcPath +=	" not found, ";
	else
		{
		l_oRcPath = "config read from: " + l_oRcPath;
		l_oRcPath += ", ";
		}
	log << l_oRcPath;
	return ( l_psRc );
	M_EPILOG
	}
	
void rc_close ( FILE * a_psRc )
	{
	M_PROLOG
	int l_iDummy = 0;
	HString l_oTmp;
	read_rc_line ( l_oTmp, l_oTmp, 0, l_iDummy );
	if ( a_psRc )fclose ( a_psRc );
	return;
	M_EPILOG
	}

void rc_set_variable ( char const * a_pcValue, bool & a_rbVariable )
	{
	M_PROLOG
	static HString l_oMessage;
	if ( ! strcasecmp ( a_pcValue, "yes" ) )a_rbVariable = true;
	else if ( ! strcasecmp ( a_pcValue, "no" ) )a_rbVariable = false;
	else if ( ! strcasecmp ( a_pcValue, "true" ) )a_rbVariable = true;
	else if ( ! strcasecmp ( a_pcValue, "false" ) )a_rbVariable = false;
	else if ( ! strcasecmp ( a_pcValue, "on" ) )a_rbVariable = true;
	else if ( ! strcasecmp ( a_pcValue, "off" ) )a_rbVariable = false;
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
	if ( * a_ppcVariable )xfree ( * a_ppcVariable );
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

