/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hwindowfactory.hxx - this file is integral part of `yaal' project.

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
/*! \file hconsole/hwindowfactory.hxx
 * \brief Declaration of HWindowFactory class.
 */

#ifndef YAAL_HCONSOLE_HWINDOWFACTORY_HXX_INCLUDED
#define YAAL_HCONSOLE_HWINDOWFACTORY_HXX_INCLUDED 1

#include "hcore/hsingleton.hxx"
#include "tools/hxml.hxx"

namespace yaal {

namespace hconsole {

class HWindow;
class HTUIProcess;

class HWindowCreatorInterface {
public:
	typedef yaal::hcore::HPointer<HWindow> window_ptr_t;
protected:
	virtual void do_initialize_globals( void ) {};
	virtual void do_cleanup_globals( void ) {};
	virtual window_ptr_t do_new_instance( HTUIProcess*, yaal::tools::HXml::HConstNodeProxy const& ) = 0;
	virtual bool do_apply_resources( HTUIProcess*, window_ptr_t, yaal::tools::HXml::HConstNodeProxy const& ) {
		return ( true );
	}
public:
	virtual ~HWindowCreatorInterface( void ) {}
	void initialize_globals( void );
	void cleanup_globals( void );
	window_ptr_t new_instance( HTUIProcess*, yaal::tools::HXml::HConstNodeProxy const& );
	bool apply_resources( HTUIProcess*, window_ptr_t, yaal::tools::HXml::HConstNodeProxy const& );
	typedef yaal::hcore::HPointer<HWindowCreatorInterface> ptr_t;
};

class HWindowFactory : public yaal::hcore::HSingleton<HWindowFactory> {
public:
	typedef HWindowFactory this_type;
	typedef yaal::hcore::HMap<yaal::hcore::HString, HWindowCreatorInterface::ptr_t> creators_t;
private:
	creators_t _creators;
public:
	void register_window_creator( yaal::hcore::HString const&, HWindowCreatorInterface::ptr_t );
	HWindowCreatorInterface::window_ptr_t create_window( HTUIProcess*, yaal::tools::HXml::HConstNodeProxy const& );
	bool is_type_valid( yaal::hcore::HString const& );
	creators_t::iterator begin( void );
	creators_t::iterator end( void );
	void initialize_globals( void );
	void cleanup_globals( void );
private:
	HWindowFactory( void ) : _creators() {}
	~HWindowFactory( void ) {}
	static int life_time( int );
	friend class yaal::hcore::HSingleton<HWindowFactory>;
	friend class yaal::hcore::HDestructor<HWindowFactory>;
};

}

}

#endif /* #ifndef YAAL_HCONSOLE_HWINDOWFACTORY_HXX_INCLUDED */

