/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	ooption.cxx - this file is integral part of `yaal' project.

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

#include <cstdlib>

#include "base.h"
M_VCSID( "$Id: "__ID__" $" )
#include "ooption.h"
#include "rc_file.h"
#include "hstring.h"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::hcore::rc_file;

namespace yaal
{

namespace hcore
{

void set_option( OOption const& a_sOption, HString const& a_oValue )
	{
	M_PROLOG
	switch ( a_sOption.f_eValueType )
		{
		case ( D_BOOL ):
			rc_set_variable( ! a_oValue.is_empty() ? a_oValue.raw() : "true",
					*static_cast<bool*>( a_sOption.f_pvValue ) );
		break;
		case ( D_CHAR ):
			if ( ! a_oValue.is_empty() )
				rc_set_variable( a_oValue.raw(),
						*static_cast<char*>( a_sOption.f_pvValue ) );
		break;
		case ( D_INT ):
			rc_set_variable( a_oValue.raw(),
					*static_cast<int*>( a_sOption.f_pvValue ) );
		break;
		case ( D_CHAR_PTR ):
			rc_set_variable( a_oValue.raw(),
					static_cast<char**>( a_sOption.f_pvValue ) );
		break;
		case ( D_HSTRING ):
			( *static_cast<HString*>( a_sOption.f_pvValue ) ) = a_oValue;
		break;
		case (  D_DOUBLE_LONG ):
			rc_set_variable( a_oValue.raw(),
					*static_cast<double long*>( a_sOption.f_pvValue ) );
		break;
		case (  D_DOUBLE ):
			rc_set_variable( a_oValue.raw(),
					*static_cast<double*>( a_sOption.f_pvValue ) );
		break;
		case ( D_VOID ):
			break;
		default:
			M_THROW( "unknown type", a_sOption.f_eValueType );
		}
	if ( a_sOption.CALLBACK )
		a_sOption.CALLBACK->first( a_sOption.CALLBACK->second );
	return;
	M_EPILOG
	}

}

}

