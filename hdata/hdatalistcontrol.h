/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hdatalistcontrol.h - this file is integral part of `stdhapi' project.

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

#ifndef __HDATALISTCONTROL_H
#define __HDATALISTCONTROL_H

#include "../hconsole/hlistcontrol.h"
#include "hdatacontrol.h"

class HDataWindow;

class HDataListControl : public HDataControl, public virtual HListControl
	{
protected:
	/*{*/
	/*}*/
public:
	/*{*/
	HDataListControl ( HRecordSet *, HDataWindow *, int, int, int, int, const char *, 
			bool = false, bool = true, bool = true, bool = true,
			bool = true, int = -1, int = -1, int = -1 );
	virtual ~HDataListControl ( void );
	virtual void load ( long int );
	virtual long int get_current_id ( void );
	virtual void add_new ( void );
	virtual void cancel_new ( void );
	/*}*/
protected:
	/*{*/
	/*}*/
	};

#endif /* not __HDATALISTCONTROL_H */
