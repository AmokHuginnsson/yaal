/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hdatawindow.c - this file is integral part of `stdhapi' project.

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

#include "../config.h"

/* We need ncurses.h here because of KEY_HOME. */

#ifdef HAVE_NCURSES_H
#	include <ncurses.h>
#elif defined ( HAVE_NCURSES_NCURSES_H )
#	include <ncurses/ncurses.h>
#else /* HAVE_NCURSES_NCURSES_H */
#	error "No ncurses header available."
#endif /* not HAVE_NCURSES_NCURSES_H */

#include "../hcore/hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "hdatawindow.h"
#include "hdatalistcontrol.h"
#include "hdatatreecontrol.h"
#include "hdataeditcontrol.h"
#include "hdatastatusbarcontrol.h"

HDataWindow::HDataWindow ( const char * a_pcTitle, HDataBase * a_poDataBase,
		OResource * a_psDataControlInfo )
	: HWindow ( a_pcTitle ), HRecordSet ( a_poDataBase )
	{
	M_PROLOG
	f_bModified = false;
	f_poMainControl = NULL;
	f_poSyncStore = NULL;
	f_psResourcesArray = a_psDataControlInfo;
	M_REGISTER_POSTPROCESS_HANDLER ( D_KEY_COMMAND_('n'), NULL,
			HDataWindow::handler_add_new );
	M_REGISTER_POSTPROCESS_HANDLER ( D_KEY_COMMAND_('e'), NULL,
			HDataWindow::handler_edit );
	M_REGISTER_POSTPROCESS_HANDLER ( D_KEY_COMMAND_('d'), NULL,
			HDataWindow::handler_delete );
	M_REGISTER_POSTPROCESS_HANDLER ( D_KEY_COMMAND_('w'), NULL,
			HDataWindow::handler_save );
	M_REGISTER_POSTPROCESS_HANDLER ( D_KEY_CTRL_('r'), NULL,
			HDataWindow::handler_requery );
	M_REGISTER_POSTPROCESS_HANDLER ( D_KEY_ESC, NULL,
			HDataWindow::handler_cancel );
	return;
	M_EPILOG
	}

HDataWindow::~HDataWindow ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

#define M_SETUP_STANDART	f_psResourcesArray [ l_iCtr ].f_iRow,\
						f_psResourcesArray [ l_iCtr ].f_iColumn,\
						f_psResourcesArray [ l_iCtr ].f_iHeight,\
						f_psResourcesArray [ l_iCtr ].f_iWidth,\
						f_psResourcesArray [ l_iCtr ].f_pcLabel

#define M_SETUP_ATTRIBUTES l_psAttr->f_bDrawLabel,\
						l_psAttr->f_iDisabledAttribute,\
						l_psAttr->f_iEnabledAttribute,\
						l_psAttr->f_iFocusedAttribute

#define M_SETUP_COLUMN l_psCI->f_iPlacement,\
						l_psCI->f_pcName,\
						l_psCI->f_iWidth,\
						l_psCI->f_iAlign,\
						l_psCI->f_iType

int HDataWindow::init ( void )
	{
	M_PROLOG
	int l_iCtr = 0;
	char l_pcValue [ ] = "";
	char l_pcMask [ ] = D_MASK_DEFAULT;
	HDataControl * l_poDataControl = NULL;
	OAttributes l_sAttributes, * l_psAttr = & l_sAttributes;
	OEditControlResource l_sEditControlResource;
	OListControlResource l_sListControlResource;
/* ECR stands for EditControlResource */
	OEditControlResource * l_psECR = & l_sEditControlResource;
/* LCR stands for ListControlResource */
	OListControlResource * l_psLCR = & l_sListControlResource;
	l_sAttributes.f_bDrawLabel = true;
	l_sAttributes.f_iDisabledAttribute = -1;
	l_sAttributes.f_iEnabledAttribute = -1;
	l_sAttributes.f_iFocusedAttribute = -1;
	HWindow::init ( );
	while ( f_psResourcesArray [ l_iCtr ].f_pcLabel )
		{
		if ( f_psResourcesArray [ l_iCtr ].f_psAttributes )
			l_psAttr = f_psResourcesArray [ l_iCtr ].f_psAttributes;
		switch ( f_psResourcesArray [ l_iCtr ].f_iType )
			{
			case ( D_CONTROL_EDIT ):
				{
				l_sEditControlResource.f_iMaxStringSize = 127;
				l_sEditControlResource.f_pcValue = l_pcValue;
				l_sEditControlResource.f_pcMask = l_pcMask;
				l_sEditControlResource.f_bReplace = false;
				l_sEditControlResource.f_bRightAligned = false;
				l_sEditControlResource.f_bMultiLine = false;
				l_sEditControlResource.f_bPassword = false;
				l_sEditControlResource.f_iMaxHistoryLevel = 8;
				if ( f_psResourcesArray [ l_iCtr ].f_pvTypeSpecific )
					l_psECR = ( OEditControlResource * ) f_psResourcesArray [ l_iCtr ].f_pvTypeSpecific;
				l_poDataControl = ( HDataControl * ) new HDataEditControl ( this,
						M_SETUP_STANDART, l_psECR->f_iMaxStringSize, l_psECR->f_pcValue,
						l_psECR->f_pcMask, l_psECR->f_bReplace, l_psECR->f_bRightAligned,
						l_psECR->f_bMultiLine, l_psECR->f_bPassword,
						l_psECR->f_iMaxHistoryLevel,
						M_SETUP_ATTRIBUTES );
				break;
				}
			case ( D_CONTROL_LIST ):
				{
				l_sListControlResource.f_bCheckable = false;
				l_sListControlResource.f_bSortable = true;
				l_sListControlResource.f_bSearchable = true;
				l_sListControlResource.f_bDrawHeader = true;
				if ( f_psResourcesArray [ l_iCtr ].f_pvTypeSpecific )
					l_psLCR = ( OListControlResource * ) f_psResourcesArray [ l_iCtr ].f_pvTypeSpecific;
				l_poDataControl = new HDataListControl ( this, this, M_SETUP_STANDART,
						l_psLCR->f_bCheckable, l_psLCR->f_bSortable, l_psLCR->f_bSearchable,
						l_psLCR->f_bDrawHeader, M_SETUP_ATTRIBUTES );
				break;
				}
			case ( D_CONTROL_TREE ):
				{
				l_poDataControl = new HDataTreeControl ( this, this, M_SETUP_STANDART,
						M_SETUP_ATTRIBUTES );
				break;
				}
			case ( D_CONTROL_COMBO ):
				{
				break;
				}
			case ( D_CONTROL_DATE ):
				{
				break;
				}
			case ( D_CONTROL_CHECK ):
				{
				break;
				}
			default :
				{
				break;
				}
			}
		switch ( f_psResourcesArray [ l_iCtr ].f_iFlags )
			{
			case ( D_CONTROL_MAIN ):
				{
				f_oTable = f_psResourcesArray [ l_iCtr ].f_pcTable;
				f_oColumns = f_psResourcesArray [ l_iCtr ].f_pcColumns;
				f_oFilter = f_psResourcesArray [ l_iCtr ].f_pcFilter;
				f_oSort = f_psResourcesArray [ l_iCtr ].f_pcSort;
				f_poMainControl = l_poDataControl;
				f_oViewModeControls.add_tail ( & l_poDataControl );
				l_poDataControl->enable ( true );
				break;
				}
			case ( D_CONTROL_DATA ):
				{
				link ( l_iCtr, l_poDataControl );
				f_oEditModeControls.add_tail ( & l_poDataControl );
				break;
				}
			case ( D_CONTROL_FILTER ):
				{
				l_poDataControl->enable ( true );
				f_oViewModeControls.add_tail ( & l_poDataControl );
				break;
				}
			default :
				{
				M_THROW ( "unknown resource purpouse",
						f_psResourcesArray [ l_iCtr ].f_iFlags );
				break;
				}
			}
		l_iCtr ++;
		}
	f_poMainControl->set_focus ( );
	if ( f_poMainControl )
		{
		f_poMainControl->load ( );
		f_poMainControl->process_input ( KEY_HOME );
		}
	refresh ( );
	return ( 0 );
	M_EPILOG
	}

HStatusBarControl * HDataWindow::init_bar ( const char * a_pcLabel )
	{
	M_PROLOG
	return ( new HDataStatusBarControl ( this, a_pcLabel ) );
	M_EPILOG
	}

void HDataWindow::link ( int a_iChild, HDataControl * a_poDataControl )
	{
	M_PROLOG
	int l_iParent = 0;
	char l_pcName [ ] = "";
	HDataListControl * l_poPDC = NULL; /* PDC stands for ParentDataControl */
/* CI stands for ColumnInfo */
	OColumnInfo l_sColumnInfo, * l_psCI = & l_sColumnInfo;
	l_psCI->f_iPlacement = -1;		/* -1 means add at the end */
	l_psCI->f_pcName = l_pcName;
	l_psCI->f_iWidth = 1; 				/* width is awlays proportional */
	l_psCI->f_iAlign = D_ALIGN_LEFT;
	l_psCI->f_iType = D_TYPE_HSTRING;
	l_iParent = f_psResourcesArray [ a_iChild ].f_iParent;
	if ( f_psResourcesArray [ l_iParent ].f_iType == D_CONTROL_LIST )
		{
		l_poPDC = dynamic_cast < HDataListControl * > ( f_oControls [ l_iParent ] );
		if ( ! l_poPDC )
			M_THROW ( "wrong control resource order",
					l_iParent );
		if ( f_psResourcesArray [ a_iChild ].f_psColumnInfo )
			l_psCI = f_psResourcesArray [ a_iChild ].f_psColumnInfo;
		l_poPDC->add_column ( M_SETUP_COLUMN, a_poDataControl );
		}
	else M_THROW ( "unknown parent type",
			f_psResourcesArray [ l_iParent ].f_iType );
	return;
	M_EPILOG
	}

void HDataWindow::set_mode ( int a_iMode )
	{
	M_PROLOG
	int l_iCtr = 0, l_iCount = 0;
	switch ( a_iMode )
		{
		case ( D_MODE_VIEW ):
			{
			l_iCount = f_oEditModeControls.quantity ( );
			for ( l_iCtr = 0; l_iCtr < l_iCount; l_iCtr ++ )
				f_oEditModeControls [ l_iCtr ]->enable ( false );
			l_iCount = f_oViewModeControls.quantity ( );
			for ( l_iCtr = 0; l_iCtr < l_iCount; l_iCtr ++ )
				f_oViewModeControls [ l_iCtr ]->enable ( true );
			if ( l_iCount )
				f_oViewModeControls [ 0 ]->set_focus ( );
			break;
			}
		case ( D_MODE_EDIT ):
			{
			l_iCount = f_oViewModeControls.quantity ( );
			for ( l_iCtr = 0; l_iCtr < l_iCount; l_iCtr ++ )
				f_oViewModeControls [ l_iCtr ]->enable ( false );
			l_iCount = f_oEditModeControls.quantity ( );
			for ( l_iCtr = 0; l_iCtr < l_iCount; l_iCtr ++ )
				f_oEditModeControls [ l_iCtr ]->enable ( true );
			if ( l_iCount )
				f_oEditModeControls [ 0 ]->set_focus ( );
			break;
			}
		default :
			{
			M_THROW ( "unknown window mode", a_iMode );
			break;
			}
		}
	return;
	M_EPILOG
	}

void HDataWindow::sync ( void )
	{
	M_PROLOG
	int l_iCtr = 0, l_iCount = 0;
	HRecordSet::sync ( );
	if ( f_poSyncStore )
		{
		l_iCount = f_poSyncStore->get_size ( );
		if ( f_oValues.quantity ( ) >= l_iCount )
			for ( l_iCtr = 0; l_iCtr < l_iCount; l_iCtr ++ )
				( * f_poSyncStore ) [ l_iCtr ] = f_oValues [ l_iCtr ];
		( * f_poSyncStore ).m_lId = m_lId;
		}
	else if ( ( f_iMode == D_MODE_ADDING ) || ( f_iMode == D_MODE_EDITING ) )
		{
		l_iCount = f_oEditModeControls.quantity ( );
		if ( f_oValues.quantity ( ) >= l_iCount )
			for ( l_iCtr = 0; l_iCtr < l_iCount; l_iCtr ++ )
				f_oValues [ l_iCtr ] = ( HString & ) ( HInfo ) * ( HControl * ) f_oEditModeControls [ l_iCtr ];
		m_lId = f_poMainControl->get_current_id ( );
		}
	return;
	M_EPILOG
	}

void HDataWindow::set_sync_store ( HItem * a_poItem )
	{
	f_poSyncStore = a_poItem;
	return;
	}

int HDataWindow::handler_add_new ( int, void * )
	{
	M_PROLOG
	if ( f_iMode != D_MODE_NORMAL )
		{
		f_poStatusBar->message ( D_FG_BRIGHTRED,
				_ ( "You can not add new rocord now." ) );
		return ( 0 );
		}
	add_new ( );
	if ( f_poMainControl )f_poMainControl->add_new ( );
	set_mode ( D_MODE_EDIT );
	return ( 0 );
	M_EPILOG
	}

int HDataWindow::handler_edit ( int, void * )
	{
	M_PROLOG
	if ( f_iMode != D_MODE_NORMAL )
		{
		f_poStatusBar->message ( D_FG_BRIGHTRED,
				_ ( "You can not start editing of this record." ) );
		return ( 0 );
		}
	if ( ! f_iSetQuantity )
		{
		f_poStatusBar->message ( D_FG_BRIGHTRED,
				_ ( "There is nothing to edit." ) );
		return ( 0 );
		}
	set_mode ( D_MODE_EDIT );
	edit ( );
	return ( 0 );
	M_EPILOG
	}

int HDataWindow::handler_delete ( int, void * )
	{
	M_PROLOG
	if ( f_iMode != D_MODE_NORMAL )
		{
		f_poStatusBar->message ( D_FG_BRIGHTRED,
				_ ( "You can not delete this record." ) );
		return ( 0 );
		}
	if ( ! f_iSetQuantity )
		{
		f_poStatusBar->message ( D_FG_BRIGHTRED,
				_ ( "There is nothing to remove." ) );
		return ( 0 );
		}
	if ( f_poMainControl )
		m_lId = f_poMainControl->get_current_id ( );
	remove ( );
	f_poMainControl->load ( );
	return ( 0 );
	M_EPILOG
	}

int HDataWindow::handler_save ( int, void * )
	{
	M_PROLOG
	if ( ( f_iMode != D_MODE_ADDING ) && ( f_iMode != D_MODE_EDITING ) )
		{
		f_poStatusBar->message ( D_FG_BRIGHTRED, _( "There is nothing to save." ) );
		return ( 0 );
		}
	m_lId = update ( );
	f_bModified = false;
	f_poMainControl->load ( );
	set_mode ( D_MODE_VIEW );
	return ( 0 );
	M_EPILOG
	}

int HDataWindow::handler_requery ( int, void * )
	{
	M_PROLOG
	if ( f_iMode != D_MODE_NORMAL )
		{
		f_poStatusBar->message ( D_FG_BRIGHTRED,
				_ ( "Finish your current operation first." ) );
		return ( 0 );
		}
	set_mode ( D_MODE_VIEW );
	f_poMainControl->load ( );
	refresh ( );
	return ( 0 );
	M_EPILOG
	}

int HDataWindow::handler_cancel ( int, void * )
	{
	M_PROLOG
	int l_iMode = f_iMode;
	if ( ( f_iMode != D_MODE_ADDING ) && ( f_iMode != D_MODE_EDITING ) )
		return ( 0 );
	cancel ( );
	set_mode ( D_MODE_VIEW );
	if ( ( l_iMode == D_MODE_ADDING ) && f_poMainControl )
		f_poMainControl->cancel_new ( );
	f_bModified = false;
	f_poStatusBar->refresh ( );
	f_poStatusBar->message ( D_FG_BRIGHTRED, _ ( "Dropping all changes." ) );
	return ( 0 );
	M_EPILOG
	}

bool HDataWindow::is_modified ( void )
	{
	return ( f_bModified );
	}

void HDataWindow::set_modified ( bool a_bModified )
	{
	M_PROLOG
	bool l_bModified = f_bModified;
	f_bModified = a_bModified;
	if ( ! l_bModified )
		f_poStatusBar->refresh ( );
	return;
	M_EPILOG
	}

