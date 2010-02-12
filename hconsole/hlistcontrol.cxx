/*
---          `yaal' (c) 1978 by Marcin 'Amok' Konarski           ---

	hlistcontrol.cxx - this file is integral part of `yaal' project.

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

#include <cstring>
#include <cctype>
#include <libintl.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hcore/hlog.hxx"
#include "hlistcontrol.hxx"
#include "hconsole.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::hconsole::list_control_helper;

namespace yaal
{

namespace hconsole
{

HListControl::HColumnInfo::HColumnInfo( void )
	: f_bDescending( false ), f_iWidthRaw( 0 ), f_iWidth( 0 ), f_eAlign( BITS::ALIGN::LEFT ),
	f_iShortcutIndex( 0 ), f_cShortcut( 0 ), f_eType( TYPE::HSTRING ), f_oName(),
	f_poControl( NULL )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HListControl::HColumnInfo::~HColumnInfo( void )
	{
	M_PROLOG
	f_poControl = NULL;
	return;
	M_EPILOG
	}

HListControl::HColumnInfo::HColumnInfo( HColumnInfo const& a_roColumnInfo )
	: f_bDescending( false ), f_iWidthRaw( 0 ), f_iWidth( 0 ), f_eAlign( BITS::ALIGN::LEFT ),
	f_iShortcutIndex( 0 ), f_cShortcut( 0 ), f_eType( TYPE::HSTRING ), f_oName(),
	f_poControl( NULL )
	{
	M_PROLOG
	( *this ) = a_roColumnInfo;
	return;
	M_EPILOG
	}

HListControl::HColumnInfo& HListControl::HColumnInfo::operator = ( HColumnInfo const& a_roColumnInfo )
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
	return ( *this );
	M_EPILOG
	}

HListControl::HListControl( HWindow* a_poParent, int a_iRow, int a_iColumn,
		int a_iHeight, int a_iWidth, char const* a_pcLabel, HAbstractControler::ptr_t const& a_oData )
						: HControl( a_poParent, a_iRow, a_iColumn, a_iHeight, a_iWidth,
								a_pcLabel ),
							HSearchableControl( true ),
	f_bCheckable( false ), f_bSortable( true ),
	f_bDrawHeader( true ), f_bEditable( false ),
	f_iControlOffset( 0 ), f_iCursorPosition( 0 ), f_iSumForOne( 0 ),
	f_oHeader(), f_iSortColumn( -1 ), f_sMatch(),
	f_oCursor(), f_oFirstVisibleRow(), f_oControler( a_oData )
	{
	M_PROLOG
	f_oControler->set_control( this );
	schedule_refresh();
	return;
	M_EPILOG
	}

HListControl::~HListControl( void )
	{
	return;
	}

void HListControl::do_refresh( void )
	{
	M_PROLOG
	bool l_bChecked = false;
	int l_iCtr = 0;
	int l_iCtrLoc = 0;
	int l_iColumnOffset = 0;
	int l_iTmp = 0;
	int l_iColumns = static_cast<int>( f_oHeader.size() );
	int l_iHR = f_bDrawHeader ? 1 : 0; /* HR stands for header row */
	int l_iSize = static_cast<int>( f_oControler->size() );
	HColumnInfo * l_poColumnInfo = NULL;
	HConsole& cons = HCons::get_instance();
	l_iTmp = f_iWidthRaw;
	if ( f_bFocused )
		cons.curs_set( CURSOR::INVISIBLE );
	draw_label(); /* raw* set here */
	f_iRowRaw += l_iHR;
	if ( ! f_iSumForOne )
		return;
	if ( f_iWidthRaw != l_iTmp )
		recalculate_column_widths();
/* we need to decrement f_iHeightRaw because we have additional row, 
 * the list control header */
	if ( f_bDrawHeader )
		f_iHeightRaw --;
	f_oVarTmpBuffer.hs_realloc( f_iWidthRaw + 1 );
	if ( l_iSize > 0 )
		{
		iterator_t it = f_oFirstVisibleRow;
		for ( l_iCtr = 0;
					l_iCtr < ( l_iSize > f_iHeightRaw ? f_iHeightRaw : l_iSize );
					++ l_iCtr, ++ it )
			{
			l_iColumnOffset = 0;
			for ( l_iCtrLoc = 0; l_iCtrLoc < l_iColumns; l_iCtrLoc ++ )
				{
				l_poColumnInfo = & f_oHeader[ l_iCtrLoc ];
				if ( l_poColumnInfo->f_iWidthRaw )
					{
					l_bChecked = get_text_for_cell( it, l_iCtrLoc, l_poColumnInfo->f_eType );
					draw_cell( it, l_iCtr, l_iCtrLoc, l_iColumnOffset, l_poColumnInfo, l_bChecked );
					l_iColumnOffset += l_poColumnInfo->f_iWidthRaw;
					}
				if ( ( it == f_oCursor ) && l_poColumnInfo->f_poControl )
					(*it)[ l_iCtrLoc ].set_child_control_data( l_poColumnInfo->f_poControl );
				}
			}
		}
	draw_background( l_iCtr );
	f_iRowRaw -= l_iHR;
	draw_header( l_iColumns );
	f_iRowRaw += l_iHR;
	cons.set_attr( ! f_bEnabled ?
			( ! f_bFocused ? f_uiAttributeFocused : f_uiAttributeEnabled )
			: f_uiAttributeDisabled );
	if ( l_iSize > f_iHeightRaw )
		draw_scroll( f_iColumnRaw + l_iColumnOffset - 1 );
	f_iRowRaw -= l_iHR;
	return;
	M_EPILOG
	}

void HListControl::draw_header( int a_iColumns )
	{
	M_PROLOG
	int l_iCtr = 0;
	int l_iCtrLoc = 0;
	int l_iColumnOffset = 0;
	int l_iHR = f_bDrawHeader ? 1 : 0; /* HR stands for header row */
	HColumnInfo * l_poColumnInfo = NULL;
	HConsole& cons = HCons::get_instance();
	for ( l_iCtr = 0; l_iCtr < a_iColumns; l_iCtr ++ )
		{
		l_poColumnInfo = & f_oHeader[ l_iCtr ];
		if ( l_poColumnInfo->f_iWidthRaw )
			{
			if ( f_bDrawHeader )
				{
				f_oVarTmpBuffer = l_poColumnInfo->f_oName;
				set_attr_label();
				f_oVarTmpBuffer.format( "%%-%ds",
							l_poColumnInfo->f_iWidthRaw );
				M_ENSURE( cons.c_mvprintf( f_iRowRaw, f_iColumnRaw + l_iColumnOffset,
							f_oVarTmpBuffer.raw(),
								l_poColumnInfo->f_oName.left(
									l_poColumnInfo->f_iWidthRaw ).raw() ) != C_ERR );
				set_attr_shortcut();
				M_ENSURE( cons.c_mvprintf( f_iRowRaw,
							f_iColumnRaw + l_iColumnOffset + l_poColumnInfo->f_iShortcutIndex,
							"%c", l_poColumnInfo->f_cShortcut ) != C_ERR );
				if ( f_iSortColumn == l_iCtr )
					M_ENSURE( cons.c_mvprintf( f_iRowRaw,
								f_iColumnRaw + l_iColumnOffset
								+ l_poColumnInfo->f_iWidthRaw - 2,
								"%c", l_poColumnInfo->f_bDescending ? '^' : 'v' ) != C_ERR );
				}
			l_iColumnOffset += l_poColumnInfo->f_iWidthRaw;
			if ( l_iCtr < a_iColumns )
				{
				cons.set_attr( f_uiAttributeDisabled );
				for ( l_iCtrLoc = 0; l_iCtrLoc < ( f_iHeightRaw + l_iHR );
						l_iCtrLoc ++ )
					{
					M_ENSURE( cons.c_move( f_iRowRaw + l_iCtrLoc,
								f_iColumnRaw + l_iColumnOffset - 1 ) != C_ERR );
					M_ENSURE( cons.c_addch( GLYPHS::VERTICAL_LINE ) != C_ERR );
					}
				}
			}
		}
	if ( ! f_bSingleLine )
		{
		f_oVarTmpBuffer.format( " %d/%d ", f_iControlOffset + f_iCursorPosition + 1, static_cast<int>( f_oControler->size() ) );
		if ( f_iLabelLength < f_iWidthRaw )
			{
			int clip = static_cast<int>( ( ( f_iWidthRaw - f_iLabelLength ) < f_oVarTmpBuffer.get_length() ) ? f_oVarTmpBuffer.get_length() - ( f_iWidthRaw - f_iLabelLength ) : 0 );
			cons.c_mvprintf( f_iRowRaw - 1, static_cast<int>( f_iColumnRaw + f_iWidthRaw + clip - f_oVarTmpBuffer.get_length() ), f_oVarTmpBuffer.raw() + clip );
			}
		}
	return;
	M_EPILOG
	}

void HListControl::draw_background( int a_iFrom )
	{
	M_PROLOG
	int l_iCtr = a_iFrom;
	set_attr_data();
	f_oVarTmpBuffer.fillz( '.', 0, f_iWidthRaw );
	for ( ; l_iCtr < f_iHeightRaw; l_iCtr ++ )
		M_ENSURE( HCons::get_instance().c_mvprintf( f_iRowRaw + l_iCtr, f_iColumnRaw,
					f_oVarTmpBuffer.raw() ) != C_ERR );
	return;
	M_EPILOG
	}

void HListControl::draw_scroll( int a_iPosX )
	{
	M_PROLOG
	double l_dScaled = 0;
	int l_iSize = static_cast<int>( f_oControler->size() );
	HConsole& cons = HCons::get_instance();
	if ( f_iControlOffset )
		{
		M_ENSURE( cons.c_move( f_iRowRaw, a_iPosX ) != C_ERR );
		M_ENSURE( cons.c_addch( GLYPHS::UP_ARROW ) != C_ERR );
		}
	if ( ( l_iSize - f_iControlOffset ) > f_iHeightRaw )
		{
		M_ENSURE( cons.c_move( f_iRowRaw + f_iHeightRaw - 1, a_iPosX ) != C_ERR );
		M_ENSURE( cons.c_addch( GLYPHS::DOWN_ARROW ) != C_ERR );
		}
	l_dScaled = f_iHeightRaw - 3;
	l_dScaled *= static_cast<double>(
			f_iControlOffset + f_iCursorPosition );
	l_dScaled /= static_cast<double>( l_iSize );
	M_ENSURE( cons.c_mvprintf (
				f_iRowRaw + static_cast<int>( l_dScaled + 1.5 ),
				a_iPosX, "#" ) != C_ERR );
	return;
	M_EPILOG
	}

void HListControl::draw_cell( iterator_t& a_oIt, int a_iRow, int a_iColumn, int a_iColumnOffset, HColumnInfo const* const  a_poColumnInfo, bool a_bChecked )
	{
	M_PROLOG
	int l_iTmp = 0;
	HConsole& cons = HCons::get_instance();
	l_iTmp = static_cast<int>( f_oVarTmpBuffer.get_length() );
	switch ( a_poColumnInfo->f_eAlign )
		{
		case ( BITS::ALIGN::LEFT ):
			{
			if ( l_iTmp < a_poColumnInfo->f_iWidthRaw )
				f_oVarTmpBuffer.fillz( '_', l_iTmp, a_poColumnInfo->f_iWidthRaw - l_iTmp );
			}
		break;
		case ( BITS::ALIGN::CENTER ):
			{
			if ( l_iTmp > a_poColumnInfo->f_iWidthRaw )
				f_oVarTmpBuffer = f_oVarTmpBuffer.right(
						a_poColumnInfo->f_iWidthRaw );
			else if ( l_iTmp < a_poColumnInfo->f_iWidthRaw )
				{
				f_oVarTmpBuffer.insert( 0, ( a_poColumnInfo->f_iWidthRaw - l_iTmp ) / 2, '_' );
				l_iTmp = static_cast<int>( f_oVarTmpBuffer.get_length() );
				f_oVarTmpBuffer.fillz( '_', l_iTmp, a_poColumnInfo->f_iWidthRaw - l_iTmp );
				}
			}
		break;
		case ( BITS::ALIGN::RIGHT ):
			{
			if ( l_iTmp > a_poColumnInfo->f_iWidthRaw )
				f_oVarTmpBuffer.erase( 0, l_iTmp - a_poColumnInfo->f_iWidthRaw );
			else if ( l_iTmp < a_poColumnInfo->f_iWidthRaw )
				f_oVarTmpBuffer.insert( 0, ( a_poColumnInfo->f_iWidthRaw - l_iTmp ) - 1, '_' );
			}
		break;
		default :
			M_THROW( "unknown align", a_poColumnInfo->f_eAlign );
		}
	if ( a_iRow == f_iCursorPosition )
		{
		if ( a_bChecked )
			cons.set_attr( ! f_bEnabled
					? ( ! f_bFocused ? ~ ( f_uiAttributeFocused >> 8 )
						: ~ ( f_uiAttributeEnabled >> 8 ) )
					: ~ ( f_uiAttributeDisabled >> 8 ) );
		else
			cons.set_attr( f_bEnabled ? ( f_bFocused ? ~f_uiAttributeFocused
						: ~ f_uiAttributeEnabled ) : ~ f_uiAttributeDisabled );
		}
	else
		{
		if ( a_bChecked )
			cons.set_attr( f_bEnabled
					? ( f_bFocused ? ~ ( f_uiAttributeFocused >> 8 )
						: ~ ( f_uiAttributeEnabled >> 8 ) )
					: ~ ( f_uiAttributeDisabled >> 8 ) );
		else
			set_attr_data();
		}
	M_ENSURE( cons.c_mvprintf( f_iRowRaw + a_iRow,
				f_iColumnRaw + a_iColumnOffset, f_oVarTmpBuffer.raw()	) != C_ERR );
	if ( f_bSearchActived )
		highlight( f_iRowRaw + a_iRow,
				f_iColumnRaw + a_iColumnOffset, f_sMatch.f_iMatchNumber,
				( a_oIt == f_sMatch.f_oCurrentMatch )
				&& ( a_iColumn == f_sMatch.f_iColumnWithMatch ) );
	return;
	M_EPILOG
	}

namespace
{

template<typename tType>
inline void increment( tType& iterator, int count )
	{
	for ( int i = 0; i < count; ++ i, ++ iterator )
		;
	}

template<typename tType>
inline void decrement( tType& iterator, int count )
	{
	for ( int i = 0; i < count; ++ i, -- iterator )
		;
	}

template<typename tType, typename ttType>
inline void cyclic_increment( ttType& model, tType& iterator, int count )
	{
	for ( int i = 0; i < count; ++ i )
		{
		++ iterator;
		if ( ! iterator.is_valid() )
			iterator = model->begin();
		}
	}

template<typename tType, typename ttType>
inline void cyclic_decrement( ttType& model, tType& iterator, int count )
	{
	for ( int i = 0; i < count; ++ i )
		{
		-- iterator;
		if ( ! iterator.is_valid() )
			iterator = model->rbegin();
		}
	}

}

void HListControl::handle_key_page_up( void )
	{
	if ( ! f_iCursorPosition )
		{
		if ( f_iControlOffset )
			{
			f_iControlOffset -= ( f_iHeightRaw - 1 );
			decrement( f_oFirstVisibleRow, f_iHeightRaw - 1 );
			}
		else
			HCons::get_instance().bell();
		if ( f_iControlOffset < 0 )
			{
			f_iControlOffset = 0;
			f_oFirstVisibleRow = f_oControler->begin();
			}
		}
	else
		{
		f_iCursorPosition = 0;
		f_oCursor = f_oFirstVisibleRow;
		}
	return;
	}

void HListControl::handle_key_page_down( void )
	{
	int l_iSize = static_cast<int>( f_oControler->size() );
	if ( l_iSize >= f_iHeightRaw )
		{
		if ( f_iCursorPosition == ( f_iHeightRaw - 1 ) )
			{
			if ( f_iControlOffset < ( l_iSize - f_iHeightRaw ) )
				{
				f_iControlOffset += ( f_iHeightRaw - 1 );
				increment( f_oFirstVisibleRow, f_iHeightRaw - 1 );
				if ( f_iControlOffset > ( l_iSize - f_iHeightRaw ) )
					{
					f_iControlOffset = l_iSize - f_iHeightRaw;
					f_oFirstVisibleRow = f_oControler->rbegin();
					decrement( f_oFirstVisibleRow, f_iHeightRaw - 1 );
					}
				}
			else
				HCons::get_instance().bell();
			}
		else
			{
			f_iCursorPosition = f_iHeightRaw - 1;
			f_oCursor = f_oFirstVisibleRow;
			increment( f_oCursor, f_iHeightRaw - 1 );
			}
		}
	else
		{
		if ( f_iCursorPosition != ( l_iSize - 1 ) )
			{
			f_iCursorPosition = l_iSize - 1;
			f_oCursor = f_oControler->rbegin();
			}
		else
			HCons::get_instance().bell();
		}
	return;
	}

void HListControl::handle_key_up( void )
	{
	if ( ( f_iControlOffset + f_iCursorPosition ) > 0 )
		{
		if ( f_iCursorPosition > 0 )
			{
			f_iCursorPosition --;
			-- f_oCursor;
			}
		else if ( f_iControlOffset > 0 )
			{
			-- f_oFirstVisibleRow;
			-- f_oCursor;
			f_iControlOffset --;
			}
		}
	else
		HCons::get_instance().bell();
	return;
	}

void HListControl::handle_key_home( void )
	{
	reset();
	return;
	}

void HListControl::handle_key_end( void )
	{
	int l_iSize = static_cast<int>( f_oControler->size() );
	f_oCursor = f_oControler->rbegin();
	if ( l_iSize >= f_iHeightRaw )
		{
		f_iCursorPosition = f_iHeightRaw - 1;
		f_iControlOffset = l_iSize - f_iHeightRaw;
		f_oFirstVisibleRow = f_oCursor;
		decrement( f_oFirstVisibleRow, f_iHeightRaw - 1 );
		}
	else
		f_iCursorPosition = l_iSize - 1;
	return;
	}

void HListControl::handle_key_down( void )
	{
	if ( ( f_iCursorPosition + f_iControlOffset ) < ( f_oControler->size() - 1 ) )
		{
		f_iCursorPosition ++;
		++ f_oCursor;
		if ( f_iCursorPosition >= f_iHeightRaw )
			{
			f_iCursorPosition = f_iHeightRaw - 1;
			f_iControlOffset ++;
			++ f_oFirstVisibleRow;
			}
		}
	else
		HCons::get_instance().bell();
	return;
	}

void HListControl::handle_key_ctrl_n ( void )
	{
	if ( f_bBackwards )
		go_to_match_previous();
	else
		go_to_match();
	return;
	}

void HListControl::handle_key_ctrl_p( void )
	{
	if ( f_bBackwards )
		go_to_match();
	else
		go_to_match_previous();
	return;
	}

void HListControl::handle_key_space( void )
	{
	M_ASSERT( ! f_oControler->empty() );
	M_ASSERT( f_oCursor.is_valid() );
	if ( f_bCheckable )
		f_oCursor->switch_state();
	return;
	}

void HListControl::handle_key_tab( void )
	{
	f_bFocused = false;	/* very  */
	schedule_refresh();				/* magic */
	return;
	}

int HListControl::do_process_input( int a_iCode )
	{
	M_PROLOG
	int l_iCtr = 0;
	int l_iErrorCode = 0;
	a_iCode = HControl::do_process_input( a_iCode );
	f_oVarTmpBuffer = "";
	switch ( a_iCode )
		{
		case ( KEY_CODES::PAGE_UP ):		handle_key_page_up();		break;
		case ( KEY_CODES::PAGE_DOWN ):	handle_key_page_down();	break;
		case ( KEY_CODES::UP ):					handle_key_up();				break;
		case ( KEY_CODES::HOME ):				handle_key_home();			break;
		case ( KEY_CODES::END ):				handle_key_end();				break;
		case ( KEY_CODES::DOWN ):				handle_key_down();			break;
		case ( KEY<'n'>::ctrl ):				handle_key_ctrl_n();		break;
		case ( KEY<'p'>::ctrl ):				handle_key_ctrl_p();		break;
		case ( ' ' ):											handle_key_space();			break;
		case ( '\t' ):										handle_key_tab();
/* there is no break in previous `case():', because this list must give up
 * its focus and be refreshed and parent window must give focus
 * to another control */
		default :
			{
			l_iErrorCode = static_cast<int>( f_oHeader.size() );
			for ( l_iCtr = 0; l_iCtr < l_iErrorCode; l_iCtr ++ )
				if ( tolower( a_iCode ) == tolower( f_oHeader[ l_iCtr ].f_cShortcut ) )
					break;
			if ( l_iCtr < l_iErrorCode )
				{
				l_iErrorCode = 0;
				sort_by_column( l_iCtr,
						a_iCode == tolower( a_iCode ) ? OListBits::ASCENDING : OListBits::DESCENDING );
				}
			else
				l_iErrorCode = a_iCode;
			}
		}
	a_iCode = l_iErrorCode;
	if ( ! l_iErrorCode )
		{
		schedule_refresh();
		f_poParent->status_bar()->message( COLORS::FG_LIGHTGRAY, f_oVarTmpBuffer.raw() );
		}
	return ( a_iCode );
	M_EPILOG
	}

void HListControl::add_column( int const& a_riColumn, char const* a_pcName,
		int const& a_riWidth, BITS::ALIGN::align_t const& a_reAlign, const type_t& a_reType, 
		HControl* a_poControl )
	{
	M_PROLOG
	int l_iShortcutIndex = 0;
	HColumnInfo l_oColumnInfo;
	int l_iSize = static_cast<int>( f_oControler->size() );
	if ( l_iSize )
		M_THROW( "cannot add new column when list not empty", l_iSize );
	f_oVarTmpBuffer = a_pcName;
	l_iShortcutIndex = static_cast<int>( f_oVarTmpBuffer.find( '&' ) );
	if ( l_iShortcutIndex > -1 )
		{
		f_oVarTmpBuffer.set_at( l_iShortcutIndex, 0 );
		f_oVarTmpBuffer += a_pcName + l_iShortcutIndex + 1;
		}
	else
		l_iShortcutIndex = 0;
	f_iSumForOne += a_riWidth;
	l_oColumnInfo.f_iWidth = a_riWidth;
	l_oColumnInfo.f_eType = a_reType;
	l_oColumnInfo.f_eAlign = a_reAlign;
	l_oColumnInfo.f_iShortcutIndex = l_iShortcutIndex;
	l_oColumnInfo.f_cShortcut = f_oVarTmpBuffer[ l_iShortcutIndex ];
	l_oColumnInfo.f_oName = f_oVarTmpBuffer;
	l_oColumnInfo.f_poControl = a_poControl;
	if ( ! f_oHeader.is_empty() && ( a_riColumn >= 0 ) )
		f_oHeader.insert( f_oHeader.n_th( a_riColumn ), &l_oColumnInfo );
	else
		f_oHeader.push_back( l_oColumnInfo );
	recalculate_column_widths();
	return;
	M_EPILOG
	}

int HListControl::set_focus( char a_cShorcut )
	{
	M_PROLOG
	return ( HControl::set_focus( a_cShorcut ) );
	M_EPILOG
	}

void HListControl::recalculate_column_widths( void )
	{
	M_PROLOG
	int l_iCtr = 0;
	int l_iCtrLoc = 0;
	int l_iColumns = 0;
	int l_iColumnOffset = 0;
	int l_iNewWidth = 0;
	l_iColumns = static_cast<int>( f_oHeader.size() );
	for ( l_iCtr = 0; l_iCtr < l_iColumns; l_iCtr ++ )
		{
		l_iNewWidth = f_oHeader [ l_iCtr ].f_iWidth;
		if ( l_iNewWidth )
			{
			if ( ! f_iSumForOne )
				M_THROW( "width of all columns equals 0", f_iSumForOne );
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

void HListControl::sort_by_column( int a_iColumn, OListBits::sort_order_t a_eOrder )
	{
	M_PROLOG
	if ( ! f_bSortable )
		return;
	f_iSortColumn = a_iColumn;
	f_oHeader[ a_iColumn ].f_bDescending = a_eOrder == OListBits::DESCENDING;
	long int l_iSize = f_oControler->size();
	if ( l_iSize > 128 )
		f_poParent->status_bar()->init_progress(
				static_cast<double>( l_iSize )
				* static_cast<double>( l_iSize ) / 2.,
				" Sorting ..." );
	list_control_helper::OSortHelper l_oHelper =
		{ a_iColumn, a_eOrder, f_oHeader[ f_iSortColumn ].f_eType,
		0, f_oControler->size(), f_poParent };
	f_oControler->sort( l_oHelper );
	f_iControlOffset = f_iCursorPosition = 0;
	return;
	M_EPILOG
	}

int HListControl::do_click( mouse::OMouse& a_rsMouse )
	{
	M_PROLOG
	int l_iRow = 0, l_iColumn = 0, l_iCtr = 0;
	int l_iWidth = 0, l_iColumns = static_cast<int>( f_oHeader.size() );
	HColumnInfo* l_poColumnInfo = NULL;
	if ( ! HControl::do_click( a_rsMouse ) )
		return ( 1 );
	l_iRow = ( a_rsMouse.f_iRow - f_iRowRaw ) - ( f_bDrawHeader ? 1 : 0 );
	if ( l_iRow == f_iCursorPosition )
		return ( 1 );
	if ( l_iRow < 0 ) /* header clicked */
		{
		l_iColumn = a_rsMouse.f_iColumn + f_iColumnRaw - 1;
		for ( l_iCtr = 0; l_iCtr < l_iColumns; l_iCtr ++ )
			{
			l_poColumnInfo = &f_oHeader [ l_iCtr ];
			l_iWidth += l_poColumnInfo->f_iWidthRaw;
			if ( l_iColumn <= l_iWidth )
				{
				sort_by_column( l_iCtr,
						l_poColumnInfo->f_bDescending ? OListBits::ASCENDING : OListBits::DESCENDING );
				schedule_refresh();
				break;
				}
			}
		}
	else if ( l_iRow < f_oControler->size() )
		{
		f_iCursorPosition = l_iRow;
		schedule_refresh();
		}
	return ( 0 );
	M_EPILOG
	}

bool HListControl::is_searchable( void )
	{
	return ( f_bSearchable );
	}

void HListControl::reset( void )
	{
	M_PROLOG
	f_oFirstVisibleRow = f_oCursor = f_oControler->begin();
	f_iControlOffset = f_iCursorPosition = 0;
	M_EPILOG
	}

void HListControl::go_to_match( void )
	{
	M_PROLOG
	int l_iCtr = 0, l_iCtrLoc = 0, l_iMoveFirstRow = 0;
	int l_iSize = static_cast<int>( f_oControler->size() );
	int l_iCount = l_iSize + 1;
	int l_iColumns = static_cast<int>( f_oHeader.size() );
	int l_iControlOffsetOrig = f_iControlOffset, l_iCursorPositionOrig = f_iCursorPosition;
	char const * l_pcHighlightStart = NULL;
	char const * l_pcOutcome = NULL;
	iterator_t l_oCursorOrig = f_oCursor;
	iterator_t l_oFirstVisibleRowOrig = f_oFirstVisibleRow;
	if ( ! f_bSearchActived )
		return;
	if ( f_sMatch.f_oCurrentMatch != f_oCursor )
		f_sMatch.f_iMatchNumber = -1;
	f_sMatch.f_oCurrentMatch = f_oCursor;
	while ( l_iCount -- )
		{
		for ( l_iCtr = f_sMatch.f_iColumnWithMatch; l_iCtr < l_iColumns; l_iCtr ++ )
			{
			get_text_for_cell( f_oCursor, l_iCtr, TYPE::HSTRING );
			l_pcHighlightStart = NULL;
			l_iCtrLoc = 0;
			for ( HPattern::HMatchIterator it = f_oPattern.find( f_oVarTmpBuffer.raw() ),
					end = f_oPattern.end(); it != end; ++ it, ++ l_iCtrLoc )
				{
				if ( l_iCtrLoc > f_sMatch.f_iMatchNumber )
					{
					l_pcHighlightStart = it->raw();
					break;
					}
				}
			if ( l_pcHighlightStart )
				break;
			f_sMatch.f_iMatchNumber = -1;
			}
		if ( l_pcHighlightStart )
			break;
		f_sMatch.f_iColumnWithMatch = 0;
/* this part is from process_input, but slightly modified */
		if ( ( f_iCursorPosition + f_iControlOffset ) < ( l_iSize - 1 ) )
			{
			f_iCursorPosition ++;
			if ( f_iCursorPosition >= f_iHeightRaw )
				{
				f_iCursorPosition = f_iHeightRaw - 1;
				f_iControlOffset ++;
				l_iMoveFirstRow ++;
				}
			++ f_oCursor;
			}
		else
			{
			f_oCursor = f_oFirstVisibleRow = f_oControler->begin();
			f_iControlOffset = f_iCursorPosition = 0;
			l_iMoveFirstRow = 0;
			l_pcOutcome = _( "search hit BOTTOM, continuing at TOP" );
			}
/* end od it */
		}
	if ( l_pcHighlightStart )
		{
		if ( l_iMoveFirstRow )
			increment( f_oFirstVisibleRow, l_iMoveFirstRow );
		f_sMatch.f_iColumnWithMatch = l_iCtr;
		f_sMatch.f_iMatchNumber = l_iCtrLoc;
		f_sMatch.f_oCurrentMatch = f_oCursor;
		f_oVarTmpBuffer = "";
		}
	else
		{
		f_oCursor = l_oCursorOrig;
		f_oFirstVisibleRow = l_oFirstVisibleRowOrig;
		f_iCursorPosition = l_iCursorPositionOrig;
		f_iControlOffset = l_iControlOffsetOrig;
		f_sMatch.f_iMatchNumber = -1;
		f_sMatch.f_iColumnWithMatch = 0;
		f_oVarTmpBuffer = HString( _( "pattern not found: " ) ) + f_oPattern.error();
		}
	if ( l_pcOutcome )
		f_oVarTmpBuffer = l_pcOutcome;
	return;
	M_EPILOG
	}

void HListControl::go_to_match_previous ( void )
	{
	M_PROLOG
	int l_iCtr = 0, l_iCtrLoc = 0, l_iMoveFirstRow = 0;
	int l_iSize = static_cast<int>( f_oControler->size() );
	int l_iCount = l_iSize + 1;
	int l_iColumns = static_cast<int>( f_oHeader.size() );
	int l_iControlOffsetOrig = f_iControlOffset, l_iCursorPositionOrig = f_iCursorPosition;
	char const * l_pcHighlightStart = NULL;
	char const * l_pcOutcome = NULL;
	iterator_t l_oCursorOrig = f_oCursor;
	iterator_t l_oFirstVisibleRowOrig = f_oFirstVisibleRow;
	if ( ! f_bSearchActived )
		return;
	if ( f_sMatch.f_oCurrentMatch != f_oCursor )
		f_sMatch.f_iMatchNumber = -1;
	f_sMatch.f_oCurrentMatch = f_oCursor;
	while ( l_iCount -- )
		{
		for ( l_iCtr = f_sMatch.f_iColumnWithMatch; l_iCtr >= 0; l_iCtr -- )
			{
			get_text_for_cell( f_oCursor, l_iCtr, TYPE::HSTRING );
			l_pcHighlightStart = NULL;
			l_iCtrLoc = 0;
			if ( f_sMatch.f_iMatchNumber < 0 )
				f_sMatch.f_iMatchNumber = static_cast<int>( distance( f_oPattern.find( f_oVarTmpBuffer.raw() ), f_oPattern.end() ) );
			for ( HPattern::HMatchIterator it = f_oPattern.find( f_oVarTmpBuffer.raw() ),
					end = f_oPattern.end(); it != end; ++ it, ++ l_iCtrLoc )
				{
				if ( l_iCtrLoc == ( f_sMatch.f_iMatchNumber - 1 ) )
					{
					l_pcHighlightStart = it->raw();
					break;
					}
				if ( l_iCtrLoc >= f_sMatch.f_iMatchNumber )
					{
					l_pcHighlightStart = NULL;
					break;
					}
				}
			if ( l_pcHighlightStart )
				break;
			f_sMatch.f_iMatchNumber = -1;
			}
		if ( l_pcHighlightStart )
			break;
		f_sMatch.f_iColumnWithMatch = l_iColumns - 1;
/* this part is from process_input, but slightly modified */
		if ( ( f_iControlOffset + f_iCursorPosition ) > 0 )
			{
			if ( f_iCursorPosition > 0 )
				f_iCursorPosition --;
			else if ( f_iControlOffset > 0 )
				{
				f_iControlOffset --;
				l_iMoveFirstRow ++;
				}
			cyclic_decrement( f_oControler, f_oCursor, 1 );
			}
		else
			{
			if ( l_iSize >= f_iHeightRaw )
				{
				f_oCursor = f_oControler->begin();
				cyclic_decrement( f_oControler, f_oCursor, 1 );
				f_oFirstVisibleRow = f_oCursor;
				cyclic_decrement( f_oControler, f_oFirstVisibleRow, f_iHeightRaw - 1 );
				f_iCursorPosition = f_iHeightRaw - 1;
				f_iControlOffset = l_iSize - f_iHeightRaw;
				}
			else
				f_iCursorPosition = l_iSize - 1;
			l_iMoveFirstRow = 0;
			l_pcOutcome = _( "search hit TOP, continuing at BOTTOM" );
			}
/* end od it */
		}
	if ( l_pcHighlightStart )
		{
		if ( l_iMoveFirstRow )
			cyclic_decrement( f_oControler, f_oFirstVisibleRow, l_iMoveFirstRow );
		f_sMatch.f_iColumnWithMatch = l_iCtr;
		f_sMatch.f_iMatchNumber = l_iCtrLoc;
		f_sMatch.f_oCurrentMatch = f_oCursor;
		f_oVarTmpBuffer = "";
		}
	else
		{
		f_oCursor = l_oCursorOrig;
		f_oFirstVisibleRow = l_oFirstVisibleRowOrig;
		f_iCursorPosition = l_iCursorPositionOrig;
		f_iControlOffset = l_iControlOffsetOrig;
		f_sMatch.f_iMatchNumber = -1;
		f_sMatch.f_iColumnWithMatch = 0;
		f_oVarTmpBuffer = _( "pattern not found" );
		}
	if ( l_pcOutcome )
		f_oVarTmpBuffer = l_pcOutcome;
	return;
	M_EPILOG
	}

void HListControl::set_flags( flag_t a_eFlags, flag_t a_eMask )
	{
	M_PROLOG
	if ( !!( a_eMask & FLAG::SORTABLE ) )
		f_bSortable = !!( a_eFlags & FLAG::SORTABLE ) ? true : false;
	if ( !!( a_eMask & FLAG::CHECKABLE ) )
		f_bCheckable = !!( a_eFlags & FLAG::CHECKABLE ) ? true : false;
	if ( !!( a_eMask & FLAG::EDITABLE ) )
		f_bEditable = !!( a_eFlags & FLAG::EDITABLE ) ? true : false;
	if ( !!( a_eMask & FLAG::DRAW_HEADER ) )
		f_bDrawHeader = !!( a_eFlags & FLAG::DRAW_HEADER ) ? true : false;
	return;
	M_EPILOG
	}

bool HListControl::get_text_for_cell( iterator_t& a_oIt, int a_iColumn, type_t a_eType )
	{
	M_PROLOG
	M_ASSERT( a_oIt.is_valid() );
	HAbstractRow& l_oItem = *a_oIt;
	switch ( a_eType.value() )
		{
		case ( TYPE::INT_LONG ):
			f_oVarTmpBuffer = l_oItem [ a_iColumn ].get_long();
		break;
		case ( TYPE::DOUBLE ):
			f_oVarTmpBuffer = l_oItem [ a_iColumn ].get_double();
		break;
		case ( TYPE::HSTRING ):
			f_oVarTmpBuffer = l_oItem [ a_iColumn ].get_string();
		break;
		case ( TYPE::HTIME ):
			f_oVarTmpBuffer = l_oItem [ a_iColumn ].get_time();
		break;
		default :
			M_THROW( "unknown type", a_eType.value() );
		}
	return ( l_oItem.get_checked() );
	M_EPILOG
	}

list_control_helper::HAbstractControler::ptr_t& HListControl::get_controler( void )
	{
	return ( f_oControler );
	}

int long HListControl::get_row_count( void )
	{
	return ( f_oControler->size() );
	}

void HListControl::remove_current_row ( void )
	{
	M_PROLOG
	bool l_bFlag = true;
	if ( f_iControlOffset
			&& ( ( f_iControlOffset + f_iHeightRaw ) == f_oControler->size() ) )
		{
		f_iControlOffset --;
		++ f_oFirstVisibleRow;
		}
	else if ( f_iCursorPosition && ( f_iCursorPosition == ( f_oControler->size() - 1 ) ) )
		f_iCursorPosition --;
	else
		l_bFlag = false;
	if ( f_oCursor == f_oFirstVisibleRow )
		++ f_oFirstVisibleRow;
	n_bNeedRepaint = true;
	iterator_t it = f_oCursor;
	if ( l_bFlag )
		++ f_oCursor;
	f_oControler->erase( it );
	schedule_refresh();
	return;
	M_EPILOG
	}

void HListControl::do_update( void )
	{
	M_PROLOG
	if ( f_oControler->size() )
		{
		f_iControlOffset = f_iCursorPosition = 0;
		f_oFirstVisibleRow = f_oControler->begin();
		f_oCursor = f_oControler->begin();
		}
	return;
	M_EPILOG
	}

type_t HListControl::get_column_type( int a_iColumn )
	{
	M_PROLOG
	return ( f_oHeader[ a_iColumn ].f_eType );
	M_EPILOG
	}

namespace list_control_helper
{

void OSortHelper::progress( void )
	{
	M_PROLOG
	++ f_lComparedItems;
	if ( ( f_iSize > 1024 ) && ! ( f_lComparedItems % 1024 ) )
		f_poWindow->status_bar()->update_progress( static_cast<double>( f_lComparedItems ) );
	return;
	M_EPILOG
	}

template<>
bool compare_cells( HInfo const& a_oLeft, HInfo const& a_oRight, OSortHelper& a_roSortHelper )
	{
	M_PROLOG
	if ( a_roSortHelper.f_poWindow )
		a_roSortHelper.progress();
	bool lower = false;
	switch ( a_roSortHelper.f_eType.value() )
		{
		case ( TYPE::INT_LONG ):
			lower = a_oLeft.get<long>() < a_oRight.get<long>();
		break;
		case ( TYPE::DOUBLE ):
			lower = a_oLeft.get<double>() < a_oRight.get<double>();
		break;
		case ( TYPE::HSTRING ):
			lower = strcasecmp( a_oLeft.get<HString const &>(),
					 a_oRight.get<HString const&>() ) < 0;
		break;
		case ( TYPE::HTIME ):
			lower = static_cast<time_t>( a_oLeft.get<HTime const&>() ) < static_cast<time_t>( a_oRight.get<HTime const&>() );
		break;
		default:
			break;
		}
	return ( lower );
	M_EPILOG
	}

template<>
HListControler<>::model_ptr_t HListControler<>::get_model( void )
	{
	return ( f_oList );
	}

HAbstractControler::~HAbstractControler( void )
	{
	return;
	}

HAbstractControler::HAbstractControler( void ) : f_poControl( NULL )
	{
	return;
	}

void HAbstractControler::set_control( HListControl* a_poControl )
	{
	f_poControl = a_poControl;
	return;
	}

void HAbstractControler::erase( HAbstractControler::HModelIteratorWrapper& )
	{
	return;
	}

void HAbstractControler::add_tail( void )
	{
	return;
	}

template<>
yaal::hcore::HString const HCell<>::get_long( void )
	{
	M_PROLOG
	return ( HString ( (*f_rtData)[ f_iColumn ].get<int long>() ) );
	M_EPILOG
	}

template<>
yaal::hcore::HString const HCell<>::get_double( void )
	{
	M_PROLOG
	return ( HString ( (*f_rtData)[ f_iColumn ].get<double>() ) );
	M_EPILOG
	}

template<>
yaal::hcore::HString const HCell<>::get_string( void )
	{
	M_PROLOG
	return ( (*f_rtData)[ f_iColumn ].get<yaal::hcore::HString const &>() );
	M_EPILOG
	}

template<>
char const * HCell<>::get_time( void )
	{
	M_PROLOG
	return ( (*f_rtData)[ f_iColumn ].get<yaal::hcore::HTime const &>() );
	M_EPILOG
	}

HAbstractControler::HModelIteratorWrapper::HModelIteratorWrapper( void ) : f_oIteratorPtr()
	{
	return;
	}

HAbstractControler::HModelIteratorWrapper::HModelIteratorWrapper( iterator_ptr_t const& a_oIt ) : f_oIteratorPtr( a_oIt )
	{
	return;
	}

bool HAbstractControler::HModelIteratorWrapper::is_valid( void ) const
	{
	return ( f_oIteratorPtr->is_valid() );
	}

bool HAbstractControler::HModelIteratorWrapper::operator==( HAbstractControler::HModelIteratorWrapper const& a_oIt )
	{
	M_PROLOG
	return ( f_oIteratorPtr.raw() ? f_oIteratorPtr->is_equal( *a_oIt.f_oIteratorPtr ) : a_oIt.f_oIteratorPtr.raw() == NULL );
	M_EPILOG
	}

bool HAbstractControler::HModelIteratorWrapper::operator!=( HAbstractControler::HModelIteratorWrapper const& a_oIt )
	{
	M_PROLOG
	return ( f_oIteratorPtr.raw() ? f_oIteratorPtr->is_not_equal( *a_oIt.f_oIteratorPtr ) : a_oIt.f_oIteratorPtr.raw() != NULL );
	M_EPILOG
	}

HAbstractControler::HModelIteratorWrapper& HAbstractControler::HModelIteratorWrapper::operator++( void )
	{
	f_oIteratorPtr->next();
	return ( *this );
	}

HAbstractControler::HModelIteratorWrapper& HAbstractControler::HModelIteratorWrapper::operator--( void )
	{
	f_oIteratorPtr->previous();
	return ( *this );
	}

HAbstractRow& HAbstractControler::HModelIteratorWrapper::operator*( void )
	{
	return ( f_oIteratorPtr->dereference() );
	}

HAbstractRow* HAbstractControler::HModelIteratorWrapper::operator->( void )
	{
	return ( f_oIteratorPtr->call() );
	}

HAbstractControler::HModelIteratorWrapper::HModelIteratorWrapper( HAbstractControler::HModelIteratorWrapper const& a_oIt )
	: f_oIteratorPtr()
	{
	operator=( a_oIt );
	return;
	}

HAbstractControler::HModelIteratorWrapper& HAbstractControler::HModelIteratorWrapper::operator=( HAbstractControler::HModelIteratorWrapper const& a_oIt )
	{
	M_PROLOG
	if ( &a_oIt != this )
		{
		if ( a_oIt.f_oIteratorPtr.raw() )
			a_oIt.f_oIteratorPtr->assign_to( f_oIteratorPtr );
		}
	return ( *this );
	M_EPILOG
	}

HAbstractControler::iterator_ptr_t& HAbstractControler::HModelIteratorWrapper::raw( void )
	{
	return ( f_oIteratorPtr );
	}

HAbstractControler::HAbstractModelIterator::HAbstractModelIterator( void )
	{
	return;
	}

HAbstractControler::HAbstractModelIterator::~HAbstractModelIterator( void )
	{
	return;
	}

HAbstractRow::~HAbstractRow( void )
	{
	return;
	}

template<>
void HRow<>::switch_state( void )
	{
	f_roIterator->m_bChecked = ! f_roIterator->m_bChecked;
	return;
	}

template<>
bool HRow<>::get_checked( void )
	{
	return ( f_roIterator->m_bChecked );
	}

template<>
int long HRow<>::get_id( void )
	{
	return ( f_roIterator->m_lId );
	}

template<>
HRow<>::HRow( iterator_t& a_oIt ) : f_roIterator( a_oIt ), f_oCells( a_oIt->get_size() )
	{
	M_PROLOG
	int long l_iCellCount = a_oIt->get_size();
	for ( int i = 0; i < l_iCellCount; ++ i )
		f_oCells[ i ] = HCell<>::ptr_t( new HCell<>( f_roIterator, i ) );
	return;
	M_EPILOG
	}

HAbstractCell::~HAbstractCell( void )
	{
	return;
	}

template<>
void HCell<>::set_child_control_data( HControl* a_poControl )
	{
	M_PROLOG
	a_poControl->set( (*f_rtData)[ f_iColumn ] );
	a_poControl->refresh();
	return;
	M_EPILOG
	}

template<>
bool CompareListControlItems<>::operator() ( HItem const& a_oLeft,
		HItem const& a_oRight ) const
	{
	M_PROLOG
	HItem const& l_oLeft = f_roSortHelper.f_eOrder == yaal::hcore::OListBits::ASCENDING ? a_oLeft : a_oRight;
	HItem const& l_oRight = f_roSortHelper.f_eOrder == yaal::hcore::OListBits::ASCENDING ? a_oRight : a_oLeft;
	return ( list_control_helper::compare_cells( l_oLeft[ f_roSortHelper.f_iSortColumn ],
				l_oRight[ f_roSortHelper.f_iSortColumn ], f_roSortHelper ) );
	M_EPILOG
	}

}

}

}
