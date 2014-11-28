/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hinfo.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCONSOLE_HINFO_HXX_INCLUDED
#define YAAL_HCONSOLE_HINFO_HXX_INCLUDED 1

#include "hcore/hstring.hxx"
#include "hcore/htime.hxx"
#include "hcore/hexception.hxx" /* types definitions */
#include "hcore/hstrongenum.hxx"
#include "hcore/reflection.hxx"
#include "hcore/hvariant.hxx"

namespace yaal {

namespace hconsole {

/*! \brief Type agnostic data container.
 */
class HInfo {
public:
	HInfo( void ) { }
	virtual ~HInfo( void ) { }
	int long long get_integer( void ) const {
		return ( do_get_integer() );
	}
	double long get_real( void ) const {
		return ( do_get_real() );
	}
	yaal::hcore::HString const& get_string( void ) const {
		return ( do_get_string() );
	}
	yaal::hcore::HTime const& get_time( void ) const {
		return ( do_get_time() );
	}
	void set_integer( int long long int_ ) {
		return ( do_set_integer( int_ ) );
	}
	void set_real( double long double_ ) {
		return ( do_set_real( double_ ) );
	}
	void set_string( yaal::hcore::HString const& string_ ) {
		return ( do_set_string( string_ ) );
	}
	void set_time( yaal::hcore::HTime const& time_ ) {
		return ( do_set_time( time_ ) );
	}
protected:
	virtual int long long do_get_integer( void ) const = 0;
	virtual double long do_get_real( void ) const = 0;
	virtual yaal::hcore::HString const& do_get_string( void ) const = 0;
	virtual yaal::hcore::HTime const& do_get_time( void ) const = 0;
	virtual void do_set_integer( int long long ) = 0;
	virtual void do_set_real( double long ) = 0;
	virtual void do_set_string( yaal::hcore::HString const& ) = 0;
	virtual void do_set_time( yaal::hcore::HTime const& ) = 0;
};

class HInfoMultiVal : public HInfo {
private:
	u64_t _type;
	int long long _integer;
	double long _real;
	mutable yaal::hcore::HString _string;
	mutable yaal::hcore::HTime _time;
public:
	typedef HInfoMultiVal this_type;
	typedef HInfo base_type;
	HInfoMultiVal( void );
	HInfoMultiVal( int long long );
	HInfoMultiVal( double long );
	HInfoMultiVal( yaal::hcore::HString const& );
	HInfoMultiVal( yaal::hcore::HTime const& );
	HInfoMultiVal( HInfoMultiVal const& );
	HInfoMultiVal& operator = ( HInfoMultiVal const& );
	virtual ~HInfoMultiVal( void );
	void swap( HInfoMultiVal& );
protected:
	virtual int long long do_get_integer( void ) const;
	virtual double long do_get_real( void ) const;
	virtual yaal::hcore::HString const& do_get_string( void ) const;
	virtual yaal::hcore::HTime const& do_get_time( void ) const;
	virtual void do_set_integer( int long long );
	virtual void do_set_real( double long );
	virtual void do_set_string( yaal::hcore::HString const& );
	virtual void do_set_time( yaal::hcore::HTime const& );
private:
	void purge( void );
};

class HInfoVariant : public HInfo {
private:
	type_id_t _type;
	typedef yaal::hcore::HVariant<int long long, double long, yaal::hcore::HString, yaal::hcore::HTime> data_t;
	data_t _data;
public:
	typedef HInfoVariant this_type;
	typedef HInfo base_type;
	HInfoVariant( void );
	HInfoVariant( int long long );
	HInfoVariant( double long );
	HInfoVariant( yaal::hcore::HString const& );
	HInfoVariant( yaal::hcore::HTime const& );
	HInfoVariant( HInfoVariant const& );
	HInfoVariant& operator = ( HInfoVariant const& );
	virtual ~HInfoVariant( void );
protected:
	virtual int long long do_get_integer( void ) const;
	virtual double long do_get_real( void ) const;
	virtual yaal::hcore::HString const& do_get_string( void ) const;
	virtual yaal::hcore::HTime const& do_get_time( void ) const;
	virtual void do_set_integer( int long long );
	virtual void do_set_real( double long );
	virtual void do_set_string( yaal::hcore::HString const& );
	virtual void do_set_time( yaal::hcore::HTime const& );
private:
	void purge( void );
};

}

}

#endif /* #ifndef YAAL_HCONSOLE_HINFO_HXX_INCLUDED */

