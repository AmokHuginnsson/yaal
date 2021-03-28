/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_HCORE_NUMBER_HXX_INCLUDED
#define YAAL_HCORE_NUMBER_HXX_INCLUDED 1

#include "hcore/pod.hxx"
#include "hcore/hstring.hxx"

namespace yaal {

namespace hcore {

namespace number {

int const DECIMAL_DIGITS_IN_LEAF_CONST = 9;
int const HARDCODED_MINIMUM_PRECISION = 16;
static code_point_t const A_MINUS  = code_point_t( '-' );
static code_point_t const A_DOT    = code_point_t( '.' );
static code_point_t const A_ZERO   = code_point_t( '0' );
static code_point_t const A_SPACER = code_point_t( '_' );
extern HString const VALID_CHARACTERS;
extern HString const DIGITS_AND_DOT;
extern HString const DIGITS_AND_SPACER;
extern HString const DIGITS_SPACER_AND_DOT;
extern HString const DIGITS;
extern i32_t const DECIMAL_SHIFT[];

}

}

}

#endif /* #ifndef YAAL_HCORE_NUMBER_HXX_INCLUDED */

