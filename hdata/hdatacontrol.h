/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hdatacontrol.h - this file is integral part of `yaal' project.

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

#ifndef __YAAL_HDATA_HDATACONTROL_H
#define __YAAL_HDATA_HDATACONTROL_H

#include "dbwrapper/hrecordset.h"
#include "hconsole/hcontrol.h"

namespace yaal
{

namespace hdata
{

class HDataControl : public virtual hconsole::HControl
	{
protected:
	dbwrapper::HSQLDescriptor::ptr_t f_oSQL;
public:
	HDataControl ( dbwrapper::HSQLDescriptor::ptr_t );
	virtual ~HDataControl ( void );
	virtual void load ( int long = 0 );
	virtual int long get_current_id ( void );
	virtual void add_new ( void );
	virtual void cancel_new ( void );
private:
	HDataControl ( HDataControl const & );
	HDataControl & operator = ( HDataControl const & );
	};

}

}

#endif /* not __YAAL_HDATA_HDATACONTROL_H */
