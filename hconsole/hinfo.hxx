/* Read yaal/LICENSE.md file for copyright and licensing information. */

#ifndef YAAL_HCONSOLE_HINFO_HXX_INCLUDED
#define YAAL_HCONSOLE_HINFO_HXX_INCLUDED 1

#include "hcore/hstring.hxx"
#include "hcore/htime.hxx"
#include "hcore/hexception.hxx" /* types definitions */
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
	typedef u64_t type_mask_t;
	type_mask_t _type;
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
	virtual int long long do_get_integer( void ) const override;
	virtual double long do_get_real( void ) const override;
	virtual yaal::hcore::HString const& do_get_string( void ) const override;
	virtual yaal::hcore::HTime const& do_get_time( void ) const override;
	virtual void do_set_integer( int long long ) override;
	virtual void do_set_real( double long ) override;
	virtual void do_set_string( yaal::hcore::HString const& ) override;
	virtual void do_set_time( yaal::hcore::HTime const& ) override;
private:
	void purge( void );
};

class HInfoVariant : public HInfo {
private:
	TYPE _type;
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
	virtual int long long do_get_integer( void ) const override;
	virtual double long do_get_real( void ) const override;
	virtual yaal::hcore::HString const& do_get_string( void ) const override;
	virtual yaal::hcore::HTime const& do_get_time( void ) const override;
	virtual void do_set_integer( int long long ) override;
	virtual void do_set_real( double long ) override;
	virtual void do_set_string( yaal::hcore::HString const& ) override;
	virtual void do_set_time( yaal::hcore::HTime const& ) override;
private:
	void purge( void );
};

}

}

#endif /* #ifndef YAAL_HCONSOLE_HINFO_HXX_INCLUDED */

