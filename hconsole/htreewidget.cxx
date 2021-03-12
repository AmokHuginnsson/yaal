/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstring>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "htreewidget.hxx"
#include "hwindow.hxx"
#include "tools/keycode.hxx"

using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::hconsole::mouse;

namespace yaal {

namespace hconsole {

HTreeWidget::HNodeWidget::HNodeWidget( HAbstractTreeModel::HAbstractTreeModelNode::ptr_t data_ )
	: _unfolded( false ),
	_rowRaw( 0 ), _columnRaw( 0 ),
	_widthRaw( 0 ), _data( data_ ) {
	M_PROLOG
	return;
	M_EPILOG
}

HTreeWidget::HNodeWidget::~HNodeWidget( void ) {
	M_PROLOG
	return;
	M_DESTRUCTOR_EPILOG
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
  if ( ! node->has_children() )
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

HAbstractTreeModel::HAbstractTreeModelNode::ptr_t HTreeWidget::HNodeWidget::data( void ) {
	return ( _data );
}

HTreeWidget::HTreeWidget( HWindow* parent_, int row_, int column_,
		int height_, int width_, yaal::hcore::HString const& label_,
		HWidgetAttributesInterface const& attr_,
		HAbstractTreeModel::ptr_t const& model_ )
	: HWidget( parent_, row_, column_, height_, width_, label_, attr_ ),
	_model( model_ ), _view(), _selected( nullptr ) {
	M_PROLOG
	attr_.apply( *this );
	_model->register_listener( this );
	return;
	M_EPILOG
}

HTreeWidget::~HTreeWidget( void ) {
	M_PROLOG
	return;
	M_DESTRUCTOR_EPILOG
}

void HTreeWidget::do_paint( void ) {
	M_PROLOG
	HConsole& cons = HConsole::get_instance();
	if ( _focused ) {
		cons.curs_set( CURSOR::INVISIBLE );
	}
	draw_label();
	_varTmpBuffer.reserve( _widthRaw );
	_varTmpBuffer.fillz( '_'_ycp, 0, _widthRaw );
	for ( int ctr( 0 ); ctr < _heightRaw; ++ ctr ) {
		cons.mvprintf( _rowRaw + ctr, _columnRaw, _varTmpBuffer );
	}
	if ( _view.get_root() ) {
		draw_node( _view.get_root(), _rowRaw );
	}
	return;
	M_EPILOG
}

int HTreeWidget::draw_node( tree_view_t::node_t node_, int row_ ) {
	M_PROLOG
	int row = row_;
	HConsole& cons = HConsole::get_instance();
	M_ASSERT( node_ );
	if ( (**node_)._data->is_valid() && ! (**node_)._data->get_string().is_empty() ) {
		row ++;
		HString const& str = (**node_)._data->get_string();
		(**node_)._rowRaw = row;
		(**node_)._columnRaw = _columnRaw + node_->get_level() * 2 - 1;
		(**node_)._widthRaw = static_cast<int>( str.get_length() ) + 2;
		set_attr_data();
		if ( ! ( (**node_)._unfolded || ! node_->has_children() ) ) {
			cons.mvprintf( row, (**node_)._columnRaw, "+" );
		} else if ( node_->has_children() ) {
			cons.mvprintf( row, (**node_)._columnRaw, "-" );
		}
		if ( node_ == _selected )
			cons.set_attr(
				COLOR::complementary(
					_enabled ? ( _focused ? _attributeFocused._data : _attributeEnabled._data ) : _attributeDisabled._data
				)
			);
		cons.mvprintf( row, (**node_)._columnRaw + 1, str );
	}
	if ( node_->has_children() && ( (**node_)._unfolded || ! node_->get_level() ) ) {
		for ( tree_view_t::HNode::iterator it = node_->begin(); it != node_->end(); ++ it ) {
			row = draw_node( &*it, row );
		}
	}
	return row;
	M_EPILOG
}

int HTreeWidget::do_process_input( int code_ ) {
	M_PROLOG
	bool wasFolded = false;
	int errorCode = 0;
	code_ = HWidget::do_process_input( code_ );
	tree_view_t::node_t node = _selected;
	switch ( code_ ) {
		case ( KEY_CODE::HOME ): {
			node = _view.get_root();
			if ( node->has_children() ) {
				_selected = &*node->begin();
			}
		} break;
		case ( KEY_CODE::END ): {
			node = _view.get_root();
			if ( node->has_children() ) {
				_selected = &*prev( node->end() );
			}
		} break;
		case ( KEY_CODE::PAGE_UP ): {
		} break;
		case ( KEY_CODE::PAGE_DOWN ): {
		} break;
		case ( KEY_CODE::UP ): {
			_selected = previous( node );
			if ( _selected == node ) {
				if ( node->get_level() > 1 ) {
					_selected = node->get_parent();
				}
			} else {
				node = _selected;
				while ( (**node)._unfolded ) {
					if ( node->has_children() ) {
						_selected = &*node->begin();
						node = _selected;
						_selected = previous( node, true );
						node = _selected;
					}
				}
			}
		} break;
		case ( KEY_CODE::RIGHT ): {
			wasFolded = ! (**node)._unfolded;
			if ( node->has_children() ) {
				(**node)._unfolded = true;
				if ( wasFolded ) {
					break;
				}
			}
		}
		/* when node is unfolded, right key works as down key */
		/* fall through */
		case ( KEY_CODE::DOWN ): {
			if ( (**node)._unfolded ) {
				if ( node->has_children() ) {
					_selected = &*node->begin();
				}
			} else {
				node = _selected;
				node = next( node );
				while ( ! node ) {
					node = _selected->get_parent();
					if ( ! node || ( node->get_level() < 1 ) ) {
						break;
					}
					_selected = node;
					node = next( node );
				}
				if ( node && ( node->get_level() > 0 ) ) {
					_selected = node;
				}
			}
		} break;
		case ( KEY_CODE::LEFT ): {
			if ( (**node)._unfolded && node->get_level() ) {
				(**node)._unfolded = false;
			} else if ( node->get_level() > 1 ) {
				_selected = node->get_parent();
			}
		} break;
		case ( ' ' ):
		case ( '\r' ): {
			if ( node->has_children() ) {
				(**node)._unfolded = ! (**node)._unfolded;
			} else {
				errorCode = code_;
			}
		} break; /* I have to think more about it. */
		case ( '\t' ): {
			_focused = false;	/* very  */
			schedule_repaint();	/* magic */
		} /* fall through */ /* no break on purpose, see hlistwidget.c */
		default: {
			errorCode = code_;
		} break;
	}
	code_ = errorCode;
	if ( ! errorCode ) {
		schedule_repaint();
		if ( _window ) {
			_window->status_bar()->clear( COLOR::FG_LIGHTGRAY );
		}
	}
	return code_;
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
	return handled;
	M_EPILOG
}

bool HTreeWidget::do_click( tree_view_t::node_t node_, OMouse& mouse_ ) {
	M_PROLOG
	if ( (**node_).hit_test( mouse_._row, mouse_._column ) ) {
		(**node_).click( mouse_._column );
		_selected = node_;
		return ( true );
	}
	if ( node_->has_children() && ( (**node_)._unfolded || ! node_->get_level() ) ) {
		for ( tree_view_t::HNode::iterator it = node_->begin(); it != node_->end(); ++ it )
			if ( do_click( &*it, mouse_ ) )
				return ( true );
	}
	return ( false );
	M_EPILOG
}

HTreeWidget::tree_view_t::node_t HTreeWidget::previous( tree_view_t::node_t node, bool wrap ) {
	M_PROLOG
	tree_view_t::node_t p = nullptr;
	tree_view_t::node_t parent = node->get_parent();
	if ( parent ) {
		tree_view_t::HNode::iterator it;
		for ( it = parent->begin(); ( it != parent->end() ) && ( &*it != node ); ++ it )
			;
		if ( it != parent->end() ) {
			if ( it == parent->begin() ) {
				if ( wrap ) {
					it = prev( parent->end() );
				}
			} else {
				-- it;
			}
		}
		if ( it != parent->end() ) {
			p = &*it;
		}
	}
	return p;
	M_EPILOG
}

HTreeWidget::tree_view_t::node_t HTreeWidget::next( tree_view_t::node_t node ) {
	M_PROLOG
	tree_view_t::node_t parent = node->get_parent();
	tree_view_t::node_t n = nullptr;
	if ( parent ) {
		tree_view_t::HNode::iterator it;
		for ( it = parent->begin(); ( it != parent->end() ) && ( &*it != node ); ++ it )
			;
		if ( it != parent->end() )
			++ it;
		if ( it != parent->end() )
			n = &*it;
	}
	return n;
	M_EPILOG
}

void HTreeWidget::set_model( HAbstractTreeModel::ptr_t model_ ) {
	M_PROLOG
	_model = model_;
	_model->register_listener( this );
	return;
	M_EPILOG
}

HAbstractTreeModel::ptr_t HTreeWidget::get_model( void ) const {
	M_PROLOG
	return ( _model );
	M_EPILOG
}

void HTreeWidgetModelListener::on_model_changed( void ) {
	M_PROLOG
	do_on_model_changed();
	return;
	M_EPILOG
}

void HTreeWidget::build_view( tree_view_t::node_t viewNode_, HAbstractTreeModel::HAbstractTreeModelNode::ptr_t modelNode_ ) {
	M_PROLOG
	for ( int i( 0 ), COUNT( modelNode_->get_child_count() ); i < COUNT; ++ i ) {
		HAbstractTreeModel::HAbstractTreeModelNode::ptr_t mn( modelNode_->get_child( i ) );
		tree_view_t::node_t vn( &*viewNode_->add_node( mn ) );
		build_view( vn, mn );
	}
	return;
	M_EPILOG
}

void HTreeWidget::do_on_model_changed( void ) {
	M_PROLOG
	tree_view_t tv;
	HAbstractTreeModel::HAbstractTreeModelNode::ptr_t n( _model->get_root() );
	build_view( tv.create_new_root( n ), n );
	_view.swap( tv );
	return;
	M_EPILOG
}

HWidget::ptr_t HTreeWidgetCreator::do_new_instance( HWindow* window_, yaal::tools::HXml::HConstNodeProxy const& node_ ) {
	M_PROLOG
	HTreeWidgetAttributes attrs;
	prepare_attributes( attrs, node_ );
	OResource r( get_resource( node_ ) );
	HTreeWidget* tree( create_widget<HTreeWidget>( window_, r._row, r._column, r._height, r._width, r._label, attrs ) );
	apply_resources( tree->get_pointer(), node_ );
	return ( tree->get_pointer() );
	M_EPILOG
}

bool HTreeWidgetCreator::do_prepare_attributes( HWidgetAttributesInterface&, yaal::tools::HXml::HConstNodeProxy const& ) {
	M_PROLOG
	return ( true );
	M_EPILOG
}

bool HTreeWidgetCreator::do_apply_resources( HWidget::ptr_t, yaal::tools::HXml::HConstNodeProxy const& ) {
	return ( false );
}

namespace {

bool register_creator( void ) {
	HWidgetFactory::get_instance().register_widget_creator( "tree",
			HWidgetCreatorInterface::ptr_t( static_cast<HWidgetCreatorInterface*>( new ( memory::yaal ) HTreeWidgetCreator() ) ) );
	return ( true );
}

bool volatile registered = register_creator();

}

}

}

