/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hdatatreecontrol.cxx - this file is integral part of `yaal' project.

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

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "hdatatreecontrol.hxx"
#include "hdatawindow.hxx"

using namespace yaal::hcore;
using namespace yaal::hconsole;
using namespace yaal::dbwrapper;

namespace yaal
{

namespace hdata
{

HDataTreeControl::HDataTreeControl(
		HDataWindow* a_poWindow, int a_iRow, int a_iColumn, int a_iHeight,
		int a_iWidth, char const* a_pcTitle )
								: HControl( a_poWindow, a_iRow, a_iColumn, a_iHeight,
										a_iWidth, a_pcTitle ),
								HTreeControl( NULL, 0, 0, 0, 0, NULL ),
								HDataControl()
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HDataTreeControl::~HDataTreeControl ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

void HDataTreeControl::load ( int long /*a_iId*/ )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

}

}

