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
	char operator [] ( int idx_ ) const {
		return ( do_get_byte( idx_ ) );
	}
	int get_char( void ) const {
		return ( do_get_char() );
	}
	int get_int_short( void ) const {
		return ( do_get_int_short() );
	}
	int get_int( void ) const {
		return ( do_get_int() );
	}
	int long get_int_long( void ) const {
		return ( do_get_int_long() );
	}
	double get_double( void ) const {
		return ( do_get_double() );
	}
	double long get_double_long( void ) const {
		return ( do_get_double_long() );
	}
	void* get_pointer( void ) const {
		return ( do_get_pointer() );
	}
	yaal::hcore::HString const& get_string( void ) const {
		return ( do_get_string() );
	}
	yaal::hcore::HTime const& get_time( void ) const {
		return ( do_get_time() );
	}
	void set_char( char ch_ ) {
		return ( do_set_char( ch_ ) );
	}
	void set_short( int short short_ ) {
		return ( do_set_int_short( short_ ) );
	}
	void set_int( int int_ ) {
		return ( do_set_int( int_ ) );
	}
	void set_int_long( int long intLong_ ) {
		return ( do_set_int_long( intLong_ ) );
	}
	void set_double( double double_ ) {
		return ( do_set_double( double_ ) );
	}
	void set_double_long( double long doubleLong_ ) {
		return ( do_set_double_long( doubleLong_ ) );
	}
	void set_pointer( void* pointer_ ) {
		return ( do_set_pointer( pointer_ ) );
	}
	void set_string( yaal::hcore::HString const& string_ ) {
		return ( do_set_string( string_ ) );
	}
	void set_time( yaal::hcore::HTime const& time_ ) {
		return ( do_set_time( time_ ) );
	}
protected:
	virtual char do_get_byte( int ) const = 0;
	virtual char do_get_char( void ) const = 0;
	virtual int short do_get_int_short( void ) const = 0;
	virtual int do_get_int( void ) const = 0;
	virtual int long do_get_int_long( void ) const = 0;
	virtual double do_get_double( void ) const = 0;
	virtual double long do_get_double_long( void ) const = 0;
	virtual void* do_get_pointer( void ) const = 0;
	virtual yaal::hcore::HString const& do_get_string( void ) const = 0;
	virtual yaal::hcore::HTime const& do_get_time( void ) const = 0;
	virtual void do_set_char( char ) = 0;
	virtual void do_set_int_short( int short ) = 0;
	virtual void do_set_int( int ) = 0;
	virtual void do_set_int_long( int long ) = 0;
	virtual void do_set_double( double ) = 0;
	virtual void do_set_double_long( double long ) = 0;
	virtual void do_set_pointer( void* ) = 0;
	virtual void do_set_string( yaal::hcore::HString const& ) = 0;
	virtual void do_set_time( yaal::hcore::HTime const& ) = 0;
};

class HInfoMultiVal : public HInfo {
private:
	type_id_t   _type;
	char        _char;
	int short   _intShort;
	int			  	_int;
	int long  	_intLong;
	double      _double;
	double long _doubleLong;
	void*       _pointer;
	yaal::hcore::HString _string;
	yaal::hcore::HTime   _time;
public:
	typedef HInfoMultiVal this_type;
	typedef HInfo base_type;
	HInfoMultiVal( void );
	HInfoMultiVal( int );
	HInfoMultiVal( int long );
	HInfoMultiVal( double );
	HInfoMultiVal( double long );
	HInfoMultiVal( char const* const );
	HInfoMultiVal( yaal::hcore::HString const& );
	HInfoMultiVal( yaal::hcore::HTime const& );
	HInfoMultiVal( void* const );
	HInfoMultiVal( HInfoMultiVal const& );
	HInfoMultiVal& operator = ( HInfoMultiVal const& );
	virtual ~HInfoMultiVal( void );
protected:
	virtual char do_get_byte( int ) const;
	virtual char do_get_char( void ) const;
	virtual int short do_get_int_short( void ) const;
	virtual int do_get_int( void ) const;
	virtual int long do_get_int_long( void ) const;
	virtual double do_get_double( void ) const;
	virtual double long do_get_double_long( void ) const;
	virtual void* do_get_pointer( void ) const;
	virtual yaal::hcore::HString const& do_get_string( void ) const;
	virtual yaal::hcore::HTime const& do_get_time( void ) const;
	virtual void do_set_char( char );
	virtual void do_set_int_short( int short );
	virtual void do_set_int( int );
	virtual void do_set_int_long( int long );
	virtual void do_set_double( double );
	virtual void do_set_double_long( double long );
	virtual void do_set_pointer( void* );
	virtual void do_set_string( yaal::hcore::HString const& );
	virtual void do_set_time( yaal::hcore::HTime const& );
private:
	void purge( void );
};

class HInfoVariant : public HInfo {
private:
	type_id_t		_type;
	typedef yaal::hcore::HVariant<void const*, int long, double long, yaal::hcore::HString, yaal::hcore::HTime> data_t;
	data_t _data;
public:
	typedef HInfoVariant this_type;
	typedef HInfo base_type;
	HInfoVariant( void );
	HInfoVariant( int );
	HInfoVariant( int long );
	HInfoVariant( double );
	HInfoVariant( double long );
	HInfoVariant( char const* const );
	HInfoVariant( yaal::hcore::HString const& );
	HInfoVariant( yaal::hcore::HTime const& );
	HInfoVariant( void* const );
	HInfoVariant( HInfoVariant const& );
	HInfoVariant& operator = ( HInfoVariant const& );
	virtual ~HInfoVariant( void );
protected:
	virtual char do_get_byte( int ) const;
	virtual char do_get_char( void ) const;
	virtual int short do_get_int_short( void ) const;
	virtual int do_get_int( void ) const;
	virtual int long do_get_int_long( void ) const;
	virtual double do_get_double( void ) const;
	virtual double long do_get_double_long( void ) const;
	virtual void* do_get_pointer( void ) const;
	virtual yaal::hcore::HString const& do_get_string( void ) const;
	virtual yaal::hcore::HTime const& do_get_time( void ) const;
	virtual void do_set_char( char );
	virtual void do_set_int_short( int short );
	virtual void do_set_int( int );
	virtual void do_set_int_long( int long );
	virtual void do_set_double( double );
	virtual void do_set_double_long( double long );
	virtual void do_set_pointer( void* );
	virtual void do_set_string( yaal::hcore::HString const& );
	virtual void do_set_time( yaal::hcore::HTime const& );
private:
	void purge( void );
};

class HInfoString : public HInfo {
private:
	yaal::hcore::HString const& _data;
public:
	typedef HInfoString this_type;
	typedef HInfo base_type;
	HInfoString( yaal::hcore::HString const& );
	HInfoString( HInfoString const& );
	virtual ~HInfoString( void );
protected:
	virtual char do_get_byte( int ) const;
	virtual char do_get_char( void ) const;
	virtual int short do_get_int_short( void ) const;
	virtual int do_get_int( void ) const;
	virtual int long do_get_int_long( void ) const;
	virtual double do_get_double( void ) const;
	virtual double long do_get_double_long( void ) const;
	virtual void* do_get_pointer( void ) const;
	virtual yaal::hcore::HString const& do_get_string( void ) const;
	virtual yaal::hcore::HTime const& do_get_time( void ) const M_DEBUG_CODE( __attribute__(( noreturn )) );
	virtual void do_set_char( char ) M_DEBUG_CODE( __attribute__(( noreturn )) );
	virtual void do_set_int_short( int short ) M_DEBUG_CODE( __attribute__(( noreturn )) );
	virtual void do_set_int( int ) M_DEBUG_CODE( __attribute__(( noreturn )) );
	virtual void do_set_int_long( int long ) M_DEBUG_CODE( __attribute__(( noreturn )) );
	virtual void do_set_double( double ) M_DEBUG_CODE( __attribute__(( noreturn )) );
	virtual void do_set_double_long( double long ) M_DEBUG_CODE( __attribute__(( noreturn )) );
	virtual void do_set_pointer( void* ) M_DEBUG_CODE( __attribute__(( noreturn )) );
	virtual void do_set_string( yaal::hcore::HString const& ) M_DEBUG_CODE( __attribute__(( noreturn )) );
	virtual void do_set_time( yaal::hcore::HTime const& ) M_DEBUG_CODE( __attribute__(( noreturn )) );
private:
	void purge( void );
};

}

}

#endif /* #ifndef YAAL_HCONSOLE_HINFO_HXX_INCLUDED */

