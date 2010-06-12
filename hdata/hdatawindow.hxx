/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hdatawindow.hxx - this file is integral part of `yaal' project.

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
	typedef HDataWindow this_type;
	typedef HWindow base_type;
public:
	/*! \brief Data window operation mode.
	 */
	struct DOCUMENT
		{
		/*! \brief Data window operation mode.
		 */
		typedef enum
			{
			VIEW, /*!< Browse data. */
			EDIT  /*!< Edit current record. */
			} mode_t;
		};
	/*! \brief DML helper, buffer for currently edited row.
	 */
	struct ORowBuffer
		{
		int _idColNo;
		yaal::hconsole::HItem _item;
		ORowBuffer( int idColNo_, int size_ ) : _idColNo( idColNo_ ), _item( size_ ) {}
		};
protected:
	bool _modified;
	DOCUMENT::mode_t _documentMode;
	HDataControl* _mainControl;
	resources_t* _resourcesArray;
	ORowBuffer* _syncStore;
	typedef yaal::hcore::HList<HDataControl*> controls_t;
	controls_t _viewModeControls;
	controls_t _editModeControls;
	HDataProcess* _owner;
	yaal::dbwrapper::HSQLDescriptor::ptr_t _dB;
	yaal::dbwrapper::HSQLDescriptor::MODE::mode_t _mode;
public:
	HDataWindow( char const*, HDataProcess*, resources_t* = NULL );
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
