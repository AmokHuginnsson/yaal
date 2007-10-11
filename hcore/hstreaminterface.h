/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hstreaminterface.h - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#ifndef __HSTREAMINTERFACE_H
#define __HSTREAMINTERFACE_H

#include "hcore/hstring.h"

namespace yaal
{

namespace hcore
{

class HStreamInterface;
HStreamInterface& endl( HStreamInterface& );
HStreamInterface& flush( HStreamInterface& );

class HStreamInterface
	{
protected:
	HString f_oVarTmpBuffer;
public:
	HStreamInterface( void );
	virtual ~HStreamInterface( void );
	HStreamInterface& operator << ( char const* const );
	HStreamInterface& operator << ( char const );
	HStreamInterface& operator << ( int const );
	HStreamInterface& operator << ( int unsigned const );
	HStreamInterface& operator << ( int long const );
	HStreamInterface& operator << ( int long unsigned const );
	HStreamInterface& operator << ( double const );
	HStreamInterface& operator << ( void const* const );
	HStreamInterface& operator << ( HStreamInterface& ( *const )( HStreamInterface& ) );
	int read_until( yaal::hcore::HString&, char const* const = "\r\n" );
	int read( void* const, int const );
	int write( void const* const, int const );
private:
	virtual int do_write( void const* const, int const ) = 0;
	virtual int do_read( void* const, int const ) = 0;
	virtual void do_flush( void ) const = 0;
	friend HStreamInterface& endl( HStreamInterface& );
	friend HStreamInterface& flush( HStreamInterface& );
	};

}

}

#endif /* not __HSTREAMINTERFACE_H */

