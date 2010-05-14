/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hdataeditcontrol.cxx - this file is integral part of `yaal' project.

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
#include "hconsole/console.hxx"
#include "hdataeditcontrol.hxx"

using namespace yaal::hcore;
using namespace yaal::hconsole;
using namespace yaal::dbwrapper;

namespace yaal
{

namespace hdata
{

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
									HDataControl()
	{
	M_PROLOG
	return;
	M_EPILOG
	}

int HDataEditControl::do_process_input ( int code_ )
	{
	M_PROLOG
	bool noChange = false;
	HDataWindow * window = NULL;
	switch ( code_ )
		{
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
	if ( ! ( code_ || noChange ) )
		{
		window = dynamic_cast<HDataWindow*>( _parent );
		M_ASSERT ( window );
		window->set_modified();
		}
	return ( code_ );
	M_EPILOG
	}

}

}

