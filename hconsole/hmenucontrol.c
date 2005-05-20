/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hmenucontrol.c - this file is integral part of `stdhapi' project.

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

#include "hcore/hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "hcore/hlog.h"
#include "hmenucontrol.h"

using namespace stdhapi::hcore;

namespace stdhapi
{

namespace hconsole
{

HMenuControl::HMenuNode::HMenuNode ( HMenuNode * a_poNode )
						: HTreeControl::HNodeControl ( a_poNode )
	{
	M_PROLOG
	HItem l_oDummy ( 1 );
	f_tLeaf = l_oDummy;
	return;
	M_EPILOG
	}

HMenuControl::HMenuNode::~HMenuNode ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

int HMenuControl::HMenuNode::load_sub_menu ( OMenuItem * a_psSubMenu )
	{
	M_PROLOG
	int l_iCtr = 0;
	HString str;
	HMenuNode * l_poNode = NULL;
	HInfo l_oInfo;
	while ( a_psSubMenu [ l_iCtr ].f_pcLabel )
		{
		l_poNode = new HMenuNode ( this );
		l_oInfo ( a_psSubMenu [ l_iCtr ].f_pcLabel );
		l_oInfo ( static_cast < void * > ( & a_psSubMenu [ l_iCtr ] ) );
		l_poNode->f_tLeaf [ 0 ] = l_oInfo;
		f_oBranch.add_tail ( reinterpret_cast < HTree < HItem >::HNode * * > ( & l_poNode ) );
		if ( a_psSubMenu [ l_iCtr ].f_psSubMenu )
			l_poNode->load_sub_menu ( a_psSubMenu [ l_iCtr ].f_psSubMenu );
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
	return;
	M_EPILOG
	}

void HMenuControl::init ( HProcess * a_poProcess, OMenuItem * a_psMenu )
	{
	M_PROLOG
	HItem l_oDummy ( 0 );
	HMenuNode * l_poNode = NULL;
	if ( f_poRoot )
		M_THROW ( "menu already initialised", g_iErrNo );
	if ( ! ( a_poProcess && a_psMenu ) )
		M_THROW ( "process can not run without core data ( process, menu )",
				g_iErrNo );
	f_poProcess = a_poProcess;
	if ( a_psMenu )
		{
		f_poRoot = l_poNode = new HMenuControl::HMenuNode ( 0 );
		f_poRoot->get_object ( ) = l_oDummy;
		l_poNode->load_sub_menu ( a_psMenu );
		if ( ! f_poSelected && l_poNode->f_oBranch.quantity ( ) )
			f_poSelected = l_poNode->f_oBranch [ 0 ];
		refresh ( );
		}
	return;
	M_EPILOG
	}

int HMenuControl::process_input( int a_iCode )
	{
	M_PROLOG
	OMenuItem const * l_psMenu = NULL;
	a_iCode = HTreeControl::process_input ( a_iCode );
	if ( ( a_iCode == '\r' ) || ( a_iCode == ' ' ) )
		{
		l_psMenu = static_cast < OMenuItem * > ( static_cast < HMenuNode * > ( f_poSelected )->f_tLeaf [ 0 ].get < void * > ( ) );
		if ( l_psMenu->HANDLER )
			( f_poProcess->* ( l_psMenu->HANDLER ) )( );
		a_iCode = 0;
		}
	return ( a_iCode );
	M_EPILOG
	}

}

}

