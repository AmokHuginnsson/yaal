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

#include "hcore/hstring.hxx"

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
 * \param[in,out] output_ - set of output descriptors to observe.
 * \param outputCount_ - a number of output descriptors to observe.
 * \param timeOut_ - wait that many miliseconds for an event.
 * \param restartable_ - should wait be automatically restarted after interrupt from signal.
 * \return number of signaled events.
 */
int wait_for_io( int* input_, int inputCount_, int* output_, int outputCount_, int long* timeOut_, bool restartable_ = true );

/*! \brief Get human readable user name from system user id.
 * \param uid_ - system id of user.
 * \return human readable user name.
 */
yaal::hcore::HString get_user_name( int uid_ );

/*! \brief Get human readable group name from system group id.
 * \param gid_ - system id of group.
 * \return human readable group name.
 */
yaal::hcore::HString get_group_name( int gid_ );

/*! \brief Get amount of free/avaialable for allocation memory.
 * \return Available memory size in bytes.
 */
int long get_available_memory_size( void );

}

}

}

#endif /* #ifndef YAAL_HCORE_SYSTEM_HXX_INCLUDED */

