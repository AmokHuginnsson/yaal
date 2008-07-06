/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	ooption.h - this file is integral part of `yaal' project.

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

#ifndef __YAAL_HCORE_OOPTION_H
#define __YAAL_HCORE_OOPTION_H

#include "hcore/hpair.h"

namespace yaal
{

namespace hcore
{

typedef void ( *param_callback_t )( void* );
typedef yaal::hcore::HPair<param_callback_t, void*> simple_callback_t; 

typedef struct
	{
	char const* const f_pcName;
	type_t f_eValueType;
	void* f_pvValue;
	char const* const f_pcShortForm;
	enum
		{
		D_NONE,
		D_OPTIONAL,
		D_REQUIRED
		} f_eSwitchType;
	char const* f_pcArgument;
	char const* f_pcDescription;
	simple_callback_t* CALLBACK;
	} OOption;

void set_option( OOption const&, HString const& );

}

}

#endif /* not __YAAL_HCORE_OOPTION_H */

