/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hwindowlist.cxx - this file is integral part of `yaal' project.

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
M_VCSID ( "$Id$" )
#include "hwindowlistcontrol.h"

using namespace yaal::hcore;
using namespace yaal::hconsole::list_control_helper;

namespace yaal
{

namespace hconsole
{

HWindowListControl::HWindowListControl ( HWindow * a_poParent, int a_iRow,
		int a_iColumn, int a_iHeight, int a_iWidth, char const * a_pcLabel,
		HAbstractControler::ptr_t const& a_oControler )
									:	HControl ( a_poParent, a_iRow, a_iColumn, a_iHeight,
											a_iWidth, a_pcLabel ),
										HSearchableControl( false ),
										HListControl( a_poParent, a_iRow, a_iColumn, a_iHeight,
												a_iWidth, a_pcLabel, a_oControler )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HWindowListControl::~HWindowListControl ( void )
	{
	M_PROLOG
/*	HWindowListControl::remove_tail ( D_FORCE_REMOVE_ELEMENT ); */
	return;
	M_EPILOG
	}

int HWindowListControl::do_process_input ( int a_iCode )
	{
	M_PROLOG
	a_iCode = HListControl::do_process_input ( a_iCode );
	if ( ( f_oControler->size() > 1 ) && ( ( a_iCode == '\r' ) || ( a_iCode == ' ' ) ) )
		a_iCode = KEY < '\t' >::meta;
	return ( a_iCode );
	M_EPILOG
	}

void HWindowListControl::do_refresh ( void )
	{
	M_PROLOG
	HListControl::do_refresh ( );
	M_EPILOG
	}

namespace list_control_helper
{

template<>
yaal::hcore::HString const HCell<HWindow::ptr_t>::get_string( void )
	{
	return ( f_rtData->get_title() );
	}

/*
int long GetIdFromCell( HItem const & )
	{
	return ( 0 );
	}

bool GetStateFromCell( HItem const & )
	{
	return ( false );
	}
*/

template<>
bool compare_cells( HWindow::ptr_t const& a_oLeft, HWindow::ptr_t const& a_oRight, OSortHelper & a_roSortHelper )
	{
	a_roSortHelper.progress();
	return ( strcasecmp ( a_oLeft->get_title(),
			 a_oRight->get_title() ) > 0 );
	}

typedef yaal::hcore::HList<HWindow::ptr_t>::iterator window_iterator_t;

template<>
HRow<window_iterator_t>::HRow( iterator_t& a_oIt ) : f_roIterator( a_oIt ), f_oCells( 1 )
	{
	f_oCells[ 0 ] = HCell<window_iterator_t>::ptr_t( new HCell<window_iterator_t>( f_roIterator, 0 ) );
	return;
	}

template<>
void HRow<window_iterator_t>::switch_state( void )
	{
	return;
	}

template<>
int long HRow<window_iterator_t>::get_id( void )
	{
	return ( -1 );
	}

template<>
bool HRow<window_iterator_t>::get_checked( void )
	{
	return ( false );
	}

template<>
yaal::hcore::HString const HCell<window_iterator_t>::get_long( void )
	{
	return ( 0 );
	}

template<>
yaal::hcore::HString const HCell<window_iterator_t>::get_double( void )
	{
	return ( 0 );
	}

template<>
yaal::hcore::HString const HCell<window_iterator_t>::get_string( void )
	{
	return ( (*f_rtData)->get_title() );
	}

template<>
char const * HCell<window_iterator_t>::get_time( void )
	{
	return ( "" );
	}

template<>
void HCell<window_iterator_t>::set_child_control_data( HControl* )
	{
	return;
	}

template<>
bool CompareListControlItems<HWindow::ptr_t>::operator() ( HWindow::ptr_t const& a_oLeft,
		HWindow::ptr_t const& a_oRight ) const
	{
	M_PROLOG
	HWindow::ptr_t const& l_oLeft = f_roSortHelper.f_eOrder == yaal::hcore::OListBits::D_ASCENDING ? a_oLeft : a_oRight;
	HWindow::ptr_t const& l_oRight = f_roSortHelper.f_eOrder == yaal::hcore::OListBits::D_ASCENDING ? a_oRight : a_oLeft;
	return ( strcasecmp( l_oLeft->get_title(), l_oRight->get_title() ) > 0 );
	M_EPILOG
	}

}

}

}

