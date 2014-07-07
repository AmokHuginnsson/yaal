/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hcomboboxcontrol.cxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <cstring>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hcomboboxcontrol.hxx"

using namespace yaal::hcore;

namespace yaal {

namespace hconsole {

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
									_mode ( MODE::EDITCONTROL ), _droppedWidth ( droppedWidth_ ) {
	M_PROLOG
	return;
	M_EPILOG
}

HComboboxControl::~HComboboxControl ( void ) {
	M_PROLOG
	return;
	M_EPILOG
}

int HComboboxControl::kill_focus( void ) {
	M_PROLOG
	if ( _mode == MODE::LISTCONTROL ) {
		_mode = MODE::EDITCONTROL;
		_window->schedule_repaint( true );
	}
	return ( HControl::kill_focus() );
	M_EPILOG
}

void HComboboxControl::do_paint( void ) {
	M_PROLOG
	int width( 0 );
	HConsole& cons( HConsole::get_instance() );
	if ( _mode == MODE::EDITCONTROL ) {
/* ripped from HControl::draw_label() */
		width = ( _width > 0 ) ? _width
			: cons.get_width() + _width - _columnRaw;
/* end of ripped part */
		HEditControl::do_paint();
		cons.move( _rowRaw, _columnRaw + width - 1 );
		set_attr_label();
		cons.addch( GLYPHS::DOWN_ARROW );
		cons.move( _rowRaw, _columnRaw + HEditControl::_cursorPosition );
		_heightRaw = 0;
	} else {
		int height( _height );
		width = _width;
		_width = _droppedWidth;
		int size = static_cast<int>( _controler->size() );
		int hR = ( _drawLabel ? 1 : 0 ) + ( _drawHeader && ! _singleLine ? 1 : 0 );
		if ( size < ( _height - hR ) )
			_height = size + hR;
		HListControl::do_paint();
		_height = height;
		_width = width;
	}
	return;
	M_EPILOG
}

int HComboboxControl::do_process_input( int code_ ) {
	M_PROLOG
	if ( _mode == MODE::EDITCONTROL ) {
		switch ( code_ ) {
			case ( KEY_CODES::UP ):
				_mode = MODE::LISTCONTROL;
			break;
			case ( KEY_CODES::DOWN ):
				_mode = MODE::LISTCONTROL;
			break;
			default :
				return ( HEditControl::do_process_input ( code_ ) );
		}
		schedule_repaint();
	} else {
		if ( code_ != '\r' )
			return ( HListControl::do_process_input ( code_ ) );
		close_combo();
	}
	return ( 0 );
	M_EPILOG
}

bool HComboboxControl::do_click( mouse::OMouse& mouse_ ) {
	M_PROLOG
	bool handled( false );
	if ( _mode == MODE::EDITCONTROL ) {
		handled = HEditControl::do_click( mouse_ );
		if ( ! handled ) {
			_widthRaw = ( _width > 0 ) ? _width
				: HConsole::get_instance().get_width() + _width - _columnRaw;
			if ( mouse_._column == ( _columnRaw + _widthRaw - 1 ) ) {
				_mode = MODE::LISTCONTROL;
				schedule_repaint();
				handled = true;
			}
		}
	} else {
		handled = HListControl::do_click( mouse_ );
		if ( ! handled ) {
			close_combo();
			handled = true;
		}
	}
	return ( handled );
	M_EPILOG
}

void HComboboxControl::close_combo( void ) {
	M_PROLOG
	_mode = MODE::EDITCONTROL;
	if ( ! _controler->is_empty() ) {
		set_text( (*_cursor)[ 0 ].get_string() );
	}
	_window->schedule_repaint( true );
	return;
	M_EPILOG
}

}

}

