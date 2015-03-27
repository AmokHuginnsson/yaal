/*
---          `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	tools.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_TOOLS_TOOLS_HXX_INCLUDED
#define YAAL_TOOLS_TOOLS_HXX_INCLUDED 1

#include "tools/hserial.hxx"

namespace yaal {

/*! \brief Addtional useful functions.
 *
 * This namespace contains some very cool functions that
 * easies performing complex tasks.
 */
namespace tools {

extern M_YAAL_TOOLS_PUBLIC_API yaal::hcore::HString _serialDevice_;
extern M_YAAL_TOOLS_PUBLIC_API yaal::hcore::HString _defaultEncoding_;
extern M_YAAL_TOOLS_PUBLIC_API HSerial::SPEED::speed_t _baudRate_;
extern M_YAAL_TOOLS_PUBLIC_API HSerial::flag_t _serialFlags_;
extern M_YAAL_TOOLS_PUBLIC_API int _collectorConnectionTimeOut_;
extern M_YAAL_TOOLS_PUBLIC_API int _compressionLevel_;
extern M_YAAL_TOOLS_PUBLIC_API int long _zBufferSize_;
extern M_YAAL_TOOLS_PUBLIC_API bool _ignoreSignalSIGINT_;
extern M_YAAL_TOOLS_PUBLIC_API bool _ignoreSignalSIGTSTP_;
extern M_YAAL_TOOLS_PUBLIC_API bool _ignoreSignalSIGQUIT_;
extern M_YAAL_TOOLS_PUBLIC_API bool _disableXON_;
extern M_YAAL_TOOLS_PUBLIC_API bool _leaveCtrlC_;
extern M_YAAL_TOOLS_PUBLIC_API bool _leaveCtrlZ_;
extern M_YAAL_TOOLS_PUBLIC_API bool _leaveCtrlS_;
extern M_YAAL_TOOLS_PUBLIC_API bool _leaveCtrlQ_;
extern M_YAAL_TOOLS_PUBLIC_API bool _leaveCtrlBackSlash_;
void banner( void );

}

}

#endif /* #ifndef YAAL_TOOLS_TOOLS_HXX_INCLUDED */

