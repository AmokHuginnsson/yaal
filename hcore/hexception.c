/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hexception.c - this file is integral part of `stdhapi' project.

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

#include <string.h>
#include <stdio.h>

#ifdef _STDIO_H
#include <execinfo.h>
#endif /* _STDIO_H */

#include "hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "xalloc.h"
#include "hlog.h"

HException::HException ( const char * a_pcFileName,
												 const char * a_pcFunctionName,
												 int a_iLine, const char * a_pcMessage, int a_iCode )
	{
	f_cChar = 0;
	f_iInt = 0;
	f_lLong = 0;
	f_dDouble = 0;
	f_pcCharPtr = 0;
	f_pvVoidPtr = 0;
	f_iFrame = 0;
	f_iCode = a_iCode;
	f_pcMessage = 0;
	f_pcMessage = xstrdup ( a_pcMessage );
	f_pcFileName = 0;
	f_pcFileName = xstrdup ( a_pcFileName );
	f_pcFunctionName = 0;
	f_pcFunctionName = xstrdup ( a_pcFunctionName );
	core::log << "Exception: " << f_pcMessage << ", code: " << f_iCode;
	core::log << '.' << endl;
	log ( a_pcFileName, a_pcFunctionName, a_iLine );
	return;
	}

HException::~HException ( void )
	{
	core::log ( "Exception registers: c:0x%02x i:%d l:%ld d:%f pv:%p pc:%s\n",
			f_cChar, f_iInt, f_lLong, f_dDouble, f_pvVoidPtr, f_pcCharPtr );
	if ( f_pcCharPtr )xfree ( f_pcCharPtr );
	if ( f_pcFunctionName )xfree ( f_pcFunctionName );
	if ( f_pcFileName )xfree ( f_pcFileName );
	if ( f_pcMessage )xfree ( f_pcMessage );
	return;
	}

void HException::set ( char a_cChar, int a_iInt, long a_lLong, double a_dDouble,
											char * a_pcStr, void * a_pvVoidPtr )
	{
	f_cChar = a_cChar;
	f_iInt = a_iInt;
	f_lLong = a_lLong;
	f_dDouble = a_dDouble;
	if ( a_pcStr )f_pcCharPtr = xstrdup ( a_pcStr );
	f_pvVoidPtr = a_pvVoidPtr;
	return;
	}

void HException::print_error ( bool a_bFull )
	{
	fprintf ( stderr, "\nException: %s, %d.\n", f_pcMessage, f_iCode );
	if ( a_bFull )fprintf ( stderr,
			"Exception registers:\nc:0x%02x\ti:%d\tl:%ld\td:%f\tpv:%p\npc:%s\n",
			f_cChar, f_iInt, f_lLong, f_dDouble, f_pvVoidPtr, f_pcCharPtr );
	return;
	}

void HException::log ( const char * a_pcFileName,
											 const char * a_pcFunctionName, int a_iLine )
	{
	int l_iLength = strlen ( a_pcFileName );
	if ( f_iFrame && ! ( strcmp ( f_pcFileName, a_pcFileName ) || strcmp ( f_pcFunctionName, a_pcFunctionName ) ) )return;
	core::log ( "Exception frame %2d: %16s : %4d : %s\n", f_iFrame ++,
			a_pcFileName + ( l_iLength > 16 ? l_iLength - 16 : 0 ),
			a_iLine, a_pcFunctionName );
	return;
	}

#ifdef _EXECINFO_H
void HException::dump_call_stack ( int a_iLevel )
#else /* _EXECINFO_H */
void HException::dump_call_stack ( int )
#endif /* not _EXECINFO_H */
	{
#ifdef _EXECINFO_H
	int l_iCtr = 0, l_iSize = 0;
	char l_pcBuffer [ 4000 ];
	char ** l_ppcStrings = NULL;
	void ** l_ppvArray = ( void ** ) & l_pcBuffer;

	l_iSize = backtrace ( l_ppvArray, 1000 );
	l_ppcStrings = backtrace_symbols  ( l_ppvArray, l_iSize );

	core::log << "Obtained " << ( int ) l_iSize << " stack frames." << endl;
	if ( a_iLevel < l_iSize )l_iSize = a_iLevel;
	for  ( l_iCtr = 0; l_iCtr < l_iSize; l_iCtr ++ )
		core::log << l_ppcStrings [ l_iCtr ] << endl;

	xfree ( l_ppcStrings );
#endif /* _EXECINFO_H */
	return;
	}

