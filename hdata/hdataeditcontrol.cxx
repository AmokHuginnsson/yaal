/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hdataeditcontrol.cxx - this file is integral part of `yaal' project.

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

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hdataeditcontrol.hxx"
#include "hconsole/console.hxx"
#include "hcore/hlog.hxx"

using namespace yaal::hcore;
using namespace yaal::hconsole;
using namespace yaal::dbwrapper;

namespace yaal {

namespace hdata {

HDataEditControl::HDataEditControl( HDataWindow * parent_,
		int row_, int column_, int height_, int width_,
		char const * label_, int bufferSize_, char const * value_,
		char const * mask_, bool replace_, bool multiLine_,
		bool readOnly_, bool rightAligned_,
		bool password_, int maxHistoryLevel_ )
								:	HControl ( parent_, row_, column_, height_,
										width_, label_ ),
									HEditControl( parent_,
										row_, column_, height_, width_,
										label_, bufferSize_, value_,
										mask_, replace_, multiLine_,
										readOnly_, rightAligned_,
										password_, maxHistoryLevel_ ),
									HDataControl() {
	M_PROLOG
	return;
	M_EPILOG
}

int HDataEditControl::do_process_input ( int code_ ) {
	M_PROLOG
	bool noChange = false;
	HDataWindow * window = NULL;
	switch ( code_ ) {
		case ( '\t' ):
		case ( '\r' ):
		case ( KEY_CODES::LEFT ):
		case ( KEY_CODES::RIGHT ):
		case ( KEY < 'a' >::ctrl ):
		case ( KEY_CODES::HOME ):
		case ( KEY < 'e' >::ctrl ):
		case ( KEY_CODES::END ):
		case ( KEY_CODES::INSERT ):
		case ( KEY < 'f' >::meta ):
		case ( KEY < 'b' >::meta ):
			noChange = true;
		break;
		case ( KEY_CODES::PAGE_UP ):
		case ( KEY_CODES::PAGE_DOWN ):
		case ( KEY_CODES::UP ):
		case ( KEY_CODES::DOWN ):
			if ( _multiLine )
				noChange = true;
		break;
		case ( KEY_CODES::BACKSPACE ):
			if ( ! ( _controlOffset + _cursorPosition ) )
				noChange = true;
		break;
		case ( KEY_CODES::DELETE ):
			if ( _string.is_empty() )
				noChange = true;
		break;
		default:
		break;
	}
	code_ = HEditControl::do_process_input ( code_ );
	if ( ! ( code_ || noChange ) ) {
		window = dynamic_cast<HDataWindow*>( _window );
		M_ASSERT ( window );
		window->set_modified();
	}
	return ( code_ );
	M_EPILOG
}

}

}

