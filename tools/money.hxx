/* Read yaal/LICENSE.md file for copyright and licensing information. */

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
yaal::hcore::HString const& get_last_error( void );

}

}

}

#endif /* #ifndef YAAL_TOOLS_MONEY_HXX_INCLUDED */

