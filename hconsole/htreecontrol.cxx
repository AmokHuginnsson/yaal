/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski           ---

	htreecontrol.cxx - this file is integral part of `yaal' project.

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

#include "hcore/hexception.h"
M_VCSID ( "$Id$" )
#include "htreecontrol.h"

using namespace yaal::hcore;
using namespace yaal::hconsole::mouse;

namespace yaal
{

namespace hconsole
{

HTreeControl::HNodeControl::HNodeControl( HNodeControl* a_poNode )
						: HTree<HItem>::HNode( a_poNode ), f_bUnfolded( false ),
						f_iRowRaw( 0 ), f_iColumnRaw( 0 ), f_iWidthRaw( 0 )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HTreeControl::HNodeControl::~HNodeControl( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

void HTreeControl::HNodeControl::expand( void )
	{
	M_PROLOG
	HNodeControl * l_poNode = NULL;
	f_bUnfolded = true;
	if ( f_poTrunk )
		{
		l_poNode = dynamic_cast < HTreeControl::HNodeControl * > ( f_poTrunk );
		M_ASSERT ( l_poNode );
		l_poNode->expand();
		}
	return;
	M_EPILOG
	}

void HTreeControl::HNodeControl::collapse( void )
	{
	M_PROLOG
	HTreeControl::HNodeControl * * l_ppoNodeControl = NULL;
	f_bUnfolded = false;
  if  ( ! f_oBranch.size() )
		return;
	for ( l_ppoNodeControl = reinterpret_cast < HTreeControl::HNodeControl * * > ( & f_oBranch.go ( 0 ) );
			l_ppoNodeControl;
			l_ppoNodeControl = reinterpret_cast < HTreeControl::HNodeControl * * > ( f_oBranch.to_tail ( 1, branch_t::D_TREAT_AS_OPENED ) ) )
		( * l_ppoNodeControl )->collapse();
	return;
	M_EPILOG
	}

bool HTreeControl::HNodeControl::hit_test( int a_iRow, int a_iColumn ) const
	{
	M_PROLOG
	if ( f_iRowRaw == a_iRow )
		{
		if ( a_iColumn >= f_iColumnRaw )
			{
			if ( a_iColumn < ( f_iColumnRaw + f_iWidthRaw - 1 ) )
				return ( true );
			}
		}
	return ( false );
	M_EPILOG
	}

void HTreeControl::HNodeControl::click( int a_iColumn )
	{
	M_PROLOG
	if ( a_iColumn == f_iColumnRaw )
		f_bUnfolded = ! f_bUnfolded;
	return;
	M_EPILOG
	}

HTreeControl::HTreeControl( HWindow* a_poParent, int a_iRow, int a_iColumn,
		int a_iHeight, int a_iWidth, char const* a_pcLabel )
	: HControl( a_poParent, a_iRow, a_iColumn, a_iHeight, a_iWidth, a_pcLabel )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HTreeControl::~HTreeControl( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

void HTreeControl::do_refresh( void )
	{
	M_PROLOG
	int l_iCtr = 0;
	if ( f_bFocused )
		curs_set ( CURSOR::D_INVISIBLE );
	draw_label();
	f_oVarTmpBuffer.hs_realloc ( f_iWidthRaw + 1 );
	memset ( f_oVarTmpBuffer.raw(), '_', f_iWidthRaw );
	f_oVarTmpBuffer [ f_iWidthRaw ] = 0;
	for ( l_iCtr = 0; l_iCtr < f_iHeightRaw; l_iCtr ++ )
		c_mvprintf ( f_iRowRaw + l_iCtr, f_iColumnRaw, f_oVarTmpBuffer );
	if ( f_poRoot )
		draw_node ( dynamic_cast < HNodeControl * > ( f_poRoot ), f_iRowRaw );
	return;
	M_EPILOG
	}

int HTreeControl::draw_node( HNodeControl* a_poNode, int a_iRow )
	{
	M_PROLOG
	int l_iCtr = 0;
	int l_iRow = a_iRow;
	HString const * l_poString = NULL;
	HInfo * l_poInfo = NULL;
	M_ASSERT ( a_poNode );
	l_iCtr = a_poNode->f_oBranch.size();
	if ( a_poNode->f_tLeaf )
		{
		l_iRow ++;
		l_poInfo = & a_poNode->f_tLeaf [ 0 ];
		l_poString = & l_poInfo->get < HString const & >();
		a_poNode->f_iRowRaw = l_iRow;
		a_poNode->f_iColumnRaw = f_iColumnRaw + a_poNode->f_iLevel * 2 - 1; 
		a_poNode->f_iWidthRaw = l_poString->get_length() + 2;
		set_attr_data();
		if ( ! a_poNode->f_bUnfolded && l_iCtr )
			c_mvprintf ( l_iRow, a_poNode->f_iColumnRaw, "+" );
		else if ( l_iCtr )
			c_mvprintf ( l_iRow, a_poNode->f_iColumnRaw, "-" );
		if ( a_poNode == f_poSelected )
			set_attr ( f_bEnabled ? ( f_bFocused ? ~f_uiAttributeFocused
						: ~ f_uiAttributeEnabled ) : ~ f_uiAttributeDisabled );
		c_mvprintf ( l_iRow, a_poNode->f_iColumnRaw + 1, * l_poString );
		}
	if ( l_iCtr && ( a_poNode->f_bUnfolded || ! a_poNode->f_iLevel ) )
		{
		a_poNode->f_oBranch.go ( - 1 );
		while ( l_iCtr -- )
			l_iRow = draw_node ( dynamic_cast < HNodeControl * > ( * a_poNode->f_oBranch.to_tail() ),
					l_iRow );
		}
	return ( l_iRow );
	M_EPILOG
	}

int HTreeControl::do_process_input( int a_iCode )
	{
	M_PROLOG
	bool l_bWasFolded = false;
	int l_iErrorCode = 0;
	HNodeControl * l_poNode = NULL;
	a_iCode = HControl::do_process_input( a_iCode );
	l_poNode = dynamic_cast<HNodeControl*>( f_poSelected );
	switch ( a_iCode )
		{
		case ( KEY_CODES::D_HOME ):
			l_poNode = dynamic_cast<HNodeControl*>( f_poRoot );
			if ( l_poNode->f_oBranch.size() )
				f_poSelected = l_poNode->f_oBranch [ 0 ];
		break;
		case ( KEY_CODES::D_END ):
			l_poNode = dynamic_cast<HNodeControl*>( f_poRoot );
			if ( l_poNode->f_oBranch.size() )
				f_poSelected = l_poNode->f_oBranch [ - 1 ];
		break;
		case ( KEY_CODES::D_PAGE_UP ):
			break;
		case ( KEY_CODES::D_PAGE_DOWN ):
			break;
		case ( KEY_CODES::D_UP ):
			{
			f_poSelected = l_poNode->previous();
			if ( f_poSelected == l_poNode )
				{
				if ( l_poNode->f_iLevel > 1 ) 
					f_poSelected = l_poNode->f_poTrunk;
				}
			else
				{
				l_poNode = dynamic_cast<HNodeControl*>( f_poSelected );
				while ( l_poNode->f_bUnfolded )
					{
					if ( l_poNode->f_oBranch.size() )
						{
						f_poSelected = l_poNode->f_oBranch [ 0 ];
						l_poNode = dynamic_cast<HNodeControl*>( f_poSelected );
						f_poSelected = l_poNode->previous( branch_t::D_TREAT_AS_CLOSED );
						l_poNode = dynamic_cast<HNodeControl*>( f_poSelected );
						}
					}
				}
			}
		break;
		case ( KEY_CODES::D_RIGHT ):
			{
			l_bWasFolded = ! l_poNode->f_bUnfolded;
			if ( l_poNode->f_oBranch.size() )
				{
				l_poNode->f_bUnfolded = true;
				if ( l_bWasFolded )
					break;
				}
			}
		/* when node is unfolded, right key works as down key */
		case ( KEY_CODES::D_DOWN ):
			{
			if ( l_poNode->f_bUnfolded )
				{
				if ( l_poNode->f_oBranch.size() )
					f_poSelected = l_poNode->f_oBranch [ 0 ];
				}
			else
				{
				l_poNode = dynamic_cast<HNodeControl*>( f_poSelected );
				M_ASSERT ( l_poNode );
				l_poNode = dynamic_cast<HNodeControl*>( l_poNode->next() );
				while ( ! l_poNode )
					{
					l_poNode = dynamic_cast<HNodeControl*>( dynamic_cast<HNodeControl*>( f_poSelected )->f_poTrunk );
					if ( ! l_poNode || ( l_poNode->f_iLevel < 1 ) )
						break;
					f_poSelected = l_poNode;
					l_poNode = dynamic_cast<HNodeControl*>( l_poNode->next() );
					}
				if ( l_poNode && ( l_poNode->f_iLevel > 0 ) )
					f_poSelected = l_poNode;
				}
			}
		break;
		case ( KEY_CODES::D_LEFT ):
			if ( l_poNode->f_bUnfolded && l_poNode->f_iLevel )
				l_poNode->f_bUnfolded = false;
			else if ( l_poNode->f_iLevel > 1 )
				f_poSelected = l_poNode->f_poTrunk;
		break;
		case ( ' ' ):
		case ( '\r' ):
			if ( l_poNode->f_oBranch.size() )
				l_poNode->f_bUnfolded = ! l_poNode->f_bUnfolded;
			else
				l_iErrorCode = a_iCode;
		break; /* I have to think more about it. */
		case ( '\t' ):
			f_bFocused = false;	/* very  */
			schedule_refresh();	/* magic */
		/* no break on purpouse, see hlistcontrol.c */
		default :
			l_iErrorCode = a_iCode;
		}
	a_iCode = l_iErrorCode;
	if ( ! l_iErrorCode )
		{
		schedule_refresh();
		f_poParent->status_bar()->message( COLORS::D_FG_LIGHTGRAY, "" );
		}
	return ( a_iCode );
	M_EPILOG
	}

int HTreeControl::set_focus( char a_cShorcut )
	{
	M_PROLOG
	return ( HControl::set_focus ( a_cShorcut ) );
	M_EPILOG
	}

int HTreeControl::do_click( OMouse& a_rsMouse )
	{
	M_PROLOG
	if ( ! HControl::do_click( a_rsMouse ) )
		return ( 1 );
	if ( do_click( dynamic_cast<HNodeControl*>( f_poRoot ), a_rsMouse ) )
		schedule_refresh();
	return ( 0 );
	M_EPILOG
	}

bool HTreeControl::do_click ( HNodeControl* a_poNode, OMouse& a_rsMouse )
	{
	M_PROLOG
	int l_iCtr = 0;
	l_iCtr = a_poNode->f_oBranch.size();
	if ( a_poNode->hit_test( a_rsMouse.f_iRow, a_rsMouse.f_iColumn ) )
		{
		a_poNode->click( a_rsMouse.f_iColumn );
		f_poSelected = a_poNode;
		return ( true );
		}
	if ( l_iCtr && ( a_poNode->f_bUnfolded || ! a_poNode->f_iLevel ) )
		{
		a_poNode->f_oBranch.go ( - 1 );
		while ( l_iCtr -- )
			if ( do_click ( dynamic_cast < HNodeControl * > ( * a_poNode->f_oBranch.to_tail() ), a_rsMouse ) )
				return ( true );
		}
	return ( false );
	M_EPILOG
	}

}

}

