/* Read yaal/LICENSE.md file for copyright and licensing information. */

#ifndef YAAL_TOOLS_ANSI_HXX_INCLUDED
#define YAAL_TOOLS_ANSI_HXX_INCLUDED 1

#include "hcore/hstreaminterface.hxx"

namespace yaal {

namespace ansi {

class HSequence {
public:
	typedef HSequence this_type;
	static int const SEQUENCE_BUFFER_SIZE = 32;
private:
	char _data[SEQUENCE_BUFFER_SIZE];
public:
	HSequence( char const* );
	HSequence( HSequence const& ) = default;
	char const* operator* ( void ) const {
		return ( _data );
	}
private:
	HSequence& operator = ( HSequence const& );
};

typedef yaal::hcore::HExceptionT<HSequence> HSequenceException;

extern M_YAAL_TOOLS_PUBLIC_API HSequence const reset;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const bold;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const underline;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const blink;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const reverse;

extern M_YAAL_TOOLS_PUBLIC_API HSequence const black;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const red;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const green;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const brown;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const blue;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const magenta;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const cyan;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const lightgray;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const gray;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const brightred;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const brightgreen;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const yellow;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const brightblue;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const brightmagenta;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const brightcyan;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const white;

extern M_YAAL_TOOLS_PUBLIC_API HSequence const bgblack;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const bgred;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const bggreen;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const bgbrown;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const bgblue;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const bgmagenta;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const bgcyan;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const bglightgray;

extern M_YAAL_TOOLS_PUBLIC_API HSequence const bggray;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const bgbrightred;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const bgbrightgreen;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const bgyellow;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const bgbrightblue;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const bgbrightmagenta;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const bgbrightcyan;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const bgwhite;

extern M_YAAL_TOOLS_PUBLIC_API HSequence const up;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const down;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const left;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const right;

extern M_YAAL_TOOLS_PUBLIC_API HSequence const up_bol;

extern M_YAAL_TOOLS_PUBLIC_API HSequence const save;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const restore;

extern M_YAAL_TOOLS_PUBLIC_API HSequence const clear;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const clrtoeol;

HSequence move( int, int );
HSequence up_n( int );
HSequence down_n( int );
HSequence left_n( int );
HSequence right_n( int );
HSequence up_bol_n( int );

enum class PLANE {
	FOREGROUND,
	BACKGROUND
};

HSequence gray_scale( PLANE, int );
HSequence color256( PLANE, int, int, int );
HSequence rgb( PLANE, int, int, int );

yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface&, HSequence const& );

}

}

#endif /* #ifndef YAAL_TOOLS_ANSI_HXX_INCLUDED */

