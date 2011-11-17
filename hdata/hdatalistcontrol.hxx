/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hdatalistcontrol.hxx - this file is integral part of `yaal' project.

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
/*! \file hdata/hdatalistcontrol.hxx
 * \brief Declaration of HDataListControl class.
 */

#ifndef YAAL_HDATA_HDATALISTCONTROL_HXX_INCLUDED
#define YAAL_HDATA_HDATALISTCONTROL_HXX_INCLUDED 1

#include "hconsole/hlistcontrol.hxx"
#include "hdata/hdatacontrol.hxx"

namespace yaal {

namespace hdata {

class HDataWindow;

/*! \brief Database enabled list based TUI control.
 *
 * This TUI control support automatic display of database content.
 */
class HDataListControl : public HDataControl, public virtual yaal::hconsole::HListControl {
public:
	yaal::hconsole::list_control_helper::HListControler<>::ptr_t _dataControler;
	HDataListControl( HDataWindow*, int, int, int, int, char const* );
	virtual ~HDataListControl( void );
	virtual void load( int long );
	virtual int long get_current_id( void );
	virtual void add_new( void );
	virtual void cancel_new( void );
};

}

}

#endif /* #ifndef YAAL_HDATA_HDATALISTCONTROL_HXX_INCLUDED */

