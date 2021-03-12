/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "color.hxx"

#undef COLOR

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::ansi;

namespace yaal {

namespace tools {

COLOR::color_t COLOR::fg_to_bg( COLOR::color_t fg_ ) {
	M_PROLOG
	color_t bg( ATTR_DEFAULT );
	switch ( fg_ ) {
		case ( FG_BLACK ):         bg = BG_BLACK;         break;
		case ( FG_RED ):           bg = BG_RED;           break;
		case ( FG_GREEN ):         bg = BG_GREEN;         break;
		case ( FG_BROWN ):         bg = BG_BROWN;         break;
		case ( FG_BLUE ):          bg = BG_BLUE;          break;
		case ( FG_MAGENTA ):       bg = BG_MAGENTA;       break;
		case ( FG_CYAN ):          bg = BG_CYAN;          break;
		case ( FG_LIGHTGRAY ):     bg = BG_LIGHTGRAY;     break;
		case ( FG_GRAY ):          bg = BG_GRAY;          break;
		case ( FG_BRIGHTRED ):     bg = BG_BRIGHTRED;     break;
		case ( FG_BRIGHTGREEN ):   bg = BG_BRIGHTGREEN;   break;
		case ( FG_YELLOW ):        bg = BG_YELLOW;        break;
		case ( FG_BRIGHTBLUE ):    bg = BG_BRIGHTBLUE;    break;
		case ( FG_BRIGHTMAGENTA ): bg = BG_BRIGHTMAGENTA; break;
		case ( FG_BRIGHTCYAN ):    bg = BG_BRIGHTCYAN;    break;
		case ( FG_WHITE ):         bg = BG_WHITE;         break;
		default: {
			M_THROW( "Invalid color:", fg_ );
		}
	}
	return bg;
	M_EPILOG
}

COLOR::color_t COLOR::complementary( COLOR::color_t attr_ ) {
	return ( static_cast<COLOR::color_t>( ~attr_ ) );
}

COLOR::color_t COLOR::combine( COLOR::color_t part1_, COLOR::color_t part2_ ) {
	return ( static_cast<COLOR::color_t>( part1_ | part2_ ) );
}

COLOR::color_t COLOR::from_string( yaal::hcore::HString const& name_ ) {
	HString name( name_ );
	name.lower();
	color_t color( ATTR_DEFAULT );
	if ( name == "black" ) {
		color = FG_BLACK;
	} else if ( name == "gray" ) {
		color = FG_GRAY;
	} else if ( name == "lightgray" ) {
		color = FG_LIGHTGRAY;
	} else if ( name == "white" ) {
		color = FG_WHITE;
	} else if ( name == "red" ) {
		color = FG_RED;
	} else if ( name == "green" ) {
		color = FG_GREEN;
	} else if ( name == "blue" ) {
		color = FG_BLUE;
	} else if ( name == "cyan" ) {
		color = FG_CYAN;
	} else if ( name == "magenta" ) {
		color = FG_MAGENTA;
	} else if ( name == "brown" ) {
		color = FG_BROWN;
	} else if ( name == "brightred" ) {
		color = FG_BRIGHTRED;
	} else if ( name == "brightgreen" ) {
		color = FG_BRIGHTGREEN;
	} else if ( name == "brightblue" ) {
		color = FG_BRIGHTBLUE;
	} else if ( name == "brightcyan" ) {
		color = FG_BRIGHTCYAN;
	} else if ( name == "brightmagenta" ) {
		color = FG_BRIGHTMAGENTA;
	} else if ( name == "yellow" ) {
		color = FG_YELLOW;
	} else if ( name == "reset" ) {
		color = ATTR_RESET;
	} else if ( name == "bold" ) {
		color = ATTR_BOLD;
	} else if ( name == "underline" ) {
		color = ATTR_UNDERLINE;
	} else if ( name == "blink" ) {
		color = ATTR_BLINK;
	} else if ( name == "reverse" ) {
		color = ATTR_REVERSE;
	} else {
		throw HColorException( "Bad color name: `"_ys.append( name_ ).append( "'." ) );
	}
	return color;
}

yaal::ansi::HSequence const& COLOR::to_ansi( color_t color_ ) {
	M_PROLOG
	HSequence const* seq( nullptr );
	switch ( color_ ) {
		case ( ATTR_DEFAULT ):
		case ( ATTR_RESET ):       seq = &ansi::reset;         break;
		case ( ATTR_BOLD ):        seq = &ansi::bold;          break;
		case ( ATTR_UNDERLINE ):   seq = &ansi::underline;     break;
		case ( ATTR_BLINK ):       seq = &ansi::blink;         break;
		case ( ATTR_REVERSE ):     seq = &ansi::reverse;       break;
		case ( FG_BLACK ):         seq = &ansi::black;         break;
		case ( FG_RED ):           seq = &ansi::red;           break;
		case ( FG_GREEN ):         seq = &ansi::green;         break;
		case ( FG_BROWN ):         seq = &ansi::brown;         break;
		case ( FG_BLUE ):          seq = &ansi::blue;          break;
		case ( FG_MAGENTA ):       seq = &ansi::magenta;       break;
		case ( FG_CYAN ):          seq = &ansi::cyan;          break;
		case ( FG_LIGHTGRAY ):     seq = &ansi::lightgray;     break;
		case ( FG_GRAY ):          seq = &ansi::gray;          break;
		case ( FG_BRIGHTRED ):     seq = &ansi::brightred;     break;
		case ( FG_BRIGHTGREEN ):   seq = &ansi::brightgreen;   break;
		case ( FG_YELLOW ):        seq = &ansi::yellow;        break;
		case ( FG_BRIGHTBLUE ):    seq = &ansi::brightblue;    break;
		case ( FG_BRIGHTMAGENTA ): seq = &ansi::brightmagenta; break;
		case ( FG_BRIGHTCYAN ):    seq = &ansi::brightcyan;    break;
		case ( FG_WHITE ):         seq = &ansi::white;         break;
		default: {
			seq = &ansi::reset;
		}
	}
	return ( *seq );
	M_EPILOG
}

}

}

