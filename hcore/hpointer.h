/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hpointer.h - this file is integral part of `yaal' project.

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

#ifndef __YAAL_HCORE_HPOINTER_H
#define __YAAL_HCORE_HPOINTER_H

#line 31

#define D_VCSID_YAAL_HCORE_HPOINTER_H "$Id$"

#include "hcore/base.h"

namespace yaal
{

namespace hcore
{

/* Due to extream vitality of this struct,
 * none of the methods are guarded. */

struct REFERENCE_COUNTER_TYPE
	{
	static int const D_STRICT = 0;
	static int const D_WEAK = 1;
	};

template<typename tType>
class HPointerFromThisInterface;

template<typename tType, template<typename, typename>class pointer_type_t,
				 template<typename>class access_type_t>
class HPointer;

/*! \brief Pointer life time tracker access policy.
 */
template<typename tType>
struct HPointerStrict
	{
	static tType* raw( tType* );
	static tType& object_at( tType*, int );
	static void delete_pointee( tType*& );
	static void delete_pointee_array( tType*& );
	template<typename ptr_t>
	static void initialize_from_this( HPointerFromThisInterface<tType>*, ptr_t const& );
	template<typename ptr_t>
	static void initialize_from_this( void*, ptr_t const& );
	static void inc_reference_counter( int* );
	static void dec_reference_counter( int* );
	};

/*! \brief Pointer's observer access policy.
 */
template<typename tType>
struct HPointerWeak
	{
	inline static void delete_pointee( tType*& ){}
	inline static void delete_pointee_array( tType*& ){}
	static void inc_reference_counter( int* );
	static void dec_reference_counter( int* );
	};

/*! \brief Pointer to scalar deletion policy.
 */
template<typename tType, typename access_type_t>
struct HPointerScalar
	{
	static void delete_pointee( tType*& );
	static tType* raw( tType* );
	};

/*! \brief Pointer to array deletion policy.
 */
template<typename tType, typename access_type_t>
struct HPointerArray
	{
	static void delete_pointee( tType*& );
	static tType& object_at( tType*, int );
	};

/*! \brief Smart pointer, reference counting implementation.
 *
 * \tparam tType - object type which life time will be guarded.
 * \tparam pointer_type_t - select pointer type, one of HPointerScalar or HPointerArray.
 * \tparam access_type_t - defines pointer kind, one of HPointerStrict or HPointerWeak.
 */
template<typename tType, template<typename, typename>class pointer_type_t = HPointerScalar,
				 template<typename>class access_type_t = HPointerStrict>
class HPointer
	{
	class HShared
		{
		int f_piReferenceCounter[ 2 ];
		HShared( void ) : f_piReferenceCounter()
			{
			f_piReferenceCounter[ REFERENCE_COUNTER_TYPE::D_STRICT ] = 0;
			f_piReferenceCounter[ REFERENCE_COUNTER_TYPE::D_WEAK ] = 0;
			}
		friend class HPointer;
		};
	/*
	 * WARNING!
	 *
	 * Pointer to guarded object must be part of each
	 * smart-pointer instance so smart-pointer instance
	 * type information is preserved.
	 *
	 * It is necessary for proper resolving vtable
	 * bases in up/down casting of  smart-pointers 
	 * of objects of classes from multiple-inheritance
	 * (virtual or non-virtual) hierarchies.
	 */
	tType* f_ptObject;
	HShared* f_poShared;
public:
	HPointer( void );
	explicit HPointer( tType* const );
	virtual ~HPointer( void );
	HPointer( HPointer const& );
	template<typename hier_t, template<typename> class alien_access_t>
	HPointer( HPointer<hier_t, pointer_type_t, alien_access_t> const& );
	HPointer& operator = ( HPointer const& );
	template<typename hier_t, template<typename> class alien_access_t>
	HPointer& operator = ( HPointer<hier_t, pointer_type_t, alien_access_t> const& );
	tType const& operator* ( void ) const;
	tType& operator* ( void );
	tType const& operator[] ( int ) const;
	tType& operator[] ( int );
	template<typename hier_t, template<typename>class alien_access_t>
	bool operator == ( HPointer<hier_t, pointer_type_t, alien_access_t> const& ) const;
	template<typename hier_t>
	bool operator == ( hier_t const* const ) const;
	template<typename hier_t, template<typename>class alien_access_t>
	bool operator != ( HPointer<hier_t, pointer_type_t, alien_access_t> const& ) const;
	template<typename hier_t>
	bool operator != ( hier_t const* const ) const;
	tType const* operator->( void ) const;
	tType* operator->( void );
	tType const* raw ( void ) const;
	tType* raw ( void );
	bool operator! ( void ) const;
	void swap( HPointer& );
private:
	bool release( void ) throw();
	template<typename hier_t, template<typename> class alien_access_t>
	void acquire( HPointer<hier_t, pointer_type_t, alien_access_t> const& );
	void assign( tType*&, tType* );
	template<typename hier_t>
	void assign( tType*&, hier_t* );
	};

template<typename tType>
class HPointerFromThisInterface
	{
protected:
	typedef HPointer<tType, HPointerScalar, HPointerStrict> ptr_t;
	typedef HPointer<tType, HPointerScalar, HPointerWeak> weak_t;
	weak_t f_oSelfObserver;
public:
	HPointerFromThisInterface( void );
	virtual ~HPointerFromThisInterface( void ){}
	ptr_t get_pointer( void );
	ptr_t const get_pointer( void ) const;
protected:
	void initialize_observer( ptr_t const& );
	friend struct HPointerStrict<tType>;
	};

template<typename tType>
void HPointerStrict<tType>::delete_pointee( tType*& a_ptPointer )
	{
	delete a_ptPointer;
	a_ptPointer = NULL;
	}

template<typename tType>
void HPointerStrict<tType>::delete_pointee_array( tType*& a_ptPointer )
	{
	delete [] a_ptPointer;
	a_ptPointer = NULL;
	}

template<typename tType>
tType& HPointerStrict<tType>::object_at( tType* a_ptPointer, int a_iIndex )
	{
	return ( a_ptPointer[ a_iIndex ] );
	}

template<typename tType>
tType* HPointerStrict<tType>::raw( tType* a_ptPointer )
	{
	return ( a_ptPointer );
	}

template<typename tType>
void HPointerStrict<tType>::inc_reference_counter( int* a_piReferenceCounter )
	{
	++ a_piReferenceCounter[ REFERENCE_COUNTER_TYPE::D_STRICT ];
	++ a_piReferenceCounter[ REFERENCE_COUNTER_TYPE::D_WEAK ];
	}

template<typename tType>
void HPointerWeak<tType>::inc_reference_counter( int* a_piReferenceCounter )
	{
	++ a_piReferenceCounter[ REFERENCE_COUNTER_TYPE::D_WEAK ];
	}

template<typename tType>
void HPointerStrict<tType>::dec_reference_counter( int* a_piReferenceCounter )
	{
	M_ASSERT( a_piReferenceCounter[ REFERENCE_COUNTER_TYPE::D_STRICT ] > 0 );
	M_ASSERT( a_piReferenceCounter[ REFERENCE_COUNTER_TYPE::D_WEAK ] > 0 );
	-- a_piReferenceCounter[ REFERENCE_COUNTER_TYPE::D_STRICT ];
	-- a_piReferenceCounter[ REFERENCE_COUNTER_TYPE::D_WEAK ];
	}

template<typename tType>
void HPointerWeak<tType>::dec_reference_counter( int* a_piReferenceCounter )
	{
	M_ASSERT( a_piReferenceCounter[ REFERENCE_COUNTER_TYPE::D_WEAK ] > 0 );
	-- a_piReferenceCounter[ REFERENCE_COUNTER_TYPE::D_WEAK ];
	}

template<typename tType, typename access_type_t>
void HPointerScalar<tType, access_type_t>::delete_pointee( tType*& a_ptPointer )
	{
	access_type_t::delete_pointee( a_ptPointer );
	}

template<typename tType, typename access_type_t>
void HPointerArray<tType, access_type_t>::delete_pointee( tType*& a_ptPointer )
	{
	access_type_t::delete_pointee_array( a_ptPointer );
	}

template<typename tType, typename access_type_t>
tType& HPointerArray<tType, access_type_t>::object_at( tType* a_ptPointer, int a_iIndex )
	{
	return ( access_type_t::object_at( a_ptPointer[ a_iIndex ], a_iIndex ) );
	}

template<typename tType, typename access_type_t>
tType* HPointerScalar<tType, access_type_t>::raw( tType* a_ptPointer )
	{
	return ( access_type_t::raw( a_ptPointer ) );
	}

template<typename tType, template<typename, typename>class pointer_type_t,
				 template<typename>class access_type_t>
HPointer<tType, pointer_type_t, access_type_t>::HPointer( void )
	: f_ptObject( NULL ), f_poShared( NULL )
	{
	return;
	}

template<typename tType, template<typename, typename>class pointer_type_t,
				 template<typename>class access_type_t>
HPointer<tType, pointer_type_t, access_type_t>::HPointer( tType* const a_ptPointer )
	: f_ptObject( a_ptPointer ), f_poShared( new HShared() )
	{
	f_poShared->f_piReferenceCounter[ REFERENCE_COUNTER_TYPE::D_STRICT ] = 1;
	f_poShared->f_piReferenceCounter[ REFERENCE_COUNTER_TYPE::D_WEAK ] = 1;
	access_type_t<tType>::initialize_from_this( a_ptPointer, *this );
	return;
	}

template<typename tType, template<typename, typename>class pointer_type_t,
				 template<typename>class access_type_t>
HPointer<tType, pointer_type_t, access_type_t>::~HPointer( void )
	{
	f_poShared && release();
	return;
	}

template<typename tType, template<typename, typename>class pointer_type_t,
				 template<typename>class access_type_t>
HPointer<tType, pointer_type_t, access_type_t>::HPointer( HPointer<tType, pointer_type_t, access_type_t> const& a_roPointer )
	: f_ptObject( NULL ), f_poShared( NULL )
	{
	operator = ( a_roPointer );
	return;
	}

template<typename tType, template<typename, typename>class pointer_type_t,
				 template<typename>class access_type_t>
template<typename hier_t, template<typename>class alien_access_t>
HPointer<tType, pointer_type_t, access_type_t>::HPointer( HPointer<hier_t, pointer_type_t, alien_access_t> const& a_roPointer )
	: f_ptObject( NULL ), f_poShared( NULL )
	{
	operator = ( a_roPointer );
	return;
	}

template<typename tType, template<typename, typename>class pointer_type_t,
				 template<typename>class access_type_t>
HPointer<tType, pointer_type_t, access_type_t>& HPointer<tType, pointer_type_t, access_type_t>::operator = ( HPointer<tType, pointer_type_t, access_type_t> const& a_roPointer )
	{
	acquire( a_roPointer );
	return ( *this );
	}

template<typename tType, template<typename, typename>class pointer_type_t,
				 template<typename>class access_type_t>
template<typename hier_t, template<typename>class alien_access_t>
HPointer<tType, pointer_type_t, access_type_t>& HPointer<tType, pointer_type_t, access_type_t>::operator = ( HPointer<hier_t, pointer_type_t, alien_access_t> const& a_roPointer )
	{
	acquire( a_roPointer );
	return ( *this );
	}

template<typename tType, template<typename, typename>class pointer_type_t,
				 template<typename>class access_type_t>
template<typename hier_t, template<typename>class alien_access_t>
void HPointer<tType, pointer_type_t, access_type_t>::acquire( HPointer<hier_t, pointer_type_t, alien_access_t> const& from )
	{
	HPointer const& alien = reinterpret_cast<HPointer const&>( from );
	if ( ( &alien != this ) && ( f_poShared != alien.f_poShared ) )
		{
		M_ASSERT( ( ! ( f_poShared && alien.f_poShared ) )
				|| ( ( f_poShared && alien.f_poShared )
					&& ( f_ptObject != reinterpret_cast<hier_t*>( alien.f_ptObject ) ) ) );
		if ( f_poShared )
			release();
		if ( alien.f_poShared && ( alien.f_poShared->f_piReferenceCounter[ REFERENCE_COUNTER_TYPE::D_STRICT ] > 0 ) )
			{
			access_type_t<tType>::inc_reference_counter( alien.f_poShared->f_piReferenceCounter );
			f_poShared = alien.f_poShared;
			assign( f_ptObject, reinterpret_cast<hier_t*>( alien.f_ptObject ) );
			}
		else
			{
			f_poShared = NULL;
			f_ptObject = NULL;
			}
		}
	return;
	}

template<typename tType, template<typename, typename>class pointer_type_t,
				 template<typename>class access_type_t>
template<typename hier_t>
void HPointer<tType, pointer_type_t, access_type_t>::assign( tType*& to, hier_t* from )
	{
	to = dynamic_cast<tType*>( from );
	}

template<typename tType, template<typename, typename>class pointer_type_t,
				 template<typename>class access_type_t>
void HPointer<tType, pointer_type_t, access_type_t>::assign( tType*& to, tType* from )
	{
	to = from;
	}

template<typename tType, template<typename, typename>class pointer_type_t,
				 template<typename>class access_type_t>
bool HPointer<tType, pointer_type_t, access_type_t>::release( void ) throw()
	{
	M_ASSERT( f_poShared );
	if ( f_poShared->f_piReferenceCounter[ REFERENCE_COUNTER_TYPE::D_STRICT ] == 1 )
		pointer_type_t<tType, access_type_t<tType> >::delete_pointee( f_ptObject );
	access_type_t<tType>::dec_reference_counter( f_poShared->f_piReferenceCounter );
	if ( ! f_poShared->f_piReferenceCounter[ REFERENCE_COUNTER_TYPE::D_WEAK ] )
		{
		delete f_poShared;
		f_poShared = NULL;
		}
	return ( ! ( f_poShared && f_poShared->f_piReferenceCounter[ REFERENCE_COUNTER_TYPE::D_STRICT ] ) );
	}

template<typename tType, template<typename, typename>class pointer_type_t,
				 template<typename>class access_type_t>
tType const& HPointer<tType, pointer_type_t, access_type_t>::operator* ( void ) const
	{
	M_ASSERT( f_poShared && ( f_poShared->f_piReferenceCounter[ REFERENCE_COUNTER_TYPE::D_STRICT ] > 0 ) );
	return ( *f_ptObject );
	}

template<typename tType, template<typename, typename>class pointer_type_t,
				 template<typename>class access_type_t>
tType& HPointer<tType, pointer_type_t, access_type_t>::operator* ( void )
	{
	M_ASSERT( f_poShared && ( f_poShared->f_piReferenceCounter[ REFERENCE_COUNTER_TYPE::D_STRICT ] > 0 ) );
	return ( *f_ptObject );
	}

template<typename tType, template<typename, typename>class pointer_type_t,
				 template<typename>class access_type_t>
tType const& HPointer<tType, pointer_type_t, access_type_t>::operator[] ( int a_iIndex ) const
	{
	M_ASSERT( f_poShared && ( f_poShared->f_piReferenceCounter[ REFERENCE_COUNTER_TYPE::D_STRICT ] > 0 ) );
	M_ASSERT( a_iIndex >= 0 );
	return ( pointer_type_t<tType, access_type_t<tType> >::object_at( f_ptObject, a_iIndex ) );
	}

template<typename tType, template<typename, typename>class pointer_type_t,
				 template<typename>class access_type_t>
tType& HPointer<tType, pointer_type_t, access_type_t>::operator[] ( int a_iIndex )
	{
	M_ASSERT( f_poShared && ( f_poShared->f_piReferenceCounter[ REFERENCE_COUNTER_TYPE::D_STRICT ] > 0 ) );
	M_ASSERT( a_iIndex >= 0 );
	return ( pointer_type_t<tType, access_type_t<tType> >::object_at( f_ptObject, a_iIndex ) );
	}

template<typename tType, template<typename, typename>class pointer_type_t,
				 template<typename>class access_type_t>
template<typename hier_t, template<typename>class alien_access_t>
bool HPointer<tType, pointer_type_t, access_type_t>::operator == ( HPointer<hier_t, pointer_type_t, alien_access_t>const & a_roPointer ) const
	{
	HPointer const* alien = reinterpret_cast<HPointer const *>( &a_roPointer );
	return ( f_ptObject == reinterpret_cast<hier_t*>( alien->f_ptObject ) );
	}

template<typename tType, template<typename, typename>class pointer_type_t,
				 template<typename>class access_type_t>
template<typename hier_t>
bool HPointer<tType, pointer_type_t, access_type_t>::operator == ( hier_t const* const a_ptPointer ) const
	{
	return ( f_ptObject == a_ptPointer );
	}

template<typename tType, template<typename, typename>class pointer_type_t,
				 template<typename>class access_type_t>
template<typename hier_t, template<typename>class alien_access_t>
bool HPointer<tType, pointer_type_t, access_type_t>::operator != ( HPointer<hier_t, pointer_type_t, alien_access_t>const & a_roPointer ) const
	{
	return ( ! operator == ( a_roPointer ) );
	}

template<typename tType, template<typename, typename>class pointer_type_t,
				 template<typename>class access_type_t>
template<typename hier_t>
bool HPointer<tType, pointer_type_t, access_type_t>::operator != ( hier_t const* const a_ptPointer ) const
	{
	return ( ! operator == ( a_ptPointer ) );
	}

template<typename tType, template<typename, typename>class pointer_type_t,
				 template<typename>class access_type_t, typename hier_t>
bool operator == ( hier_t const* const a_ptPointer, HPointer<tType, pointer_type_t, access_type_t> const& a_oPointer )
	{
	return ( a_oPointer == a_ptPointer );
	}

template<typename tType, template<typename, typename>class pointer_type_t,
				 template<typename>class access_type_t, typename hier_t>
bool operator != ( hier_t const* const a_ptPointer, HPointer<tType, pointer_type_t, access_type_t> const& a_oPointer )
	{
	return ( a_oPointer != a_ptPointer );
	}

template<typename tType, template<typename, typename>class pointer_type_t,
				 template<typename>class access_type_t>
tType const* HPointer<tType, pointer_type_t, access_type_t>::operator->( void ) const
	{
	M_ASSERT( f_poShared && ( f_poShared->f_piReferenceCounter[ REFERENCE_COUNTER_TYPE::D_STRICT ] > 0 ) );
	return ( access_type_t<tType>::raw( f_ptObject ) );
	}

template<typename tType, template<typename, typename>class pointer_type_t,
				 template<typename>class access_type_t>
tType* HPointer<tType, pointer_type_t, access_type_t>::operator->( void )
	{
	M_ASSERT( f_poShared && ( f_poShared->f_piReferenceCounter[ REFERENCE_COUNTER_TYPE::D_STRICT ] > 0 ) );
	return ( access_type_t<tType>::raw( f_ptObject ) );
	}

template<typename tType, template<typename, typename>class pointer_type_t,
				 template<typename>class access_type_t>
tType const* HPointer<tType, pointer_type_t, access_type_t>::raw( void ) const
	{
	return ( f_ptObject );
	}

template<typename tType, template<typename, typename>class pointer_type_t,
				 template<typename>class access_type_t>
tType* HPointer<tType, pointer_type_t, access_type_t>::raw( void )
	{
	return ( f_ptObject );
	}

template<typename tType, template<typename, typename>class pointer_type_t,
				 template<typename>class access_type_t>
bool HPointer<tType, pointer_type_t, access_type_t>::operator ! ( void ) const
	{
	return ( ! f_ptObject );
	}

template<typename tType>
template<typename ptr_t>
void HPointerStrict<tType>::initialize_from_this( HPointerFromThisInterface<tType>* obj,
		ptr_t const& ptr )
	{
	obj->initialize_observer( ptr );
	}

template<typename tType>
template<typename ptr_t>
void HPointerStrict<tType>::initialize_from_this( void*,
		ptr_t const& )
	{
	}

template<typename tType>
HPointerFromThisInterface<tType>::HPointerFromThisInterface( void ) : f_oSelfObserver()
	{
	return;
	}

template<typename tType>
void HPointerFromThisInterface<tType>::initialize_observer( ptr_t const& a_oFirstOwner )
	{
	f_oSelfObserver = a_oFirstOwner;
	}

template<typename tType>
typename HPointerFromThisInterface<tType>::ptr_t HPointerFromThisInterface<tType>::get_pointer( void )
	{
	return ( f_oSelfObserver );
	}

template<typename tType>
typename HPointerFromThisInterface<tType>::ptr_t const HPointerFromThisInterface<tType>::get_pointer( void ) const
	{
	return ( f_oSelfObserver );
	}

}

}

#endif /* not __YAAL_HCORE_HPOINTER_H */

