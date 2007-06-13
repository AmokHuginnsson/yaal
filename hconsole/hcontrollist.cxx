/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hcontrollist.cxx - this file is integral part of `yaal' project.

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
#include "hcontrollist.h"
#include "hstatusbarcontrol.h"

using namespace yaal::hcore;

namespace yaal
{

namespace hconsole
{

HControlList::HControlList( model_t::cyclic_iterator& a_roFocused ) : f_oList(), f_roFocused( a_roFocused )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

void HControlList::next_enabled( char a_cShorcut )
	{
	M_PROLOG
	bool l_bLoop = true;
	HControlList::model_t::cyclic_iterator it = f_roFocused;
	do
		{
		++ f_roFocused;
		if ( dynamic_cast<HStatusBarControl*>( &(*(*f_roFocused))) )
			continue;
		l_bLoop = (*f_roFocused)->set_focus( a_cShorcut );
		if ( f_roFocused == it )
			l_bLoop = false;
		}
	while ( l_bLoop );
	if ( f_roFocused != it )
		{
		(*it)->kill_focus();
		(*f_roFocused)->set_focus( -1 );
		}
	return;
	M_EPILOG
	}

void HControlList::add_control( HControl::ptr_t a_oControl )
	{
	M_PROLOG
	f_oList.push_back( a_oControl );
	if ( ! f_roFocused.is_valid() )
		f_roFocused = f_oList.rbegin();
	return;
	M_EPILOG
	}

void HControlList::refresh_all( bool a_bForce )
	{
	M_PROLOG
	for ( model_t::iterator it = f_oList.begin();
			it != f_oList.end(); ++ it )
		if ( ( it != f_roFocused ) && ( a_bForce || (*it)->need_repaint() ) )
			(*it)->refresh();
	if ( !! (*f_roFocused) && ( a_bForce || (*f_roFocused)->need_repaint() ) )
		(*f_roFocused)->refresh();
	return;
	M_EPILOG
	}

void HControlList::update_all( void )
	{
	M_PROLOG
	for ( model_t::iterator it = f_oList.begin();
			it != f_oList.end(); ++ it )
			(*it)->update();
	return;
	M_EPILOG
	}

int HControlList::hit_test_all( mouse::OMouse& a_rsMouse )
	{
	M_PROLOG
	if ( (*f_roFocused)->hit_test ( a_rsMouse.f_iRow, a_rsMouse.f_iColumn ) )
		return ( (*f_roFocused)->click ( a_rsMouse ) );
	for ( model_t::iterator it = f_oList.begin();
			it != f_oList.end(); ++ it )
		if ( (*it)->hit_test ( a_rsMouse.f_iRow, a_rsMouse.f_iColumn ) )
			return ( (*it)->click ( a_rsMouse ) );
	return( 0 );
	M_EPILOG
	}

HControl* HControlList::get_control_by_no( int a_iOffset )
	{
	M_PROLOG
	model_t::iterator it = f_oList.begin();
	while ( a_iOffset -- > 0 )
		++ it;
	return ( &**it );
	M_EPILOG
	}

void HControlList::pop_front( void )
	{
	M_PROLOG
	f_oList.remove_head();
	M_EPILOG
	}

void HControlList::select( HControl const* a_poControl )
	{
	M_PROLOG
	if ( (*f_roFocused) != a_poControl )
		{
		model_t::iterator it;
		for (  it = f_oList.begin(); it != f_oList.end(); ++ it )
			{
			if ( (*it) == a_poControl )
				{
				f_roFocused = it;
				break;
				}
			}
		if ( it == f_oList.end() )
			M_THROW ( "bogus object", reinterpret_cast < int > ( a_poControl ) );
		}
	return;
	M_EPILOG
	}

void HControlList::select( HControl::ptr_t const& a_oControl )
	{
	M_PROLOG
	select( a_oControl.raw() );
	return;
	M_EPILOG
	}

void HControlList::exchange( int a_iFormer, int a_iLatter )
	{
	M_PROLOG
	f_oList.exchange( a_iFormer + 1, a_iLatter + 1 );
	return;
	M_EPILOG
	}

}

}

