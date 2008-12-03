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

#ifndef YAAL_HDATA_HDATAWINDOW_HXX_INCLUDED
#define YAAL_HDATA_HDATAWINDOW_HXX_INCLUDED

#include "dbwrapper/hdatabase.hxx"
#include "hconsole/hwindow.hxx"
#include "hconsole/hlistcontrol.hxx"
#include "hdata/hdatacontrol.hxx"
#include "hdata/hdatalistcontrol.hxx"
#include "hdata/oresource.hxx"

namespace yaal
{

namespace hdata
{

class HDataProcess;

/*! \brief Data connected TUI Window implementation.
 *
 * This class automates DML operations.
 */
class HDataWindow : public hconsole::HWindow
	{
protected:
	typedef HDataWindow self_t;
	typedef HWindow hier_t;
public:
	/*! \brief Data window operation mode.
	 */
	struct DOCUMENT
		{
		/*! \brief Data window operation mode.
		 */
		typedef enum
			{
			D_VIEW, /*!< Browse data. */
			D_EDIT  /*!< Edit current record. */
			} mode_t;
		};
	/*! \brief DML helper, buffer for currently edited row.
	 */
	struct ORowBuffer
		{
		int f_iIdColNo;
		yaal::hconsole::HItem f_oItem;
		ORowBuffer( int a_iIdColNo, int a_iSize ) : f_iIdColNo( a_iIdColNo ), f_oItem( a_iSize ) {}
		};
protected:
	bool f_bModified;
	DOCUMENT::mode_t f_eDocumentMode;
	HDataControl* f_poMainControl;
	OResource* f_psResourcesArray;
	ORowBuffer* f_poSyncStore;
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
	void set_sync_store( ORowBuffer* = NULL );
	bool is_modified( void ) const;
	void set_modified( bool = true );
	void sync( yaal::dbwrapper::HRecordSet::iterator );
protected:
	void link( int, HDataControl* );
	void set_mode( DOCUMENT::mode_t );
	void sync( void );
	void sync( int, hconsole::HEditControl& );
	void sync( int, HDataListControl& );
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

#endif /* not YAAL_HDATA_HDATAWINDOW_HXX_INCLUDED */
