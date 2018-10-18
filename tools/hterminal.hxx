/* Read yaal/LICENSE.md file for copyright and licensing information. */

#ifndef YAAL_TOOLS_HTERMINAL_HXX_INCLUDED
#define YAAL_TOOLS_HTERMINAL_HXX_INCLUDED 1

#include "hcore/hchunk.hxx"
#include "hcore/hstreaminterface.hxx"
#include "hcore/hsingleton.hxx"

namespace yaal {

namespace tools {

class M_YAAL_TOOLS_PUBLIC_API HTerminal : public yaal::hcore::HSingleton<HTerminal> {
public:
	typedef HTerminal this_type;
	typedef yaal::hcore::HSingleton<HTerminal> base_type;
	class HSize {
		int _lines;
		int _columns;
	public:
		HSize( void )
			: _lines( -1 )
			, _columns( -1 ) {
		}
		HSize( int lines_, int columns_ )
			: _lines( lines_ )
			, _columns( columns_ ) {
		}
		int lines( void ) const {
			return ( _lines );
		}
		int columns( void ) const {
			return ( _columns );
		}
	};
private:
	yaal::hcore::HChunk _termios;
public:
	HTerminal( void );
	bool exists( void ) const;
	void init( void );
	void reset( void );
	HSize size( void ) const;
	static int life_time( int ) {
		return ( 50 );
	}
	code_point_t get_character( void );
	code_point_t get_key( void );
private:
	void enable_raw_mode( void );
	void disable_raw_mode( void );
	bool read_byte( yaal::u8_t& );
private:
	friend class yaal::hcore::HSingleton<HTerminal>;
	friend class yaal::hcore::HDestructor<HTerminal>;
};

typedef yaal::hcore::HExceptionT<HTerminal, yaal::hcore::HExceptionT<yaal::hcore::HSingleton<HTerminal>>> HTerminalException;

template<typename T>
bool is_a_tty( T const& );

}

}

#endif /* #ifndef YAAL_TOOLS_HTERMINAL_HXX_INCLUDED */

