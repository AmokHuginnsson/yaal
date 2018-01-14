/* Read yaal/LICENSE.md file for copyright and licensing information. */

#ifndef YAAL_TOOLS_HTERMINAL_HXX_INCLUDED
#define YAAL_TOOLS_HTERMINAL_HXX_INCLUDED 1

#include "hcore/hchunk.hxx"
#include "hcore/hpair.hxx"
#include "hcore/hstreaminterface.hxx"

namespace yaal {

namespace tools {

class HTerminal {
public:
	typedef HTerminal this_type;
	typedef yaal::hcore::HPair<int, int> coord_t;
private:
	yaal::hcore::HChunk _termios;
public:
	HTerminal( void );
	bool exists( void ) const;
	void init( void );
	void flush( void );
	coord_t size( void ) const;
};

extern HTerminal _terminal_;

template<typename T>
bool is_a_tty( T const& );

}

}

#endif /* #ifndef YAAL_TOOLS_HTERMINAL_HXX_INCLUDED */

