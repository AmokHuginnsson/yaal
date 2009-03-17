/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	cl_switch.hxx - this file is integral part of `yaal' project.

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

/*! \file hcore/cl_switch.hxx
 * \brief Declaration of command line hadling routines.
 */

#ifndef YAAL_HCORE_CL_SWITCH_HXX_INCLUDED
#define YAAL_HCORE_CL_SWITCH_HXX_INCLUDED

#include "hcore/ooption.hxx"

namespace yaal
{

namespace hcore
{

/*! \brief Handling of command line switches.
 * 
 * cl_switch gives easy to use API for interpreting and handling
 * command line switches that are common in Unix world.
 */
namespace cl_switch
	{

/*! \brief Parse command line options and set program setup variables.
 *
 * \param argc - argument count, number of arguments on command line.
 * \param argv - table of arguments from command line.
 * \param setup - array of supported program options descriptions.
 * \param count - number of supported program options.
 * \param[out] unknown - index of first unknown option in \e argv.
 */
int decode_switches( int const& argc, char* const* const argv,
		OOption* const& setup, int const& count, int* const unknown = NULL );

	}

}

}

#endif /* not YAAL_HCORE_CL_SWITCH_HXX_INCLUDED */

