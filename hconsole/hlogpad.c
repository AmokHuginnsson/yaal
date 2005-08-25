/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hlogpad.c - this file is integral part of `stdhapi' project.

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

#include <ncurses.h>

#include "hcore/hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "hlogpad.h"

using namespace stdhapi::hcore;

namespace stdhapi
{

namespace hconsole
{

HLogPad::HLogLine::HLogLine ( void )
	: f_eType ( D_NONE ), f_iAttribute ( - 1 ), f_oText ( )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HLogPad::HLogLine::~HLogLine ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HLogPad::HLogPad ( HWindow * a_poParent, int a_iRow, int a_iColumn,
		int a_iHeight, int a_iWidth, char const * const a_pcLabel )
	: HControl ( a_poParent, a_iRow, a_iColumn, a_iHeight, a_iWidth, a_pcLabel ),
	f_iLines ( 0 ), f_iOffsetRow ( 0 ), f_iOffsetColumn ( 0 ),
	f_iAttribute ( 0 ), f_oContents ( )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HLogPad::~HLogPad ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

void HLogPad::refresh ( void )
	{
	M_PROLOG
	int l_iCtr = 0, l_iRow = 0, l_iCursor = 0, l_iColumn = 0;
	int l_iBG = f_bFocused ? D_BG_GRAY : D_BG_BLACK;
	HLogLine * l_poLogLine;
	draw_label ( );
	f_oVarTmpBuffer.hs_realloc ( f_iWidthRaw + 1 );
	memset ( static_cast < char * > ( f_oVarTmpBuffer ), ' ', f_iWidthRaw );
	static_cast < char * > ( f_oVarTmpBuffer ) [ f_iWidthRaw ] = 0;
	f_iAttribute = D_FG_BLACK | l_iBG;
	for ( l_iCtr = 0; l_iCtr < f_iHeightRaw; l_iCtr ++ )
		c_printf ( f_iRowRaw + l_iCtr, f_iColumnRaw, f_iAttribute, f_oVarTmpBuffer );
	if ( f_oContents.quantity ( ) )
		{
		l_poLogLine = & f_oContents.go ( 0 );
		l_iCtr = 0;
		while ( l_poLogLine && ( l_iRow < f_iHeightRaw ) )
			{
			if ( l_poLogLine->f_eType == HLogLine::D_ATTRIBUTE )
				f_iAttribute = l_poLogLine->f_iAttribute | l_iBG;
			else
				{
				if ( ( l_iCtr >= f_iOffsetRow ) && ( l_iCursor < f_iWidthRaw ) )
					{
					if ( f_iOffsetColumn > l_iColumn )
						f_oVarTmpBuffer = l_poLogLine->f_oText.mid ( f_iOffsetColumn - l_iColumn );
					else
						f_oVarTmpBuffer = l_poLogLine->f_oText;
					if ( ( l_iCursor + f_oVarTmpBuffer.get_length ( ) ) >= f_iWidthRaw )
						f_oVarTmpBuffer [ f_iWidthRaw - l_iCursor ] = 0;
					if ( f_oVarTmpBuffer [ 0 ] )
						c_printf ( f_iRowRaw + l_iRow, f_iColumnRaw + l_iCursor, f_iAttribute, f_oVarTmpBuffer );
					}
				else
					f_oVarTmpBuffer = "";
				if ( l_poLogLine->f_eType == HLogLine::D_TEXT_EOL )
					{
					l_iCursor = 0;
					l_iColumn = 0;
					if ( l_iCtr >= f_iOffsetRow )
						l_iRow ++;
					l_iCtr ++;
					}
				else
					{
					l_iCursor += f_oVarTmpBuffer.get_length ( );
					l_iColumn += l_poLogLine->f_oText.get_length ( );
					}
				}
			l_poLogLine = f_oContents.to_tail ( 1, D_TREAT_AS_OPENED );
			}
		}
	return;
	M_EPILOG
	}

void HLogPad::add ( int a_iAttribute )
	{
	M_PROLOG
	HLogLine l_oLogLine;
	l_oLogLine.f_eType = HLogLine::D_ATTRIBUTE;
	l_oLogLine.f_iAttribute = a_iAttribute;
	f_oContents.add_tail ( ) = l_oLogLine;
	return;
	M_EPILOG
	}

void HLogPad::add ( char const * const a_pcText )
	{
	M_PROLOG
	int l_iIndexNL = 0, l_iIndexChar = 0;
	HLogLine l_oLogLine;
	HLogLine * l_poLogLine = NULL;
	if ( f_oContents.quantity ( ) )
		l_poLogLine = & f_oContents.tail ( );
	if ( ! l_poLogLine || ( l_poLogLine->f_eType != HLogLine::D_TEXT ) )
		{
		l_poLogLine = & l_oLogLine;
		l_poLogLine->f_eType = HLogLine::D_TEXT;
		l_poLogLine->f_oText = "";
		}
	f_oVarTmpBuffer = a_pcText;
	while ( static_cast < char * > ( f_oVarTmpBuffer ) [ 0 ] )
		{
		l_iIndexNL = f_oVarTmpBuffer.find_one_of ( "\r\n" );
		if ( l_iIndexNL >= 0 )
			{
			l_poLogLine->f_oText += f_oVarTmpBuffer.left ( l_iIndexNL );
			l_poLogLine->f_eType = HLogLine::D_TEXT_EOL;
			f_iLines ++;
			l_iIndexChar = f_oVarTmpBuffer.find_other_than ( "\r\n", l_iIndexNL + 1 );
			if ( l_iIndexChar >= 0 )
				f_oVarTmpBuffer = f_oVarTmpBuffer.mid ( l_iIndexChar );
			else
				f_oVarTmpBuffer = "";
			}
		else
			{
			l_poLogLine->f_oText += f_oVarTmpBuffer;
			f_oVarTmpBuffer = "";
			}
		if ( l_poLogLine == & l_oLogLine )
			f_oContents.add_tail ( l_poLogLine );
		l_poLogLine = & l_oLogLine;
		l_poLogLine->f_eType = HLogLine::D_TEXT;
		l_poLogLine->f_oText = "";
		}
	if ( f_iLines > f_iHeightRaw )
		f_iOffsetRow = f_iLines - f_iHeightRaw;
	n_bNeedRepaint = true;
	refresh ( );
	return;
	M_EPILOG
	}

void HLogPad::add ( int a_iAttribute, char const * const a_pcText )
	{
	M_PROLOG
	add ( a_iAttribute );
	add ( a_pcText );
	return;
	M_EPILOG
	}

int HLogPad::process_input ( int a_iCode )
	{
	M_PROLOG
	int l_iCode = 0;
	switch ( a_iCode )
		{
		case ( KEY_DOWN ):
			{
			if ( f_iLines > ( f_iHeightRaw + f_iOffsetRow ) )
				f_iOffsetRow ++;
			break;
			}
		case ( KEY_UP ):
			{
			if ( f_iOffsetRow > 0 )
				f_iOffsetRow --;
			break;
			}
		case ( KEY_LEFT ):
			{
			if ( f_iOffsetColumn > 0 )
				f_iOffsetColumn --;
			break;
			}
		case ( KEY_RIGHT ):
			{
			f_iOffsetColumn ++;
			if ( f_iOffsetColumn < 0 )
				f_iOffsetColumn = 0;
			break;
			}
		case ( KEY_HOME ):
			{
			f_iOffsetRow = 0;
			f_iOffsetColumn = 0;
			break;
			}
		case ( KEY_END ):
			{
			f_iOffsetColumn = 0;
			if ( f_iLines > f_iHeightRaw )
				f_iOffsetRow = f_iLines - f_iHeightRaw;
			break;
			}
		default :
			{
			l_iCode = a_iCode;
			break;
			}
		}
	if ( ! l_iCode )
		refresh ( );
	return ( l_iCode );
	M_EPILOG
	}

}

}

