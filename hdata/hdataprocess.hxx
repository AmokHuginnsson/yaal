/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hdataprocess.hxx - this file is integral part of `yaal' project.

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
/*! \file hdata/hdataprocess.hxx
 * \brief Declaration of HDataProcess class.
 */

#ifndef YAAL_HDATA_HDATAPROCESS_HXX_INCLUDED
#define YAAL_HDATA_HDATAPROCESS_HXX_INCLUDED 1

#include "hconsole/htuiprocess.hxx"
#include "hconsole/hwindow.hxx"
#include "hconsole/omenuitem.hxx"
#include "tools/hxml.hxx"
#include "dbwrapper/hdatabase.hxx"

namespace yaal {

namespace hdata {

/*! \brief A backbone of database enabled TUI based application.
 *
 * HDataProcess enables rapid building of database connected TUI
 * applications with simple XML resource files.
 */
class HDataProcess : public hconsole::HTUIProcess {
public:
	typedef HDataProcess this_type;
	typedef HTUIProcess base_type;
protected:
	dbwrapper::database_ptr_t _dataBase;
public:
	HDataProcess( void );
	virtual ~HDataProcess( void );
	void init_xrc( char const*, char const* );
/* cppcheck-suppress functionConst */
	dbwrapper::database_ptr_t data_base( void );
protected:
	virtual void do_quit( void );
	virtual void do_close_window( void );
	void force_close_window( void );
	void force_quit( void );
private:
	HDataProcess( HDataProcess const& );
	HDataProcess& operator = ( HDataProcess const& );
};

}

}

#endif /* #ifndef YAAL_HDATA_HDATAPROCESS_HXX_INCLUDED */

