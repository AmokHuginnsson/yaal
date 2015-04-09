/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  hstrongenum.hxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want redistribute binary package you can not demand any fees
     demand any fees for it. You cannot even demand
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#ifndef YAAL_HCORE_HSTRONGENUM_HXX_INCLUDED
#define YAAL_HCORE_HSTRONGENUM_HXX_INCLUDED 1

namespace yaal {

namespace hcore {

/*! \brief C++ enum interface with strong typing.
 */
template<typename EnumID>
class HStrongEnum {
	typename EnumID::enum_t _value;
public:
	HStrongEnum( typename EnumID::enum_t value_ ) : _value( value_ ) {}
	HStrongEnum( HStrongEnum const& e ) : _value( e._value ) {}
	bool operator == ( HStrongEnum const& other ) const {
		return ( other._value == _value );
	}
	bool operator == ( typename EnumID::enum_t const& other ) const {
		return ( other == _value );
	}
	bool operator != ( HStrongEnum const& other ) const {
		return ( other._value != _value );
	}
	bool operator != ( typename EnumID::enum_t const& other ) const {
		return ( other != _value );
	}
	typename EnumID::enum_t value( void ) const
		{ return ( _value ); }
	operator typename EnumID::enum_t() const {
		return ( _value );
	}
};

}

}

#endif /* #ifndef YAAL_HCORE_HSTRONGENUM_HXX_INCLUDED */

