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

/*! \brief A set of dubious ASCII characters.
 */
class EscapeSet {
	static int const ESCAPE_SET_SIZE = 128;
	char _data[ESCAPE_SET_SIZE];
public:
	EscapeSet( char const*, int );
	bool has( int ) const;
};

/*! \brief Escape dubious ASCII characters in string.
 *
 * \param string - a string to escape.
 * \param dubious - an EscapeSet of dubious characters to escape in the string.
 * \param esc - a special character to be used for escaping dubious characters.
 */
void escape( yaal::hcore::HString& string, EscapeSet const& duboius, yaal::code_point_t esc = '\\'_ycp );

/*! \brief Drop escape character from string.
 *
 * \param string - a string to process.
 * \param esc - an escape character to be dropped.
 */
void unescape( yaal::hcore::HString& string, yaal::code_point_t esc = '\\'_ycp );

/*! \brief A two-way map of ASCII characters.
 */
struct EscapeTable {
	static int const ESCAPE_TABLE_SIZE = 128;
	char _rawToSafe[ESCAPE_TABLE_SIZE];
	char _safeToRaw[ESCAPE_TABLE_SIZE];
	EscapeTable( char const*, int, char const*, int );
};

/*! \brief Mask sensitive ASCII characters with their plain counterparts.
 *
 * Example use is to change '\n' in string into '\\n'.
 */
void escape( yaal::hcore::HString&, EscapeTable const&, yaal::code_point_t = '\\'_ycp );

/*! \brief Render sensitive ASCII characters from their escaped form.
 *
 * \param string - an escaped string to render.
 * \param escapeTable - a two-way translation table containing mapping of sensitive ASCII characters.
 * \param esc - an escape character used for escaping.
 * \param dropUnmatched - remove `esc` character from escaped string also for characters that were not found in escapeTable.
 */
void unescape( yaal::hcore::HString& string, EscapeTable const& escapeTable, yaal::code_point_t esc = '\\'_ycp, bool dropUnmatched = false );
void semantic_unescape( yaal::hcore::HString& );
yaal::hcore::HString escape_copy( yaal::hcore::HString, EscapeTable const&, yaal::code_point_t = '\\'_ycp );
yaal::hcore::HString unescape_copy( yaal::hcore::HString, EscapeTable const&, yaal::code_point_t = '\\'_ycp );
typedef yaal::hcore::HArray<yaal::code_point_t> escape_mask_map_t;
void mask_escape( yaal::hcore::HString&, escape_mask_map_t&, yaal::code_point_t = '\\'_ycp );
void unmask_escape( yaal::hcore::HString&, escape_mask_map_t const&, yaal::code_point_t = '\\'_ycp );
void hide( yaal::hcore::HString&, yaal::hcore::HString const& );
void unhide( yaal::hcore::HString&, yaal::hcore::HString const& );

}

}

}

#endif /* #ifndef YAAL_TOOLS_UTIL_ESCAPE_HXX_INCLUDED */

