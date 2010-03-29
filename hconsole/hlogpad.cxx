/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hlogpad.cxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hlogpad.hxx"

using namespace yaal::hcore;

namespace yaal
{

namespace hconsole
{

HLogPad::HLogLine::HLogLine ( void )
	: f_eType ( NONE ), f_iAttribute ( COLORS::ATTR_NORMAL ), f_oText()
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
	f_iAttribute ( 0 ), f_oContents()
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

void HLogPad::do_refresh ( void )
	{
	M_PROLOG
	int l_iCtr = 0, l_iRow = 0, l_iCursor = 0, l_iColumn = 0;
	int l_iBG = f_bFocused ? COLORS::BG_GRAY : COLORS::BG_BLACK;
	HConsole& cons = HCons::get_instance();
	draw_label();
	f_oVarTmpBuffer.hs_realloc( f_iWidthRaw + 1 );
	f_oVarTmpBuffer.fillz( ' ', 0, f_iWidthRaw );
	f_iAttribute = COLORS::ATTR_NORMAL | l_iBG;
	for ( l_iCtr = 0; l_iCtr < f_iHeightRaw; l_iCtr ++ )
		cons.c_cmvprintf( f_iRowRaw + l_iCtr,
				f_iColumnRaw, f_iAttribute, f_oVarTmpBuffer.raw() );
	if ( f_oContents.size() )
		{
		l_iCtr = 0;
		for ( contents_t::iterator it = f_oContents.begin(); ( it != f_oContents.end() ) && ( l_iRow < f_iHeightRaw ); ++ it )
			{
			if ( it->f_eType == HLogLine::ATTRIBUTE )
				f_iAttribute = it->f_iAttribute | l_iBG;
			else
				{
				if ( ( l_iCtr >= f_iOffsetRow ) && ( l_iCursor < f_iWidthRaw ) )
					{
					if ( f_iOffsetColumn > l_iColumn )
						f_oVarTmpBuffer = it->f_oText.mid ( f_iOffsetColumn - l_iColumn );
					else
						f_oVarTmpBuffer = it->f_oText;
					if ( ( l_iCursor + f_oVarTmpBuffer.get_length() ) >= f_iWidthRaw )
						f_oVarTmpBuffer.set_at( f_iWidthRaw - l_iCursor, 0 );
					if ( f_oVarTmpBuffer[ 0 ] )
						cons.c_cmvprintf( f_iRowRaw + l_iRow,
								f_iColumnRaw + l_iCursor, f_iAttribute, f_oVarTmpBuffer.raw() );
					}
				else
					f_oVarTmpBuffer = "";
				if ( it->f_eType == HLogLine::TEXT_EOL )
					{
					l_iCursor = 0;
					l_iColumn = 0;
					if ( l_iCtr >= f_iOffsetRow )
						l_iRow ++;
					l_iCtr ++;
					}
				else
					{
					l_iCursor += static_cast<int>( f_oVarTmpBuffer.get_length() );
					l_iColumn += static_cast<int>( it->f_oText.get_length() );
					}
				}
			}
		}
	return;
	M_EPILOG
	}

void HLogPad::add ( int a_iAttribute )
	{
	M_PROLOG
	HLogLine l_oLogLine;
	l_oLogLine.f_eType = HLogLine::ATTRIBUTE;
	l_oLogLine.f_iAttribute = a_iAttribute;
	f_oContents.push_back ( l_oLogLine );
	return;
	M_EPILOG
	}

void HLogPad::add ( yaal::hcore::HString const& a_oText )
	{
	M_PROLOG
	int l_iIndexNL = 0, l_iIndexChar = 0;
	HLogLine l_oLogLine;
	HLogLine* it = NULL;
	if ( f_oContents.size() )
		it = &f_oContents.tail();
	if ( ! it || ( it->f_eType != HLogLine::TEXT ) )
		{
		it = &l_oLogLine;
		it->f_eType = HLogLine::TEXT;
		it->f_oText = "";
		}
	f_oVarTmpBuffer = a_oText;
	while ( ! f_oVarTmpBuffer.is_empty() )
		{
		l_iIndexNL = static_cast<int>( f_oVarTmpBuffer.find_one_of( "\r\n" ) );
		if ( l_iIndexNL >= 0 )
			{
			it->f_oText += f_oVarTmpBuffer.left( l_iIndexNL );
			it->f_eType = HLogLine::TEXT_EOL;
			f_iLines ++;
			l_iIndexChar = static_cast<int>( f_oVarTmpBuffer.find_other_than( "\r\n", l_iIndexNL + 1 ) );
			if ( l_iIndexChar >= 0 )
				f_oVarTmpBuffer = f_oVarTmpBuffer.mid( l_iIndexChar );
			else
				f_oVarTmpBuffer = "";
			}
		else
			{
			it->f_oText += f_oVarTmpBuffer;
			f_oVarTmpBuffer = "";
			}
		if ( it == &l_oLogLine )
			f_oContents.push_back( *it );
		it = &l_oLogLine;
		it->f_eType = HLogLine::TEXT;
		it->f_oText = "";
		}
	if ( f_iLines > f_iHeightRaw )
		f_iOffsetRow = f_iLines - f_iHeightRaw;
	schedule_refresh();
	return;
	M_EPILOG
	}

void HLogPad::add ( int a_iAttribute, yaal::hcore::HString const& a_oText )
	{
	M_PROLOG
	add ( a_iAttribute );
	add ( a_oText );
	return;
	M_EPILOG
	}

int HLogPad::do_process_input ( int a_iCode )
	{
	M_PROLOG
	int l_iCode = 0;
	switch ( a_iCode )
		{
		case ( KEY_CODES::DOWN ):
			if ( f_iLines > ( f_iHeightRaw + f_iOffsetRow ) )
				f_iOffsetRow ++;
		break;
		case ( KEY_CODES::UP ):
			if ( f_iOffsetRow > 0 )
				f_iOffsetRow --;
		break;
		case ( KEY_CODES::LEFT ):
			if ( f_iOffsetColumn > 0 )
				f_iOffsetColumn --;
		break;
		case ( KEY_CODES::RIGHT ):
			f_iOffsetColumn ++;
			if ( f_iOffsetColumn < 0 )
				f_iOffsetColumn = 0;
		break;
		case ( KEY_CODES::HOME ):
			f_iOffsetRow = 0;
			f_iOffsetColumn = 0;
		break;
		case ( KEY_CODES::END ):
			f_iOffsetColumn = 0;
			if ( f_iLines > f_iHeightRaw )
				f_iOffsetRow = f_iLines - f_iHeightRaw;
		break;
		default :
			l_iCode = a_iCode;
		}
	if ( ! l_iCode )
		schedule_refresh();
	return ( l_iCode );
	M_EPILOG
	}

}

}

