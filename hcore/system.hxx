/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hcore/system.hxx
 * \brief Operating system calls.
 */

#ifndef YAAL_HCORE_SYSTEM_HXX_INCLUDED
#define YAAL_HCORE_SYSTEM_HXX_INCLUDED 1

#include "hcore/hstring.hxx"
#include "hcore/pod.hxx"
#include "hcore/harray.hxx"
#include "hcore/bitmaskenum.hxx"

namespace yaal {

namespace hcore {

/*! \brief Low level system interface belongs here.
 */
namespace system {

/*! \brief Raw file descriptor type.
 */
typedef int file_descriptor_t;

/*! \brief System user id type.
 */
typedef yaal::u32_t user_id_t;
typedef yaal::hcore::HArray<yaal::hcore::HString> argv_t;

int close( int );
int getpid( void );
int kill( int, int );

/*! \brief I/O event type that user can wait for.
 */
enum class IO_EVENT_TYPE {
	NONE = 0,
	READ = 1,
	WRITE = 2,
	INTERRUPT = 4,
	TIMEOUT = 8
};
M_USING_ENUMBITMASK();

}

}

M_ENUM_IS_A_BITMASK( ::yaal::hcore::system::IO_EVENT_TYPE );

namespace hcore {

namespace system {

#undef INFINITY
static int const INFINITY = -1;

/*! \brief Wait for input/output event on given file descriptor.
 *
 * \param fd - a file descriptor to observe.
 * \param eventType - an event type to check for.
 * \param timeoutMS - wait that many milliseconds for an event.
 * \param restartable - should wait be automatically restarted after interrupt from signal.
 * \return A type of event that actually occurred.
 */
IO_EVENT_TYPE wait_for_io( file_descriptor_t fd, IO_EVENT_TYPE eventType, int timeoutMS = INFINITY, bool restartable = true );

/*! \brief Get current process'es effective user id.
 *
 * \return Current process'es effective user id.
 */
user_id_t get_user_id( void );

/*! \brief Get human readable user name from system user id.
 *
 * \param uid_ - system id of user.
 * \return human readable user name.
 */
yaal::hcore::HString get_user_name( user_id_t uid_ );

/*! \brief Get current process'es effective group id.
 *
 * \return Current process'es effective group id.
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

typedef yaal::u32_t mode_t;

mode_t get_umask( void );
void set_umask( mode_t );

/*! \brief Types of system resources limits.
 */
enum class RESOURCE_LIMIT_TYPE {
	MEMORY_SIZE,
	STACK_SIZE,
	CPU_TIME,
	OPEN_FILES,
	FILE_SIZE,
	PROCESS_COUNT,
	CORE_SIZE
};

/*! \brief Store information about given resource limits.
 */
class HResourceLimit {
public:
	static i64_t const UNLIMITED = -1;
private:
	i64_t _soft;
	i64_t _hard;
public:
	HResourceLimit( i64_t soft_, i64_t hard_ )
		: _soft( soft_ )
		, _hard( hard_ ) {
	}
	i64_t soft( void ) const {
		return ( _soft );
	}
	i64_t hard( void ) const {
		return ( _hard );
	}
};

/*! \brief Get value of system resource limit.
 *
 * \param resourceLimitType - a type of a resource to get a value of limit for.
 * \return A resource limit information.
 */
HResourceLimit get_limit( RESOURCE_LIMIT_TYPE resourceLimitType );

/*! \brief Set resource limit to new value.
 *
 * \param resourceLimitType - a type of a resource to set a new limit value for.
 * \param resourceLimits - soft and hard resource limit values to set.
 */
void set_limit( RESOURCE_LIMIT_TYPE resourceLimitType, HResourceLimit const& resourceLimits );

/*! \brief Replace current process image by running other program.
 *
 * \param image - an program to run.
 * \param args - arguments to pass to newly executed program.
 */
void exec( yaal::hcore::HString const& image, argv_t const& args ) __attribute__((noreturn));

}

}

}

#endif /* #ifndef YAAL_HCORE_SYSTEM_HXX_INCLUDED */

