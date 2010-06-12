/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hpipe.hxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#ifndef YAAL_HCORE_HPIPE_HXX_INCLUDED
#define YAAL_HCORE_HPIPE_HXX_INCLUDED

#include "hcore/hstreaminterface.hxx"
#include "hcore/hpointer.hxx"

namespace yaal
{

namespace hcore
{

/*! \brief Unnamed pipe C++ wrapper.
 */
class HPipe : public yaal::hcore::HStreamInterface
	{
protected:
	typedef HPipe this_type;
	typedef HStreamInterface base_type;
private:
	int _pipe[ 2 ];
public:
	typedef yaal::hcore::HPointer<HPipe> ptr_t;
	HPipe( void );
	~HPipe( void );
	int get_reader_fd( void ) const;
protected:
	virtual int long do_write( void const* const, int long const& );
	virtual int long do_read( void* const, int long const& );
	virtual void do_flush( void ) const;
	virtual bool do_is_valid( void ) const;
	};

typedef HExceptionT<HPipe,HStreamInterfaceException> HPipeException;

}

}

#endif /* not YAAL_HCORE_HPIPE_HXX_INCLUDED */

