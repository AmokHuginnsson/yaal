/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "ansi.hxx"
#include "hterminal.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;

namespace yaal {

namespace ansi {

namespace {
static char const* TERM( getenv( "TERM" ) );
static bool const has256colors( TERM ? ( to_string( TERM ).find( "256" ) != HString::npos ) : false );
}

HSequence::HSequence( char const* data_ )
	: _data() {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wunknown-warning-option"
#pragma GCC diagnostic ignored "-Wstringop-truncation"
	::strncpy( _data, data_, SEQUENCE_BUFFER_SIZE );
#pragma GCC diagnostic pop
	_data[SEQUENCE_BUFFER_SIZE - 1] = 0;
	return;
}

HSequence const reset( "\033[0m" );
HSequence const bold( "\033[1m" );
HSequence const underline( "\033[4m" );
HSequence const blink( "\033[5m" );
HSequence const reverse( "\033[7m" );

HSequence const black( "\033[22;30m" );
HSequence const red( "\033[22;31m" );
HSequence const green( "\033[22;32m" );
HSequence const brown( "\033[22;33m" );
HSequence const blue( "\033[22;34m" );
HSequence const magenta( "\033[22;35m" );
HSequence const cyan( "\033[22;36m" );
HSequence const lightgray( "\033[22;37m" );

HSequence const gray( has256colors ? "\033[1;90m" : "\033[1;30m" );
HSequence const brightred( has256colors ? "\033[1;91m" : "\033[1;31m" );
HSequence const brightgreen( has256colors ? "\033[1;92m" : "\033[1;32m" );
HSequence const yellow( has256colors ? "\033[1;93m" : "\033[1;33m" );
HSequence const brightblue( has256colors ? "\033[1;94m" : "\033[1;34m" );
HSequence const brightmagenta( has256colors ? "\033[1;95m" : "\033[1;35m" );
HSequence const brightcyan( has256colors ? "\033[1;96m" : "\033[1;36m" );
HSequence const white( has256colors ? "\033[1;97m" : "\033[1;37m" );

HSequence const bgblack( "\033[40m" );
HSequence const bgred( "\033[41m" );
HSequence const bggreen( "\033[42m" );
HSequence const bgbrown( "\033[43m" );
HSequence const bgblue( "\033[44m" );
HSequence const bgmagenta( "\033[45m" );
HSequence const bgcyan( "\033[46m" );
HSequence const bglightgray( "\033[47m" );

HSequence const bggray( "\033[100m" );
HSequence const bgbrightred( "\033[101m" );
HSequence const bgbrightgreen( "\033[102m" );
HSequence const bgyellow( "\033[103m" );
HSequence const bgbrightblue( "\033[104m" );
HSequence const bgbrightmagenta( "\033[105m" );
HSequence const bgbrightcyan( "\033[106m" );
HSequence const bgwhite( "\033[107m" );

HSequence const home( "\033[H" );
HSequence const up( "\033[A" );
HSequence const down( "\033[B" );
HSequence const left( "\033[D" );
HSequence const right( "\033[C" );

HSequence const save( "\033[s" );
HSequence const restore( "\033[u" );

HSequence const clear( "\033[2J" );
HSequence const clrtoeol( "\033[K" );

HSequence const up_bol( "\033[F" );

HSequence gray_scale( PLANE plane_, int level_ ) {
	if ( ( level_ < 0 ) || ( level_ >= 24 ) ) {
		throw HSequenceException( "Invalid gray scale level value: "_ys.append( level_ ) );
	}
	char code[HSequence::SEQUENCE_BUFFER_SIZE];
	::snprintf(
		code,
		HSequence::SEQUENCE_BUFFER_SIZE - 1,
		"\033[%d;5;%dm",
		plane_ == PLANE::FOREGROUND ? 38 : 48, level_ + 232
	);
	return code;
}

HSequence color256( PLANE plane_, int r_, int g_, int b_ ) {
	if ( ( r_ < 0 ) || ( r_ >= 6 ) || ( g_ < 0 ) || ( g_ >= 6 ) || ( b_ < 0 ) || ( b_ >= 6 ) ) {
		throw HSequenceException( "Invalid 6x6x6 color cube value: "_ys.append( r_ ).append( ", " ).append( g_ ).append( ", " ).append( b_ ) );
	}
	char code[HSequence::SEQUENCE_BUFFER_SIZE];
	::snprintf(
		code,
		HSequence::SEQUENCE_BUFFER_SIZE - 1,
		"\033[%d;5;%dm",
		plane_ == PLANE::FOREGROUND ? 38 : 48, r_ * 36 + g_ * 6 + b_ + 16
	);
	return code;
}

HSequence rgb( PLANE plane_, int r_, int g_, int b_ ) {
	if ( ( r_ < 0 ) || ( r_ >= 256 ) || ( g_ < 0 ) || ( g_ >= 256 ) || ( b_ < 0 ) || ( b_ >= 256 ) ) {
		throw HSequenceException( "Invalid RGB color value: "_ys.append( r_ ).append( ", " ).append( g_ ).append( ", " ).append( b_ ) );
	}
	char code[HSequence::SEQUENCE_BUFFER_SIZE];
	::snprintf(
		code,
		HSequence::SEQUENCE_BUFFER_SIZE - 1,
		"\033[%d;2;%d;%d;%dm",
		plane_ == PLANE::FOREGROUND ? 38 : 48, r_, g_, b_
	);
	return code;
}

HSequence move( int row_, int col_ ) {
	char code[HSequence::SEQUENCE_BUFFER_SIZE];
	::snprintf( code, HSequence::SEQUENCE_BUFFER_SIZE - 1, "\033[%d;%dH", row_, col_ );
	return code;
}

HSequence up_n( int by_ ) {
	char code[HSequence::SEQUENCE_BUFFER_SIZE];
	::snprintf( code, HSequence::SEQUENCE_BUFFER_SIZE - 1, "\033[%dA", by_ );
	return code;
}

HSequence down_n( int by_ ) {
	char code[HSequence::SEQUENCE_BUFFER_SIZE];
	::snprintf( code, HSequence::SEQUENCE_BUFFER_SIZE - 1, "\033[%dB", by_ );
	return code;
}

HSequence left_n( int by_ ) {
	char code[HSequence::SEQUENCE_BUFFER_SIZE];
	::snprintf( code, HSequence::SEQUENCE_BUFFER_SIZE - 1, "\033[%dD", by_ );
	return code;
}

HSequence right_n( int by_ ) {
	static char code[HSequence::SEQUENCE_BUFFER_SIZE];
	::snprintf( code, HSequence::SEQUENCE_BUFFER_SIZE - 1, "\033[%dC", by_ );
	return code;
}

HSequence up_bol_n( int by_ ) {
	char code[HSequence::SEQUENCE_BUFFER_SIZE];
	::snprintf( code, HSequence::SEQUENCE_BUFFER_SIZE - 1, "\033[%dF", by_ );
	return code;
}

yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& stream_, HSequence const& seq_ ) {
	M_PROLOG
	if ( is_a_tty( stream_ ) ) {
		stream_ << *seq_;
	}
	return stream_;
	M_EPILOG
}

}

}

