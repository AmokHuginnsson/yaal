/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hcontrollist.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCONSOLE_HCONTROLLIST_HXX_INCLUDED
#define YAAL_HCONSOLE_HCONTROLLIST_HXX_INCLUDED 1

#include "hcore/hlist.hxx"
#include "hconsole/hcontrol.hxx"

namespace yaal {

namespace hconsole {

class HControl;

/*! \brief Keep list of TUI controls for a given window.
 */
class HControlList {
public:
	typedef HControlList this_type;
	typedef hcore::HList<HControl::ptr_t> model_t;
private:
	model_t _list;
	model_t::cyclic_iterator& _focused;
public:
	HControlList( model_t::cyclic_iterator& );
	/* find next enabled control in window, if short cut char is specified */
	void next_enabled( char = 0 ); /* enabled and match shortcut char */
	void pop_front( void );
	void select( HControl const* );	/* this one should be private :( */
	void select( HControl::ptr_t const& );	/* this one should be private :( */
	void add_control( HControl::ptr_t );
	void refresh_all( bool );
	void update_all( void );
	void hit_test_all( mouse::OMouse& );
	HControl* get_control_by_no( int );
	void exchange( int, int );
};

typedef yaal::hcore::HExceptionT<HControlList> HControlListException;

}

}

#endif /* #ifndef YAAL_HCONSOLE_HCONTROLLIST_HXX_INCLUDED */

