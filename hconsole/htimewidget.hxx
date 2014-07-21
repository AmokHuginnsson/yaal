/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	htimewidget.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCONSOLE_HTIMEWIDGET_HXX_INCLUDED
#define YAAL_HCONSOLE_HTIMEWIDGET_HXX_INCLUDED 1

#include "hcore/htime.hxx"
#include "hconsole/hwindow.hxx"

namespace yaal {

namespace hconsole {

/*! \brief Implementation of TUI Time widget class.
 *
 * Time widget allows setting and/or getting time.
 */
class HTimeWidget : public virtual HWidget {
public:
	typedef HTimeWidget this_type;
	typedef HWidget base_type;
private:
	yaal::hcore::HTime _time;
public:
	HTimeWidget( HWindow*, int, int, int, int, char const* );
	virtual ~HTimeWidget( void );
protected:
	virtual void do_paint( void );
	virtual int do_process_input( int );
	virtual bool do_click( mouse::OMouse& );
};

}

}

#endif /* #ifndef YAAL_HCONSOLE_HTIMEWIDGET_HXX_INCLUDED */

