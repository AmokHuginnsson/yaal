/* Read yaal/LICENSE.md file for copyright and licensing information. */

#ifndef YAAL_TOOLS_HTERMINAL_HXX_INCLUDED
#define YAAL_TOOLS_HTERMINAL_HXX_INCLUDED 1

#include "hcore/hchunk.hxx"
#include "hcore/hpair.hxx"
#include "hcore/hstreaminterface.hxx"
#include "hcore/hsingleton.hxx"

namespace yaal {

namespace tools {

class M_YAAL_TOOLS_PUBLIC_API HTerminal : public yaal::hcore::HSingleton<HTerminal> {
public:
	typedef HTerminal this_type;
	typedef yaal::hcore::HSingleton<HTerminal> base_type;
	typedef yaal::hcore::HPair<int, int> coord_t;
private:
	yaal::hcore::HChunk _termios;
public:
	HTerminal( void );
	bool exists( void ) const;
	void init( void );
	void flush( void );
	coord_t size( void ) const;
	static int life_time( int ) {
		return ( 50 );
	}
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

