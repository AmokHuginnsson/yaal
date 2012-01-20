/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hpipe.hxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#ifndef YAAL_HCORE_HPIPE_HXX_INCLUDED
#define YAAL_HCORE_HPIPE_HXX_INCLUDED 1

#include "hcore/hstreaminterface.hxx"
#include "hcore/hpointer.hxx"

namespace yaal {

namespace hcore {

/*! \brief Unnamed pipe C++ wrapper.
 */
class HPipe : public yaal::hcore::HStreamInterface {
public:
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
	virtual int long do_write( void const* const, int long );
	virtual int long do_read( void* const, int long );
	virtual void do_flush( void ) const;
	virtual bool do_is_valid( void ) const;
};

typedef HExceptionT<HPipe, HStreamInterfaceException> HPipeException;

}

}

#endif /* #ifndef YAAL_HCORE_HPIPE_HXX_INCLUDED */

