/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hcollector.hxx - this file is integral part of `yaal' project.

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
/*! \file tools/hcollector.hxx
 * \brief Declaration of HCollector class.
 */

#ifndef YAAL_TOOLS_HCOLLECTOR_HXX_INCLUDED
#define YAAL_TOOLS_HCOLLECTOR_HXX_INCLUDED

#include "hcore/hstring.hxx"
#include "tools/hserial.hxx"

namespace yaal
{

namespace tools
{

/*! \brief Collector device interface.
 */
class HCollector : public HSerial
	{
protected:
	typedef HCollector this_type;
	typedef HSerial base_type;
public:
	/*! \brief Protocol description for Collector device.
	 */
	struct PROTOCOL
		{
		static char const* const SYN;
		static char const* const ACK;
		static char const* const DTA; /* warrning! no endline */
		static char const* const FIN; /* warrning! no endline, but \0 at end,
																							 so sizeof() retruns 4 */
		static char const* const ERR;
		static int const RECV_BUF_SIZE = 8; /* 5 should be enought but you never know */
		};
private:
	int			_lines;
	char		_readBuf [ PROTOCOL::RECV_BUF_SIZE + 1 /* for \0 - the terminator */ ];
	hcore::HString	_line;
public:
	HCollector( char const* = NULL ); /* device path */
	int send_line( char const* );
	int receive_line( hcore::HString& );
	int establish_connection ( int = 9999 );
	int wait_for_connection ( int = 9999 );
	int read_collector( void ( * )( char const* const, int ) );
private:
	bool test_char( char const*, int ) const;
	};

typedef yaal::hcore::HExceptionT<HCollector, HSerialException> HCollectorException;

}

}
	
#endif /* not YAAL_TOOLS_HCOLLECTOR_HXX_INCLUDED */
