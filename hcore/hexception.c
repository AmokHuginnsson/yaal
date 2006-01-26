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
#include <stdlib.h>

#ifdef _STDIO_H
#include <execinfo.h>
#endif /* _STDIO_H */

#include "hexception.h"
M_CVSID ( "$CVSHeader$" )
#include "xalloc.h"
#include "hlog.h"

namespace stdhapi
{

namespace hcore
{

HException::HException ( char const * const a_pcFileName,
												 char const * const a_pcFunctionName,
												 int const a_iLine, char const * const a_pcMessage,
												 int const a_iCode )
	: f_bLocal ( false ), f_cChar ( 0 ), f_iInt ( 0 ), f_lLong ( 0 ),
	f_dDouble ( 0 ), f_pcCharPtr ( NULL ), f_pvVoidPtr ( NULL ), f_iFrame ( 0 ),
	f_pcFileName ( NULL ), f_pcFunctionName ( NULL ),
	f_iCode ( a_iCode ), f_pcMessage ( NULL )
	{
	f_pcMessage = xstrdup ( a_pcMessage );
	f_pcFileName = xstrdup ( a_pcFileName );
	f_pcFunctionName = xstrdup ( a_pcFunctionName );
	hcore::log << "Exception: " << f_pcMessage << ", code: " << f_iCode;
	hcore::log << '.' << endl;
	log ( a_pcFileName, a_pcFunctionName, a_iLine );
	return;
	}

HException::HException ( HException const & a_roException )
	: f_bLocal ( false ), f_cChar ( a_roException.f_cChar ),
	f_iInt ( a_roException.f_iInt ), f_lLong ( a_roException.f_lLong ),
	f_dDouble ( a_roException.f_dDouble ), f_pcCharPtr ( NULL ),
	f_pvVoidPtr ( a_roException.f_pvVoidPtr ),
	f_iFrame ( a_roException.f_iFrame ), f_pcFileName ( NULL ),
	f_pcFunctionName ( NULL ), f_iCode ( a_roException.f_iCode ),
	f_pcMessage ( NULL )
	{
	f_pcCharPtr = xstrdup ( a_roException.f_pcCharPtr );
	f_pcMessage = xstrdup ( a_roException.f_pcMessage );
	f_pcFileName = xstrdup ( a_roException.f_pcFileName );
	f_pcFunctionName = xstrdup ( a_roException.f_pcFunctionName );
	a_roException.f_bLocal = true;
	return;
	}

HException::~HException ( void )
	{
	try
		{
		if ( ! f_bLocal )
			hcore::log (
					"Exception registers: c:0x%02x i:%d l:%ld d:%f pv:%p pc:%s\n",
					f_cChar, f_iInt, f_lLong, f_dDouble, f_pvVoidPtr, f_pcCharPtr );
		}
	catch ( ... )
		{
		}
	if ( f_pcCharPtr )
		xfree ( f_pcCharPtr );
	if ( f_pcFunctionName )
		xfree ( f_pcFunctionName );
	if ( f_pcFileName )
		xfree ( f_pcFileName );
	if ( f_pcMessage )
		xfree ( f_pcMessage );
	return;
	}

void HException::set ( char const a_cChar, int const a_iInt,
		long const a_lLong, double const a_dDouble, char const * const a_pcStr,
		void * const a_pvVoidPtr )
	{
	f_cChar = a_cChar;
	f_iInt = a_iInt;
	f_lLong = a_lLong;
	f_dDouble = a_dDouble;
	if ( f_pcCharPtr )
		xfree ( f_pcCharPtr );
	if ( a_pcStr )
		f_pcCharPtr = xstrdup ( a_pcStr );
	f_pvVoidPtr = a_pvVoidPtr;
	return;
	}

void HException::set ( char const * const a_pcStr )
	{
	if ( f_pcCharPtr )
		xfree ( f_pcCharPtr );
	if ( a_pcStr )
		f_pcCharPtr = xstrdup ( a_pcStr );
	return;
	}

void HException::print_error ( bool const a_bFull ) const
	{
	fprintf ( stderr, "\nException: %s, %d.\n", f_pcMessage, f_iCode );
	if ( a_bFull )
		fprintf ( stderr,
				"Exception registers:\nc:0x%02x\ti:%d\tl:%ld\td:%f\tpv:%p\npc:%s\n",
				f_cChar, f_iInt, f_lLong, f_dDouble, f_pvVoidPtr, f_pcCharPtr );
	return;
	}

void HException::log ( char const * const a_pcFileName,
											 char const * const a_pcFunctionName, int const a_iLine )
	{
	size_t l_uiLength = strlen ( a_pcFileName );
	if ( f_iFrame
			&& ! ( strcmp ( f_pcFileName, a_pcFileName )
				|| strcmp ( f_pcFunctionName, a_pcFunctionName ) ) )
		return;
	hcore::log ( "Exception frame %2d: %16s : %4d : %s\n", f_iFrame ++,
			a_pcFileName + ( l_uiLength > 16 ? l_uiLength - 16 : 0 ),
			a_iLine, a_pcFunctionName );
	return;
	}

char const * HException::what ( void ) const
	{
	return ( f_pcMessage );
	}

#ifdef _EXECINFO_H
void HException::dump_call_stack ( int const a_iLevel )
#else /* _EXECINFO_H */
void HException::dump_call_stack ( int const )
#endif /* not _EXECINFO_H */
	{
#ifdef _EXECINFO_H
	int l_iCtr = 0, l_iSize = 0;
	char * l_pcBuffer = NULL;
	char * * l_ppcStrings = NULL;
	void * * l_ppvPointer =	reinterpret_cast < void * * > ( & l_pcBuffer );

	l_pcBuffer = xcalloc < char > ( 4000 );
	l_iSize = backtrace ( l_ppvPointer, 1000 );
	l_ppcStrings = backtrace_symbols  ( l_ppvPointer, l_iSize );

	hcore::log << "Obtained " << l_iSize << " stack frames." << endl;
	if ( a_iLevel < l_iSize )
		l_iSize = a_iLevel;
	for  ( l_iCtr = 0; l_iCtr < l_iSize; l_iCtr ++ )
		hcore::log << l_ppcStrings [ l_iCtr ] << endl;

	xfree ( l_ppcStrings );
	xfree ( l_pcBuffer );
#endif /* _EXECINFO_H */
	return;
	}

HException * HException::operator-> ( void )
	{
	return ( this );
	}

void HException::failed_assert ( char const * const a_pcFileName,
		char const * const a_pcFunctionName, int const a_iLine,
		char const * const a_pcMessage )
	{
	M_PROLOG
	fprintf ( stderr, "Failed assertion: `%s' at: %s: %4d: %s\n",
			a_pcMessage, a_pcFileName, a_iLine, a_pcFunctionName );
	if ( ! g_iErrNo )
		g_iErrNo ++;
	exit ( g_iErrNo );
	M_EPILOG
	}

}

}

