/* Read yaal/LICENSE.md file for copyright and licensing information. */

#ifndef HINFOTIME_HXX_INCLUDED
#define HINFOTIME_HXX_INCLUDED 1

#include "hcore/htime.hxx"
#include "hconsole/hinfo.hxx"

namespace yaal {

namespace hconsole {

class HInfoTime : public HInfo {
private:
	yaal::hcore::HTime& _data;
public:
	typedef HInfoTime this_type;
	typedef HInfo base_type;
	HInfoTime( yaal::hcore::HTime& );
	virtual ~HInfoTime( void );
protected:
	virtual int long long do_get_integer( void ) const override;
	virtual double long do_get_real( void ) const override M_DEBUG_CODE( __attribute__(( noreturn )) );
	virtual yaal::hcore::HString const& do_get_string( void ) const override M_DEBUG_CODE( __attribute__(( noreturn )) );
	virtual yaal::hcore::HTime const& do_get_time( void ) const override;
	virtual void do_set_integer( int long long ) override;
	virtual void do_set_real( double long ) override M_DEBUG_CODE( __attribute__(( noreturn )) );
	virtual void do_set_string( yaal::hcore::HString const& ) override;
	virtual void do_set_time( yaal::hcore::HTime const& ) override;
private:
	void purge( void );
};

}

}

#endif /* #ifndef HINFOTIME_HXX_INCLUDED */

