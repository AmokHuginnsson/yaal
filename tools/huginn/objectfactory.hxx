/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  objectfactory.hxx - this file is integral part of `yaal' project.

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

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_OBJECTFACTORY_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_OBJECTFACTORY_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"

namespace yaal {

namespace tools {

namespace huginn {

namespace string {
HHuginn::class_t get_class( void );
}

namespace integer {
HHuginn::class_t get_class( void );
}

namespace real {
HHuginn::class_t get_class( void );
}

namespace list {
HHuginn::class_t get_class( void );
}

namespace deque {
HHuginn::class_t get_class( void );
}

namespace dict {
HHuginn::class_t get_class( void );
}

namespace order {
HHuginn::class_t get_class( void );
}

namespace lookup {
HHuginn::class_t get_class( void );
}

namespace set {
HHuginn::class_t get_class( void );
}

class HObjectFactory final {
	HHuginn::class_t _string;
	HHuginn::class_t _integer;
	HHuginn::class_t _real;
	HHuginn::class_t _list;
	HHuginn::class_t _deque;
	HHuginn::class_t _dict;
	HHuginn::class_t _order;
	HHuginn::class_t _lookup;
	HHuginn::class_t _set;
	HHuginn::class_t _exception;
	HHuginn::class_t _conversionException;
	HHuginn::class_t _arithmeticException;
public:
	HObjectFactory( void );
	void register_exception_classes( HHuginn* );
	HHuginn::value_t create_string( yaal::hcore::HString const& value_ = yaal::hcore::HString() ) const {
		return ( yaal::hcore::make_pointer<HHuginn::HString>( _string.raw(), value_ ) );
	}
	HHuginn::value_t create_integer( HHuginn::HInteger::value_type value_ ) const {
		return ( yaal::hcore::make_pointer<HHuginn::HInteger>( _integer.raw(), value_ ) );
	}
	HHuginn::value_t create_real( HHuginn::HReal::value_type value_ ) const {
		return ( yaal::hcore::make_pointer<HHuginn::HReal>( _real.raw(), value_ ) );
	}
	HHuginn::value_t create_list( HHuginn::values_t const& values_ = HHuginn::values_t() ) const {
		return ( yaal::hcore::make_pointer<HHuginn::HList>( _list.raw(), values_ ) );
	}
	HHuginn::value_t create_deque( HHuginn::HDeque::values_t const& values_ = HHuginn::HDeque::values_t() ) const {
		return ( yaal::hcore::make_pointer<HHuginn::HDeque>( _deque.raw(), values_ ) );
	}
	HHuginn::value_t create_dict( void ) const {
		return ( yaal::hcore::make_pointer<HHuginn::HDict>( _dict.raw() ) );
	}
	HHuginn::value_t create_dict( HHuginn::HDict::values_t const& data_, HHuginn::type_t keyType_ ) const {
		return ( yaal::hcore::make_pointer<HHuginn::HDict>( _dict.raw(), data_, keyType_ ) );
	}
	HHuginn::value_t create_order( void ) const {
		return ( yaal::hcore::make_pointer<HHuginn::HOrder>( _order.raw() ) );
	}
	HHuginn::value_t create_order( HHuginn::HOrder::values_t const& data_, HHuginn::type_t keyType_ ) const {
		return ( yaal::hcore::make_pointer<HHuginn::HOrder>( _order.raw(), data_, keyType_ ) );
	}
	HHuginn::value_t create_lookup( void ) const {
		return ( yaal::hcore::make_pointer<HHuginn::HLookup>( _lookup.raw() ) );
	}
	HHuginn::value_t create_lookup( HHuginn::HLookup::values_t const& data_ ) const {
		return ( yaal::hcore::make_pointer<HHuginn::HLookup>( _lookup.raw(), data_ ) );
	}
	HHuginn::value_t create_set( void ) const {
		return ( yaal::hcore::make_pointer<HHuginn::HSet>( _set.raw() ) );
	}
	HHuginn::value_t create_set( HHuginn::HSet::values_t const& data_ ) const {
		return ( yaal::hcore::make_pointer<HHuginn::HSet>( _set.raw(), data_ ) );
	}
	HHuginn::HClass const* exception_class( void ) const {
		return ( _exception.raw() );
	}
	HHuginn::HClass const* conversion_exception_class( void ) const {
		return ( _conversionException.raw() );
	}
	HHuginn::HClass const* arithmetic_exception_class( void ) const {
		return ( _arithmeticException.raw() );
	}
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_OBJECTFACTORY_HXX_INCLUDED */

