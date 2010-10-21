/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hconsole.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCONSOLE_HCONSOLE_HXX_INCLUDED
#define YAAL_HCONSOLE_HCONSOLE_HXX_INCLUDED 1

namespace yaal
{

namespace hconsole
{

extern M_YAAL_HCONSOLE_PUBLIC_API int _latency_;
extern M_YAAL_HCONSOLE_PUBLIC_API int _attributeDisabled_;
extern M_YAAL_HCONSOLE_PUBLIC_API int _attributeEnabled_;
extern M_YAAL_HCONSOLE_PUBLIC_API int _attributeFocused_;
extern M_YAAL_HCONSOLE_PUBLIC_API int _attributeStatusBar_;
extern M_YAAL_HCONSOLE_PUBLIC_API int _attributeSearchHighlight_;
extern M_YAAL_HCONSOLE_PUBLIC_API bool _useMouse_;
extern M_YAAL_HCONSOLE_PUBLIC_API bool _disableXON_;
extern M_YAAL_HCONSOLE_PUBLIC_API bool _leaveCtrlC_;
extern M_YAAL_HCONSOLE_PUBLIC_API bool _leaveCtrlZ_;
extern M_YAAL_HCONSOLE_PUBLIC_API bool _leaveCtrlS_;
extern M_YAAL_HCONSOLE_PUBLIC_API bool _leaveCtrlQ_;
extern M_YAAL_HCONSOLE_PUBLIC_API bool _leaveCtrlBackSlash_;
extern M_YAAL_HCONSOLE_PUBLIC_API char _commandComposeCharacter_;
extern M_YAAL_HCONSOLE_PUBLIC_API int _commandComposeDelay_;
void banner( void );

}

}

#endif /* #ifndef YAAL_HCONSOLE_HCONSOLE_HXX_INCLUDED */

