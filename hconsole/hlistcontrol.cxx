/*
---          `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski           ---

	hlistcontrol.cxx - this file is integral part of `yaal' project.

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

#include <cstring>
#include <cctype>
#include <libintl.h>

#include "hcore/hexception.h"
M_VCSID ( "$Id$" )
#include "hcore/hlog.h"
#include "hlistcontrol.h"
#include "hconsole.h"

using namespace yaal::hcore;

namespace yaal
{

namespace hconsole
{

HListControl::HColumnInfo::HColumnInfo ( void )
	: f_bDescending ( false ), f_iWidthRaw ( 0 ), f_iWidth ( 0 ), f_eAlign ( BITS::ALIGN::D_LEFT ),
	f_iShortcutIndex ( 0 ), f_cShortcut ( 0 ), f_eType ( D_HSTRING ), f_oName ( ),
	f_poControl ( NULL )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HListControl::HColumnInfo::~HColumnInfo ( void )
	{
	M_PROLOG
	f_poControl = NULL;
	return;
	M_EPILOG
	}

HListControl::HColumnInfo::HColumnInfo ( HColumnInfo const & a_roColumnInfo )
	: f_bDescending ( false ), f_iWidthRaw ( 0 ), f_iWidth ( 0 ), f_eAlign ( BITS::ALIGN::D_LEFT ),
	f_iShortcutIndex ( 0 ), f_cShortcut ( 0 ), f_eType ( D_HSTRING ), f_oName ( ),
	f_poControl ( NULL )
	{
	M_PROLOG
	( * this ) = a_roColumnInfo;
	return;
	M_EPILOG
	}

HListControl::HColumnInfo & 
	HListControl::HColumnInfo::operator = ( HColumnInfo const & a_roColumnInfo )
	{
	M_PROLOG
	if ( this != & a_roColumnInfo )
		{
		f_bDescending = a_roColumnInfo.f_bDescending;
		f_iWidthRaw = a_roColumnInfo.f_iWidthRaw;
		f_iWidth = a_roColumnInfo.f_iWidth;
		f_eType = a_roColumnInfo.f_eType;
		f_eAlign = a_roColumnInfo.f_eAlign;
		f_iShortcutIndex = a_roColumnInfo.f_iShortcutIndex;
		f_cShortcut = a_roColumnInfo.f_cShortcut;
		f_oName = a_roColumnInfo.f_oName;
		f_poControl = a_roColumnInfo.f_poControl;
		}
	return ( * this );
	M_EPILOG
	}

HListControl::HListControl ( HWindow * a_poParent, int a_iRow, int a_iColumn,
		int a_iHeight, int a_iWidth, char const * a_pcLabel, bool a_bCheckable,
		bool a_bSortable, bool a_bSearchable, bool a_bDrawHeader, bool a_bDrawLabel,
		int a_iDisabledAttribute, int a_iEnabledAttribute,
		int a_iFocudesAttribute )
						: HControl ( a_poParent, a_iRow, a_iColumn, a_iHeight, a_iWidth,
								a_pcLabel, a_bDrawLabel, a_iDisabledAttribute,
								a_iEnabledAttribute, a_iFocudesAttribute ),
							HSearchableControl ( a_bSearchable ),
	f_bCheckable ( a_bCheckable ), f_bSortable ( a_bSortable ),
	f_bDrawHeader ( a_bDrawHeader ), f_bEditable ( false ),
	f_iControlOffset ( 0 ), f_iCursorPosition ( 0 ), f_iSumForOne ( 0 ),
	f_oHeader ( ),
	f_iSortColumn ( - 1 ),
	f_oList ( ), f_oFirstVisibleRow ( ), f_sMatch ( )
	{
	M_PROLOG
	HListControl::refresh ( );
	return;
	M_EPILOG
	}

HListControl::~HListControl ( void )
	{
	return;
	}

void HListControl::refresh ( void )
	{
	M_PROLOG
	int l_iCtr = 0;
	int l_iCtrLoc = 0;
	int l_iColumnOffset = 0;
	int l_iTmp = 0;
	int l_iColumns = f_oHeader.size ( );
	int l_iHR = f_bDrawHeader ? 1 : 0; /* HR stands for header row */
	int l_iSize = f_oList.size ( );
	double l_dScaled = 0;
	HItem l_oItem ( l_iColumns );
	HColumnInfo * l_poColumnInfo = NULL;
	l_iTmp = f_iWidthRaw;
	if ( f_bFocused )
		curs_set ( CURSOR::D_INVISIBLE );
	draw_label ( );
	if ( ! f_iSumForOne )
		return;
	if ( f_iWidthRaw != l_iTmp )
		recalculate_column_widths ( );
/* we need to decrement f_iHeightRaw because we have additional row, 
 * the list control header */
	if ( f_bDrawHeader )
		f_iHeightRaw --;
	f_oVarTmpBuffer.hs_realloc ( f_iWidthRaw + 1 );
	if ( l_iSize > 0 )
		{
		item_list_t::HIterator it = f_oFirstVisibleRow;
		for ( l_iCtr = 0;
					l_iCtr < ( l_iSize > f_iHeightRaw ? f_iHeightRaw : l_iSize );
					++ l_iCtr, ++ it )
			{
			l_iColumnOffset = 0;
			l_oItem = *it;
			for ( l_iCtrLoc = 0; l_iCtrLoc < l_iColumns; l_iCtrLoc ++ )
				{
				l_poColumnInfo = & f_oHeader [ l_iCtrLoc ];
				if ( l_poColumnInfo->f_iWidthRaw )
					{
					f_oVarTmpBuffer [ 0 ] = 0;
					switch ( l_poColumnInfo->f_eType )
						{
						case ( D_LONG_INT ):
							f_oVarTmpBuffer = l_oItem [ l_iCtrLoc ].get < int long > ( );
						break;
						case ( D_DOUBLE ):
							f_oVarTmpBuffer = l_oItem [ l_iCtrLoc ].get < double > ( );
						break;
						case ( D_HSTRING ):
							f_oVarTmpBuffer = l_oItem [ l_iCtrLoc ].get < HString const & > ( );
						break;
						case ( D_HTIME ):
							f_oVarTmpBuffer = static_cast < char const * > ( l_oItem [ l_iCtrLoc ].get < HTime const & > ( ) );
						break;
						default :
							M_THROW ( "unknown type", l_poColumnInfo->f_eType );
						}
					l_iTmp = f_oVarTmpBuffer.get_length ( );
					switch ( l_poColumnInfo->f_eAlign )
						{
						case ( BITS::ALIGN::D_LEFT ):
							{
							if ( l_iTmp < l_poColumnInfo->f_iWidthRaw )
								f_oVarTmpBuffer.fill ( '_', l_poColumnInfo->f_iWidthRaw - l_iTmp, l_iTmp );
							f_oVarTmpBuffer [ l_poColumnInfo->f_iWidthRaw ] = 0;
							}
						break;
						case ( BITS::ALIGN::D_CENTER ):
							{
							if ( l_iTmp > l_poColumnInfo->f_iWidthRaw )
								f_oVarTmpBuffer = f_oVarTmpBuffer.right (
										l_poColumnInfo->f_iWidthRaw );
							else if ( l_iTmp < l_poColumnInfo->f_iWidthRaw )
								{
								memmove ( f_oVarTmpBuffer.raw ( )
										+ ( l_poColumnInfo->f_iWidthRaw - l_iTmp ) / 2, 
										f_oVarTmpBuffer, l_iTmp + 1 );
								f_oVarTmpBuffer.fill ( '_',	( l_poColumnInfo->f_iWidthRaw - l_iTmp ) / 2 );
								l_iTmp = f_oVarTmpBuffer.get_length ( );
								f_oVarTmpBuffer.fillz ( '_', l_poColumnInfo->f_iWidthRaw - l_iTmp, l_iTmp );
								}
							}
						break;
						case ( BITS::ALIGN::D_RIGHT ):
							{
							if ( l_iTmp > l_poColumnInfo->f_iWidthRaw )
								f_oVarTmpBuffer = f_oVarTmpBuffer.right (
										l_poColumnInfo->f_iWidthRaw );
							else if ( l_iTmp < l_poColumnInfo->f_iWidthRaw )
								{
								memmove ( f_oVarTmpBuffer.raw ( )
										+ ( l_poColumnInfo->f_iWidthRaw - l_iTmp ) - 1, 
										f_oVarTmpBuffer, l_iTmp + 1 );
								f_oVarTmpBuffer.fill ( '_', ( l_poColumnInfo->f_iWidthRaw - l_iTmp ) - 1 );
								}
							}
						break;
						default :
							M_THROW ( "unknown align", l_poColumnInfo->f_eAlign );
						}
					if ( l_iCtr == f_iCursorPosition )
						{
						if ( l_oItem.m_bChecked )
							set_attr ( ! f_bEnabled
									? ( ! f_bFocused ? ~ ( f_uiFocusedAttribute >> 8 )
										: ~ ( f_uiEnabledAttribute >> 8 ) )
									: ~ ( f_uiDisabledAttribute >> 8 ) );
						else
							set_attr ( f_bEnabled ? ( f_bFocused ? ~f_uiFocusedAttribute
										: ~ f_uiEnabledAttribute ) : ~ f_uiDisabledAttribute );
						}
					else
						{
						if ( l_oItem.m_bChecked )
							set_attr ( f_bEnabled
									? ( f_bFocused ? ~ ( f_uiFocusedAttribute >> 8 )
										: ~ ( f_uiEnabledAttribute >> 8 ) )
									: ~ ( f_uiDisabledAttribute >> 8 ) );
						else
							set_attr_data ( );
						}
					M_ENSURE ( c_mvprintf ( f_iRowRaw + l_iCtr + l_iHR,
								f_iColumnRaw + l_iColumnOffset, f_oVarTmpBuffer	) != C_ERR );
					if ( f_bSearchActived )
						highlight ( f_iRowRaw + l_iCtr + l_iHR,
								f_iColumnRaw + l_iColumnOffset, f_sMatch.f_iMatchNumber,
								( it == f_sMatch.f_oCurrentMatch ) &&
								( l_iCtrLoc == f_sMatch.f_iColumnWithMatch ) );
					l_iColumnOffset += l_poColumnInfo->f_iWidthRaw;
					}
				if ( l_iCtr == f_iCursorPosition )
					{
					if ( l_poColumnInfo->f_poControl )
						l_poColumnInfo->f_poControl->set ( l_oItem [ l_iCtrLoc ] );
					}
				}
			}
		}
	l_iColumnOffset = 0;
	set_attr_data ( );
	f_oVarTmpBuffer.fillz ( '.', f_iWidthRaw );
	for ( ; l_iCtr < f_iHeightRaw; l_iCtr ++ )
		M_ENSURE ( c_mvprintf ( f_iRowRaw + l_iCtr + l_iHR, f_iColumnRaw,
					f_oVarTmpBuffer ) != C_ERR );
	for ( l_iCtr = 0; l_iCtr < l_iColumns; l_iCtr ++ )
		{
		l_poColumnInfo = & f_oHeader [ l_iCtr ];
		if ( l_poColumnInfo->f_iWidthRaw )
			{
			if ( f_bDrawHeader )
				{
				f_oVarTmpBuffer = l_poColumnInfo->f_oName;
				set_attr_label ( );
				f_oVarTmpBuffer.format ( "%%-%ds",
							l_poColumnInfo->f_iWidthRaw );
				M_ENSURE ( c_mvprintf ( f_iRowRaw, f_iColumnRaw + l_iColumnOffset,
							f_oVarTmpBuffer, static_cast < char const * const > (
								l_poColumnInfo->f_oName.left (
									l_poColumnInfo->f_iWidthRaw ) ) ) != C_ERR );
				set_attr_shortcut ( );
				M_ENSURE ( c_mvprintf ( f_iRowRaw,
							f_iColumnRaw + l_iColumnOffset + l_poColumnInfo->f_iShortcutIndex,
							"%c", l_poColumnInfo->f_cShortcut ) != C_ERR );
				if ( f_iSortColumn == l_iCtr )
					M_ENSURE ( c_mvprintf ( f_iRowRaw,
								f_iColumnRaw + l_iColumnOffset
								+ l_poColumnInfo->f_iWidthRaw - 2,
								"%c", l_poColumnInfo->f_bDescending ? '^' : 'v' ) != C_ERR );
				}
			l_iColumnOffset += l_poColumnInfo->f_iWidthRaw;
			if ( l_iCtr < l_iColumns )
				{
				set_attr ( f_uiDisabledAttribute );
				for ( l_iCtrLoc = 0; l_iCtrLoc < ( f_iHeightRaw + l_iHR );
						l_iCtrLoc ++ )
					{
					M_ENSURE ( c_move ( f_iRowRaw + l_iCtrLoc,
								f_iColumnRaw + l_iColumnOffset - 1 ) != C_ERR );
					M_ENSURE ( c_addch ( GLYPHS::D_VERTICAL_LINE ) != C_ERR );
					}
				}
			}
		}
	set_attr ( ! f_bEnabled ?
			( ! f_bFocused ? f_uiFocusedAttribute : f_uiEnabledAttribute )
			: f_uiDisabledAttribute );
	if ( l_iSize )
		{
		if ( f_iControlOffset )
			{
			M_ENSURE ( c_move ( f_iRowRaw + l_iHR,
						f_iColumnRaw + l_iColumnOffset - 1 ) != C_ERR );
			M_ENSURE ( c_addch ( GLYPHS::D_UP_ARROW ) != C_ERR );
			}
		if ( ( l_iSize - f_iControlOffset ) > f_iHeightRaw )
			{
			M_ENSURE ( c_move ( f_iRowRaw + f_iHeightRaw - ( 1 - l_iHR ),
						f_iColumnRaw + l_iColumnOffset - 1 ) != C_ERR );
			M_ENSURE ( c_addch ( GLYPHS::D_DOWN_ARROW ) != C_ERR );
			}
		l_dScaled = f_iHeightRaw - 3;
		l_dScaled *= static_cast < double > (
				f_iControlOffset + f_iCursorPosition );
		l_dScaled /= static_cast < double > ( l_iSize );
		M_ENSURE ( c_mvprintf (
					f_iRowRaw + static_cast < int > ( l_dScaled + 1.5 + l_iHR ),
					f_iColumnRaw + l_iColumnOffset - 1, "#" ) != C_ERR );
		}
	return;
	M_EPILOG
	}

void HListControl::handle_key_page_up ( void )
	{
	if ( ! f_iCursorPosition )
		{
		if ( f_iControlOffset )
			{
			f_iControlOffset -= f_iHeightRaw;
			f_iControlOffset ++;
			}
		else
			bell ( );
		if ( f_iControlOffset < 0 )
			f_iControlOffset = 0;
		}
	else
		f_iCursorPosition = 0;
	return;
	}

void HListControl::handle_key_page_down ( void )
	{
	int l_iSize = f_oList.size ( );
	if ( l_iSize >= f_iHeightRaw )
		{
		if ( f_iCursorPosition == ( f_iHeightRaw - 1 ) )
			{
			if ( f_iControlOffset >= ( l_iSize - f_iHeightRaw ) )
				bell ( );
			f_iControlOffset += f_iHeightRaw;
			f_iControlOffset --;
			if ( f_iControlOffset > ( l_iSize - f_iHeightRaw ) )
				f_iControlOffset = l_iSize - f_iHeightRaw;
			}
		else
			f_iCursorPosition = f_iHeightRaw - 1;
		}
	else
		{
		if ( f_iCursorPosition == ( l_iSize - 1 ) )
			bell ( );
		f_iCursorPosition = l_iSize - 1;
		}
	return;
	}

void HListControl::handle_key_up ( void )
	{
	if ( ( f_iControlOffset + f_iCursorPosition ) > 0 )
		{
		if ( f_iCursorPosition > 0 )
			f_iCursorPosition --;
		else if ( f_iControlOffset > 0 )
			{
			f_iControlOffset --;
			}
		}
	else
		bell ( );
	return;
	}

void HListControl::handle_key_home ( void )
	{
	f_iCursorPosition = 0;
	f_iControlOffset = 0;
	return;
	}

void HListControl::handle_key_end ( void )
	{
	int l_iSize = f_oList.size ( );
	if ( l_iSize >= f_iHeightRaw )
		{
		f_iCursorPosition = f_iHeightRaw - 1;
		f_iControlOffset = l_iSize - f_iHeightRaw;
		}
	else
		f_iCursorPosition = l_iSize - 1;
	return;
	}

void HListControl::handle_key_down ( void )
	{
	if ( ( f_iCursorPosition + f_iControlOffset ) < ( f_oList.size ( ) - 1 ) )
		{
		f_iCursorPosition ++;
		if ( f_iCursorPosition >= f_iHeightRaw )
			{
			f_iCursorPosition = f_iHeightRaw - 1;
			f_iControlOffset ++;
			}
		}
	else
		bell ( );
	return;
	}

void HListControl::handle_key_ctrl_n ( void )
	{
	if ( f_bBackwards )
		go_to_match_previous ( );
	else
		go_to_match ( );
	return;
	}

void HListControl::handle_key_ctrl_p ( void )
	{
	if ( f_bBackwards )
		go_to_match ( );
	else
		go_to_match_previous ( );
	return;
	}

void HListControl::handle_key_space ( void )
	{
	item_list_t::HIterator it = f_oFirstVisibleRow;
	for ( int i = 0; i < f_iCursorPosition; ++ i, ++ it )
		;
	if ( f_bCheckable && ! f_oList.empty ( ) )
		it->m_bChecked = ! it->m_bChecked;
	return;
	}

void HListControl::handle_key_tab ( void )
	{
	f_bFocused = false;	/* very  */
	refresh ( );				/* magic */
	return;
	}

int HListControl::process_input ( int a_iCode )
	{
	M_PROLOG
	int l_iCtr = 0;
	int l_iErrorCode = 0;
	a_iCode = HControl::process_input ( a_iCode );
	switch ( a_iCode )
		{
		case ( KEY_CODES::D_PAGE_UP ):		handle_key_page_up ( );		break;
		case ( KEY_CODES::D_PAGE_DOWN ):	handle_key_page_down ( );	break;
		case ( KEY_CODES::D_UP ):					handle_key_up ( );				break;
		case ( KEY_CODES::D_HOME ):				handle_key_home ( );			break;
		case ( KEY_CODES::D_END ):				handle_key_end ( );				break;
		case ( KEY_CODES::D_DOWN ):				handle_key_down ( );			break;
		case ( KEY < 'n' >::ctrl ):				handle_key_ctrl_n ( );		break;
		case ( KEY < 'p' >::ctrl ):				handle_key_ctrl_p ( );		break;
		case ( ' ' ):											handle_key_space ( );			break;
		case ( '\t' ):										handle_key_tab ( );
/* there is no break in previous `case ( ):', because this list must give up
 * its focus and be refreshed and parent window must give focus
 * to another control */
		default :
			{
			l_iErrorCode = f_oHeader.size ( );
			for ( l_iCtr = 0; l_iCtr < l_iErrorCode; l_iCtr ++ )
				if ( tolower ( a_iCode ) == tolower ( f_oHeader [ l_iCtr ].f_cShortcut ) )
					break;
			if ( l_iCtr < l_iErrorCode )
				{
				l_iErrorCode = 0;
				sort_by_column ( l_iCtr,
						a_iCode == tolower ( a_iCode ) ? OListBits::D_ASCENDING : OListBits::D_DESCENDING );
				}
			else
				l_iErrorCode = a_iCode;
			}
		}
	a_iCode = l_iErrorCode;
	if ( ! l_iErrorCode )
		{
		refresh ( );
		f_poParent->status_bar ( )->message ( COLORS::D_FG_LIGHTGRAY, "" );
		}
	return ( a_iCode );
	M_EPILOG
	}

void HListControl::add_column ( int const & a_riColumn, char const * a_pcName,
		int const & a_riWidth, BITS::ALIGN::align_t const & a_reAlign, const type_t & a_reType, 
		HControl * a_poControl )
	{
	M_PROLOG
	int l_iShortcutIndex = 0;
	HColumnInfo l_oColumnInfo;
	int l_iSize = f_oList.size ( );
	if ( l_iSize )
		M_THROW ( "can not add new column when list not empty", l_iSize );
	f_oVarTmpBuffer = a_pcName;
	l_iShortcutIndex = f_oVarTmpBuffer.find ( '&' );
	if ( l_iShortcutIndex > -1 )
		{
		f_oVarTmpBuffer [ l_iShortcutIndex ] = 0;
		f_oVarTmpBuffer += a_pcName + l_iShortcutIndex + 1;
		}
	else
		l_iShortcutIndex = 0;
	f_iSumForOne += a_riWidth;
	l_oColumnInfo.f_iWidth = a_riWidth;
	l_oColumnInfo.f_eType = a_reType;
	l_oColumnInfo.f_eAlign = a_reAlign;
	l_oColumnInfo.f_iShortcutIndex = l_iShortcutIndex;
	l_oColumnInfo.f_cShortcut = f_oVarTmpBuffer [ l_iShortcutIndex ];
	l_oColumnInfo.f_oName = f_oVarTmpBuffer;
	l_oColumnInfo.f_poControl = a_poControl;
	f_oHeader.add_at ( a_riColumn, & l_oColumnInfo );
	recalculate_column_widths ( );
	refresh ( );
	return;
	M_EPILOG
	}

void HListControl::add_tail ( HItem const & a_roItem )
	{
	M_PROLOG
	f_oList.push_back ( a_roItem );
	int l_iSize = f_oList.size ( );
	if ( l_iSize > f_iHeightRaw )
		{
		f_iCursorPosition = f_iHeightRaw - 1;
		f_iControlOffset = l_iSize - f_iHeightRaw;
		if ( f_oFirstVisibleRow != f_oList.end ( ) )
			++ f_oFirstVisibleRow;
		}
	else
		f_iCursorPosition = l_iSize - 1;
	if ( f_oFirstVisibleRow == f_oList.end ( ) )
		f_oFirstVisibleRow = f_oList.begin ( );
	n_bNeedRepaint = true;
	M_EPILOG
	}

#if 0
HItem & HListControl::add_orderly ( HItem & a_roItem, sort_order_t a_eOrder )
	{
	M_PROLOG
	HItem * l_poDummy = NULL;
	l_poDummy = & HList < HItem >::add_orderly ( a_roItem, a_eOrder );
	f_iCursorPosition = 0;
	f_iControlOffset = 0;
	f_oFirstVisibleRow = f_poHook;
	f_poSelected = f_poHook;
	n_bNeedRepaint = true;
	return ( * l_poDummy );
	M_EPILOG
	}
#endif


int HListControl::set_focus ( char a_cShorcut )
	{
	M_PROLOG
	return ( HControl::set_focus ( a_cShorcut ) );
	M_EPILOG
	}

void HListControl::recalculate_column_widths ( void )
	{
	M_PROLOG
	int l_iCtr = 0;
	int l_iCtrLoc = 0;
	int l_iColumns = 0;
	int l_iColumnOffset = 0;
	int l_iNewWidth = 0;
	l_iColumns = f_oHeader.size ( );
	for ( l_iCtr = 0; l_iCtr < l_iColumns; l_iCtr ++ )
		{
		l_iNewWidth = f_oHeader [ l_iCtr ].f_iWidth;
		if ( l_iNewWidth )
			{
			if ( ! f_iSumForOne )
				M_THROW ( "width of all columns equals 0", f_iSumForOne );
			l_iNewWidth *= f_iWidthRaw;
			l_iNewWidth /= f_iSumForOne;
			l_iCtrLoc = l_iCtr; /* last one with non zero width */
			f_oHeader [ l_iCtr ].f_iWidthRaw = l_iNewWidth;
			l_iColumnOffset += l_iNewWidth;
			}
		}
	/* last column with non zero width should fill space */
	l_iColumnOffset -= f_oHeader [ l_iCtrLoc ].f_iWidthRaw;
	f_oHeader [ l_iCtrLoc ].f_iWidthRaw = ( f_iWidthRaw - l_iColumnOffset );
	return;
	M_EPILOG
	}

#if 0
OListBits::status_t HListControl::remove_element ( treatment_t const & a_eFlag, HItem * * a_ppoItem )
	{
	M_PROLOG
	bool l_bFlag = true;
	status_t l_eError = D_OK;
	if ( f_iControlOffset
			&& ( ( f_iControlOffset + f_iHeightRaw ) == l_iSize ) )
		{
		f_iControlOffset --;
		to_head ( f_oFirstVisibleRow );
		}
	else if ( f_iCursorPosition && ( f_iCursorPosition == ( l_iSize - 1 ) ) )
		f_iCursorPosition --;
	else
		l_bFlag = false;
	if ( f_poSelected == f_oFirstVisibleRow )
		to_tail ( f_oFirstVisibleRow );
	n_bNeedRepaint = true;
	l_eError = HList < HItem > ::remove_element ( a_eFlag, a_ppoItem );
	if ( l_bFlag )
		to_head ( );
	refresh ( );
	return ( l_eError );
	M_EPILOG
	}
#endif

#if 0
OListBits::status_t HListControl::remove_tail ( treatment_t const & a_eFlag, HItem * * a_ppoItem )
	{
	M_PROLOG
	status_t l_eError = D_OK;
	if ( f_iControlOffset
			&& ( ( f_iControlOffset + f_iHeightRaw ) == l_iSize )  )
		{
		f_iControlOffset --;
		to_head ( f_oFirstVisibleRow );
		if ( f_iCursorPosition < ( f_iHeightRaw - 1 ) )
			{
			f_iCursorPosition ++;
			to_tail ( );
			}
		}
	else if ( f_iCursorPosition && ( f_iCursorPosition == ( l_iSize - 1 ) ) )
		f_iCursorPosition --;
	n_bNeedRepaint = true;
	l_eError = HList < HItem > ::remove_tail ( a_eFlag, a_ppoItem );
	if ( is_enabled ( ) )
		refresh ( );
	return ( l_eError );
	M_EPILOG
	}
#endif

namespace
{

struct OProgressHelper
	{
	int long f_lComparedItems;
	int long f_iSize;
	HWindow * f_poWindow;
	};

class CompareListControlItems
	{
	OListBits::sort_order_t f_eOrder;
	int f_iSortColumn;
	type_t f_eType;
	OProgressHelper & f_roProgressHelper;
public:
	CompareListControlItems ( OListBits::sort_order_t a_eOrder, int a_iColumn, type_t a_eType, OProgressHelper & a_roProgressHelper )
		: f_eOrder ( a_eOrder ), f_iSortColumn ( a_iColumn ), f_eType ( a_eType ), f_roProgressHelper ( a_roProgressHelper ) { }
	bool operator ( ) ( HListControl::item_list_t::HIterator const &, HListControl::item_list_t::HIterator const & ) const;
	};

bool CompareListControlItems::operator ( ) ( HListControl::item_list_t::HIterator const & a_oLeft,
		HListControl::item_list_t::HIterator const & a_oRight ) const
	{
	M_PROLOG
	double l_dDifference = 0;
	HListControl::item_list_t::HIterator const & l_oLeft = f_eOrder == OListBits::D_ASCENDING ? a_oLeft : a_oRight;
	HListControl::item_list_t::HIterator const & l_oRight = f_eOrder == OListBits::D_ASCENDING ? a_oRight : a_oLeft;
	HInfo const & l_roLeftInfo = ( *l_oLeft ) [ f_iSortColumn ];
	HInfo const & l_roRightInfo = ( *l_oRight ) [ f_iSortColumn ];
	++ f_roProgressHelper.f_lComparedItems;
	if ( ( f_roProgressHelper.f_iSize > 1024 ) && ! ( f_roProgressHelper.f_lComparedItems % 1024 ) )
		f_roProgressHelper.f_poWindow->status_bar ( )->update_progress ( static_cast < double > ( f_roProgressHelper.f_lComparedItems ) );
	switch ( f_eType )
		{
		case ( D_LONG_INT ):
			return ( l_roLeftInfo.get< long > ( ) > l_roRightInfo.get < long > ( ) );
		case ( D_DOUBLE ):
			l_dDifference = l_roLeftInfo.get < double > ( ) > l_roRightInfo.get < double > ( );
		break;
		case ( D_HSTRING ):
			return ( strcasecmp ( l_roLeftInfo.get < HString const & > ( ),
					 l_roRightInfo.get < HString const & > ( ) ) > 0 );
		case ( D_HTIME ):
			l_dDifference = static_cast < time_t > ( l_roLeftInfo.get < HTime const & > ( ) ) > static_cast < time_t > ( l_roRightInfo.get < HTime const & > ( ) );
		break;
		default:
			break;
		}
	return ( l_dDifference > 0 ? 1 : ( l_dDifference < 0 ? - 1 : 0 ) );
	M_EPILOG
	}

}

void HListControl::sort_by_column ( int a_iColumn, OListBits::sort_order_t a_eOrder )
	{
	M_PROLOG
	if ( ! f_bSortable )
		return;
	f_iSortColumn = a_iColumn;
	f_oHeader [ a_iColumn ].f_bDescending = a_eOrder == OListBits::D_DESCENDING;
	long int l_iSize = f_oList.size ( );
	if ( l_iSize > 128 )
		f_poParent->status_bar ( )->init_progress (
				static_cast < double > ( l_iSize )
				* static_cast < double > ( l_iSize ) / 2.,
				" Sorting ..." );
	OProgressHelper l_oHelper = { 0, f_oList.size ( ), f_poParent };
	f_oList.sort ( CompareListControlItems ( a_eOrder, a_iColumn, f_oHeader [ f_iSortColumn ].f_eType, l_oHelper ) );
	f_oFirstVisibleRow = f_oList.begin ( );
	f_iControlOffset = f_iCursorPosition = 0;
	return;
	M_EPILOG
	}

int HListControl::click ( mouse::OMouse & a_rsMouse )
	{
	M_PROLOG
	int l_iRow = 0, l_iColumn = 0, l_iCtr = 0;
	int l_iWidth = 0, l_iColumns = f_oHeader.size ( );
	HColumnInfo * l_poColumnInfo = NULL;
	if ( ! HControl::click ( a_rsMouse ) )
		return ( 1 );
	l_iRow = ( a_rsMouse.f_iRow - f_iRowRaw ) - ( f_bDrawHeader ? 1 : 0 );
	if ( l_iRow == f_iCursorPosition )
		return ( 1 );
	if ( l_iRow < 0 ) /* header clicked */
		{
		l_iColumn = a_rsMouse.f_iColumn + f_iColumnRaw - 1;
		for ( l_iCtr = 0; l_iCtr < l_iColumns; l_iCtr ++ )
			{
			l_poColumnInfo = & f_oHeader [ l_iCtr ];
			l_iWidth += l_poColumnInfo->f_iWidthRaw;
			if ( l_iColumn <= l_iWidth )
				{
				sort_by_column ( l_iCtr,
						l_poColumnInfo->f_bDescending ? OListBits::D_ASCENDING : OListBits::D_DESCENDING );
				refresh ( );
				break;
				}
			}
		}
	else if ( l_iRow < f_oList.size ( ) )
		{
		f_iCursorPosition = l_iRow;
		refresh ( );
		}
	return ( 0 );
	M_EPILOG
	}

bool HListControl::is_searchable ( void )
	{
	return ( f_bSearchable );
	}

void HListControl::go_to_match ( void )
	{
	M_PROLOG
/*
	int l_iCtr = 0, l_iCtrLoc = 0, l_iMoveFirstRow = 0;
	int l_iCount = f_oList.size ( ) + 1, l_iColumns = f_oHeader.size ( );
	int l_iControlOffsetOrig = f_iControlOffset, l_iCursorPositionOrig = f_iCursorPosition;
	char const * l_pcHighlightStart = NULL;
	HItem * l_poItem = NULL;
	HElement * l_poSelectedOrig = f_poSelected;
	HElement * l_poFirstVisibleRowOrig = f_oFirstVisibleRow;
	if ( ! f_bSearchActived )
		return;
	if ( f_sMatch.f_poCurrentMatch != f_poSelected )
		f_sMatch.f_iMatchNumber = - 1;
	f_sMatch.f_poCurrentMatch = f_poSelected;
	while ( l_iCount -- )
		{
		l_poItem = & f_poSelected->get_object ( );
		for ( l_iCtr = f_sMatch.f_iColumnWithMatch; l_iCtr < l_iColumns; l_iCtr ++ )
			{
			l_pcHighlightStart = ( * l_poItem ) [ l_iCtr ].get < HString const & > ( );
			l_iCtrLoc = 0;
			while ( ( l_pcHighlightStart = f_oPattern.matches ( l_pcHighlightStart ) ) )
				{
				if ( l_iCtrLoc > f_sMatch.f_iMatchNumber )
					break;
				l_pcHighlightStart ++;
				l_iCtrLoc ++;
				}
			if ( l_pcHighlightStart )
				break;
			f_sMatch.f_iMatchNumber = - 1;
			}
		if ( l_pcHighlightStart )
			break;
		f_sMatch.f_iColumnWithMatch = 0;
/ * this part is from process_input, but slightly modified * /
		if ( ( f_iCursorPosition + f_iControlOffset ) < ( l_iSize - 1 ) )
			{
			f_iCursorPosition ++;
			if ( f_iCursorPosition >= f_iHeightRaw )
				{
				f_iCursorPosition = f_iHeightRaw - 1;
				f_iControlOffset ++;
				l_iMoveFirstRow ++;
				}
			to_tail ( );
			}
		else
			{
			f_poSelected = f_oFirstVisibleRow = f_poHook;
			f_iControlOffset = f_iCursorPosition = 0;
			l_iMoveFirstRow = 0;
			f_poParent->status_bar ( )->message ( _ ( "search hit BOTTOM, continuing at TOP" ) );
			}
/ * end od it * /
		}
	if ( l_pcHighlightStart )
		{
		if ( l_iMoveFirstRow )
			to_tail ( f_oFirstVisibleRow, l_iMoveFirstRow );
		f_sMatch.f_iColumnWithMatch = l_iCtr;
		f_sMatch.f_iMatchNumber = l_iCtrLoc;
		f_sMatch.f_poCurrentMatch = f_poSelected;
		}
	else
		{
		f_poSelected = l_poSelectedOrig;
		f_oFirstVisibleRow = l_poFirstVisibleRowOrig;
		f_iCursorPosition = l_iCursorPositionOrig;
		f_iControlOffset = l_iControlOffsetOrig;
		f_sMatch.f_iMatchNumber = - 1;
		f_sMatch.f_iColumnWithMatch = 0;
		f_poParent->status_bar ( )->message ( HString ( _ ( "pattern not found: " ) ) + f_oPattern.error ( ) );
		}*/
	return;
	M_EPILOG
	}

void HListControl::go_to_match_previous ( void )
	{
	M_PROLOG
/*
	int l_iCtr = 0, l_iCtrLoc = 0, l_iMoveFirstRow = 0;
	int l_iCount = f_oList.size ( ) + 1, l_iColumns = f_oHeader.size ( );
	int l_iControlOffsetOrig = f_iControlOffset, l_iCursorPositionOrig = f_iCursorPosition;
	char const * l_pcHighlightStart = NULL;
	HItem * l_poItem = NULL;
	HElement * l_poSelectedOrig = f_poSelected;
	HElement * l_poFirstVisibleRowOrig = f_oFirstVisibleRow;
	if ( ! f_bSearchActived )
		return;
	if ( f_sMatch.f_poCurrentMatch != f_poSelected )
		f_sMatch.f_iMatchNumber = - 1;
	f_sMatch.f_poCurrentMatch = f_poSelected;
	while ( l_iCount -- )
		{
		l_poItem = & f_poSelected->get_object ( );
		for ( l_iCtr = f_sMatch.f_iColumnWithMatch; l_iCtr >= 0; l_iCtr -- )
			{
			l_pcHighlightStart = ( * l_poItem ) [ l_iCtr ].get < HString const & > ( );
			l_iCtrLoc = 0;
			if ( f_sMatch.f_iMatchNumber < 0 )
				f_sMatch.f_iMatchNumber = f_oPattern.count ( l_pcHighlightStart );
			while ( ( l_pcHighlightStart = f_oPattern.matches ( l_pcHighlightStart ) ) )
				{
				if ( l_iCtrLoc == ( f_sMatch.f_iMatchNumber - 1 ) )
					break;
				if ( l_iCtrLoc >= f_sMatch.f_iMatchNumber )
					{
					l_pcHighlightStart = NULL;
					break;
					}
				l_pcHighlightStart ++;
				l_iCtrLoc ++;
				}
			if ( l_pcHighlightStart )
				break;
			f_sMatch.f_iMatchNumber = - 1;
			}
		if ( l_pcHighlightStart )
			break;
		f_sMatch.f_iColumnWithMatch = l_iColumns - 1;
/ * this part is from process_input, but slightly modified * /
		if ( ( f_iControlOffset + f_iCursorPosition ) > 0 )
			{
			if ( f_iCursorPosition > 0 )
				f_iCursorPosition --;
			else if ( f_iControlOffset > 0 )
				{
				f_iControlOffset --;
				l_iMoveFirstRow ++;
				}
			to_head ( );
			}
		else
			{
			if ( l_iSize >= f_iHeightRaw )
				{
				f_poSelected = f_poHook;
				to_head ( );
				f_oFirstVisibleRow = f_poSelected;
				to_head ( f_oFirstVisibleRow, f_iHeightRaw - 1 );
				f_iCursorPosition = f_iHeightRaw - 1;
				f_iControlOffset = l_iSize - f_iHeightRaw;
				}
			else
				f_iCursorPosition = l_iSize - 1;
			l_iMoveFirstRow = 0;
			f_poParent->status_bar ( )->message ( _ ( "search hit TOP, continuing at BOTTOM" ) );
			}
/ * end od it * /
		}
	if ( l_pcHighlightStart )
		{
		if ( l_iMoveFirstRow )
			to_head ( f_oFirstVisibleRow, l_iMoveFirstRow );
		f_sMatch.f_iColumnWithMatch = l_iCtr;
		f_sMatch.f_iMatchNumber = l_iCtrLoc;
		f_sMatch.f_poCurrentMatch = f_poSelected;
		}
	else
		{
		f_poSelected = l_poSelectedOrig;
		f_oFirstVisibleRow = l_poFirstVisibleRowOrig;
		f_iCursorPosition = l_iCursorPositionOrig;
		f_iControlOffset = l_iControlOffsetOrig;
		f_sMatch.f_iMatchNumber = - 1;
		f_sMatch.f_iColumnWithMatch = 0;
		f_poParent->status_bar ( )->message ( _ ( "pattern not found" ) );
		}*/
	return;
	M_EPILOG
	}

}

}

