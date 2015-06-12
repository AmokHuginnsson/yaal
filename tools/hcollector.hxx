/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hcollector.hxx - this file is integral part of `yaal' project.

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
/*! \file tools/hcollector.hxx
 * \brief Declaration of HCollector class.
 */

#ifndef YAAL_TOOLS_HCOLLECTOR_HXX_INCLUDED
#define YAAL_TOOLS_HCOLLECTOR_HXX_INCLUDED 1

#include "hcore/hstring.hxx"
#include "tools/hserial.hxx"

namespace yaal {

namespace tools {

/*! \brief Collector device interface.
 */
class HCollector : public HSerial {
public:
	typedef HCollector this_type;
	typedef HSerial base_type;
	/*! \brief Protocol description for Collector device.
	 */
	static int const RECV_BUF_SIZE = 8; /* 5 should be enought but you never know */
private:
	int  _lines;
	char _readBuf[ RECV_BUF_SIZE + 1 /* for \0 - the terminator */ ];
	hcore::HString _line;
public:
	HCollector( char const* = NULL ); /* device path */
	int send_line( char const* );
	int receive_line( hcore::HString& );
	int establish_connection( int = 9999 );
	int wait_for_connection( int = 9999 );
	int read_collector( void (*)( char const*, int ) );
private:
	bool test_char( char const*, int ) const;
};

typedef yaal::hcore::HExceptionT<HCollector, HSerialException> HCollectorException;

}

}

#endif /* #ifndef YAAL_TOOLS_HCOLLECTOR_HXX_INCLUDED */

