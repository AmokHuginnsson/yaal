/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hcollector.h - this file is integral part of `yaal' project.

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

#ifndef __YAAL_TOOLS_HCOLLECTOR_H
#define __YAAL_TOOLS_HCOLLECTOR_H

#include "hcore/hstring.h"
#include "tools/hserial.h"

namespace yaal
{

namespace tools
{

class HCollector : public HSerial
	{
public:
	struct PROTOCOL
		{
		static char const* const D_SYN;
		static char const* const D_ACK;
		static char const* const D_DTA; /* warrning! no endline */
		static char const* const D_FIN; /* warrning! no endline, but \0 at end,
																							 so sizeof() retruns 4 */
		static char const* const D_ERR;
		static int const D_RECV_BUF_SIZE = 8; /* 5 should be enought but you never know */
		};
private:
	int			f_iLines;
	char		f_pcReadBuf [ PROTOCOL::D_RECV_BUF_SIZE + 1 /* for \0 - the terminator */ ];
	hcore::HString	f_oLine;
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

}

}
	
#endif /* not __YAAL_TOOLS_HCOLLECTOR_H */
