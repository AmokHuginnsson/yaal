/* Read yaal/LICENSE.md file for copyright and licensing information. */
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

