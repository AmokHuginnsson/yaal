/* Read yaal/LICENSE.md file for copyright and licensing information. */
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

class M_YAAL_HCONSOLE_PUBLIC_API HWindowFactory : public yaal::hcore::HSingleton<HWindowFactory> {
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

