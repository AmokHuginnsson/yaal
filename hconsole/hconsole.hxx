/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hconsole.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCONSOLE_HCONSOLE_HXX_INCLUDED
#define YAAL_HCONSOLE_HCONSOLE_HXX_INCLUDED

namespace yaal
{

namespace hconsole
	{

extern int	n_iLatency;
extern int	n_iAttributeDisabled;
extern int	n_iAttributeEnabled;
extern int	n_iAttributeFocused;
extern int	n_iAttributeStatusBar;
extern int	n_iAttributeSearchHighlight;
extern bool	n_bUseMouse;
extern bool	n_bDisableXON;
extern bool	n_bLeaveCtrlC;
extern bool	n_bLeaveCtrlZ;
extern bool	n_bLeaveCtrlS;
extern bool	n_bLeaveCtrlQ;
extern bool	n_bLeaveCtrlBackSlash;
extern char	n_cCommandComposeCharacter;
extern int	n_iCommandComposeDelay;
void yaal_hconsole_banner ( void );

	}

}

#endif /* not YAAL_HCONSOLE_HCONSOLE_HXX_INCLUDED */

