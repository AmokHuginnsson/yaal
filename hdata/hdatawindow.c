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

#define M_STANDART_SETUP	f_psResourcesArray [ l_iCtr ].f_iRow,\
						f_psResourcesArray [ l_iCtr ].f_iColumn,\
						f_psResourcesArray [ l_iCtr ].f_iHeight,\
						f_psResourcesArray [ l_iCtr ].f_iWidth,\
						f_psResourcesArray [ l_iCtr ].f_pcLabel

#define M_ATTRIBUTES_SETUP l_psAttr->f_iDisabledAttribute,\
						l_psAttr->f_iEnabledAttribute,\
						l_psAttr->f_iFocusedAttribute
						
int HDataWindow::init ( void )
	{
	M_PROLOG
	int l_iCtr = 0;
	char l_pcValue [ ] = "";
	char l_pcMask [ ] = D_DEFAULT_MASK;
	HDataControl * l_poDataControl = NULL;
	OAttributes l_sAttributes, * l_psAttr = & l_sAttributes;
	OEditControlResource l_sEditControlResource;
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
				if ( f_psResourcesArray [ l_iCtr ].f_pvControlSpecific )
					l_psECR = ( OEditControlResource * ) f_psResourcesArray [ l_iCtr ].f_pvControlSpecific;
				l_poDataControl = ( HDataControl * ) new HEditControl ( ( HWindow * ) this,
						M_STANDART_SETUP, l_psECR->f_iMaxStringSize, l_psECR->f_pcValue,
						l_psECR->f_pcMask, l_psECR->f_bReplace, l_psECR->f_bMultiLine,
						l_psECR->f_bPassword, l_psECR->f_iMaxHistoryLevel,
						M_ATTRIBUTES_SETUP );
				break;
				}
			case ( D_CONTROL_LIST ):
				{
				l_poDataControl = new HDataListControl ( this, this, M_STANDART_SETUP,
						M_ATTRIBUTES_SETUP );
				break;
				}
			case ( D_CONTROL_TREE ):
				{
				l_poDataControl = new HDataTreeControl ( this, this, M_STANDART_SETUP,
						M_ATTRIBUTES_SETUP );
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
		if ( f_psResourcesArray [ l_iCtr ].f_iFlags == D_CONTROL_MAIN )
			f_poMainControl = l_poDataControl;
		l_iCtr ++;
		}
	if ( f_poMainControl )f_poMainControl->populate ( );
	return ( 0 );
	M_EPILOG
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
	return ( 0 );
	M_EPILOG
	}

