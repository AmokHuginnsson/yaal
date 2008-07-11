/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hstringstream.h - this file is integral part of `yaal' project.

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

#ifndef __YAAL_TOOLS_HSTRINGSTREAM_H
#define __YAAL_TOOLS_HSTRINGSTREAM_H

#include "hcore/hstring.h"
#include "hcore/hstreaminterface.h"

namespace yaal
{

namespace tools
{

class HStringStream : public yaal::hcore::HStreamInterface
	{
	mutable bool f_bUsed;
	mutable yaal::hcore::HString f_oBuffer;
public:
	HStringStream( void );
	HStringStream( yaal::hcore::HString const& );
	HStringStream& operator = ( yaal::hcore::HString const& );
	char const* raw( void ) const;
	yaal::hcore::HString const& string( void ) const;
	void use( void ) const;
	void clear( void );
	char const* consume( void ) const;
	bool is_empty( void ) const;
private:
	virtual int long do_write( void const* const, int long const& );
	virtual void do_flush( void ) const;
	virtual int long do_read( void* const, int long const& );
	};

char const* operator << ( yaal::hcore::HStreamInterface const&, HStringStream const& );

}

}

#endif /* not __YAAL_TOOLS_HSTRINGSTREAM_H */

