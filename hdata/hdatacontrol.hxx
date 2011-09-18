/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hdatacontrol.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HDATA_HDATACONTROL_HXX_INCLUDED
#define YAAL_HDATA_HDATACONTROL_HXX_INCLUDED 1

#include "dbwrapper/hrecordset.hxx"
#include "hconsole/hcontrol.hxx"
#include "hdata/oresource.hxx"

namespace yaal {

namespace hdata {

/*! \brief Base class for database connected TUI controls.
 */
class HDataControl : public virtual hconsole::HControl {
protected:
	OResource const* _resource;
	dbwrapper::HSQLDescriptor::ptr_t _SQL;
public:
	HDataControl( void );
	virtual ~HDataControl( void );
	void set_resource( OResource const* );
	void set_dbd( dbwrapper::HSQLDescriptor::ptr_t );
	virtual void load( int long = 0 );
	virtual int long get_current_id( void );
	virtual void add_new( void );
	virtual void cancel_new( void );
private:
	HDataControl ( HDataControl const & );
	HDataControl & operator = ( HDataControl const & );
};

}

}

#endif /* #ifndef YAAL_HDATA_HDATACONTROL_HXX_INCLUDED */

