/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hcollector.h - this file is integral part of `stdhapi' project.

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

#ifndef __TOOLS_HCOLLECTOR_H
#define __TOOLS_HCOLLECTOR_H

#include "hserial.h"

namespace stdhapi
{

namespace tools
{

#define D_PROTO_SYN						"SYN\n"
#define D_PROTO_ACK						"ACK\n"
#define D_PROTO_DTA						"DTA" /* warrning! no endline */
#define D_PROTO_FIN						"FIN" /* warrning! no endline, but \0 at end,
																			 so sizeof ( ) retruns 4 */
#define D_PROTO_ERR						"ERR\n"

#define D_PROTO_RECV_BUF_SIZE	8 /* 5 should be enought but you never know */
#define D_RECV_BUF_SIZE				256

class HCollector : public HSerial
	{
protected:
	/*{*/
	int			f_iLines;
	char		f_pcReadBuf [ D_PROTO_RECV_BUF_SIZE ];
	hcore::HString	f_oLine;
	/*}*/
public:
	/*{*/
	HCollector ( char const * = NULL ); /* device path */
	int send_line ( char const * );
	int receive_line ( char * & );
	int establish_connection ( int = 9999 );
	int wait_for_connection ( int = 9999 );
	int read_collector ( void ( * ) ( char *, int ) );
	/*}*/
protected:
	/*{*/
	bool test_char ( char const *, int );
	/*}*/
	};

}

}
	
#endif /* not __TOOLS_HCOLLECTOR_H */
