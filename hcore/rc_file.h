/*
---         `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski           ---

	rc_file.h - this file is integral part of `yaal' project.

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

#ifndef __YAAL_HCORE_RC_FILE_H
#define __YAAL_HCORE_RC_FILE_H

#include "hcore/hfile.h"

namespace yaal
{

namespace hcore
{

struct OVariable
	{
	type_t f_eType;
	char const * f_pcKey;
	void * f_pvValue;
	};

namespace rc_file
{

int process_rc_file_internal ( char const * const, char const * const,
		OVariable const * const, int const,
		bool const ( * const ) ( HString &, HString & ) = NULL );

template < typename tType >
int process_rc_file ( char const * const a_pcConfigName,
		char const * const a_pcSection,
		tType const & a_tData,
		bool const ( * const CALLBACK ) ( HString &, HString & ) = NULL )
	{
	return ( process_rc_file_internal ( a_pcConfigName, a_pcSection, a_tData,
			( sizeof ( tType ) / sizeof ( OVariable ) ) - 1, CALLBACK ) );
	}

void rc_set_variable ( char const * const, bool & );
void rc_set_variable ( char const * const, char & );
void rc_set_variable ( char const * const, char ** );
void rc_set_variable ( char const * const, int & );

}

}

}

#endif /* not __YAAL_HCORE_RC_FILE_H */

