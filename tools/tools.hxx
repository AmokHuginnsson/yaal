/*
---          `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	tools.hxx - this file is integral part of `yaal' project.

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
	
extern yaal::hcore::HString _serialDevice_;
extern yaal::hcore::HString _defaultEncoding_;
extern HSerial::speed_t _baudRate_;
extern HSerial::flag_t _serialFlags_;
extern int _collectorConnectionTimeOut_;
extern int _compressionLevel_;
extern int long _zBufferSize_;
extern bool _ignoreSignalSIGINT_;
extern bool _ignoreSignalSIGTSTP_;
extern bool _ignoreSignalSIGQUIT_;
void banner( void );

}

}

#endif /* not YAAL_TOOLS_TOOLS_HXX_INCLUDED */

