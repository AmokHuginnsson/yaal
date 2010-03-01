/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hsingleton.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/hsingleton.hxx
 * \brief Declaration of HSingleton related classes.
 *
 * HSingleton<>, HSingletonInterface, HAbstractDestructor, HDestructor<>, HLifeTimeTracker.
 */

#ifndef YAAL_HCORE_HSINGLETON_HXX_INCLUDED
#define YAAL_HCORE_HSINGLETON_HXX_INCLUDED

#include "hcore/hpointer.hxx"
#include "hcore/hmultimap.hxx"
#include "hcore/hlist.hxx"
#include "hcore/hthread.hxx"

namespace yaal
{

namespace hcore
{

/*! \brief Interface for all client to singleton pattern.
 */
class HSingletonInterface : private trait::HNonCopyable
	{
public:
	static int life_time( int );
	};

/*! \brief Interface for automatic destruction used by HLifeTimeTracker.
 */
class HAbstractDestructor
	{
public:
	virtual ~HAbstractDestructor( void ) {}
	virtual void destruct( void ) = 0;
	};

/*! \brief Singleton deleter engine.
 */
class HLifeTimeTracker
	{
	typedef HLifeTimeTracker self_t;
public:
	typedef yaal::hcore::HPointer<HAbstractDestructor> destructor_ptr_t;
private:
	typedef yaal::hcore::HList<destructor_ptr_t> destructor_list_t;
	typedef yaal::hcore::HPointer<destructor_list_t> destructor_list_ptr_t;
	typedef yaal::hcore::HMultiMap<int, destructor_ptr_t> map_stack_t;
	yaal::hcore::HMutex f_oMutex;
	map_stack_t f_oDestructors;
	HLifeTimeTracker( void );
	void do_destruct( void );
public:
	~HLifeTimeTracker( void );
	static HLifeTimeTracker& get_instance( void );
	void register_destructor( destructor_ptr_t, int const& );
#if ! defined( __HOST_OS_TYPE_FREEBSD__ )
	static void destruct( void );
#else /* not __HOST_OS_TYPE_FREEBSD__ */
	static void destruct( void* );
#endif /* __HOST_OS_TYPE_FREEBSD__ */
	};

/*! \brief HLifeTimeTracker utility used to destroy guarded objects.
 */
template<typename tType>
class HDestructor : public HAbstractDestructor
	{
	tType*& f_rptObject;
public:
	explicit HDestructor( tType*& );
	~HDestructor( void );
	virtual void destruct( void );
	};

template<typename tType>
HDestructor<tType>::HDestructor( tType*& a_rptObject ) : HAbstractDestructor(), f_rptObject( a_rptObject )
	{
	}

template<typename tType>
HDestructor<tType>::~HDestructor( void )
	{
	M_PROLOG
	destruct();
	M_EPILOG
	}

template<typename tType>
void HDestructor<tType>::destruct( void )
	{
	M_PROLOG
	if ( f_rptObject )
		delete f_rptObject;
	f_rptObject = NULL;
	M_EPILOG
	}

/*! \brief Singleton pattern implementation.
 */
template<typename tType>
class HSingleton
	{
	typedef HSingleton<tType> self_t;
	static tType* f_ptInstance;
	static tType* create_instance( int const& );
public:
	static tType& get_instance( int const& = 0 );
	};

template<typename tType>
tType* HSingleton<tType>::f_ptInstance = NULL;

template<typename tType>
tType* HSingleton<tType>::create_instance( int const& a_iLifeTime )
	{
	M_PROLOG
	M_ASSERT( ! f_ptInstance );
	HLifeTimeTracker& lt = HLifeTimeTracker::get_instance();
	HLifeTimeTracker::destructor_ptr_t p( new HDestructor<tType>( f_ptInstance ) );
	lt.register_destructor( p, tType::life_time( a_iLifeTime ) );
	return ( new tType() );
	M_EPILOG
	}

template<typename tType>
tType& HSingleton<tType>::get_instance( int const& a_iLifeTime )
	{
	M_PROLOG
	if ( ! f_ptInstance )
		{
		static HMutex l_oMutex;
		HLock l( l_oMutex );
		if ( ! f_ptInstance )
			f_ptInstance = create_instance( a_iLifeTime );
		}
	return ( *f_ptInstance );
	M_EPILOG
	}

}

}

#endif /* not YAAL_HCORE_HSINGLETON_HXX_INCLUDED */

