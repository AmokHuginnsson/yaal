/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hmainwindow.hxx - this file is integral part of `yaal' project.

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
/*! \file hconsole/hmainwindow.hxx
 * \brief Declaration of HMainWindow class.
 */

#ifndef YAAL_HCONSOLE_HMAINWINDOW_HXX_INCLUDED
#define YAAL_HCONSOLE_HMAINWINDOW_HXX_INCLUDED

#include "hconsole/hwindow.hxx"
#include "hconsole/omenuitem.hxx"
#include "hconsole/hmenucontrol.hxx"
#include "hconsole/hwindowlistcontrol.hxx"

namespace yaal
{

namespace hconsole
{

/*! \brief Main window of TUI application.
 *
 * Main window of TUI application consists of menu and active window list.
 */
class HMainWindow : public HWindow
	{
	HMenuControl* f_poMenu;
	HTUIProcess::model_ptr_t f_oWindowList;
	HTUIProcess::model_t::cyclic_iterator& f_roForegroundWindow;
public:
	HMainWindow( char const*, HTUIProcess::model_ptr_t, HTUIProcess::model_t::cyclic_iterator& );
	virtual ~HMainWindow( void );
	void init_menu( HTUIProcess*, OMenuItem* );
	virtual int init( void );
protected:
	virtual int handler_close( int, void const* ); /* closes window process */
private:
	HMainWindow( HMainWindow const& );
	HMainWindow& operator = ( HMainWindow const& );
	};

}

}

#endif /* not YAAL_HCONSOLE_HMAINWINDOW_HXX_INCLUDED */
