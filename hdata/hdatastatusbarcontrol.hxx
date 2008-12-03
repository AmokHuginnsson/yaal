/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hdatastatusbarcontrol.h - this file is integral part of `yaal' project.

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
/*! \file hdata/hdatastatusbarcontrol.h
 * \brief Declaration of HDataStatusBarControl class.
 */

#ifndef YAAL_HDATA_HDATASTATUSBARCONTROL_HXX_INCLUDED
#define YAAL_HDATA_HDATASTATUSBARCONTROL_HXX_INCLUDED

#include "hconsole/hstatusbarcontrol.hxx"
#include "hdata/hdatawindow.hxx"

namespace yaal
{

namespace hdata
{

/*! \brief Implementation of database enabled TUI Statusbar control class.
 *
 * DataStatusBar Control provides display of progress bars, allows
 * invocation of commands and specyfing search patterns.
 * Additionally it provides document status presentation.
 */
class HDataStatusBarControl : public hconsole::HStatusBarControl
	{
public:
	HDataStatusBarControl ( HDataWindow *, char const *, int = -1 );
	virtual void do_draw_label ( void );
	};

}

}

#endif /* not YAAL_HDATA_HDATASTATUSBARCONTROL_HXX_INCLUDED */

