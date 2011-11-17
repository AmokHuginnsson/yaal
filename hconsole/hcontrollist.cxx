/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hcontrollist.cxx - this file is integral part of `yaal' project.

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

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hcontrollist.hxx"
#include "hstatusbarcontrol.hxx"

using namespace yaal::hcore;

namespace yaal {

namespace hconsole {

HControlList::HControlList( model_t::cyclic_iterator& focused_ ) : _list(), _focused( focused_ ) {
	M_PROLOG
	return;
	M_EPILOG
}

void HControlList::next_enabled( char shorcut_ ) {
	M_PROLOG
	bool loop = true;
	HControlList::model_t::cyclic_iterator it = _focused;
	do {
		++ _focused;
		if ( dynamic_cast<HStatusBarControl*>( &(*(*_focused))) )
			continue;
		loop = (*_focused)->set_focus( shorcut_ ) ? true : false;
		if ( _focused == it )
			loop = false;
	} while ( loop );
	if ( _focused != it ) {
		(*it)->kill_focus();
		(*_focused)->set_focus( -1 );
	}
	return;
	M_EPILOG
}

void HControlList::add_control( HControl::ptr_t control_ ) {
	M_PROLOG
	_list.push_back( control_ );
	if ( _focused == _list.end() )
		_focused = _list.rbegin().base();
	return;
	M_EPILOG
}

void HControlList::refresh_all( bool force_ ) {
	M_PROLOG
	for ( model_t::iterator it = _list.begin();
			it != _list.end(); ++ it )
		if ( ( it != _focused ) && ( force_ || (*it)->need_repaint() ) )
			(*it)->refresh();
	if ( !! (*_focused) && ( force_ || (*_focused)->need_repaint() ) )
		(*_focused)->refresh();
	return;
	M_EPILOG
}

void HControlList::update_all( void ) {
	M_PROLOG
	for ( model_t::iterator it = _list.begin();
			it != _list.end(); ++ it )
			(*it)->update();
	return;
	M_EPILOG
}

int HControlList::hit_test_all( mouse::OMouse& mouse_ ) {
	M_PROLOG
	if ( (*_focused)->hit_test( mouse_._row, mouse_._column ) )
		return ( (*_focused)->click( mouse_ ) );
	for ( model_t::iterator it = _list.begin();
			it != _list.end(); ++ it )
		if ( (*it)->hit_test( mouse_._row, mouse_._column ) )
			return ( (*it)->click( mouse_ ) );
	return( 0 );
	M_EPILOG
}

HControl* HControlList::get_control_by_no( int offset_ ) {
	M_PROLOG
	model_t::iterator it = _list.begin();
	while ( offset_ -- > 0 )
		++ it;
	return ( &**it );
	M_EPILOG
}

void HControlList::pop_front( void ) {
	M_PROLOG
	_list.pop_front();
	M_EPILOG
}

void HControlList::select( HControl const* control_ ) {
	M_PROLOG
	if ( (*_focused) != control_ ) {
		model_t::iterator it( _list.begin() );
		for ( model_t::iterator end( _list.end() ); it != end; ++ it ) {
			if ( (*it) == control_ ) {
				_focused = it;
				break;
			}
		}
		if ( it == _list.end() )
			M_THROW( "bogus object", reinterpret_cast<int long>( control_ ) );
	}
	return;
	M_EPILOG
}

void HControlList::select( HControl::ptr_t const& control_ ) {
	M_PROLOG
	select( control_.raw() );
	return;
	M_EPILOG
}

void HControlList::exchange( int former_, int latter_ ) {
	M_PROLOG
	_list.exchange( _list.n_th( former_ + 1 ), _list.n_th( latter_ + 1 ) );
	return;
	M_EPILOG
}

}

}

