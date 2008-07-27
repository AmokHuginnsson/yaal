/*
---           hanalyser.h 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	base64.h - this file is integral part of hanalyser.h project.

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

#ifndef __YAAL_TOOLS_BASE64_H
#define __YAAL_TOOLS_BASE64_H

#include "hcore/hexception.h"
#include "hcore/hstring.h"

namespace yaal
{

namespace tools
{

struct base64
	{
	typedef base64 self_t;
	static yaal::hcore::HString encode( yaal::hcore::HString const& );
	static yaal::hcore::HString decode( yaal::hcore::HString const& );
	};

typedef yaal::hcore::HExceptionT<base64> HBase64Exception;

}

}

#endif /* not __YAAL_TOOLS_BASE64_H */

