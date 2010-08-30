/*
---          `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	signals.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_TOOLS_SIGHAND_HXX_INCLUDED
#define YAAL_TOOLS_SIGHAND_HXX_INCLUDED

#include "hcore/hpointer.hxx"
#include "hcore/hchunk.hxx"
#include "hcore/hmultimap.hxx"
#include "hcore/hlist.hxx"
#include "hcore/hthread.hxx"
#include "hcore/hsingleton.hxx"

namespace yaal
{

namespace tools
{  

/*! \brief Posix signal manager.
 *
 * Instance of HSignalService class is a global object that handles
 * dispatching of operating system level signals.
 */
class M_YAAL_TOOLS_PUBLIC_API HSignalService : public yaal::hcore::HSingleton<HSignalService>
	{
public:
	typedef yaal::hcore::HSingleton<HSignalService> base_type;
	typedef HSignalService this_type;
	typedef yaal::hcore::HBoundCall<int ( int )> handler_t;
private:
	typedef yaal::hcore::HMultiMap<int, handler_t> handlers_t;
	typedef handlers_t::value_list_t handler_list_t;
	typedef handlers_t::value_list_ptr_t handler_list_ptr_t;
	static int _exitStatus;
	bool _loop;
	yaal::hcore::HChunk _locker;
	yaal::hcore::HThread _thread;
	yaal::hcore::HMutex _mutex;
	handlers_t _handlers;
public:
	void register_handler( int, handler_t );
	void reset_signal( int );
	void stop( void	);
private:
	HSignalService( void );
	~HSignalService( void );
	void* run( void );
	void lock_on( int );
	static int life_time( int );
	void schedule_exit( int );
	static void exit( int ) __attribute__(( __noreturn__ ));
	friend class yaal::hcore::HSingleton<HSignalService>;
	friend class yaal::hcore::HDestructor<HSignalService>;
	};

/*! \brief Exception generated by HSignalService.
 */
typedef yaal::hcore::HExceptionT<HSignalService> HSignalServiceException;

}

}

#endif /* not YAAL_TOOLS_SIGHAND_HXX_INCLUDED */

