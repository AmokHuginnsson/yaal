/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hdatawindow.h - this file is integral part of `stdhapi' project.

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

#ifndef __HDATAWINDOW_H
#define __HDATAWINDOW_H

#include "../dbwrapper/hrecordset.h"
#include "../hconsole/hwindow.h"
#include "../hconsole/hlistcontrol.h"
#include "hdatacontrol.h"
#include "hdatalistcontrol.h"
#include "oresource.h"

#define D_MODE_VIEW	0
#define D_MODE_EDIT	1

class HDataWindow : public HWindow, public HRecordSet
	{
protected:
	/*{*/
	bool f_bModified;
	HDataControl * f_poMainControl;
	OResource * f_psResourcesArray;
	HList < HControl * > f_oViewModeControls;
	HList < HControl * > f_oEditModeControls;
	/*}*/
public:
	/*{*/
	HDataWindow ( const char *, HDataBase * = NULL, OResource * = NULL );
	virtual ~HDataWindow ( void );
	virtual int init ( void );
	/*}*/
protected:
	/*{*/
	void link ( int, HDataControl * );
	void set_mode ( int );
	void sync ( int, HEditControl & );
	void sync ( int, HDataListControl & );
	int handler_add_new ( int );
	int handler_edit ( int );
	int handler_delete ( int );
	int handler_save ( int );
	int handler_requery ( int );
	int handler_cancel ( int );
	/*}*/
	};

#endif /* not __HDATAWINDOW_H */
