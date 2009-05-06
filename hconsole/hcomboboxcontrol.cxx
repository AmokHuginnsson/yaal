/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hcomboboxcontrol.cxx - this file is integral part of `yaal' project.

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

#include <cstring>

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "hcomboboxcontrol.hxx"

using namespace yaal::hcore;

namespace yaal
{

namespace hconsole
{

HComboboxControl::HComboboxControl ( HWindow * a_poParent,
		int a_iRow, int a_iColumn, int a_iHeight, int a_iWidth,
		char const * a_pcLabel, int a_iDroppedWidth,
		int a_iMaxLength, char const * a_pcMask, bool a_bSearchable )
								: HControl ( a_poParent, a_iRow, a_iColumn, a_iHeight,
										a_iWidth, a_pcLabel ),
									HEditControl ( NULL, 0, 0, 0, 0, NULL, a_iMaxLength, "",
											a_pcMask ),
									HSearchableControl ( a_bSearchable ),
									HListControl ( NULL, 0, 0, 0, 0, NULL ),
									f_eMode ( MODE::EDITCONTROL ), f_iDroppedWidth ( a_iDroppedWidth )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HComboboxControl::~HComboboxControl ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

int HComboboxControl::set_focus ( char a_cCode )
	{
	M_PROLOG
	if ( f_eMode == MODE::EDITCONTROL )
		return ( HEditControl::set_focus ( a_cCode ) );
	return ( HListControl::set_focus ( a_cCode ) );
	M_EPILOG
	}

int HComboboxControl::kill_focus ( void )
	{
	M_PROLOG
	if ( f_eMode == MODE::LISTCONTROL )
		{
		f_eMode = MODE::EDITCONTROL;
		HCons::get_instance().clrscr();
		f_poParent->schedule_refresh();
		}
	return ( HControl::kill_focus() );
	M_EPILOG
	}

void HComboboxControl::do_refresh ( void )
	{
	M_PROLOG
	int l_iWidth = 0;
	int l_iHeight = 0;
	HConsole& cons = HCons::get_instance();
	if ( f_eMode == MODE::EDITCONTROL )
		{
/* ripped from HControl::draw_label() */
		l_iWidth = ( f_iWidth > 0 ) ? f_iWidth
			: cons.get_width() + f_iWidth - f_iColumnRaw;
/* end of ripped part */
		HEditControl::do_refresh();
		M_ENSURE( cons.c_move( f_iRowRaw, f_iColumnRaw + l_iWidth - 1 ) != C_ERR );
		set_attr_label();
		M_ENSURE( cons.c_addch( GLYPHS::DOWN_ARROW ) != C_ERR );
		M_ENSURE( cons.c_move( f_iRowRaw, f_iColumnRaw + HEditControl::f_iCursorPosition ) != C_ERR );
		f_iHeightRaw = 0;
		}
	else
		{
		l_iHeight = f_iHeight;
		l_iWidth = f_iWidth;
		f_iWidth = f_iDroppedWidth;
		int size = static_cast<int>( f_oControler->size() );
		int l_iHR = ( f_bDrawLabel ? 1 : 0 ) + ( f_bDrawHeader && ! f_bSingleLine ? 1 : 0 );
		if ( size < ( f_iHeight - l_iHR ) )
			f_iHeight = size + l_iHR;
		HListControl::do_refresh();
		f_iHeight = l_iHeight;
		f_iWidth = l_iWidth;
		}
	return;
	M_EPILOG
	}

int HComboboxControl::do_process_input( int a_iCode )
	{
	M_PROLOG
	if ( f_eMode == MODE::EDITCONTROL )
		{
		switch ( a_iCode )
			{
			case ( KEY_CODES::UP ):
				f_eMode = MODE::LISTCONTROL;
			break;
			case ( KEY_CODES::DOWN ):
				f_eMode = MODE::LISTCONTROL;
			break;
			default :
				return ( HEditControl::do_process_input ( a_iCode ) );
			}
		schedule_refresh();
		}
	else
		{
		if ( a_iCode != '\r' )
			return ( HListControl::do_process_input ( a_iCode ) );
		close_combo();
		}
	return ( 0 );
	M_EPILOG
	}

int HComboboxControl::do_click( mouse::OMouse& a_rsMouse )
	{
	M_PROLOG
	if ( f_eMode == MODE::EDITCONTROL )
		{
		HEditControl::do_click( a_rsMouse );
		f_iWidthRaw = ( f_iWidth > 0 ) ? f_iWidth
			: HCons::get_instance().get_width() + f_iWidth - f_iColumnRaw;
		if ( a_rsMouse.f_iColumn == ( f_iColumnRaw + f_iWidthRaw - 1 ) )
			{
			f_eMode = MODE::LISTCONTROL;
			schedule_refresh();
			}
		}
	else if ( HListControl::do_click( a_rsMouse ) )
		close_combo();
	return ( 0 );
	M_EPILOG
	}
			
void HComboboxControl::close_combo( void )
	{
	M_PROLOG
	f_eMode = MODE::EDITCONTROL;
	if ( f_oControler->empty() )
		HEditControl::set( (*f_oCursor)[ 0 ].get_string() );
	f_poParent->schedule_refresh();
	return;
	M_EPILOG
	}

}

}

