/* Read yaal/LICENSE.md file for copyright and licensing information. */

#ifndef YAAL_HCONSOLE_HINFOINTEGER_HXX_INCLUDED
#define YAAL_HCONSOLE_HINFOINTEGER_HXX_INCLUDED 1

#include "hconsole/hinfo.hxx"

namespace yaal {

namespace hconsole {

class HInfoInteger : public HInfo {
	int long long _integer;
public:
	typedef HInfoInteger this_type;
	typedef HInfo base_type;
	HInfoInteger( int long long );
	HInfoInteger( HInfoInteger const& );
	virtual ~HInfoInteger( void );
protected:
	virtual int long long do_get_integer( void ) const override;
	virtual double long do_get_real( void ) const override M_DEBUG_CODE( __attribute__(( noreturn )) );
	virtual yaal::hcore::HString const& do_get_string( void ) const override M_DEBUG_CODE( __attribute__(( noreturn )) );
	virtual yaal::hcore::HTime const& do_get_time( void ) const override M_DEBUG_CODE( __attribute__(( noreturn )) );
	virtual void do_set_integer( int long long ) override;
	virtual void do_set_real( double long ) override M_DEBUG_CODE( __attribute__(( noreturn )) );
	virtual void do_set_string( yaal::hcore::HString const& ) override;
	virtual void do_set_time( yaal::hcore::HTime const& ) override;
};

}

}

#endif /* #ifndef YAAL_HCONSOLE_HINFOINTEGER_HXX_INCLUDED */

