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
M_VCSID( "$Id: "__TID__" $" )
#include "htreecontrol.hxx"

using namespace yaal::hcore;
using namespace yaal::hconsole::mouse;

namespace yaal
{

namespace hconsole
{

HTreeControl::HNodeControl::HNodeControl( int cols )
						: _unfolded( false ),
						_rowRaw( 0 ), _columnRaw( 0 ), _widthRaw( 0 ), _data( cols )
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
	return ( _data[ idx ] );
	M_EPILOG
	}

void HTreeControl::expand( tree_t::node_t node )
	{
	M_PROLOG
	(**node)._unfolded = true;
	if ( node->get_parent() )
		expand( node->get_parent() );
	return;
	M_EPILOG
	}

void HTreeControl::collapse( tree_t::node_t node )
	{
	M_PROLOG
	(**node)._unfolded = false;
  if ( ! node->has_childs() )
		return;
	for ( tree_t::iterator it = node->begin(); it != node->end(); ++ it )
		collapse( &*it );
	return;
	M_EPILOG
	}

bool HTreeControl::HNodeControl::hit_test( int row_, int column_ ) const
	{
	M_PROLOG
	if ( _rowRaw == row_ )
		{
		if ( column_ >= _columnRaw )
			{
			if ( column_ < ( _columnRaw + _widthRaw - 1 ) )
				return ( true );
			}
		}
	return ( false );
	M_EPILOG
	}

void HTreeControl::HNodeControl::click( int column_ )
	{
	M_PROLOG
	if ( column_ == _columnRaw )
		_unfolded = ! _unfolded;
	return;
	M_EPILOG
	}

HTreeControl::HTreeControl( HWindow* parent_, int row_, int column_,
		int height_, int width_, char const* label_ )
	: HControl( parent_, row_, column_, height_, width_, label_ ),
	_tree(), _selected( NULL )
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
	int ctr = 0;
	HConsole& cons = HConsole::get_instance();
	if ( _focused )
		cons.curs_set ( CURSOR::INVISIBLE );
	draw_label();
	_varTmpBuffer.hs_realloc( _widthRaw + 1 );
	_varTmpBuffer.fillz( '_', 0, _widthRaw );
	for ( ctr = 0; ctr < _heightRaw; ctr ++ )
		cons.c_mvprintf( _rowRaw + ctr, _columnRaw, _varTmpBuffer.raw() );
	if ( _tree.get_root() )
		draw_node( _tree.get_root(), _rowRaw );
	return;
	M_EPILOG
	}

int HTreeControl::draw_node( tree_t::node_t node_, int row_ )
	{
	M_PROLOG
	int row = row_;
	HConsole& cons = HConsole::get_instance();
	M_ASSERT( node_ );
	if ( (**node_)._data.get_size() )
		{
		row ++;
		HInfo const& info = (**node_)._data[ 0 ];
		HString const& str = info.get<HString const&>();
		(**node_)._rowRaw = row;
		(**node_)._columnRaw = _columnRaw + node_->get_level() * 2 - 1; 
		(**node_)._widthRaw = static_cast<int>( str.get_length() ) + 2;
		set_attr_data();
		if ( ! ( (**node_)._unfolded || ! node_->has_childs() ) )
			cons.c_mvprintf( row, (**node_)._columnRaw, "+" );
		else if ( node_->has_childs() )
			cons.c_mvprintf( row, (**node_)._columnRaw, "-" );
		if ( node_ == _selected )
			cons.set_attr( _enabled ? ( _focused ? ~_attributeFocused
						: ~ _attributeEnabled ) : ~ _attributeDisabled );
		cons.c_mvprintf( row, (**node_)._columnRaw + 1, str.raw() );
		}
	if ( node_->has_childs() && ( (**node_)._unfolded || ! node_->get_level() ) )
		{
		for ( tree_t::iterator it = node_->begin(); it != node_->end(); ++ it )
			row = draw_node( &*it, row );
		}
	return ( row );
	M_EPILOG
	}

int HTreeControl::do_process_input( int code_ )
	{
	M_PROLOG
	bool wasFolded = false;
	int errorCode = 0;
	code_ = HControl::do_process_input( code_ );
	tree_t::node_t node = _selected;
	switch ( code_ )
		{
		case ( KEY_CODES::HOME ):
			node = _tree.get_root();
			if ( node->has_childs() )
				_selected = &*node->begin();
		break;
		case ( KEY_CODES::END ):
			node = _tree.get_root();
			if ( node->has_childs() )
				_selected = &*node->rbegin();
		break;
		case ( KEY_CODES::PAGE_UP ):
			break;
		case ( KEY_CODES::PAGE_DOWN ):
			break;
		case ( KEY_CODES::UP ):
			{
			_selected = previous( node );
			if ( _selected == node )
				{
				if ( node->get_level() > 1 ) 
					_selected = node->get_parent();
				}
			else
				{
				node = _selected;
				while ( (**node)._unfolded )
					{
					if ( node->has_childs() )
						{
						_selected = &*node->begin();
						node = _selected;
						_selected = previous( node, true );
						node = _selected;
						}
					}
				}
			}
		break;
		case ( KEY_CODES::RIGHT ):
			{
			wasFolded = ! (**node)._unfolded;
			if ( node->has_childs() )
				{
				(**node)._unfolded = true;
				if ( wasFolded )
					break;
				}
			}
		/* when node is unfolded, right key works as down key */
		case ( KEY_CODES::DOWN ):
			{
			if ( (**node)._unfolded )
				{
				if ( node->has_childs() )
					_selected = &*node->begin();
				}
			else
				{
				node = _selected;
				node = next( node );
				while ( ! node )
					{
					node = _selected->get_parent();
					if ( ! node || ( node->get_level() < 1 ) )
						break;
					_selected = node;
					node = next( node );
					}
				if ( node && ( node->get_level() > 0 ) )
					_selected = node;
				}
			}
		break;
		case ( KEY_CODES::LEFT ):
			if ( (**node)._unfolded && node->get_level() )
				(**node)._unfolded = false;
			else if ( node->get_level() > 1 )
				_selected = node->get_parent();
		break;
		case ( ' ' ):
		case ( '\r' ):
			if ( node->has_childs() )
				(**node)._unfolded = ! (**node)._unfolded;
			else
				errorCode = code_;
		break; /* I have to think more about it. */
		case ( '\t' ):
			_focused = false;	/* very  */
			schedule_refresh();	/* magic */
		/* no break on purpouse, see hlistcontrol.c */
		default :
			errorCode = code_;
		}
	code_ = errorCode;
	if ( ! errorCode )
		{
		schedule_refresh();
		_parent->status_bar()->message( COLORS::FG_LIGHTGRAY, "" );
		}
	return ( code_ );
	M_EPILOG
	}

int HTreeControl::set_focus( char shorcut_ )
	{
	M_PROLOG
	return ( HControl::set_focus ( shorcut_ ) );
	M_EPILOG
	}

int HTreeControl::do_click( OMouse& mouse_ )
	{
	M_PROLOG
	if ( ! HControl::do_click( mouse_ ) )
		return ( 1 );
	if ( _tree.get_root() && do_click( _tree.get_root(), mouse_ ) )
		schedule_refresh();
	return ( 0 );
	M_EPILOG
	}

bool HTreeControl::do_click( tree_t::node_t node_, OMouse& mouse_ )
	{
	M_PROLOG
	if ( (**node_).hit_test( mouse_._row, mouse_._column ) )
		{
		(**node_).click( mouse_._column );
		_selected = node_;
		return ( true );
		}
	if ( node_->has_childs() && ( (**node_)._unfolded || ! node_->get_level() ) )
		{
		for ( tree_t::iterator it = node_->begin(); it != node_->end(); ++ it )
			if ( do_click( &*it, mouse_ ) )
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

