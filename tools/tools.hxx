/*
---          `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	tools.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_TOOLS_TOOLS_HXX_INCLUDED
#define YAAL_TOOLS_TOOLS_HXX_INCLUDED

#include "tools/hserial.hxx"

namespace yaal
{

/*! \brief Addtional useful functions.
 * 
 * This namespace contains some very cool functions that
 * easies performing complex tasks.
 */
namespace tools
	{
	
extern char * n_pcSerialDevice;
extern HSerial::speed_t n_eBaudRate;
extern HSerial::flags_t n_eSerialFlags;
extern int n_iCollectorConnectionTimeOut;
extern bool n_bIgnoreSignalSIGINT;
extern bool n_bIgnoreSignalSIGTSTP;
extern bool n_bIgnoreSignalSIGQUIT;
void yaal_tools_banner ( void );

	}

}

#endif /* not YAAL_TOOLS_TOOLS_HXX_INCLUDED */

