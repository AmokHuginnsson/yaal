/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hstringstream.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_TOOLS_HSTRINGSTREAM_HXX_INCLUDED
#define YAAL_TOOLS_HSTRINGSTREAM_HXX_INCLUDED 1

#include "hcore/hstring.hxx"
#include "hcore/hstreaminterface.hxx"

namespace yaal {

namespace tools {

/*! \brief Perform stream operations on character string.
 */
class HStringStream : public yaal::hcore::HStreamInterface {
public:
	typedef HStringStream this_type;
	typedef yaal::hcore::HStreamInterface base_type;
private:
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
	yaal::hcore::HString const& consume( void ) const;
	bool is_empty( void ) const;
private:
	virtual int long do_write( void const* const, int long );
	virtual void do_flush( void );
	virtual int long do_read( void* const, int long );
	virtual bool do_is_valid( void ) const;
};

yaal::hcore::HString const& operator << ( yaal::hcore::HStreamInterface const&, HStringStream const& );

}

}

#endif /* #ifndef YAAL_TOOLS_HSTRINGSTREAM_HXX_INCLUDED */

