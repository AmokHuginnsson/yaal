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
M_VCSID( "$Id: "__TID__" $" )
#include "hcomboboxcontrol.hxx"

using namespace yaal::hcore;

namespace yaal
{

namespace hconsole
{

HComboboxControl::HComboboxControl ( HWindow * parent_,
		int row_, int column_, int height_, int width_,
		char const * label_, int droppedWidth_,
		int maxLength_, char const * mask_, bool searchable_ )
								: HControl ( parent_, row_, column_, height_,
										width_, label_ ),
									HEditControl ( NULL, 0, 0, 0, 0, NULL, maxLength_, "",
											mask_ ),
									HSearchableControl ( searchable_ ),
									HListControl ( NULL, 0, 0, 0, 0, NULL ),
									_mode ( MODE::EDITCONTROL ), _droppedWidth ( droppedWidth_ )
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

int HComboboxControl::set_focus ( char code_ )
	{
	M_PROLOG
	if ( _mode == MODE::EDITCONTROL )
		return ( HEditControl::set_focus ( code_ ) );
	return ( HListControl::set_focus ( code_ ) );
	M_EPILOG
	}

int HComboboxControl::kill_focus ( void )
	{
	M_PROLOG
	if ( _mode == MODE::LISTCONTROL )
		{
		_mode = MODE::EDITCONTROL;
		HConsole::get_instance().clrscr();
		_parent->schedule_refresh();
		}
	return ( HControl::kill_focus() );
	M_EPILOG
	}

void HComboboxControl::do_refresh ( void )
	{
	M_PROLOG
	int width( 0 );
	HConsole& cons( HConsole::get_instance() );
	if ( _mode == MODE::EDITCONTROL )
		{
/* ripped from HControl::draw_label() */
		width = ( _width > 0 ) ? _width
			: cons.get_width() + _width - _columnRaw;
/* end of ripped part */
		HEditControl::do_refresh();
		M_ENSURE( cons.c_move( _rowRaw, _columnRaw + width - 1 ) != C_ERR );
		set_attr_label();
		M_ENSURE( cons.c_addch( GLYPHS::DOWN_ARROW ) != C_ERR );
		M_ENSURE( cons.c_move( _rowRaw, _columnRaw + HEditControl::_cursorPosition ) != C_ERR );
		_heightRaw = 0;
		}
	else
		{
		int height( _height );
		width = _width;
		_width = _droppedWidth;
		int size = static_cast<int>( _controler->size() );
		int hR = ( _drawLabel ? 1 : 0 ) + ( _drawHeader && ! _singleLine ? 1 : 0 );
		if ( size < ( _height - hR ) )
			_height = size + hR;
		HListControl::do_refresh();
		_height = height;
		_width = width;
		}
	return;
	M_EPILOG
	}

int HComboboxControl::do_process_input( int code_ )
	{
	M_PROLOG
	if ( _mode == MODE::EDITCONTROL )
		{
		switch ( code_ )
			{
			case ( KEY_CODES::UP ):
				_mode = MODE::LISTCONTROL;
			break;
			case ( KEY_CODES::DOWN ):
				_mode = MODE::LISTCONTROL;
			break;
			default :
				return ( HEditControl::do_process_input ( code_ ) );
			}
		schedule_refresh();
		}
	else
		{
		if ( code_ != '\r' )
			return ( HListControl::do_process_input ( code_ ) );
		close_combo();
		}
	return ( 0 );
	M_EPILOG
	}

int HComboboxControl::do_click( mouse::OMouse& mouse_ )
	{
	M_PROLOG
	if ( _mode == MODE::EDITCONTROL )
		{
		HEditControl::do_click( mouse_ );
		_widthRaw = ( _width > 0 ) ? _width
			: HConsole::get_instance().get_width() + _width - _columnRaw;
		if ( mouse_._column == ( _columnRaw + _widthRaw - 1 ) )
			{
			_mode = MODE::LISTCONTROL;
			schedule_refresh();
			}
		}
	else if ( HListControl::do_click( mouse_ ) )
		close_combo();
	return ( 0 );
	M_EPILOG
	}
			
void HComboboxControl::close_combo( void )
	{
	M_PROLOG
	_mode = MODE::EDITCONTROL;
	if ( _controler->empty() )
		HEditControl::set( (*_cursor)[ 0 ].get_string() );
	_parent->schedule_refresh();
	return;
	M_EPILOG
	}

}

}

