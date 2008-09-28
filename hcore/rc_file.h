/*
---         `yaal' (c) 1978 by Marcin 'Amok' Konarski           ---

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
#include "hcore/ooption.h"

namespace yaal
{

namespace hcore
{

/*! \brief Reading configuration files.
 * 
 * rc_file gives easy to use API for reading configuration files
 * that are common in Unix world.
 */
namespace rc_file
{

typedef bool ( * RC_CALLBACK_t )( HString&, HString& );

int reload_configuration( void );
int process_rc_file_internal( HString const&, HString const&,
		OOption const* const, int const, RC_CALLBACK_t = NULL );

template<typename tType>
int process_rc_file( HString const& a_oConfigName,
		HString const& a_oSection,
		tType const& a_tData, RC_CALLBACK_t CALLBACK  = NULL )
	{
	return ( process_rc_file_internal( a_oConfigName, a_oSection, a_tData,
			( sizeof ( tType ) / sizeof ( OOption ) ) - 1, CALLBACK ) );
	}

void rc_set_variable ( char const* const, bool& );
void rc_set_variable ( char const* const, char& );
void rc_set_variable ( char const* const, char** );
void rc_set_variable ( char const* const, int& );
void rc_set_variable ( char const* const, double& );
void rc_set_variable ( char const* const, double long& );

}

}

}

#endif /* not __YAAL_HCORE_RC_FILE_H */

