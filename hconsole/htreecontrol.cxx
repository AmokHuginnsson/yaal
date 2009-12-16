/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski           ---

	htreecontrol.cxx - this file is integral part of `yaal' project.

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

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "htreecontrol.hxx"

using namespace yaal::hcore;
using namespace yaal::hconsole::mouse;

namespace yaal
{

namespace hconsole
{

HTreeControl::HNodeControl::HNodeControl( int cols )
						: f_bUnfolded( false ),
						f_iRowRaw( 0 ), f_iColumnRaw( 0 ), f_iWidthRaw( 0 ), f_oData( cols )
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

HInfo& HTreeControl::HNodeControl::operator[]( int idx )
	{
	M_PROLOG
	return ( f_oData[ idx ] );
	M_EPILOG
	}

void HTreeControl::expand( tree_t::node_t node )
	{
	M_PROLOG
	(**node).f_bUnfolded = true;
	if ( node->get_parent() )
		expand( node->get_parent() );
	return;
	M_EPILOG
	}

void HTreeControl::collapse( tree_t::node_t node )
	{
	M_PROLOG
	(**node).f_bUnfolded = false;
  if ( ! node->has_childs() )
		return;
	for ( tree_t::iterator it = node->begin(); it != node->end(); ++ it )
		collapse( &*it );
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
	: HControl( a_poParent, a_iRow, a_iColumn, a_iHeight, a_iWidth, a_pcLabel ),
	f_oTree(), f_poSelected( NULL )
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
	HConsole& cons = HCons::get_instance();
	if ( f_bFocused )
		cons.curs_set ( CURSOR::INVISIBLE );
	draw_label();
	f_oVarTmpBuffer.hs_realloc( f_iWidthRaw + 1 );
	f_oVarTmpBuffer.fillz( '_', 0, f_iWidthRaw );
	for ( l_iCtr = 0; l_iCtr < f_iHeightRaw; l_iCtr ++ )
		cons.c_mvprintf( f_iRowRaw + l_iCtr, f_iColumnRaw, f_oVarTmpBuffer.raw() );
	if ( f_oTree.get_root() )
		draw_node( f_oTree.get_root(), f_iRowRaw );
	return;
	M_EPILOG
	}

int HTreeControl::draw_node( tree_t::node_t a_poNode, int a_iRow )
	{
	M_PROLOG
	int l_iRow = a_iRow;
	HConsole& cons = HCons::get_instance();
	M_ASSERT( a_poNode );
	if ( (**a_poNode).f_oData.get_size() )
		{
		l_iRow ++;
		HInfo const& info = (**a_poNode).f_oData[ 0 ];
		HString const& str = info.get<HString const&>();
		(**a_poNode).f_iRowRaw = l_iRow;
		(**a_poNode).f_iColumnRaw = f_iColumnRaw + a_poNode->get_level() * 2 - 1; 
		(**a_poNode).f_iWidthRaw = static_cast<int>( str.get_length() ) + 2;
		set_attr_data();
		if ( ! ( (**a_poNode).f_bUnfolded || ! a_poNode->has_childs() ) )
			cons.c_mvprintf( l_iRow, (**a_poNode).f_iColumnRaw, "+" );
		else if ( a_poNode->has_childs() )
			cons.c_mvprintf( l_iRow, (**a_poNode).f_iColumnRaw, "-" );
		if ( a_poNode == f_poSelected )
			cons.set_attr( f_bEnabled ? ( f_bFocused ? ~f_uiAttributeFocused
						: ~ f_uiAttributeEnabled ) : ~ f_uiAttributeDisabled );
		cons.c_mvprintf( l_iRow, (**a_poNode).f_iColumnRaw + 1, str.raw() );
		}
	if ( a_poNode->has_childs() && ( (**a_poNode).f_bUnfolded || ! a_poNode->get_level() ) )
		{
		for ( tree_t::iterator it = a_poNode->begin(); it != a_poNode->end(); ++ it )
			l_iRow = draw_node( &*it, l_iRow );
		}
	return ( l_iRow );
	M_EPILOG
	}

int HTreeControl::do_process_input( int a_iCode )
	{
	M_PROLOG
	bool l_bWasFolded = false;
	int l_iErrorCode = 0;
	a_iCode = HControl::do_process_input( a_iCode );
	tree_t::node_t l_poNode = f_poSelected;
	switch ( a_iCode )
		{
		case ( KEY_CODES::HOME ):
			l_poNode = f_oTree.get_root();
			if ( l_poNode->has_childs() )
				f_poSelected = &*l_poNode->begin();
		break;
		case ( KEY_CODES::END ):
			l_poNode = f_oTree.get_root();
			if ( l_poNode->has_childs() )
				f_poSelected = &*l_poNode->rbegin();
		break;
		case ( KEY_CODES::PAGE_UP ):
			break;
		case ( KEY_CODES::PAGE_DOWN ):
			break;
		case ( KEY_CODES::UP ):
			{
			f_poSelected = previous( l_poNode );
			if ( f_poSelected == l_poNode )
				{
				if ( l_poNode->get_level() > 1 ) 
					f_poSelected = l_poNode->get_parent();
				}
			else
				{
				l_poNode = f_poSelected;
				while ( (**l_poNode).f_bUnfolded )
					{
					if ( l_poNode->has_childs() )
						{
						f_poSelected = &*l_poNode->begin();
						l_poNode = f_poSelected;
						f_poSelected = previous( l_poNode, true );
						l_poNode = f_poSelected;
						}
					}
				}
			}
		break;
		case ( KEY_CODES::RIGHT ):
			{
			l_bWasFolded = ! (**l_poNode).f_bUnfolded;
			if ( l_poNode->has_childs() )
				{
				(**l_poNode).f_bUnfolded = true;
				if ( l_bWasFolded )
					break;
				}
			}
		/* when node is unfolded, right key works as down key */
		case ( KEY_CODES::DOWN ):
			{
			if ( (**l_poNode).f_bUnfolded )
				{
				if ( l_poNode->has_childs() )
					f_poSelected = &*l_poNode->begin();
				}
			else
				{
				l_poNode = f_poSelected;
				l_poNode = next( l_poNode );
				while ( ! l_poNode )
					{
					l_poNode = f_poSelected->get_parent();
					if ( ! l_poNode || ( l_poNode->get_level() < 1 ) )
						break;
					f_poSelected = l_poNode;
					l_poNode = next( l_poNode );
					}
				if ( l_poNode && ( l_poNode->get_level() > 0 ) )
					f_poSelected = l_poNode;
				}
			}
		break;
		case ( KEY_CODES::LEFT ):
			if ( (**l_poNode).f_bUnfolded && l_poNode->get_level() )
				(**l_poNode).f_bUnfolded = false;
			else if ( l_poNode->get_level() > 1 )
				f_poSelected = l_poNode->get_parent();
		break;
		case ( ' ' ):
		case ( '\r' ):
			if ( l_poNode->has_childs() )
				(**l_poNode).f_bUnfolded = ! (**l_poNode).f_bUnfolded;
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
		f_poParent->status_bar()->message( COLORS::FG_LIGHTGRAY, "" );
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
	if ( f_oTree.get_root() && do_click( f_oTree.get_root(), a_rsMouse ) )
		schedule_refresh();
	return ( 0 );
	M_EPILOG
	}

bool HTreeControl::do_click( tree_t::node_t a_poNode, OMouse& a_rsMouse )
	{
	M_PROLOG
	if ( (**a_poNode).hit_test( a_rsMouse.f_iRow, a_rsMouse.f_iColumn ) )
		{
		(**a_poNode).click( a_rsMouse.f_iColumn );
		f_poSelected = a_poNode;
		return ( true );
		}
	if ( a_poNode->has_childs() && ( (**a_poNode).f_bUnfolded || ! a_poNode->get_level() ) )
		{
		for ( tree_t::iterator it = a_poNode->begin(); it != a_poNode->end(); ++ it )
			if ( do_click( &*it, a_rsMouse ) )
				return ( true );
		}
	return ( false );
	M_EPILOG
	}

HTreeControl::tree_t::node_t HTreeControl::previous( tree_t::node_t node, bool wrap )
	{
	M_PROLOG
	tree_t::node_t p = NULL;
	tree_t::node_t parent = node->get_parent();
	if ( parent )
		{
		tree_t::iterator it;
		for ( it = parent->begin(); ( it != parent->end() ) && ( &*it != node ); ++ it )
			;
		if ( it != parent->end() )
			{
			if ( it == parent->begin() )
				{
				if ( wrap )
					it = parent->rbegin();
				}
			else
				-- it;
			}
		if ( it != parent->end() )
			p = &*it;
		}
	return ( p );
	M_EPILOG
	}

HTreeControl::tree_t::node_t HTreeControl::next( tree_t::node_t node )
	{
	M_PROLOG
	tree_t::node_t parent = node->get_parent();
	tree_t::node_t n = NULL;
	if ( parent )
		{
		tree_t::iterator it;
		for ( it = parent->begin(); ( it != parent->end() ) && ( &*it != node ); ++ it )
			;
		if ( it != parent->end() )
			++ it;
		if ( it != parent->end() )
			n = &*it;
		}
	return ( n );
	M_EPILOG
	}

}

}

