/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file tools/escape.hxx
 * \brief Declarations of string escaping functions.
 */

#ifndef YAAL_TOOLS_UTIL_ESCAPE_HXX_INCLUDED
#define YAAL_TOOLS_UTIL_ESCAPE_HXX_INCLUDED 1

#include "hcore/hstring.hxx"
#include "hcore/harray.hxx"

namespace yaal {

namespace tools {

namespace util {

struct EscapeTable {
	static int const ESCAPE_TABLE_SIZE = 256;
	char _rawToSafe[ESCAPE_TABLE_SIZE];
	char _safeToRaw[ESCAPE_TABLE_SIZE];
	EscapeTable( char const*, int, char const*, int );
};

void escape( yaal::hcore::HString&, EscapeTable const&, yaal::code_point_t = '\\'_ycp );
void unescape( yaal::hcore::HString&, EscapeTable const&, yaal::code_point_t = '\\'_ycp );
void semantic_unescape( yaal::hcore::HString& );
yaal::hcore::HString escape_copy( yaal::hcore::HString, EscapeTable const&, yaal::code_point_t = '\\'_ycp );
yaal::hcore::HString unescape_copy( yaal::hcore::HString, EscapeTable const&, yaal::code_point_t = '\\'_ycp );
typedef yaal::hcore::HArray<yaal::code_point_t> escape_mask_map_t;
void mask_escape( yaal::hcore::HString&, escape_mask_map_t&, yaal::code_point_t = '\\'_ycp );
void unmask_escape( yaal::hcore::HString&, escape_mask_map_t const&, yaal::code_point_t = '\\'_ycp );

}

}

}

#endif /* #ifndef YAAL_TOOLS_UTIL_ESCAPE_HXX_INCLUDED */

