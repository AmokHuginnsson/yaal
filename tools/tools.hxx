/* Read yaal/LICENSE.md file for copyright and licensing information. */

#ifndef YAAL_TOOLS_TOOLS_HXX_INCLUDED
#define YAAL_TOOLS_TOOLS_HXX_INCLUDED 1

#include "tools/hserial.hxx"

namespace yaal {

/*! \brief Additional useful functions.
 *
 * This namespace contains some very cool functions that
 * easies performing complex tasks.
 */
namespace tools {

extern M_YAAL_TOOLS_PUBLIC_API yaal::hcore::HString _serialDevice_;
extern M_YAAL_TOOLS_PUBLIC_API yaal::hcore::HString _defaultEncoding_;
extern M_YAAL_TOOLS_PUBLIC_API HSerial::SPEED _baudRate_;
extern M_YAAL_TOOLS_PUBLIC_API HSerial::flag_t _serialFlags_;
extern M_YAAL_TOOLS_PUBLIC_API int _compressionLevel_;
extern M_YAAL_TOOLS_PUBLIC_API int _huginnMaxCallStack_;
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

