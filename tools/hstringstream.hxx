/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hstringstream.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_TOOLS_HSTRINGSTREAM_HXX_INCLUDED
#define YAAL_TOOLS_HSTRINGSTREAM_HXX_INCLUDED 1

#include "hcore/hstring.hxx"
#include "hcore/hstreaminterface.hxx"

namespace yaal
{

namespace tools
{

/*! \brief Perform stream operations on character string.
 */
class HStringStream : public yaal::hcore::HStreamInterface
	{
	mutable bool _used; /* FIXME Confirm mutable is necesarry here. */
	mutable int long _offset;
	mutable yaal::hcore::HString _buffer;
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
	virtual int long do_write( void const* const, int long );
	virtual void do_flush( void ) const;
	virtual int long do_read( void* const, int long );
	virtual bool do_is_valid( void ) const;
	};

char const* operator << ( yaal::hcore::HStreamInterface const&, HStringStream const& );

}

}

#endif /* #ifndef YAAL_TOOLS_HSTRINGSTREAM_HXX_INCLUDED */

