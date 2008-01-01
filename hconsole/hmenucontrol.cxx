/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hmenucontrol.cxx - this file is integral part of `yaal' project.

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

#include <libintl.h>

#include "hcore/hexception.h"
M_VCSID ( "$Id$" )
#include "hcore/hlog.h"
#include "hmenucontrol.h"

using namespace yaal::hcore;

namespace yaal
{

namespace hconsole
{

int HMenuControl::load_sub_menu( tree_t::node_t node, OMenuItem* a_psSubMenu )
	{
	M_PROLOG
	int l_iCtr = 0;
	HString str;
	while ( ! a_psSubMenu [ l_iCtr ].f_oLabel.is_empty() )
		{
		HNodeControl info;
		info[ 0 ]( a_psSubMenu [ l_iCtr ].f_oLabel );
		info[ 0 ]( static_cast<void*>( &a_psSubMenu[ l_iCtr ] ) );
		tree_t::iterator it = node->add_node( info );
		if ( a_psSubMenu [ l_iCtr ].f_psSubMenu )
			load_sub_menu( &*it, a_psSubMenu[ l_iCtr ].f_psSubMenu );
		l_iCtr ++;
		}
	return ( l_iCtr );
	M_EPILOG
	}

HMenuControl::HMenuControl ( HWindow * a_poParent,
		int a_iRow, int a_iColumn, int a_iHeight, int a_iWidth,
		char const * a_pcLabel )
						: HControl ( a_poParent, a_iRow, a_iColumn, a_iHeight,
								a_iWidth, a_pcLabel ),
								HTreeControl ( a_poParent, a_iRow, a_iColumn,
										a_iHeight, a_iWidth, a_pcLabel ), f_poProcess ( NULL )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HMenuControl::~HMenuControl ( void )
	{
	M_PROLOG
	f_poProcess = NULL;
	return;
	M_EPILOG
	}

void HMenuControl::init( HTUIProcess* a_poProcess, OMenuItem* a_psMenu )
	{
	M_PROLOG
	if ( f_oTree.get_root() && a_psMenu )
		M_THROW ( "menu already initialised", errno );
	if ( ! a_poProcess || ! ( f_oTree.get_root() || a_psMenu ) )
		M_THROW ( "process can not run without core data ( process, menu )",
				errno );
	f_poProcess = a_poProcess;
	tree_t::node_t l_poNode = NULL;
	if ( ! f_oTree.get_root() )
		{
		l_poNode = f_oTree.create_new_root();
		load_sub_menu( l_poNode, a_psMenu );
		}
	if ( ! f_poSelected && l_poNode->has_childs() )
		f_poSelected = &*l_poNode->begin();
	schedule_refresh();
	return;
	M_EPILOG
	}

int HMenuControl::do_process_input( int a_iCode )
	{
	M_PROLOG
	OMenuItem const * l_psMenu = NULL;
	if ( ! f_oTree.get_root() )
		M_THROW ( _ ( "menu not initialized" ), errno );
	a_iCode = HTreeControl::do_process_input ( a_iCode );
	if ( ( a_iCode == '\r' ) || ( a_iCode == ' ' ) )
		{
		l_psMenu = static_cast<OMenuItem const*>( (**f_poSelected)[ 0 ].get<void*>() );
		if ( l_psMenu->HANDLER )
			static_cast<void>( ( f_poProcess->* ( l_psMenu->HANDLER ) )() );
		a_iCode = 0;
		}
	return ( a_iCode );
	M_EPILOG
	}

}

}

