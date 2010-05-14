/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hmenucontrol.cxx - this file is integral part of `yaal' project.

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
M_VCSID( "$Id: "__TID__" $" )
#include "hcore/hlog.hxx"
#include "hmenucontrol.hxx"

using namespace yaal::hcore;

namespace yaal
{

namespace hconsole
{

int HMenuControl::load_sub_menu( tree_t::node_t node, OMenuItem* subMenu_ )
	{
	M_PROLOG
	int ctr = 0;
	HString str;
	while ( ! subMenu_[ ctr ]._label.is_empty() )
		{
		HNodeControl info( 1 );
		info[ 0 ]( subMenu_ [ ctr ]._label );
		info[ 0 ]( static_cast<void*>( &subMenu_[ ctr ] ) );
		tree_t::iterator it = node->add_node( info );
		if ( subMenu_ [ ctr ]._subMenu )
			load_sub_menu( &*it, subMenu_[ ctr ]._subMenu );
		ctr ++;
		}
	return ( ctr );
	M_EPILOG
	}

HMenuControl::HMenuControl( HWindow* parent_,
		int row_, int column_, int height_, int width_,
		char const* label_ )
	: HControl( parent_, row_, column_, height_, width_, label_ ),
	HTreeControl( parent_, row_, column_, height_, width_, label_ ),
	_process( NULL )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HMenuControl::~HMenuControl ( void )
	{
	M_PROLOG
	_process = NULL;
	return;
	M_EPILOG
	}

void HMenuControl::init( HTUIProcess* process_, OMenuItem* menu_ )
	{
	M_PROLOG
	if ( _tree.get_root() && menu_ )
		M_THROW ( "menu already initialised", errno );
	if ( ! process_ || ! ( _tree.get_root() || menu_ ) )
		M_THROW ( "process cannot run without core data ( process, menu )",
				errno );
	_process = process_;
	tree_t::node_t node = NULL;
	if ( ! _tree.get_root() )
		{
		node = _tree.create_new_root();
		load_sub_menu( node, menu_ );
		}
	if ( ! _selected && node->has_childs() )
		_selected = &*node->begin();
	schedule_refresh();
	return;
	M_EPILOG
	}

int HMenuControl::do_process_input( int code_ )
	{
	M_PROLOG
	OMenuItem* menu = NULL;
	if ( ! _tree.get_root() )
		M_THROW ( _ ( "menu not initialized" ), errno );
	code_ = HTreeControl::do_process_input ( code_ );
	if ( ( code_ == '\r' ) || ( code_ == ' ' ) )
		{
		menu = static_cast<OMenuItem*>( (**_selected)[ 0 ].get<void*>() );
		if ( menu->HANDLER )
			menu->call( _process );
		code_ = 0;
		}
	return ( code_ );
	M_EPILOG
	}

}

}

