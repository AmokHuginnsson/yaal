/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  system.hxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
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
#include "hcore/pod.hxx"

namespace yaal {

namespace hcore {

/*! \brief Low level system interface belongs here.
 */
namespace system {

int close( int );
int getpid( void );
int kill( int, int );

/*! \brief Wait for input/output event on given descriptor set.
 *
 * \param[in,out] input_ - set of input descriptors to observe.
 * \param inputCount_ - a number of input descriptors to observe.
 * \param[in,out] output_ - set of output descriptors to observe.
 * \param outputCount_ - a number of output descriptors to observe.
 * \param timeOut_ - wait that many milliseconds for an event.
 * \param restartable_ - should wait be automatically restarted after interrupt from signal.
 * \return number of signaled events.
 */
int wait_for_io( int* input_, int inputCount_, int* output_, int outputCount_, int long* timeOut_, bool restartable_ = true );

/*! \brief Get human readable user name from system user id.
 *
 * \param uid_ - system id of user.
 * \return human readable user name.
 */
yaal::hcore::HString get_user_name( int uid_ );

/*! \brief Get human readable group name from system group id.
 *
 * \param gid_ - system id of group.
 * \return human readable group name.
 */
yaal::hcore::HString get_group_name( int gid_ );

/*! \brief Get host name of local computer.
 *
 * \return Host name of local computer.
 */
yaal::hcore::HString get_host_name( void );

/*! \brief Store information about amount of given resource.
 */
class HResourceInfo {
	i64_t _available;
	i64_t _free;
	i64_t _total;
public:
	/*! \brief Construct resource info object.
	 *
	 * \param available_ - amount of resource available for allocation.
	 * \param free_ - amount of free resource.
	 * \param total_ - total amount of reource.
	 */
	HResourceInfo( i64_t available_ = 0, i64_t free_ = 0, i64_t total_ = 0 )
		: _available( available_ ), _free( free_ ), _total( total_) {}
	/*! \brief Get information about amount of resource available for allocation.
	 *
	 * \return Amount of available resource.
	 */
	i64_t available( void ) const
		{ return ( _available ); }
	/*! \brief Get information about amount of free resource.
	 *
	 * \return Free resource size.
	 */
	i64_t free( void ) const
		{ return ( _free ); }
	/*! \brief Get information about total size of resource.
	 *
	 * \return Total size of given resource.
	 */
	i64_t total( void ) const
		{ return ( _total ); }
};

/*! \brief Get amount of free/available for allocation memory.
 *
 * \return Available memory size in bytes.
 */
HResourceInfo get_memory_size_info( void );

/*! \brief Get amount of free/available disk space on given filesystem.
 *
 * \param path - path to filesystem to be checked for free space.
 * \return Available disk space in bytes.
 */
HResourceInfo get_disk_space_info( yaal::hcore::HString const& );

/*! \brief Get number of available CPU cores.
 *
 * \return Number of available CPU cores.
 */
int get_core_count_info( void );

/*! \brief Get size of system memory page.
 *
 * \return Size of system memory page in bytes.
 */
int get_page_size( void );

}

}

}

#endif /* #ifndef YAAL_HCORE_SYSTEM_HXX_INCLUDED */

