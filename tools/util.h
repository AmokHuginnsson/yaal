/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	util.h - this file is integral part of `yaal' project.

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

#ifndef __YAAL_TOOLS_UTIL_H
#define __YAAL_TOOLS_UTIL_H

#include "hcore/hstring.h"
#include "hcore/ooption.h"

namespace yaal
{

namespace tools
{

/*! \brief Small but useful utils.
 */
namespace util
	{

yaal::hcore::HString kwota_slownie( double );
void usun_ogonki( char* );
double atof_ex( yaal::hcore::HString const&, bool = false );
bool verify_IBAN( yaal::hcore::HString const& );
char const* get_last_error( void );

void show_help( yaal::hcore::OOption*, int, char const* const, char const* const, char const* const = NULL );
void dump_configuration( yaal::hcore::OOption*, int, char const* const, char const* const, char const* const = NULL );
typedef void ( * OPTION_PROCESSOR_t )( yaal::hcore::OOption*, int, char const* const, char const* const, char const* const );
struct OOptionInfo
	{
	yaal::hcore::OOption* _opt;
	int _size;
	OPTION_PROCESSOR_t PROC;
	OOptionInfo( yaal::hcore::OOption* opt, int const& size, OPTION_PROCESSOR_t const& a_PROC )
		: _opt( opt ), _size( size ), PROC( a_PROC ) {}
private:
	OOptionInfo( OOptionInfo const& );
	OOptionInfo& operator = ( OOptionInfo const& );
	};

void failure( int, char const* const, ... ) __attribute__(( __noreturn__ ));

/*! \brief Various convinient sleep functions.
 */
namespace sleep
{

int milisecond( int, bool = false );
int second( int, bool = false );

}

/*! \brief The Levenshtein and Levenshtein-Damerau string distance functions.
 */
namespace distance
{

int levenshtein_damerau( char const* const, char const* const, bool = true );

}

	}

}

}

#endif /* not __YAAL_TOOLS_UTIL_H */
