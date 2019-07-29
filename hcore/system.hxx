/* Read yaal/LICENSE.md file for copyright and licensing information. */
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

/*! \brief System user id type.
 */
typedef yaal::u32_t user_id_t;

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

/*! \brief Get current process'es effecive user id.
 *
 * \return Current process'es effecive user id.
 */
user_id_t get_user_id( void );

/*! \brief Get human readable user name from system user id.
 *
 * \param uid_ - system id of user.
 * \return human readable user name.
 */
yaal::hcore::HString get_user_name( user_id_t uid_ );

/*! \brief Get current process'es effecive group id.
 *
 * \return Current process'es effecive group id.
 */
user_id_t get_group_id( void );

/*! \brief Get human readable group name from system group id.
 *
 * \param gid_ - system id of group.
 * \return human readable group name.
 */
yaal::hcore::HString get_group_name( user_id_t gid_ );

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
	 * \param total_ - total amount of resource.
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

/*! \brief Get path of this process executable.
 *
 * \return Path to this process executable.
 */
yaal::hcore::HString get_self_exec_path( void );

/*! \brief Set CLOSE ON EXEC flag on given file descriptor.
 *
 * \param  fd - a file descriptor to act upon.
 */
void set_close_on_exec( int fd );

/*! \brief Get path to current user home directory.
 *
 * \return Current user home directory.
 */
yaal::hcore::HString home_path( void );

}

}

}

#endif /* #ifndef YAAL_HCORE_SYSTEM_HXX_INCLUDED */

