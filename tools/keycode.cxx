/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "keycode.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

int KEY_CODE::key( int code_ ) {
	if ( code_ > COMMAND_BASE ) {
		code_ -= COMMAND_BASE;
	}
	if ( code_ > META_BASE ) {
		code_ -= META_BASE;
	}
	if ( code_ > SHIFT_BASE ) {
		code_ -= SHIFT_BASE;
	}
	if ( code_ <= CONTROL_BASE ) {
		code_ += CONTROL_BASE;
	}
	return ( code_ );
}

int KEY_CODE::modifiers( int code_ ) {
	int m( 0 );
	if ( code_ > COMMAND_BASE ) {
		code_ -= COMMAND_BASE;
		m |= COMMAND_BASE;
	}
	if ( code_ > META_BASE ) {
		code_ -= META_BASE;
		m |= META_BASE;
	}
	if ( code_ > SHIFT_BASE ) {
		code_ -= SHIFT_BASE;
		m |= SHIFT_BASE;
	}
	if ( code_ <= CONTROL_BASE ) {
		code_ += CONTROL_BASE;
		m |= CONTROL_BASE;
	}
	return ( m );
}

KeyEncodingTable get_key_encoding_table( void ) {
	static KeyEncoding const keyEncodings[] = {
		{ code_point_t( KEY_CODE::UP ),                               "[A" },
		{ code_point_t( KEY_CODE::UP ),                               "OA" },
		{ code_point_t( KEY_CODE::DOWN ),                             "[B" },
		{ code_point_t( KEY_CODE::DOWN ),                             "OB" },
		{ code_point_t( KEY_CODE::LEFT ),                             "[D" },
		{ code_point_t( KEY_CODE::LEFT ),                             "OD" },
		{ code_point_t( KEY_CODE::RIGHT ),                            "[C" },
		{ code_point_t( KEY_CODE::RIGHT ),                            "OC" },
		{ code_point_t( KEY_CODE::HOME ),                             "[1~" },
		{ code_point_t( KEY_CODE::HOME ),                             "[H" },
		{ code_point_t( KEY_CODE::HOME ),                             "OH" },
		{ code_point_t( KEY_CODE::HOME ),                             "[7~" },
		{ code_point_t( KEY_CODE::END ),                              "[4~" },
		{ code_point_t( KEY_CODE::END ),                              "[F" },
		{ code_point_t( KEY_CODE::END ),                              "OF" },
		{ code_point_t( KEY_CODE::END ),                              "[8~" },
		{ code_point_t( KEY_CODE::PAGE_UP ),                          "[5~" },
		{ code_point_t( KEY_CODE::PAGE_DOWN ),                        "[6~" },
		{ code_point_t( KEY_CODE::INSERT ),                           "[2~" },
		{ code_point_t( KEY_CODE::DELETE ),                           "[3~" },
		{ code_point_t( KEY_CODE::F1 ),                               "OP" },
		{ code_point_t( KEY_CODE::F1 ),                               "[[A" },
		{ code_point_t( KEY_CODE::F2 ),                               "OQ" },
		{ code_point_t( KEY_CODE::F2 ),                               "[[B" },
		{ code_point_t( KEY_CODE::F3 ),                               "OR" },
		{ code_point_t( KEY_CODE::F3 ),                               "[[C" },
		{ code_point_t( KEY_CODE::F4 ),                               "OS" },
		{ code_point_t( KEY_CODE::F4 ),                               "[[D" },
		{ code_point_t( KEY_CODE::F5 ),                               "[15~" },
		{ code_point_t( KEY_CODE::F5 ),                               "[[E" },
		{ code_point_t( KEY_CODE::F6 ),                               "[17~" },
		{ code_point_t( KEY_CODE::F7 ),                               "[18~" },
		{ code_point_t( KEY_CODE::F8 ),                               "[19~" },
		{ code_point_t( KEY_CODE::F9 ),                               "[20~" },
		{ code_point_t( KEY_CODE::F10 ),                              "[21~" },
		{ code_point_t( KEY_CODE::F11 ),                              "[23~" },
		{ code_point_t( KEY_CODE::F12 ),                              "[24~" },
		{ code_point_t( KEY<KEY_CODE::UP>::shift ),                   "[1;2A" },
		{ code_point_t( KEY<KEY_CODE::DOWN>::shift ),                 "[1;2B" },
		{ code_point_t( KEY<KEY_CODE::LEFT>::shift ),                 "[1;2D" },
		{ code_point_t( KEY<KEY_CODE::RIGHT>::shift ),                "[1;2C" },
		{ code_point_t( KEY<KEY_CODE::F1>::shift ),                   "[1;2P" },
		{ code_point_t( KEY<KEY_CODE::F2>::shift ),                   "[1;2Q" },
		{ code_point_t( KEY<KEY_CODE::F3>::shift ),                   "[1;2R" },
		{ code_point_t( KEY<KEY_CODE::F4>::shift ),                   "[1;2S" },
		{ code_point_t( KEY<KEY_CODE::F5>::shift ),                   "[15;2~" },
		{ code_point_t( KEY<KEY_CODE::F6>::shift ),                   "[17;2~" },
		{ code_point_t( KEY<KEY_CODE::F7>::shift ),                   "[18;2~" },
		{ code_point_t( KEY<KEY_CODE::F8>::shift ),                   "[19;2~" },
		{ code_point_t( KEY<KEY_CODE::F9>::shift ),                   "[20;2~" },
		{ code_point_t( KEY<KEY_CODE::F10>::shift ),                  "[21;2~" },
		{ code_point_t( KEY<KEY_CODE::F11>::shift ),                  "[23;2~" },
		{ code_point_t( KEY<KEY_CODE::F12>::shift ),                  "[24;2~" },
		{ code_point_t( KEY<KEY_CODE::UP>::ctrl ),                    "[1;5A" },
		{ code_point_t( KEY<KEY_CODE::DOWN>::ctrl ),                  "[1;5B" },
		{ code_point_t( KEY<KEY_CODE::LEFT>::ctrl ),                  "[1;5D" },
		{ code_point_t( KEY<KEY_CODE::RIGHT>::ctrl ),                 "[1;5C" },
		{ code_point_t( KEY<KEY_CODE::F1>::ctrl ),                    "[1;5P" },
		{ code_point_t( KEY<KEY_CODE::F2>::ctrl ),                    "[1;5Q" },
		{ code_point_t( KEY<KEY_CODE::F3>::ctrl ),                    "[1;5R" },
		{ code_point_t( KEY<KEY_CODE::F4>::ctrl ),                    "[1;5S" },
		{ code_point_t( KEY<KEY_CODE::F5>::ctrl ),                    "[15;5~" },
		{ code_point_t( KEY<KEY_CODE::F6>::ctrl ),                    "[17;5~" },
		{ code_point_t( KEY<KEY_CODE::F7>::ctrl ),                    "[18;5~" },
		{ code_point_t( KEY<KEY_CODE::F8>::ctrl ),                    "[19;5~" },
		{ code_point_t( KEY<KEY_CODE::F9>::ctrl ),                    "[20;5~" },
		{ code_point_t( KEY<KEY_CODE::F10>::ctrl ),                   "[21;5~" },
		{ code_point_t( KEY<KEY_CODE::F11>::ctrl ),                   "[23;5~" },
		{ code_point_t( KEY<KEY_CODE::F12>::ctrl ),                   "[24;5~" },
		{ code_point_t( KEY<KEY<KEY_CODE::UP>::ctrl>::shift ),        "[1;6A" },
		{ code_point_t( KEY<KEY<KEY_CODE::DOWN>::ctrl>::shift ),      "[1;6B" },
		{ code_point_t( KEY<KEY<KEY_CODE::LEFT>::ctrl>::shift ),      "[1;6D" },
		{ code_point_t( KEY<KEY<KEY_CODE::RIGHT>::ctrl>::shift ),     "[1;6C" },
		{ code_point_t( KEY<KEY<KEY_CODE::F1>::ctrl>::shift ),        "[1;6P" },
		{ code_point_t( KEY<KEY<KEY_CODE::F2>::ctrl>::shift ),        "[1;6Q" },
		{ code_point_t( KEY<KEY<KEY_CODE::F3>::ctrl>::shift ),        "[1;6R" },
		{ code_point_t( KEY<KEY<KEY_CODE::F4>::ctrl>::shift ),        "[1;6S" },
		{ code_point_t( KEY<KEY<KEY_CODE::F5>::ctrl>::shift ),        "[15;6~" },
		{ code_point_t( KEY<KEY<KEY_CODE::F6>::ctrl>::shift ),        "[17;6~" },
		{ code_point_t( KEY<KEY<KEY_CODE::F7>::ctrl>::shift ),        "[18;6~" },
		{ code_point_t( KEY<KEY<KEY_CODE::F8>::ctrl>::shift ),        "[19;6~" },
		{ code_point_t( KEY<KEY<KEY_CODE::F9>::ctrl>::shift ),        "[20;6~" },
		{ code_point_t( KEY<KEY<KEY_CODE::F10>::ctrl>::shift ),       "[21;6~" },
		{ code_point_t( KEY<KEY<KEY_CODE::F11>::ctrl>::shift ),       "[23;6~" },
		{ code_point_t( KEY<KEY<KEY_CODE::F12>::ctrl>::shift ),       "[24;6~" },
		{ code_point_t( KEY<KEY_CODE::UP>::meta ),                    "[1;3A" },
		{ code_point_t( KEY<KEY_CODE::DOWN>::meta ),                  "[1;3B" },
		{ code_point_t( KEY<KEY_CODE::LEFT>::meta ),                  "[1;3D" },
		{ code_point_t( KEY<KEY_CODE::RIGHT>::meta ),                 "[1;3C" },
		{ code_point_t( KEY<KEY_CODE::INSERT>::meta ),                "[2;3~" },
		{ code_point_t( KEY<KEY_CODE::DELETE>::meta ),                "[3;3~" },
		{ code_point_t( KEY<KEY_CODE::HOME>::meta ),                  "[1;3H" },
		{ code_point_t( KEY<KEY_CODE::END>::meta ),                   "[1;3F" },
		{ code_point_t( KEY<KEY_CODE::PAGE_UP>::meta ),               "[5;3~" },
		{ code_point_t( KEY<KEY_CODE::PAGE_DOWN>::meta ),             "[6;3~" },
		{ code_point_t( KEY<KEY<KEY_CODE::UP>::ctrl>::meta ),         "[1;7A" },
		{ code_point_t( KEY<KEY<KEY_CODE::DOWN>::ctrl>::meta ),       "[1;7B" },
		{ code_point_t( KEY<KEY<KEY_CODE::LEFT>::ctrl>::meta ),       "[1;7D" },
		{ code_point_t( KEY<KEY<KEY_CODE::RIGHT>::ctrl>::meta ),      "[1;7C" },
		{ code_point_t( KEY<KEY<KEY_CODE::DELETE>::ctrl>::meta ),     "[3;7~" }
	};
	return { keyEncodings, yaal::size( keyEncodings ) };
}

}

}

