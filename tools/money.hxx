/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  money.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_TOOLS_MONEY_HXX_INCLUDED
#define YAAL_TOOLS_MONEY_HXX_INCLUDED 1

#include "hcore/hstring.hxx"
#include "hcore/hnumber.hxx"

namespace yaal {

namespace tools {

namespace money {

enum class CURRENCY {
	DOLLAR,
	POUND,
	EURO,
	PLN
};

yaal::hcore::HString money_string( yaal::hcore::HNumber const& );
yaal::hcore::HString in_words_en( yaal::hcore::HNumber const&, CURRENCY );
yaal::hcore::HString in_words_pl( yaal::hcore::HNumber const&, CURRENCY );
bool verify_IBAN( yaal::hcore::HString const& );
char const* get_last_error( void );

}

}

}

#endif /* #ifndef YAAL_TOOLS_MONEY_HXX_INCLUDED */

