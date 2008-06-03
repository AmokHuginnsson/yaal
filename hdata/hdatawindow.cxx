/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hdatawindow.cxx - this file is integral part of `yaal' project.

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

#include <libintl.h>

#include "hcore/hexception.h"
M_VCSID ( "$Id$" )
#include "hdatawindow.h"
#include "hdatalistcontrol.h"
#include "hdatatreecontrol.h"
#include "hdataeditcontrol.h"
#include "hdatastatusbarcontrol.h"

using namespace yaal::hcore;
using namespace yaal::hconsole;
using namespace yaal::dbwrapper;

namespace yaal
{

namespace hdata
{

HDataWindow::HDataWindow( char const* a_pcTitle, HDataProcess* a_poOwner,
		OResource* a_psDataControlInfo )
	: HWindow( a_pcTitle ),
	f_bModified ( false ), f_eDocumentMode( DOCUMENT::D_VIEW ), f_poMainControl( NULL ),
	f_psResourcesArray( a_psDataControlInfo ), f_poSyncStore( NULL ),
	f_oViewModeControls(), f_oEditModeControls(), f_poOwner( a_poOwner ), f_oDB( new HSQLDescriptor() )
	{
	M_PROLOG
	register_postprocess_handler( KEY<'n'>::command, NULL,
			&HDataWindow::handler_add_new );
	register_postprocess_handler( KEY<'e'>::command, NULL,
			&HDataWindow::handler_edit );
	register_postprocess_handler( KEY<'d'>::command, NULL,
			&HDataWindow::handler_delete );
	register_postprocess_handler( KEY<'w'>::command, NULL,
			&HDataWindow::handler_save );
	register_postprocess_handler( KEY<'r'>::ctrl, NULL,
			&HDataWindow::handler_requery );
	register_postprocess_handler( KEY_CODES::D_ESC, NULL,
			&HDataWindow::handler_cancel );
	return;
	M_EPILOG
	}

HDataWindow::~HDataWindow ( void )
	{
	M_PROLOG
	f_poMainControl = NULL;
	f_psResourcesArray = NULL;
	f_poSyncStore = NULL;
	return;
	M_EPILOG
	}

#define M_SETUP_STANDARD	f_psResourcesArray[ l_iCtr ].f_iRow,\
						f_psResourcesArray[ l_iCtr ].f_iColumn,\
						f_psResourcesArray[ l_iCtr ].f_iHeight,\
						f_psResourcesArray[ l_iCtr ].f_iWidth,\
						f_psResourcesArray[ l_iCtr ].f_pcLabel

int HDataWindow::init ( void )
	{
	M_PROLOG
	int l_iCtr = 0;
	char l_pcValue [ ] = "";
	char const* l_pcMask = n_pcMaskDefault;
	HDataControl* l_poDataControl = NULL;
	OAttributes l_sAttributes;
	OAttributes* l_psAttr = &l_sAttributes;
	OEditControlResource l_sEditControlResource;
	OListControlResource l_sListControlResource;
/* ECR stands for EditControlResource */
	OEditControlResource* l_psECR = &l_sEditControlResource;
/* LCR stands for ListControlResource */
	OListControlResource* l_psLCR = &l_sListControlResource;
	l_sAttributes.f_bDrawLabel = true;
	l_sAttributes.f_iDisabledAttribute = -1;
	l_sAttributes.f_iEnabledAttribute = -1;
	l_sAttributes.f_iFocusedAttribute = -1;
	HWindow::init();
	while ( f_psResourcesArray[ l_iCtr ].f_pcLabel )
		{
		if ( f_psResourcesArray[ l_iCtr ].f_psAttributes )
			l_psAttr = f_psResourcesArray[ l_iCtr ].f_psAttributes;
		switch ( f_psResourcesArray[ l_iCtr ].f_eType )
			{
			case ( DATACONTROL_BITS::TYPE::D_EDIT ):
				{
				l_sEditControlResource.f_iMaxStringSize = 127;
				l_sEditControlResource.f_pcValue = l_pcValue;
				l_sEditControlResource.f_pcMask = l_pcMask;
				l_sEditControlResource.f_bReplace = false;
				l_sEditControlResource.f_bMultiLine = false;
				l_sEditControlResource.f_bReadOnly = false;
				l_sEditControlResource.f_bRightAligned = false;
				l_sEditControlResource.f_bPassword = false;
				l_sEditControlResource.f_iMaxHistoryLevel = 8;
				if ( f_psResourcesArray [ l_iCtr ].f_pvTypeSpecific )
					l_psECR = static_cast<OEditControlResource*>( f_psResourcesArray [ l_iCtr ].f_pvTypeSpecific );
				l_poDataControl = new HDataEditControl( this,
						M_SETUP_STANDARD, l_psECR->f_iMaxStringSize, l_psECR->f_pcValue,
						l_psECR->f_pcMask, l_psECR->f_bReplace,
						l_psECR->f_bMultiLine, l_psECR->f_bReadOnly, 
						l_psECR->f_bRightAligned, l_psECR->f_bPassword,
						l_psECR->f_iMaxHistoryLevel );
				}
			break;
			case ( DATACONTROL_BITS::TYPE::D_LIST ):
				{
				l_sListControlResource.f_bCheckable = false;
				l_sListControlResource.f_bSortable = true;
				l_sListControlResource.f_bSearchable = true;
				l_sListControlResource.f_bDrawHeader = true;
				if ( f_psResourcesArray [ l_iCtr ].f_pvTypeSpecific )
					l_psLCR = static_cast<OListControlResource*>( f_psResourcesArray [ l_iCtr ].f_pvTypeSpecific );
				HDataListControl* l_poList = NULL;
				l_poDataControl = l_poList = new HDataListControl( /* FIXME this */ HSQLDescriptor::ptr_t(), this, M_SETUP_STANDARD );
				l_poList->set_flags( ( l_psLCR->f_bCheckable ? HListControl::FLAGS::D_CHECKABLE : HListControl::FLAGS::D_NONE )
						| ( l_psLCR->f_bSortable ? HListControl::FLAGS::D_SORTABLE : HListControl::FLAGS::D_NONE )
						| ( l_psLCR->f_bEditable ? HListControl::FLAGS::D_EDITABLE : HListControl::FLAGS::D_NONE )
						| ( l_psLCR->f_bDrawHeader ? HListControl::FLAGS::D_DRAW_HEADER : HListControl::FLAGS::D_NONE ),
						HListControl::FLAGS::D_ALL );
				}
			break;
			case ( DATACONTROL_BITS::TYPE::D_TREE ):
				l_poDataControl = new HDataTreeControl( /* FIXME this */ HSQLDescriptor::ptr_t(), this, M_SETUP_STANDARD );
			break;
			case ( DATACONTROL_BITS::TYPE::D_COMBO ):
			break;
			case ( DATACONTROL_BITS::TYPE::D_DATE ):
			break;
			case ( DATACONTROL_BITS::TYPE::D_CHECK ):
			break;
			default :
			break;
			}
		if ( l_poDataControl )
			{
			l_poDataControl->set_draw_label( l_psAttr->f_bDrawLabel );
			l_poDataControl->set_attributes( l_psAttr->f_iDisabledAttribute,
					l_psAttr->f_iEnabledAttribute,
					l_psAttr->f_iFocusedAttribute );
			}
		switch ( f_psResourcesArray[ l_iCtr ].f_eRole )
			{
			case ( DATACONTROL_BITS::ROLE::D_MAIN ):
				{
				/* FIXME
				f_oTable = f_psResourcesArray [ l_iCtr ].f_pcTable;
				f_oColumns = f_psResourcesArray [ l_iCtr ].f_pcColumns;
				f_oFilter = f_psResourcesArray [ l_iCtr ].f_pcFilter;
				f_oSort = f_psResourcesArray [ l_iCtr ].f_pcSort;
				*/
				f_poMainControl = l_poDataControl;
				f_oViewModeControls.add_tail( &l_poDataControl );
				l_poDataControl->enable( true );
				}
			break;
			case ( DATACONTROL_BITS::ROLE::D_DATA ):
				link( l_iCtr, l_poDataControl );
				f_oEditModeControls.add_tail( &l_poDataControl );
			break;
			case ( DATACONTROL_BITS::ROLE::D_FILTER ):
				l_poDataControl->enable ( true );
				f_oViewModeControls.add_tail( &l_poDataControl );
			break;
			default :
				M_THROW ( "unknown resource purpouse",
						f_psResourcesArray[ l_iCtr ].f_eRole );
			}
		l_iCtr ++;
		}
	f_poMainControl->set_focus();
	if ( f_poMainControl )
		{
		f_poMainControl->load();
		f_poMainControl->process_input ( KEY_CODES::D_HOME );
		}
	refresh();
	return ( 0 );
	M_EPILOG
	}

HStatusBarControl* HDataWindow::init_bar( char const* a_pcLabel )
	{
	M_PROLOG
	return ( new HDataStatusBarControl( this, a_pcLabel ) );
	M_EPILOG
	}

void HDataWindow::link( int a_iChild, HDataControl* a_poDataControl )
	{
	M_PROLOG
	int l_iParent = 0;
	char l_pcName [] = "";
	HDataListControl * l_poPDC = NULL; /* PDC stands for ParentDataControl */
/* CI stands for ColumnInfo */
	OColumnInfo l_sColumnInfo, * l_psCI = & l_sColumnInfo;
	l_psCI->f_iPlacement = HDataListControl::HColumnInfo::D_ADD_AT_THE_END;
	l_psCI->f_pcName = l_pcName;
	l_psCI->f_iWidth = 1; 				/* width is awlays proportional */
	l_psCI->f_eAlign = HControl::BITS::ALIGN::D_LEFT;
	l_psCI->f_eType = D_HSTRING;
	l_iParent = f_psResourcesArray [ a_iChild ].f_iParent;
	if ( f_psResourcesArray [ l_iParent ].f_eType == DATACONTROL_BITS::TYPE::D_LIST )
		{
		l_poPDC = dynamic_cast<HDataListControl*>( f_oControls.get_control_by_no( l_iParent + 1 /* 1 stands for offset caused by 'status bar' */ ) );
		if ( ! l_poPDC )
			M_THROW ( "wrong control resource order",
					l_iParent );
		if ( f_psResourcesArray[ a_iChild ].f_psColumnInfo )
			l_psCI = f_psResourcesArray[ a_iChild ].f_psColumnInfo;
		l_poPDC->add_column( l_psCI->f_iPlacement, l_psCI->f_pcName,
				l_psCI->f_iWidth, l_psCI->f_eAlign, l_psCI->f_eType, a_poDataControl );
		}
	else
		M_THROW( "unknown parent type", f_psResourcesArray [ l_iParent ].f_eType );
	return;
	M_EPILOG
	}

void HDataWindow::set_mode( DOCUMENT::mode_t a_eMode )
	{
	M_PROLOG
	int l_iCtr = 0, l_iCount = 0;
	switch ( a_eMode )
		{
		case ( DOCUMENT::D_VIEW ):
			{
			l_iCount = f_oEditModeControls.size();
			for ( l_iCtr = 0; l_iCtr < l_iCount; l_iCtr ++ )
				f_oEditModeControls [ l_iCtr ]->enable ( false );
			l_iCount = f_oViewModeControls.size();
			for ( l_iCtr = 0; l_iCtr < l_iCount; l_iCtr ++ )
				f_oViewModeControls [ l_iCtr ]->enable ( true );
			if ( l_iCount )
				f_oViewModeControls [ 0 ]->set_focus();
			}
		break;
		case ( DOCUMENT::D_EDIT ):
			{
			l_iCount = f_oViewModeControls.size();
			for ( l_iCtr = 0; l_iCtr < l_iCount; l_iCtr ++ )
				f_oViewModeControls [ l_iCtr ]->enable ( false );
			l_iCount = f_oEditModeControls.size();
			for ( l_iCtr = 0; l_iCtr < l_iCount; l_iCtr ++ )
				f_oEditModeControls [ l_iCtr ]->enable ( true );
			if ( l_iCount )
				f_oEditModeControls [ 0 ]->set_focus();
			}
		break;
		default :
			M_THROW ( "unknown window mode", a_eMode );
		}
	return;
	M_EPILOG
	}

void HDataWindow::sync ( void )
	{
	M_PROLOG
/* FIXME
	int l_iCtr = 0, l_iCount = 0;
	HRecordSet::sync();
	if ( f_poSyncStore )
		{
		l_iCount = f_poSyncStore->get_size();
		if ( f_oValues.size() >= l_iCount )
			for ( l_iCtr = 0; l_iCtr < l_iCount; l_iCtr ++ )
				( * f_poSyncStore ) [ l_iCtr ] ( static_cast < char const * > ( f_oValues [ l_iCtr ] ) );
		( * f_poSyncStore ).m_lId = m_lId;
		}
	else if ( ( f_eMode == D_ADDING ) || ( f_eMode == D_EDITING ) )
		{
		l_iCount = f_oEditModeControls.size();
		if ( f_oValues.size() >= l_iCount )
			for ( l_iCtr = 0; l_iCtr < l_iCount; l_iCtr ++ )
				f_oValues [ l_iCtr ] = f_oEditModeControls [ l_iCtr ]->get().get < HString const & >();
		m_lId = f_poMainControl->get_current_id();
		}
*/
	return;
	M_EPILOG
	}

void HDataWindow::set_sync_store ( HItem * a_poItem )
	{
	f_poSyncStore = a_poItem;
	return;
	}

int HDataWindow::handler_add_new ( int, void const* )
	{
	M_PROLOG
/* FIXME
	if ( f_eMode != D_OPEN )
		{
		f_oStatusBar->message ( COLORS::D_FG_BRIGHTRED,
				_ ( "You can not add new rocord now." ) );
		return ( 0 );
		}
	add_new();
	if ( f_poMainControl )
		f_poMainControl->add_new();
	set_mode ( DOCUMENT::D_EDIT );
*/
	return ( 0 );
	M_EPILOG
	}

int HDataWindow::handler_edit ( int, void const* )
	{
	M_PROLOG
/* FIXME
	if ( f_eMode != D_NORMAL )
		{
		f_oStatusBar->message ( COLORS::D_FG_BRIGHTRED,
				_ ( "You can not start editing of this record." ) );
		return ( 0 );
		}
	if ( ! f_iSetQuantity )
		{
		f_oStatusBar->message ( COLORS::D_FG_BRIGHTRED,
				_ ( "There is nothing to edit." ) );
		return ( 0 );
		}
	set_mode ( DOCUMENT::D_EDIT );
	edit();
*/
	return ( 0 );
	M_EPILOG
	}

int HDataWindow::handler_delete ( int, void const* )
	{
	M_PROLOG
/* FIXME
	if ( f_eMode != D_NORMAL )
		{
		f_oStatusBar->message ( COLORS::D_FG_BRIGHTRED,
				_ ( "You can not delete this record." ) );
		return ( 0 );
		}
	if ( ! f_iSetQuantity )
		{
		f_oStatusBar->message ( COLORS::D_FG_BRIGHTRED,
				_ ( "There is nothing to remove." ) );
		return ( 0 );
		}
	if ( f_poMainControl )
		m_lId = f_poMainControl->get_current_id();
	remove();
	f_poMainControl->load();
*/
	return ( 0 );
	M_EPILOG
	}

int HDataWindow::handler_save ( int, void const* )
	{
	M_PROLOG
/* FIXME
	if ( ( f_eMode != D_ADDING ) && ( f_eMode != D_EDITING ) )
		{
		f_oStatusBar->message ( COLORS::D_FG_BRIGHTRED, _( "There is nothing to save." ) );
		return ( 0 );
		}
	m_lId = update();
	f_bModified = false;
	f_poMainControl->load();
	set_mode ( DOCUMENT::D_VIEW );
*/
	return ( 0 );
	M_EPILOG
	}

int HDataWindow::handler_requery ( int, void const* )
	{
	M_PROLOG
/* FIXME
	if ( f_eMode != D_NORMAL )
		{
		f_oStatusBar->message ( COLORS::D_FG_BRIGHTRED,
				_ ( "Finish your current operation first." ) );
		return ( 0 );
		}
	set_mode ( DOCUMENT::D_VIEW );
	f_poMainControl->load();
	refresh();
*/
	return ( 0 );
	M_EPILOG
	}

int HDataWindow::handler_cancel ( int, void const* )
	{
	M_PROLOG
/* FIXME
	mode_t l_eMode = f_eMode;
	if ( ( f_eMode != D_ADDING ) && ( f_eMode != D_EDITING ) )
		return ( 0 );
	cancel();
	set_mode ( DOCUMENT::D_VIEW );
	if ( ( l_eMode == D_ADDING ) && f_poMainControl )
		f_poMainControl->cancel_new();
	f_bModified = false;
	f_oStatusBar->refresh();
	f_oStatusBar->message ( COLORS::D_FG_BRIGHTRED, _ ( "Dropping all changes." ) );
*/
	return ( 0 );
	M_EPILOG
	}

bool HDataWindow::is_modified ( void ) const
	{
	return ( f_bModified );
	}

void HDataWindow::set_modified ( bool a_bModified )
	{
	M_PROLOG
	bool l_bModified = f_bModified;
	f_bModified = a_bModified;
	if ( ! l_bModified )
		f_oStatusBar->refresh();
	return;
	M_EPILOG
	}

}

}

