/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski           ---

	htreewidget.cxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <cstring>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "htreewidget.hxx"

using namespace yaal::hcore;
using namespace yaal::hconsole::mouse;

namespace yaal {

namespace hconsole {

HTreeWidget::HNodeWidget::HNodeWidget( int cols )
						: _unfolded( false ),
						_rowRaw( 0 ), _columnRaw( 0 ), _widthRaw( 0 ), _data( cols ) {
	M_PROLOG
	return;
	M_EPILOG
}

HTreeWidget::HNodeWidget::~HNodeWidget( void ) {
	M_PROLOG
	return;
	M_EPILOG
}

HInfo const& HTreeWidget::HNodeWidget::operator[]( int idx ) const {
	M_PROLOG
	return ( _data[ idx ] );
	M_EPILOG
}

HInfo& HTreeWidget::HNodeWidget::operator[]( int idx ) {
	M_PROLOG
	return ( _data[ idx ] );
	M_EPILOG
}

void HTreeWidget::expand( tree_view_t::node_t node ) {
	M_PROLOG
	(**node)._unfolded = true;
	if ( node->get_parent() )
		expand( node->get_parent() );
	return;
	M_EPILOG
}

void HTreeWidget::collapse( tree_view_t::node_t node ) {
	M_PROLOG
	(**node)._unfolded = false;
  if ( ! node->has_childs() )
		return;
	for ( tree_view_t::HNode::iterator it = node->begin(); it != node->end(); ++ it )
		collapse( &*it );
	return;
	M_EPILOG
}

bool HTreeWidget::HNodeWidget::hit_test( int row_, int column_ ) const {
	M_PROLOG
	if ( _rowRaw == row_ ) {
		if ( column_ >= _columnRaw ) {
			if ( column_ < ( _columnRaw + _widthRaw - 1 ) )
				return ( true );
		}
	}
	return ( false );
	M_EPILOG
}

void HTreeWidget::HNodeWidget::click( int column_ ) {
	M_PROLOG
	if ( column_ == _columnRaw )
		_unfolded = ! _unfolded;
	return;
	M_EPILOG
}

bool HTreeWidget::HNodeWidget::is_unfolded( void ) const {
	return ( _unfolded );
}

HTreeWidget::HTreeWidget( HWindow* parent_, int row_, int column_,
		int height_, int width_, yaal::hcore::HString const& label_ )
	: HWidget( parent_, row_, column_, height_, width_, label_ ),
	_model(), _view(), _selected( NULL ) {
	M_PROLOG
	return;
	M_EPILOG
}

HTreeWidget::~HTreeWidget( void ) {
	M_PROLOG
	return;
	M_EPILOG
}

void HTreeWidget::do_paint( void ) {
	M_PROLOG
	int ctr = 0;
	HConsole& cons = HConsole::get_instance();
	if ( _focused )
		cons.curs_set ( CURSOR::INVISIBLE );
	draw_label();
	_varTmpBuffer.reserve( _widthRaw );
	_varTmpBuffer.fillz( '_', 0, _widthRaw );
	for ( ctr = 0; ctr < _heightRaw; ctr ++ )
		cons.mvprintf( _rowRaw + ctr, _columnRaw, _varTmpBuffer.raw() );
	if ( _view.get_root() )
		draw_node( _view.get_root(), _rowRaw );
	return;
	M_EPILOG
}

int HTreeWidget::draw_node( tree_view_t::node_t node_, int row_ ) {
	M_PROLOG
	int row = row_;
	HConsole& cons = HConsole::get_instance();
	M_ASSERT( node_ );
	if ( (**node_)._data.get_value_count() ) {
		row ++;
		HInfo const& info = (**node_)._data[ 0 ];
		HString const& str = info.get_string();
		(**node_)._rowRaw = row;
		(**node_)._columnRaw = _columnRaw + node_->get_level() * 2 - 1;
		(**node_)._widthRaw = static_cast<int>( str.get_length() ) + 2;
		set_attr_data();
		if ( ! ( (**node_)._unfolded || ! node_->has_childs() ) )
			cons.mvprintf( row, (**node_)._columnRaw, "+" );
		else if ( node_->has_childs() )
			cons.mvprintf( row, (**node_)._columnRaw, "-" );
		if ( node_ == _selected )
			cons.set_attr( _enabled ? ( _focused ? ~_attributeFocused._data
						: ~ _attributeEnabled._data ) : ~ _attributeDisabled._data );
		cons.mvprintf( row, (**node_)._columnRaw + 1, str.raw() );
	}
	if ( node_->has_childs() && ( (**node_)._unfolded || ! node_->get_level() ) ) {
		for ( tree_view_t::HNode::iterator it = node_->begin(); it != node_->end(); ++ it )
			row = draw_node( &*it, row );
	}
	return ( row );
	M_EPILOG
}

int HTreeWidget::do_process_input( int code_ ) {
	M_PROLOG
	bool wasFolded = false;
	int errorCode = 0;
	code_ = HWidget::do_process_input( code_ );
	tree_view_t::node_t node = _selected;
	switch ( code_ ) {
		case ( KEY_CODES::HOME ):
			node = _view.get_root();
			if ( node->has_childs() )
				_selected = &*node->begin();
		break;
		case ( KEY_CODES::END ):
			node = _view.get_root();
			if ( node->has_childs() )
				_selected = &*node->rbegin().base();
		break;
		case ( KEY_CODES::PAGE_UP ):
			break;
		case ( KEY_CODES::PAGE_DOWN ):
			break;
		case ( KEY_CODES::UP ): {
			_selected = previous( node );
			if ( _selected == node ) {
				if ( node->get_level() > 1 )
					_selected = node->get_parent();
			} else {
				node = _selected;
				while ( (**node)._unfolded ) {
					if ( node->has_childs() ) {
						_selected = &*node->begin();
						node = _selected;
						_selected = previous( node, true );
						node = _selected;
					}
				}
			}
		}
		break;
		case ( KEY_CODES::RIGHT ): {
			wasFolded = ! (**node)._unfolded;
			if ( node->has_childs() ) {
				(**node)._unfolded = true;
				if ( wasFolded )
					break;
			}
		}
		/* when node is unfolded, right key works as down key */
		case ( KEY_CODES::DOWN ): {
			if ( (**node)._unfolded ) {
				if ( node->has_childs() )
					_selected = &*node->begin();
			} else {
				node = _selected;
				node = next( node );
				while ( ! node ) {
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
			schedule_repaint();	/* magic */
		/* no break on purpouse, see hlistwidget.c */
		default :
			errorCode = code_;
		break;
	}
	code_ = errorCode;
	if ( ! errorCode ) {
		schedule_repaint();
		_window->status_bar()->clear( COLORS::FG_LIGHTGRAY );
	}
	return ( code_ );
	M_EPILOG
}

bool HTreeWidget::do_click( OMouse& mouse_ ) {
	M_PROLOG
	bool handled( HWidget::do_click( mouse_ ) );
	if ( ! handled ) {
		if ( _view.get_root() && do_click( _view.get_root(), mouse_ ) ) {
			schedule_repaint();
			handled = true;
		}
	}
	return ( handled );
	M_EPILOG
}

bool HTreeWidget::do_click( tree_view_t::node_t node_, OMouse& mouse_ ) {
	M_PROLOG
	if ( (**node_).hit_test( mouse_._row, mouse_._column ) ) {
		(**node_).click( mouse_._column );
		_selected = node_;
		return ( true );
	}
	if ( node_->has_childs() && ( (**node_)._unfolded || ! node_->get_level() ) ) {
		for ( tree_view_t::HNode::iterator it = node_->begin(); it != node_->end(); ++ it )
			if ( do_click( &*it, mouse_ ) )
				return ( true );
	}
	return ( false );
	M_EPILOG
}

HTreeWidget::tree_view_t::node_t HTreeWidget::previous( tree_view_t::node_t node, bool wrap ) {
	M_PROLOG
	tree_view_t::node_t p = NULL;
	tree_view_t::node_t parent = node->get_parent();
	if ( parent ) {
		tree_view_t::HNode::iterator it;
		for ( it = parent->begin(); ( it != parent->end() ) && ( &*it != node ); ++ it )
			;
		if ( it != parent->end() ) {
			if ( it == parent->begin() ) {
				if ( wrap )
					it = parent->rbegin().base();
			} else
				-- it;
		}
		if ( it != parent->end() )
			p = &*it;
	}
	return ( p );
	M_EPILOG
}

HTreeWidget::tree_view_t::node_t HTreeWidget::next( tree_view_t::node_t node ) {
	M_PROLOG
	tree_view_t::node_t parent = node->get_parent();
	tree_view_t::node_t n = NULL;
	if ( parent ) {
		tree_view_t::HNode::iterator it;
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

void HTreeWidget::set_model( HAbstractTreeModel::ptr_t model_ ) {
	M_PROLOG
	_model = model_;
	return;
	M_EPILOG
}

HAbstractTreeModel::ptr_t HTreeWidget::get_model( void ) const {
	M_PROLOG
	return ( _model );
	M_EPILOG
}

void HTreeWidget::do_on_model_changed( void ) {
	M_PROLOG
	return;
	M_EPILOG
}

}

}

