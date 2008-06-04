/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hdatawindow.h - this file is integral part of `yaal' project.

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

#ifndef __YAAL_HDATA_HDATAWINDOW_H
#define __YAAL_HDATA_HDATAWINDOW_H

#include "dbwrapper/hdatabase.h"
#include "hconsole/hwindow.h"
#include "hconsole/hlistcontrol.h"
#include "hdata/hdatacontrol.h"
#include "hdata/hdatalistcontrol.h"
#include "hdata/oresource.h"

namespace yaal
{

namespace hdata
{

class HDataProcess;

class HDataWindow : public hconsole::HWindow
	{
public:
	struct DOCUMENT
		{
		typedef enum
			{
			D_VIEW,
			D_EDIT
			} mode_t;
		};
protected:
	bool f_bModified;
	DOCUMENT::mode_t f_eDocumentMode;
	HDataControl* f_poMainControl;
	OResource* f_psResourcesArray;
	yaal::hconsole::HItem* f_poSyncStore;
	typedef yaal::hcore::HList<HDataControl*> controls_t;
	controls_t f_oViewModeControls;
	controls_t f_oEditModeControls;
	HDataProcess* f_poOwner;
	yaal::dbwrapper::HSQLDescriptor::ptr_t f_oDB;
	yaal::dbwrapper::HSQLDescriptor::MODE::mode_t f_eMode;
public:
	HDataWindow( char const*, HDataProcess*, OResource* = NULL );
	virtual ~HDataWindow( void );
	virtual int init( void );
	virtual hconsole::HStatusBarControl* init_bar( char const* );
	void set_sync_store( hconsole::HItem* = NULL );
	bool is_modified( void ) const;
	void set_modified( bool = true );
protected:
	void link( int, HDataControl* );
	void set_mode( DOCUMENT::mode_t );
	virtual void sync( void );
	void sync( int, hconsole::HEditControl & );
	void sync( int, HDataListControl & );
	int handler_add_new( int, void const* );
	int handler_edit( int, void const* );
	int handler_delete( int, void const* );
	int handler_save( int, void const* );
	int handler_requery( int, void const* );
	int handler_cancel( int, void const* );
private:
	HDataWindow( HDataWindow const& );
	HDataWindow& operator = ( HDataWindow const& );
	};

}

}

#endif /* not __YAAL_HDATA_HDATAWINDOW_H */
