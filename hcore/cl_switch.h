/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	cl_switch.h - this file is integral part of `stdhapi' project.

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

#ifndef __CL_SWITCH_H
#define __CL_SWITCH_H

#include "hexception.h"

namespace stdhapi
{

namespace hcore
{

typedef void ( * simple_callback_t ) ( void );

typedef struct
	{
	char const * const f_pcLongOption;
	char f_cShortOption;
	enum
		{
		D_NONE,
		D_OPTIONAL,
		D_REQUIRED
		} f_eSwitchType;
	type_t f_eValueType;
	void * f_pvValue;
	simple_callback_t CALLBACK;
	} OOption;

namespace cl_switch
	{

int decode_switches ( int const, char * const * const,
		OOption * const &, int const, int * const = NULL );

	}

}

}

#endif /* not __CL_SWITCH_H */

