/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hdatewidget.cxx - this file is integral part of `yaal' project.

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
#include "hdatewidget.hxx"

using namespace yaal::hcore;

namespace yaal {

namespace hconsole {

HDateWidget::HDateWidget( HWindow* parent_, int row_, int column_,
		int height_, int width_, char const* label_ )
	: HWidget( parent_, row_, column_, height_, width_, label_ ),
	_time( HTime::LOCAL ) {
	M_PROLOG
	return;
	M_EPILOG
}

HDateWidget::~HDateWidget ( void ) {
	M_PROLOG
	return;
	M_EPILOG
}

void HDateWidget::do_paint( void ) {
	M_PROLOG
	return;
	M_EPILOG
}

int HDateWidget::do_process_input( int code_ ) {
	M_PROLOG
	return ( code_ );
	M_EPILOG
}

bool HDateWidget::do_click( mouse::OMouse& ) {
	M_PROLOG
	return ( false );
	M_EPILOG
}

}

}

