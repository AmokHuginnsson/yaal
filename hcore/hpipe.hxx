/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hcore/hpipe.hxx
 * \brief Declaration of HPipe class.
 */

#ifndef YAAL_HCORE_HPIPE_HXX_INCLUDED
#define YAAL_HCORE_HPIPE_HXX_INCLUDED 1

#include "hcore/hstreaminterface.hxx"
#include "hcore/hpointer.hxx"

namespace yaal {

namespace hcore {

/*! \brief Unnamed pipe C++ wrapper.
 */
class HPipe {
public:
	typedef HPipe this_type;
	typedef yaal::hcore::HPointer<HPipe> ptr_t;
private:
	yaal::hcore::HStreamInterface::ptr_t _in;
	yaal::hcore::HStreamInterface::ptr_t _out;
public:
	HPipe( void );
	~HPipe( void );
	int long read( void*, int long );
	int long write( void const*, int long );
	void flush( void );
	yaal::hcore::HStreamInterface::ptr_t const& in( void ) const;
	yaal::hcore::HStreamInterface::ptr_t const& out( void ) const;
};

typedef HExceptionT<HPipe, HStreamInterfaceException> HPipeException;

}

}

#endif /* #ifndef YAAL_HCORE_HPIPE_HXX_INCLUDED */

