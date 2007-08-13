/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hsingleton.h - this file is integral part of `yaal' project.

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

#ifndef __YAAL_HCORE_HSINGLETON_H
#define __YAAL_HCORE_HSINGLETON_H

#include "hcore/hpointer.h"
#include "hcore/hmap.h"
#include "hcore/hlist.h"

namespace yaal
{

namespace hcore
{

class HAbstractDestructor
	{
public:
	virtual ~HAbstractDestructor( void ) {}
	virtual void destruct( void ) = 0;
	};

class HLifeTimeTracker
	{
public:
	typedef yaal::hcore::HPointer<HAbstractDestructor, yaal::hcore::HPointerScalar, yaal::hcore::HPointerRelaxed> destructor_ptr_t;
private:
	typedef yaal::hcore::HList<destructor_ptr_t> destructor_list_t;
	typedef yaal::hcore::HPointer<destructor_list_t, yaal::hcore::HPointerScalar, yaal::hcore::HPointerRelaxed> destructor_list_ptr_t;
	typedef yaal::hcore::HMap<int, destructor_list_ptr_t> map_stack_t;
	static map_stack_t f_oDestructors;
public:
	static void register_destructor( destructor_ptr_t, int = 0 );
	static void destruct( void );
	};

template<typename tType>
class HDestructor : public HAbstractDestructor
	{
	tType*& f_ptObject;
public:
	explicit HDestructor( tType*& );
	~HDestructor( void );
	virtual void destruct( void );
	};

template<typename tType>
HDestructor<tType>::HDestructor( tType*& a_ptObject ) : HAbstractDestructor(), f_ptObject( a_ptObject )
	{
	}

template<typename tType>
HDestructor<tType>::~HDestructor( void )
	{
	destruct();
	}

template<typename tType>
void HDestructor<tType>::destruct( void )
	{
	if ( f_ptObject )
		delete f_ptObject;
	f_ptObject = NULL;
	}

template<typename tType>
class HSingleton
	{
	static tType* f_ptInstance;
	static void create_instance( void );
public:
	static tType& get_instance( void );
	};

template<typename tType>
tType* HSingleton<tType>::f_ptInstance = NULL;

template<typename tType>
void HSingleton<tType>::create_instance( void )
	{
	HLifeTimeTracker::register_destructor( HLifeTimeTracker::destructor_ptr_t( new HDestructor<tType>( f_ptInstance ) ) );
	f_ptInstance = new tType();
	}

template<typename tType>
tType& HSingleton<tType>::get_instance( void )
	{
	if ( ! f_ptInstance )
		create_instance();
	return ( *f_ptInstance );
	}

}

}

#endif /* not __YAAL_HCORE_HSINGLETON_H */

