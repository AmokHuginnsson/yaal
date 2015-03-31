/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hconsole/hinfointeger.hxx - this file is integral part of `yaal' project.

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

