/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hmenuwidget.cxx - this file is integral part of `yaal' project.

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

#include <libintl.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hcore/hlog.hxx"
#include "hmenuwidget.hxx"

using namespace yaal::hcore;
using namespace yaal::hconsole::mouse;

namespace yaal {

namespace hconsole {

int HMenuWidget::load_sub_menu( menu_model_t::data_t::node_t node, OMenuItem* subMenu_ ) {
	M_PROLOG
	int ctr = 0;
	HString str;
	while ( ! subMenu_[ ctr ]._label.is_empty() ) {
		HInfoItem info( 1 );
		info[ 0 ].set_string( subMenu_ [ ctr ]._label );
		info[ 0 ].set_pointer( static_cast<void*>( &subMenu_[ ctr ] ) );
		menu_model_t::data_t::HNode::iterator it = node->add_node( info );
		if ( subMenu_[ ctr ]._subMenu ) {
			load_sub_menu( &*it, subMenu_[ ctr ]._subMenu );
		}
		++ ctr;
	}
	return ( ctr );
	M_EPILOG
}

HMenuWidget::HMenuWidget( HWindow* parent_,
		int row_, int column_, int height_, int width_,
		yaal::hcore::HString const& label_ )
	: HWidget( parent_, row_, column_, height_, width_, label_,
			HWidgetAttributes().label_position( HWidget::LABEL::POSITION::STACKED ) ),
	HTreeWidget( parent_, row_, column_, height_, width_, label_ ),
	_process( NULL ) {
	M_PROLOG
	return;
	M_EPILOG
}

HMenuWidget::~HMenuWidget ( void ) {
	M_PROLOG
	_process = NULL;
	return;
	M_EPILOG
}

void HMenuWidget::init( HTUIProcess* process_, OMenuItem* menu_ ) {
	M_PROLOG
	if ( _view.get_root() && menu_ ) {
		M_THROW( "menu already initialised", errno );
	}
	if ( ! process_ || ! ( _view.get_root() || menu_ ) ) {
		M_THROW( "process cannot run without core data ( process, menu )",
				errno );
	}
	_process = process_;
	menu_model_t::data_t::node_t node( NULL );
	if ( ! _view.get_root() ) {
		node = static_cast<menu_model_t*>( _model.get() )->get_data()->create_new_root();
		load_sub_menu( node, menu_ );
		on_model_changed();
	}
	if ( ! _selected && node->has_childs() ) {
		_selected = &*_view.get_root()->begin();
	}
	schedule_repaint();
	return;
	M_EPILOG
}

OMenuItem* HMenuWidget::get_selected( void ) {
	M_PROLOG
	menu_model_t::HAsIsValueTreeModelNode* node( static_cast<menu_model_t::HAsIsValueTreeModelNode*>( (**_selected).data().get() ) );
	OMenuItem* menu( static_cast<OMenuItem*>( node->get()[0].get_pointer() ) );
	return ( menu );
	M_EPILOG
}

int HMenuWidget::do_process_input( int code_ ) {
	M_PROLOG
	if ( ! _view.get_root() ) {
		M_THROW( _( "menu not initialized" ), errno );
	}
	code_ = HTreeWidget::do_process_input ( code_ );
	if ( ( code_ == '\r' ) || ( code_ == ' ' ) ) {
		OMenuItem* menu( get_selected() );
		if ( menu->HANDLER ) {
			menu->call( _process );
		}
		code_ = 0;
	}
	return ( code_ );
	M_EPILOG
}

bool HMenuWidget::do_click( OMouse& mouse_ ) {
	M_PROLOG
	tree_view_t::node_t selected( _selected );
	bool unfolded( (**_selected).is_unfolded() );
	bool handled( HTreeWidget::do_click( mouse_ ) );
	if ( handled && ( _selected == selected ) && ( (**_selected).is_unfolded() == unfolded ) ) {
		OMenuItem* menu( get_selected() );
		if ( menu->HANDLER ) {
			menu->call( _process );
		}
	}
	return ( handled );
	M_EPILOG
}

}

}

