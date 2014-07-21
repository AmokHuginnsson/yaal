/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hdatawidget.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HDATA_HDATAWIDGET_HXX_INCLUDED
#define YAAL_HDATA_HDATAWIDGET_HXX_INCLUDED 1

#include "dbwrapper/hsqldescriptor.hxx"
#include "hconsole/hwidget.hxx"
#include "hdata/oresource.hxx"

namespace yaal {

namespace hdata {

/*! \brief Base class for database connected TUI controls.
 */
class HDataWidget : public virtual hconsole::HWidget {
protected:
	OResource const* _resource;
	dbwrapper::HSQLDescriptor::ptr_t _SQL;
public:
	HDataWidget( void );
	virtual ~HDataWidget( void );
	void set_resource( OResource const* );
	void set_dbd( dbwrapper::HSQLDescriptor::ptr_t );
	virtual void load( int long = 0 );
	virtual int long get_current_id( void );
	virtual void add_new( void );
	virtual void cancel_new( void );
private:
	HDataWidget ( HDataWidget const & );
	HDataWidget & operator = ( HDataWidget const & );
};

}

}

#endif /* #ifndef YAAL_HDATA_HDATAWIDGET_HXX_INCLUDED */

