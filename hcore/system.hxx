/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	system.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/system.hxx
 * \brief Operating system calls.
 */

#ifndef YAAL_HCORE_SYSTEM_HXX_INCLUDED
#define YAAL_HCORE_SYSTEM_HXX_INCLUDED 1

namespace yaal
{

namespace hcore
{

namespace system
{

int close( int );
int getpid( void );
int kill( int, int );

/*! \brief Wait for input/output event on given descriptor set.
 *
 * \param[in,out] input_ - set of input descriptors to observe.
 * \param inputCount_ - a number of input descriptors to observe.
 * \param[in,out] output_ set of output descriptors to observe.
 * \param outputCount_ - a number of output descriptors to observe.
 * \param timeOut - wait that many miliseconds for an event.
 * \return number of signaled events.
 */
int wait_for_io( int* input_, int const& inputCount_, int* output_, int const& outputCount_, int long* timeOut_ );

}

}

}

#endif /* not YAAL_HCORE_SYSTEM_HXX_INCLUDED */

