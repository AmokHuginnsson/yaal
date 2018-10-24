/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file color.hxx
 * \brief Declaration COLOR definition.
 */

#ifndef YAAL_TOOLS_KEYCODE_HXX_INCLUDED
#define YAAL_TOOLS_KEYCODE_HXX_INCLUDED 1

#include "hcore/unicode.hxx"

namespace yaal {

namespace tools {

/*! \brief Special key codes.
 */
struct KEY_CODE {
	static int const ESCAPE       = 27;
	static int const BASE         = unicode::MASK + 1;
	static int const PAGE_UP      = BASE;
	static int const PAGE_DOWN    = PAGE_UP   + 1;
	static int const DOWN         = PAGE_DOWN + 1;
	static int const UP           = DOWN      + 1;
	static int const LEFT         = UP        + 1;
	static int const RIGHT        = LEFT      + 1;
	static int const HOME         = RIGHT     + 1;
	static int const END          = HOME      + 1;
	static int const DELETE       = END       + 1;
	static int const INSERT       = DELETE    + 1;
	static int const BACKSPACE    = INSERT    + 1;
	static int const F1           = BACKSPACE + 1;
	static int const F2           = F1        + 1;
	static int const F3           = F2        + 1;
	static int const F4           = F3        + 1;
	static int const F5           = F4        + 1;
	static int const F6           = F5        + 1;
	static int const F7           = F6        + 1;
	static int const F8           = F7        + 1;
	static int const F9           = F8        + 1;
	static int const F10          = F9        + 1;
	static int const F11          = F10       + 1;
	static int const F12          = F11       + 1;
	static int const F13          = F12       + 1;
	static int const F14          = F13       + 1;
	static int const F15          = F14       + 1;
	static int const F16          = F15       + 1;
	static int const F17          = F16       + 1;
	static int const F18          = F17       + 1;
	static int const F19          = F18       + 1;
	static int const F20          = F19       + 1;
	static int const F21          = F20       + 1;
	static int const F22          = F21       + 1;
	static int const F23          = F22       + 1;
	static int const F24          = F23       + 1;
	static int const MOUSE        = F24       + 1;
	static int const CONTROL_BASE = 96;
	static int const META_BASE    = BASE * 2;
	static int const COMMAND_BASE = BASE * 4;
};

typedef yaal::hcore::HExceptionT<KEY_CODE> HKeyCodeException;

/*! \brief Get special key code values.
 *
 * \tparam code - basic input code.
 * \retval meta - META modified code.
 * \retval ctrl - CONTROL modified code.
 * \retval commercial - CONTROL-x modified code (command).
 */
template<int code = 0>
struct KEY {
	static int const meta = code + KEY_CODE::META_BASE;
	static int meta_r( int code_ ) {
		return ( code_ + KEY_CODE::META_BASE );
	}
	static int const ctrl = code - KEY_CODE::CONTROL_BASE;
	static int ctrl_r( int code_ ) {
		return ( code_ - KEY_CODE::CONTROL_BASE );
	}
	static int const command = code + KEY_CODE::COMMAND_BASE;
	static int command_r( int code_ ) {
		return ( code_ + KEY_CODE::COMMAND_BASE );
	}
};

}

}

#endif /* #ifndef YAAL_TOOLS_KEYCODE_HXX_INCLUDED */

