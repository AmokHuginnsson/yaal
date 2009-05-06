/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hdatawindow.cxx - this file is integral part of `yaal' project.

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

#include <libintl.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "hcore/hcore.hxx"
#include "hdatawindow.hxx"
#include "hdatalistcontrol.hxx"
#include "hdatatreecontrol.hxx"
#include "hdataeditcontrol.hxx"
#include "hdatastatusbarcontrol.hxx"
#include "hdataprocess.hxx"

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
	f_bModified( false ), f_eDocumentMode( DOCUMENT::VIEW ), f_poMainControl( NULL ),
	f_psResourcesArray( a_psDataControlInfo ), f_poSyncStore( NULL ),
	f_oViewModeControls(), f_oEditModeControls(), f_poOwner( a_poOwner ),
	f_oDB( new HSQLDescriptor( a_poOwner->data_base() ) ),
	f_eMode( HSQLDescriptor::MODE::SELECT )
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
	register_postprocess_handler( KEY_CODES::ESC, NULL,
			&HDataWindow::handler_cancel );
	return;
	M_EPILOG
	}

HDataWindow::~HDataWindow( void )
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

int HDataWindow::init( void )
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
			case ( DATACONTROL_BITS::TYPE::EDIT ):
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
			case ( DATACONTROL_BITS::TYPE::LIST ):
				{
				l_sListControlResource.f_bCheckable = false;
				l_sListControlResource.f_bSortable = true;
				l_sListControlResource.f_bSearchable = true;
				l_sListControlResource.f_bDrawHeader = true;
				if ( f_psResourcesArray [ l_iCtr ].f_pvTypeSpecific )
					l_psLCR = static_cast<OListControlResource*>( f_psResourcesArray [ l_iCtr ].f_pvTypeSpecific );
				HDataListControl* l_poList = NULL;
				l_poDataControl = l_poList = new HDataListControl( this, M_SETUP_STANDARD );
				l_poList->set_flags( HListControl::flag_t( l_psLCR->f_bCheckable ? HListControl::FLAG::CHECKABLE : HListControl::FLAG::NONE )
						| ( l_psLCR->f_bSortable ? HListControl::FLAG::SORTABLE : HListControl::FLAG::NONE )
						| ( l_psLCR->f_bEditable ? HListControl::FLAG::EDITABLE : HListControl::FLAG::NONE )
						| ( l_psLCR->f_bDrawHeader ? HListControl::FLAG::DRAW_HEADER : HListControl::FLAG::NONE ),
						HListControl::FLAG::ALL );
				}
			break;
			case ( DATACONTROL_BITS::TYPE::TREE ):
				l_poDataControl = new HDataTreeControl( this, M_SETUP_STANDARD );
			break;
			case ( DATACONTROL_BITS::TYPE::COMBO ):
			break;
			case ( DATACONTROL_BITS::TYPE::DATE ):
			break;
			case ( DATACONTROL_BITS::TYPE::CHECK ):
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
			l_poDataControl->set_resource( &f_psResourcesArray[ l_iCtr ] );
			}
		switch ( f_psResourcesArray[ l_iCtr ].f_eRole )
			{
			case ( DATACONTROL_BITS::ROLE::MAIN ):
				{
				f_oDB->set_table( f_psResourcesArray[ l_iCtr ].f_pcTable );
				f_oDB->set_columns( f_psResourcesArray[ l_iCtr ].f_pcColumns );
				f_oDB->set_filter( f_psResourcesArray[ l_iCtr ].f_pcFilter );
				f_oDB->set_sort( f_psResourcesArray[ l_iCtr ].f_pcSort );
				l_poDataControl->set_dbd( f_oDB );
				f_poMainControl = l_poDataControl;
				f_oViewModeControls.add_tail( &l_poDataControl );
				l_poDataControl->enable( true );
				}
			break;
			case ( DATACONTROL_BITS::ROLE::DATA ):
				link( l_iCtr, l_poDataControl );
				f_oEditModeControls.add_tail( &l_poDataControl );
			break;
			case ( DATACONTROL_BITS::ROLE::FILTER ):
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
		f_poMainControl->process_input ( KEY_CODES::HOME );
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
	l_psCI->f_iPlacement = HDataListControl::HColumnInfo::ADD_AT_THE_END;
	l_psCI->f_pcName = l_pcName;
	l_psCI->f_iWidth = 1; 				/* width is awlays proportional */
	l_psCI->f_eAlign = HControl::BITS::ALIGN::LEFT;
	l_psCI->f_eType = TYPE::HSTRING;
	l_iParent = f_psResourcesArray [ a_iChild ].f_iParent;
	if ( f_psResourcesArray [ l_iParent ].f_eType == DATACONTROL_BITS::TYPE::LIST )
		{
		l_poPDC = dynamic_cast<HDataListControl*>( f_oControls.get_control_by_no( l_iParent + 1 /* 1 stands for offset caused by 'status bar' */ ) );
		if ( ! l_poPDC )
			M_THROW( "wrong control resource order",
					l_iParent );
		if ( f_psResourcesArray[ a_iChild ].f_psColumnInfo )
			l_psCI = f_psResourcesArray[ a_iChild ].f_psColumnInfo;
		l_poPDC->add_column( l_psCI->f_iPlacement, l_psCI->f_pcName,
				l_psCI->f_iWidth, l_psCI->f_eAlign, l_psCI->f_eType, a_poDataControl );
		}
	else
		M_THROW( "unknown parent type", f_psResourcesArray[ l_iParent ].f_eType );
	return;
	M_EPILOG
	}

void HDataWindow::set_mode( DOCUMENT::mode_t a_eMode )
	{
	M_PROLOG
	switch ( a_eMode )
		{
		case ( DOCUMENT::VIEW ):
		case ( DOCUMENT::EDIT ):
			{
			f_eDocumentMode = a_eMode;
			controls_t::iterator end = f_oEditModeControls.end();
			for ( controls_t::iterator it = f_oEditModeControls.begin(); it != end; ++ it )
				(*it)->enable( a_eMode == DOCUMENT::EDIT );
			end = f_oViewModeControls.end();
			for ( controls_t::iterator it = f_oViewModeControls.begin(); it != end; ++ it )
				(*it)->enable( a_eMode == DOCUMENT::VIEW );
			controls_t::iterator begin = a_eMode == DOCUMENT::EDIT ? f_oEditModeControls.begin() : f_oViewModeControls.begin();
			end = a_eMode == DOCUMENT::EDIT ? f_oEditModeControls.end() : f_oViewModeControls.end();
			if ( begin != end )
				(*begin)->set_focus();
			}
		break;
		default :
			M_THROW ( "unknown window mode", a_eMode );
		}
	return;
	M_EPILOG
	}

void HDataWindow::sync( HRecordSet::iterator it )
	{
	M_PROLOG
	M_ASSERT( f_poSyncStore );
	M_ASSERT( f_eDocumentMode == DOCUMENT::VIEW );
	int long l_iCount = f_poSyncStore->f_oItem.get_size();
	for ( int l_iCtr = 0; l_iCtr < l_iCount; l_iCtr ++ )
		f_poSyncStore->f_oItem[ l_iCtr ]( it[ l_iCtr ].raw() );
	if ( f_poSyncStore->f_iIdColNo >= 0 )
		f_poSyncStore->f_oItem.m_lId = lexical_cast<int>( it[ f_poSyncStore->f_iIdColNo ] );
	M_EPILOG
	}

void HDataWindow::sync( void )
	{
	M_PROLOG
	M_ASSERT( f_eDocumentMode == DOCUMENT::EDIT );
	int l_iCount = static_cast<int>( f_oEditModeControls.size() );
	for ( int l_iCtr = 0; l_iCtr < l_iCount; l_iCtr ++ )
		(*f_oDB)[ l_iCtr ] = f_oEditModeControls[ l_iCtr ]->get().get<HString const&>();
	return;
	M_EPILOG
	}

void HDataWindow::set_sync_store( ORowBuffer* a_poRB )
	{
	f_poSyncStore = a_poRB;
	return;
	}

int HDataWindow::handler_add_new( int, void const* )
	{
	M_PROLOG
	if ( f_eDocumentMode != DOCUMENT::VIEW )
		{
		f_oStatusBar->message( COLORS::FG_BRIGHTRED,
				_ ( "You cannot add new rocord now." ) );
		return ( 0 );
		}
	f_eMode = HSQLDescriptor::MODE::INSERT;
	if ( f_poMainControl )
		f_poMainControl->add_new();
	set_mode( DOCUMENT::EDIT );
	return ( 0 );
	M_EPILOG
	}

int HDataWindow::handler_edit( int, void const* )
	{
	M_PROLOG
	if ( f_eDocumentMode != DOCUMENT::VIEW )
		{
		f_oStatusBar->message( COLORS::FG_BRIGHTRED,
				_ ( "You cannot start editing of this record." ) );
		return ( 0 );
		}
	if ( ! f_oDB->get_size() )
		{
		f_oStatusBar->message( COLORS::FG_BRIGHTRED,
				_ ( "There is nothing to edit." ) );
		return ( 0 );
		}
	f_eMode = HSQLDescriptor::MODE::UPDATE;
	set_mode( DOCUMENT::EDIT );
	return ( 0 );
	M_EPILOG
	}

int HDataWindow::handler_delete( int, void const* )
	{
	M_PROLOG
	if ( f_eDocumentMode != DOCUMENT::VIEW )
		{
		f_oStatusBar->message( COLORS::FG_BRIGHTRED,
				_( "You cannot delete this record." ) );
		return ( 0 );
		}
	if ( ! f_oDB->get_size() )
		{
		f_oStatusBar->message( COLORS::FG_BRIGHTRED,
				_( "There is nothing to remove." ) );
		return ( 0 );
		}
	if ( f_poMainControl )
		{
		HString filter;
		filter.format( "id = %ld", f_poMainControl->get_current_id() );
		f_oDB->set_filter( filter );
		f_oDB->execute( HSQLDescriptor::MODE::DELETE );
		}
	f_poMainControl->load();
	return ( 0 );
	M_EPILOG
	}

int HDataWindow::handler_save( int, void const* )
	{
	M_PROLOG
	if ( f_eDocumentMode != DOCUMENT::EDIT )
		{
		f_oStatusBar->message( COLORS::FG_BRIGHTRED, _( "There is nothing to save." ) );
		return ( 0 );
		}
	if ( f_eMode == HSQLDescriptor::MODE::UPDATE )
		{
		HString filter;
		filter.format( "id = %ld", f_poMainControl->get_current_id() );
		f_oDB->set_filter( filter );
		}
	sync();
	HRecordSet::ptr_t rs = f_oDB->execute( f_eMode );
	if ( rs->get_errno() )
		f_oStatusBar->message( COLORS::FG_BRIGHTRED, rs->get_error() );
	else
		{
		f_bModified = false;
		set_mode( DOCUMENT::VIEW );
		f_poMainControl->load();
		}
	return ( 0 );
	M_EPILOG
	}

int HDataWindow::handler_requery( int, void const* )
	{
	M_PROLOG
	if ( f_eDocumentMode != DOCUMENT::VIEW )
		{
		f_oStatusBar->message( COLORS::FG_BRIGHTRED,
				_( "Finish your current operation first." ) );
		return ( 0 );
		}
	set_mode( DOCUMENT::VIEW );
	f_poMainControl->load();
	refresh();
	return ( 0 );
	M_EPILOG
	}

int HDataWindow::handler_cancel( int, void const* )
	{
	M_PROLOG
	if ( f_eDocumentMode != DOCUMENT::EDIT )
		return ( 0 );
	set_mode( DOCUMENT::VIEW );
	if ( ( f_eMode == HSQLDescriptor::MODE::INSERT ) && f_poMainControl )
		f_poMainControl->cancel_new();
	f_bModified = false;
	f_oStatusBar->refresh();
	f_oStatusBar->message( COLORS::FG_BRIGHTRED, _( "Dropping all changes." ) );
	return ( 0 );
	M_EPILOG
	}

bool HDataWindow::is_modified( void ) const
	{
	return ( f_bModified );
	}

void HDataWindow::set_modified( bool a_bModified )
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

