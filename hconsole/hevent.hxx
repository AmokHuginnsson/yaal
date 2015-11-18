/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  hevent.hxx - this file is integral part of `yaal' project.

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
/*! \file hconsole/hevent.hxx
 * \brief Declaration of HEvent class hierarchy.
 */

#ifndef YAAL_HCONSOLE_HEVENT_HXX_INCLUDED
#define YAAL_HCONSOLE_HEVENT_HXX_INCLUDED 1

#include "hcore/hstring.hxx"
#include "tools/hsignal.hxx"
#include "hconsole/mouse.hxx"

namespace yaal {

namespace hconsole {

class HEvent {
public:
	typedef HEvent this_type;
	enum class TYPE {
		KEY_PRESS,
		MOUSE,
		COMMAND,
		WINDOW,
		WIDGET
	};
public:
	virtual ~HEvent( void ) {
	}
	TYPE get_type( void ) const {
		return ( do_get_type() );
	}
private:
	virtual TYPE do_get_type( void ) const = 0;
};

typedef hcore::HBoundCall<bool ( HEvent const& )> event_listener_t;
typedef tools::HSignal<bool ( HEvent const& )> event_dispatcher_t;

class HKeyPressEvent : public HEvent {
public:
	typedef HKeyPressEvent this_type;
	typedef HEvent base_type;
private:
	int _keyCode;
public:
	HKeyPressEvent( int keyCode_ )
		: _keyCode( keyCode_ ) {
	}
	int get_key_code( void ) const {
		return ( _keyCode );
	}
private:
	virtual TYPE do_get_type( void ) const override {
		return ( TYPE::KEY_PRESS );
	}
};

class HMouseEvent : public HEvent {
public:
	typedef HMouseEvent this_type;
	typedef HEvent base_type;
private:
	mouse::OMouse _mouse;
public:
	HMouseEvent( mouse::OMouse const& mouse_ )
		: _mouse( mouse_ ) {
	}
	mouse::OMouse const& get_mouse_data( void ) const {
		return ( _mouse );
	}
private:
	virtual TYPE do_get_type( void ) const override {
		return ( TYPE::MOUSE );
	}
};

class HCommandEvent : public HEvent {
public:
	typedef HCommandEvent this_type;
	typedef HEvent base_type;
private:
	yaal::hcore::HString _command;
public:
	HCommandEvent( yaal::hcore::HString const& command_ )
		: _command( command_ ) {
	}
	yaal::hcore::HString const& get_command( void ) const {
		return ( _command );
	}
private:
	virtual TYPE do_get_type( void ) const override {
		return ( TYPE::COMMAND );
	}
};

}

}

#endif /* #ifndef YAAL_HCONSOLE_HEVENT_HXX_INCLUDED */

