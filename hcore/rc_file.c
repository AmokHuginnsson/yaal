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

#include "rc_file.h"

#include "xalloc.h"
#include "hexception.h"
#include "hlog.h"

namespace rc_file
{

int process_rc_file ( const char * a_pcRcName, const char * a_pcSection,
		OVariable * a_psVaraibles,
		bool ( * set_variables ) ( HString &, HString & ) )
	{
	bool l_pbTFTab [ ] = { false, true }, l_bSection = false, l_bOptionOK;
	int l_iCtr = 0, l_iCtrOut = 0, l_iLine = 0;
	FILE * l_psRc = 0;
	HString l_oOption, l_oValue;
	log << "process_rc_file ( ): ";
	for ( l_iCtrOut = 0; l_iCtrOut < 2; l_iCtrOut ++ )
		{
		l_psRc = rc_open ( a_pcRcName, l_pbTFTab [ l_iCtrOut ], l_psRc );
		if ( a_pcSection )::log << "section: " << a_pcSection;
		if ( l_psRc )while ( read_rc_line ( l_oOption, l_oValue, l_psRc, l_iLine ) )
			{
			if ( a_pcSection )
				{
				if ( l_oValue.is_empty ( ) )
					{
					l_oValue.format ( "[%s]", a_pcSection );
					if ( ( l_oOption == l_oValue ) && ( l_bSection = true ) )continue;
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
							rc_set_variable ( l_oValue, * ( bool * ) a_psVaraibles [ l_iCtr ].f_pvValue );
							break;
							}
						case ( D_TYPE_CHAR ):
							{
							rc_set_variable ( l_oValue, * ( char * ) a_psVaraibles [ l_iCtr ].f_pvValue );
							break;
							}
						case ( D_TYPE_INT ):
							{
							rc_set_variable ( l_oValue, * ( int * ) a_psVaraibles [ l_iCtr ].f_pvValue );
							break;
							}
						case ( D_TYPE_CHAR_POINTER ):
							{
							rc_set_variable ( l_oValue, ( char ** ) a_psVaraibles [ l_iCtr ].f_pvValue );
							break;
							}
						default :
							{
							throw new HException ( __WHERE__, "unknown type", a_psVaraibles [ l_iCtr ].f_iType );
							break;
							}
						}
					l_bOptionOK = true;
					}
				l_iCtr ++;
				}
			if ( set_variables )
				if ( set_variables ( l_oOption, l_oValue ) && ! l_bOptionOK )
					{
					::log << "failed." << endl;
					::log << "Error: unknown option found: `" << l_oOption;
					::log << "', with value: `" << l_oValue << "'." << endl;
					throw new HException ( __WHERE__, "unknown option", l_iLine );
					}
			}
		}
	if ( l_psRc )rc_close ( l_psRc );
	l_psRc = NULL;
	log << "done." << endl;
	return ( 0 );
	}

/* Reads one line from a_psFile, stores beginning of line in a_roOption, 
 * stores rest of line in a_pcValue, returns 1 if there are more lines
 * to read and 0 in other case. */

int read_rc_line ( HString & a_roOption, HString & a_roValue, FILE * a_psFile,
		int & a_riLine )
	{
	M_PROLOG
	static size_t	l_iBlockSize = 256;
	static char * l_pcBuffer = 0;
	int l_iIndex, l_iLenght, l_iSub;
#ifdef __HOST_OS_TYPE_FREEBSD__
	int l_iReadLen = 0;
	char * l_pcPtr = NULL;
	if ( ! l_pcBuffer )l_iBlockSize = 256;
#endif /* __HOST_OS_TYPE_FREEBSD__ */
	if ( ! a_psFile )
		{
		if ( l_pcBuffer )
			{
			xfree ( ( void * ) l_pcBuffer );
			l_pcBuffer = 0;
			l_iBlockSize = 0;
			}
		return ( 0 );
		}
	if ( ! l_pcBuffer )l_pcBuffer = ( char * ) xcalloc ( l_iBlockSize );
	a_roOption = a_roValue = "";
#ifdef __HOST_OS_TYPE_FREEBSD__
	while ( ( l_iReadLen = fread ( l_pcBuffer, sizeof ( char ), l_iBlockSize, a_psFile ) ) )
#else /* __HOST_OS_TYPE_FREEBSD__ */
	while ( getline ( &l_pcBuffer, &l_iBlockSize, a_psFile ) > 0 )
#endif /* not __HOST_OS_TYPE_FREEBSD__ */
		{
		a_riLine ++;
#ifdef __HOST_OS_TYPE_FREEBSD__
		l_pcPtr = ( char * ) memchr ( l_pcBuffer, '\n', l_iReadLen );
		if ( ! l_pcPtr )continue;
		* ++ l_pcPtr = 0;
		fseek ( a_psFile, l_pcPtr - l_pcBuffer - l_iReadLen, SEEK_CUR );
#endif /* __HOST_OS_TYPE_FREEBSD__ */
		for ( l_iIndex = 0; l_iIndex < ( int ) ( l_iBlockSize - 1 ); l_iIndex++ )
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
			l_iLenght = strchr ( l_pcBuffer + l_iIndex, ' ' ) - l_pcBuffer;
			l_iSub = strchr ( l_pcBuffer + l_iIndex, '\t' ) - l_pcBuffer;
			if ( ( l_iLenght < 0 ) && ( l_iSub < 0 ) )
				{
				l_iSub = l_iIndex;
				l_iLenght = strlen ( l_pcBuffer ) - 1;
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
				l_iSub = l_iSub > 0 ? l_iSub : ( l_iBlockSize - 1 );
				l_iLenght = l_iLenght > 0 ? l_iLenght : ( l_iBlockSize - 1 );
				l_iSub = l_iSub < l_iLenght ? l_iSub : l_iLenght;
				l_pcBuffer [ l_iSub ] = 0;
				a_roOption = l_pcBuffer + l_iIndex;
				l_pcBuffer [ l_iSub ] = ' ';
				for ( l_iIndex = l_iSub; l_iIndex < ( int ) ( l_iBlockSize - 1 ); l_iIndex++ )
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
					l_iSub = l_iIndex;
					l_iLenght = strlen ( l_pcBuffer ) - 1;
					for ( l_iIndex = l_iLenght; l_iIndex > l_iSub ; l_iIndex-- )
						if ( ! ( ( l_pcBuffer [ l_iIndex ] == ' ')
								|| ( l_pcBuffer [ l_iIndex ] == '\t' ) 
								|| ( l_pcBuffer [ l_iIndex ] == '\r' )
								|| ( l_pcBuffer [ l_iIndex ] == '\n' ) ) ) break;
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
	
FILE * rc_open ( const char * a_pcRcName, bool a_bLocal, FILE * a_psFile )
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
	::log << l_oRcPath;
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

void rc_set_variable ( const char * a_pcValue, bool & a_rbVariable )
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
		throw new HException ( __WHERE__, ( char * ) l_oMessage, a_rbVariable );
		}
	M_EPILOG
	return;
	}

void rc_set_variable ( const char * a_pcValue, char ** a_ppcVariable )
	{
	if ( * a_ppcVariable )xfree ( ( void * ) * a_ppcVariable );
	* a_ppcVariable = NULL;
	* a_ppcVariable = xstrdup ( a_pcValue );
	return;
	}

void rc_set_variable ( const char * a_pcValue, int & a_riVariable )
	{
	a_riVariable = atoi ( a_pcValue );
	}

void rc_set_variable ( const char * a_pcValue, char & a_rcVariable )
	{
	a_rcVariable = a_pcValue [ 0 ];
	}

}

