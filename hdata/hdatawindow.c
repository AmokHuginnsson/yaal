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

#include "hdatawindow.h"
#include "hdatalistcontrol.h"
#include "hdatatreecontrol.h"

#include "../hcore/hexception.h"

HDataWindow::HDataWindow ( const char * a_pcTitle, HDataBase * a_poDataBase,
		OResource * a_psDataControlInfo )
	: HWindow ( a_pcTitle ), HRecordSet ( a_poDataBase )
	{
	M_PROLOG
	f_bModified = false;
	f_poMainControl = NULL;
	f_poSyncStore = NULL;
	f_psResourcesArray = a_psDataControlInfo;
	register_postprocess_handler ( D_KEY_COMMAND_('n'),
			( int ( HWindow::* ) ( int ) ) & HDataWindow::handler_add_new );
	register_postprocess_handler ( D_KEY_COMMAND_('e'),
			( int ( HWindow::* ) ( int ) ) & HDataWindow::handler_edit );
	register_postprocess_handler ( D_KEY_COMMAND_('d'),
			( int ( HWindow::* ) ( int ) ) & HDataWindow::handler_delete );
	register_postprocess_handler ( D_KEY_COMMAND_('w'),
			( int ( HWindow::* ) ( int ) ) & HDataWindow::handler_save );
	register_postprocess_handler ( D_KEY_CTRL_('r'),
			( int ( HWindow::* ) ( int ) ) & HDataWindow::handler_requery );
	register_postprocess_handler ( D_KEY_ESC,
			( int ( HWindow::* ) ( int ) ) & HDataWindow::handler_cancel );
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

#define M_SETUP_ATTRIBUTES l_psAttr->f_iDisabledAttribute,\
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
	char l_pcMask [ ] = D_DEFAULT_MASK;
	HDataControl * l_poDataControl = NULL;
	OAttributes l_sAttributes, * l_psAttr = & l_sAttributes;
	OEditControlResource l_sEditControlResource;
/* ECR stands for EditControlResource */
	OEditControlResource * l_psECR = & l_sEditControlResource;
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
				l_sEditControlResource.f_iMaxStringSize = 128;
				l_sEditControlResource.f_pcValue = l_pcValue;
				l_sEditControlResource.f_pcMask = l_pcMask;
				l_sEditControlResource.f_bReplace = false;
				l_sEditControlResource.f_bMultiLine = false;
				l_sEditControlResource.f_bPassword = false;
				l_sEditControlResource.f_iMaxHistoryLevel = 8;
				if ( f_psResourcesArray [ l_iCtr ].f_pvTypeSpecific )
					l_psECR = ( OEditControlResource * ) f_psResourcesArray [ l_iCtr ].f_pvTypeSpecific;
				l_poDataControl = ( HDataControl * ) new HEditControl ( ( HWindow * ) this,
						M_SETUP_STANDART, l_psECR->f_iMaxStringSize, l_psECR->f_pcValue,
						l_psECR->f_pcMask, l_psECR->f_bReplace, l_psECR->f_bMultiLine,
						l_psECR->f_bPassword, l_psECR->f_iMaxHistoryLevel,
						M_SETUP_ATTRIBUTES );
				break;
				}
			case ( D_CONTROL_LIST ):
				{
				l_poDataControl = new HDataListControl ( this, this, M_SETUP_STANDART,
						M_SETUP_ATTRIBUTES );
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
				f_poMainControl = l_poDataControl;
				f_oViewModeControls.add_tail ( l_poDataControl );
				l_poDataControl->enable ( true );
				break;
				}
			case ( D_CONTROL_DATA ):
				{
				link ( l_iCtr, l_poDataControl );
				f_oEditModeControls.add_tail ( l_poDataControl );
				break;
				}
			case ( D_CONTROL_FILTER ):
				{
				l_poDataControl->enable ( true );
				f_oViewModeControls.add_tail ( l_poDataControl );
				break;
				}
			default :
				{
				throw new HException ( __WHERE__, "unknown resource purpouse",
						f_psResourcesArray [ l_iCtr ].f_iFlags );
				break;
				}
			}
		f_psResourcesArray [ l_iCtr ].f_poDataControl = l_poDataControl;
		l_iCtr ++;
		}
	f_poMainControl->set_focus ( );
	if ( f_poMainControl )f_poMainControl->populate ( );
	refresh ( );
	return ( 0 );
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
		l_poPDC = ( HDataListControl * ) f_psResourcesArray [ l_iParent ].f_poDataControl;
		if ( ! l_poPDC )
			throw new HException ( __WHERE__, "wrong control resource order",
					l_iParent );
		if ( f_psResourcesArray [ a_iChild ].f_psColumnInfo )
			l_psCI = f_psResourcesArray [ a_iChild ].f_psColumnInfo;
		l_poPDC->add_column ( M_SETUP_COLUMN, a_poDataControl );
		}
	else throw new HException ( __WHERE__, "unknown parent type",
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
			throw new HException ( __WHERE__, "unknown window mode", a_iMode );
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
		l_iCount = f_poSyncStore->quantity ( );
		for ( l_iCtr = 0; l_iCtr < l_iCount; l_iCtr ++ )
			( * f_poSyncStore ) [ l_iCtr ] = f_oValues [ l_iCtr ];
		}
	return;
	M_EPILOG
	}

void HDataWindow::set_sync_store ( HInfoList * a_poInfoList )
	{
	f_poSyncStore = a_poInfoList;
	return;
	}

int HDataWindow::handler_add_new ( int )
	{
	M_PROLOG
	return ( 0 );
	M_EPILOG
	}

int HDataWindow::handler_edit ( int )
	{
	M_PROLOG
	set_mode ( D_MODE_EDIT );
	return ( 0 );
	M_EPILOG
	}

int HDataWindow::handler_delete ( int )
	{
	M_PROLOG
	return ( 0 );
	M_EPILOG
	}

int HDataWindow::handler_save ( int )
	{
	M_PROLOG
	return ( 0 );
	M_EPILOG
	}

int HDataWindow::handler_requery ( int )
	{
	M_PROLOG
	return ( 0 );
	M_EPILOG
	}

int HDataWindow::handler_cancel ( int )
	{
	M_PROLOG
	set_mode ( D_MODE_VIEW );
	return ( 0 );
	M_EPILOG
	}

