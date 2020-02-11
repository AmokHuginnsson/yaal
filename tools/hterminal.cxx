/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstdlib> /* getenv */
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <termios.h>

#include "config.hxx"

#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif /* #ifdef HAVE_SYS_IOCTL_H */
#ifdef HAVE_TERMIO_H
#include <termio.h>
#endif /* #ifdef HAVE_TERMIO_H */
#ifdef HAVE_TTY_H
#include <tty.h>
#endif /* #ifdef HAVE_TTY_H */

namespace {
static tcflag_t const FWD_ECHO = ECHO;
}

#undef ECHO

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hterminal.hxx"
#include "util.hxx"
#include "tools.hxx"
#include "hcore/unicode.hxx"
#include "hcore/hfile.hxx"
#include "keycode.hxx"

using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::util;

namespace yaal {

namespace tools {

namespace TERMIOS {
static int const ORIGINAL = 0;
static int const CANONICAL = 1;
}

class HTerminal::HSequenceScanner {
	class HNode {
	public:
		typedef yaal::hcore::HResource<HNode> node_t;
	private:
		struct HEdge {
			char head;
			node_t tail;
			HEdge()
				: head( 0 )
				, tail() {
			}
		};
		bool _complete;
		static int const MAX_EDGES = 16;
		HEdge _edges[MAX_EDGES];
		code_point_t _keyCode;
	public:
		HNode( void )
			: _complete( false )
			, _edges()
			, _keyCode( unicode::CODE_POINT::NUL ) {
		}
		void add_tail( char const* sequence_, code_point_t keyCode_ ) {
			M_PROLOG
			if ( sequence_[0] ) {
				char key = sequence_[0];
				int i( 0 );
				for ( ; i < MAX_EDGES; ++ i ) {
					if ( _edges[i].head == 0 ) {
						_edges[i].head = key;
						_edges[i].tail = make_resource<HNode>();
						break;
					} else if ( _edges[i].head == key ) {
						break;
					}
				}
				M_ASSERT( i < MAX_EDGES );
				_edges[i].tail->add_tail( sequence_ + 1, keyCode_ );
			} else {
				_complete = true;
				_keyCode = keyCode_;
			}
			M_EPILOG
		}
		code_point_t scan( HSequenceScanner& sequenceScanner_, HTerminal& term_, int level_ ) {
			M_PROLOG
			code_point_t cp( unicode::CODE_POINT::INVALID );
			if ( _complete ) {
				cp = _keyCode;
			} else {
				code_point_t character( sequenceScanner_.read_code_point( term_ ) );
				int i( 0 );
				for ( ; ( i < MAX_EDGES ) && ( _edges[i].head != 0 ); ++ i ) {
					if ( static_cast<code_point_t::value_type>( _edges[i].head ) == character ) {
						cp = _edges[i].tail->scan( sequenceScanner_, term_, level_ + 1 );
						break;
					}
				}
				if ( ( cp == unicode::CODE_POINT::INVALID ) && ( level_ == 0 ) ) {
					cp = code_point_t( static_cast<code_point_t::value_type>( KEY<>::meta_r( static_cast<int>( character.get() ) ) ) );
				}
			}
			return ( cp );
			M_EPILOG
		}
	};
	HNode::node_t _trie;
public:
	HSequenceScanner( void )
		: _trie( make_resource<HNode>() ) {
		KeyEncodingTable const& keyEncodingTable( get_key_encoding_table() );
		for ( int i( 0 ); i < keyEncodingTable.size; ++ i  ) {
			KeyEncoding const& ke( keyEncodingTable.data[i] );
			_trie->add_tail( ke.escapeSequence, ke.keyCode );
		}
	}
	code_point_t scan( HTerminal& term_ ) {
		M_PROLOG
		return ( _trie->scan( *this, term_, 0 ) );
		M_EPILOG
	}
	code_point_t read_code_point( HTerminal& term_ ) {
		return ( term_.read_code_point() );
	}
};

HTerminal::HTerminal( void )
	: _termios( chunk_size<struct termios>( 2 ) )
	, _sequenceScanner( make_pointer<HSequenceScanner>() ) {
	return;
}

bool HTerminal::exists( void ) const {
	return ( ::isatty( STDIN_FILENO ) || ::isatty( STDOUT_FILENO ) || ::isatty( STDERR_FILENO ) );
}

void HTerminal::init( void ) {
	M_PROLOG
	termios& backupTermios( _termios.get<struct termios>()[TERMIOS::ORIGINAL] );
	::memset( &backupTermios, 0, sizeof ( backupTermios ) );
	M_ENSURE( tcgetattr( STDIN_FILENO, &backupTermios ) == 0 );
	termios newTermios;
	::memset( &newTermios, 0, sizeof ( newTermios ) );
	M_ENSURE( tcgetattr( STDIN_FILENO, &newTermios ) == 0 );
	if ( _disableXON_ ) {
		newTermios.c_iflag &= ~static_cast<int unsigned>( IXON );
	}
	M_ENSURE( tcsetattr( STDIN_FILENO, TCSAFLUSH, &newTermios ) == 0 );
	if ( _leaveCtrlC_ ) {
		control_character_disable( ACTION::INTERRUPT );
	}
	if ( _leaveCtrlZ_ ) {
		control_character_disable( ACTION::SUSPEND );
	}
	if ( _leaveCtrlS_ ) {
		control_character_disable( ACTION::STOP );
	}
	if ( _leaveCtrlQ_ ) {
		control_character_disable( ACTION::START );
	}
	if ( _leaveCtrlBackSlash_ ) {
		control_character_disable( ACTION::QUIT );
	}
	return;
	M_EPILOG
}

namespace {

int terminal_action_to_cc_subscript( HTerminal::ACTION action_ ) {
	int ccSubscript( VINTR );
	switch ( action_ ) {
		case ( HTerminal::ACTION::INTERRUPT ): ccSubscript = VINTR;  break;
		case ( HTerminal::ACTION::START ):     ccSubscript = VSTART; break;
		case ( HTerminal::ACTION::STOP ):      ccSubscript = VSTOP;  break;
		case ( HTerminal::ACTION::QUIT ):      ccSubscript = VQUIT;  break;
		case ( HTerminal::ACTION::SUSPEND ):   ccSubscript = VSUSP;  break;
	}
	return ( ccSubscript );
}

char unsigned terminal_action_to_default_key_code( HTerminal::ACTION action_ ) {
	char unsigned keyCode( 0 );
	switch ( action_ ) {
		case ( HTerminal::ACTION::INTERRUPT ): keyCode =  3; break;
		case ( HTerminal::ACTION::START ):     keyCode = 17; break;
		case ( HTerminal::ACTION::STOP ):      keyCode = 19; break;
		case ( HTerminal::ACTION::QUIT ):      keyCode = 28; break;
		case ( HTerminal::ACTION::SUSPEND ):   keyCode = 26; break;
	}
	return ( keyCode );
}

}

void HTerminal::control_character_enable( ACTION action_ ) {
	M_PROLOG
	termios newTermios;
	M_ENSURE( tcgetattr( STDIN_FILENO, &newTermios ) == 0 );
	newTermios.c_cc[terminal_action_to_cc_subscript( action_ )] = terminal_action_to_default_key_code( action_ );
	M_ENSURE( tcsetattr( STDIN_FILENO, TCSAFLUSH, &newTermios ) == 0 );
	return;
	M_EPILOG
}

void HTerminal::control_character_disable( ACTION action_ ) {
	M_PROLOG
	termios newTermios;
	M_ENSURE( tcgetattr( STDIN_FILENO, &newTermios ) == 0 );
	newTermios.c_cc[terminal_action_to_cc_subscript( action_ )] = 0;
	M_ENSURE( tcsetattr( STDIN_FILENO, TCSAFLUSH, &newTermios ) == 0 );
	return;
	M_EPILOG
}

void HTerminal::reset( void ) {
	M_PROLOG
	M_ENSURE( tcsetattr( STDIN_FILENO, TCSAFLUSH, _termios.get<termios>() + TERMIOS::ORIGINAL ) == 0 );
	return;
	M_EPILOG
}

void HTerminal::enable_raw_mode( void ) {
	M_PROLOG
	termios& backupTermios( _termios.get<struct termios>()[TERMIOS::CANONICAL] );
	::memset( &backupTermios, 0, sizeof ( backupTermios ) );
	M_ENSURE( tcgetattr( STDIN_FILENO, &backupTermios ) == 0 );
	termios newTermios;
	::memset( &newTermios, 0, sizeof ( newTermios ) );
	M_ENSURE( tcgetattr( STDIN_FILENO, &newTermios ) == 0 );

	newTermios.c_iflag &= ~static_cast<int unsigned>( BRKINT | ICRNL | INPCK | ISTRIP | IXON );

	/* Control modes - set 8 bit chars. */
	newTermios.c_cflag |= ( CS8 );

	/* Local modes - echoing off, canonical off, no extended functions,
	 * no signal chars (^Z,^C) */
	newTermios.c_lflag &= ~( FWD_ECHO | ICANON | IEXTEN | ISIG );

	/* Control chars - set return condition: min number of bytes and timer.
	 * We want read to return every single byte, without timeout. */
	newTermios.c_cc[VMIN] = 1;
	newTermios.c_cc[VTIME] = 0; /* 1 byte, no timer */

	/* Put terminal in raw mode after flushing. */
	M_ENSURE( tcsetattr( STDIN_FILENO, TCSAFLUSH, &newTermios ) == 0 );
	return;
	M_EPILOG
}

bool HTerminal::read_byte( yaal::u8_t& byte_ ) {
	int nRead( 0 );
	do {
		nRead = static_cast<int>( ::read( STDIN_FILENO, &byte_, sizeof ( byte_ ) ) );
	} while ( ( nRead == -1 ) && ( errno == EINTR ) );
	return ( nRead == 1 );
}

code_point_t HTerminal::read_code_point( void ) {
	M_PROLOG
	yaal::u32_t character( 0 );
#ifndef __MSVCXX__
	yaal::u8_t byte( 0 );
	if ( read_byte( byte ) ) {
		static u8_t const mask[] = { 0xff, unicode::ENC_2_BYTES_VALUE_MASK, unicode::ENC_3_BYTES_VALUE_MASK, unicode::ENC_4_BYTES_VALUE_MASK };
		int tailLength( unicode::utf8_declared_length( byte ) - 1 );
		character = byte;
		character &= mask[tailLength];
		for ( int i( 0 ); i < tailLength; ++ i ) {
			character <<= 6;
			if ( ! read_byte( byte ) ) {
				character = unicode::CODE_POINT::INVALID.get();
				break;
			}
			character |= ( static_cast<u8_t>( byte ) & unicode::TAIL_BYTES_VALUE_MASK );
		}
	}
#else /* #ifndef __MSVCXX__ */
	character = win_read_console_key();
#endif /* #else #ifndef __MSVCXX__ */
	return ( code_point_t( character ) );
	M_EPILOG
}

code_point_t HTerminal::get_character( void ) {
	M_PROLOG
	HScopeExitCall sec( call( &HTerminal::enable_raw_mode, this ), call( &HTerminal::disable_raw_mode, this ) );
	return ( read_code_point() );
	M_EPILOG
}

code_point_t HTerminal::get_key( void ) {
	M_PROLOG
	HScopeExitCall sec( call( &HTerminal::enable_raw_mode, this ), call( &HTerminal::disable_raw_mode, this ) );
	code_point_t key( read_code_point() );
	if ( key == KEY_CODE::ESCAPE ) {
		key = _sequenceScanner->scan( *this );
	}
	return ( key );
	M_EPILOG
}

void HTerminal::disable_raw_mode( void ) {
	M_PROLOG
	M_ENSURE( tcsetattr( STDIN_FILENO, TCSAFLUSH, _termios.get<termios>() + TERMIOS::CANONICAL ) == 0 );
	return;
	M_EPILOG
}

namespace {

static int const BAD_FD( -1 );

int find_term_fd( void ) {
	int fd( BAD_FD );
	if ( ::isatty( STDOUT_FILENO ) ) {
		fd = STDOUT_FILENO;
	} else if ( ::isatty( STDERR_FILENO ) ) {
		fd = STDERR_FILENO;
	} else if ( ::isatty( STDIN_FILENO ) ) {
		fd = STDIN_FILENO;
	}
	return ( fd );
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
static int const FWD_TIOCGWINSZ = TIOCGWINSZ;
#pragma GCC diagnostic pop

}

namespace {

int from_env( char const* envVarName_ ) {
	int val( -1 );
	char const* envVarValue( ::getenv( envVarName_ ) );
	if ( envVarValue ) {
		try {
			val = lexical_cast<int>( envVarValue );
		} catch ( HException const& ) {
		}
	}
	return ( val );
}

}

HTerminal::HSize HTerminal::size( void ) const {
	M_PROLOG
	M_ENSURE( exists(), "Not connected to a terminal." );
	int termFd( find_term_fd() );
	M_ASSERT( termFd != BAD_FD );
	int lines( from_env( "LINES" ) );
	int columns( from_env( "COLUMNS" ) );
	if ( ( lines < 0 ) || ( columns < 0 ) ) {
		winsize w;
		::memset( &w, 0, sizeof ( w ) );
		M_ENSURE( ioctl( termFd, FWD_TIOCGWINSZ, &w ) >= 0 );
		if ( lines < 0 ) {
			lines = w.ws_row;
		}
		if ( columns < 0 ) {
			columns = w.ws_col;
		}
	}
	return ( HSize( lines, columns ) );
	M_EPILOG
}

template<>
bool is_a_tty( int const& fd_ ) {
	return ( ::isatty( fd_ ) ? true : false );
}

template<>
bool is_a_tty( FILE* const& file_ ) {
	bool isTty( false );
	if ( file_ == stdout ) {
		isTty = ::isatty( STDOUT_FILENO ) ? true : false;
	} else if ( file_ == stderr ) {
		isTty = ::isatty( STDERR_FILENO ) ? true : false;
	} else if ( file_ == stdin ) {
		isTty = ::isatty( STDIN_FILENO ) ? true : false;
	}
	return ( isTty );
}

template<>
bool is_a_tty( HStreamInterface const& stream_ ) {
	bool isTty( false );
	if ( ( &stream_ == &cout ) || ( &stream_ == &clog ) ) {
		isTty = ::isatty( STDOUT_FILENO ) ? true : false;
	} else if ( &stream_ == &cerr ) {
		isTty = ::isatty( STDERR_FILENO ) ? true : false;
	} else if ( &stream_ == &cin ) {
		isTty = ::isatty( STDIN_FILENO ) ? true : false;
	} else if ( stream_.poll_type() == HStreamInterface::POLL_TYPE::NATIVE ) {
		isTty = ::isatty( static_cast<int>( reinterpret_cast<int_native_t>( stream_.data() ) ) ) ? true : false;
	}
	return ( isTty );
}

template<>
bool is_a_tty( HSynchronizedStream const& stream_ ) {
	return ( is_a_tty<HStreamInterface>( stream_ ) );
}

template<>
bool is_a_tty( HFile const& stream_ ) {
	return ( is_a_tty<HStreamInterface>( stream_ ) );
}

}

}

